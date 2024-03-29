#include <iostream>
#include <chrono>
#include "tables.h"
#include "position.h"
#include "types.h"

#include "cpp-httplib/httplib.h"

using namespace std;


//Computes the perft of the position for a given depth, using bulk-counting
//According to the https://www.chessprogramming.org/Perft site:
//Perft is a debugging function to walk the move generation tree of strictly legal moves to count 
//all the leaf nodes of a certain depth, which can be compared to predetermined values and used to isolate bugs
template<Color Us>
unsigned long long perft(Position& p, unsigned int depth) {
	int nmoves;
	unsigned long long nodes = 0;

	MoveList<Us> list(p);

	if (depth == 1) return (unsigned long long) list.size();

	for (Move move : list) {
		p.play<Us>(move);
		nodes += perft<~Us>(p, depth - 1);
		p.undo<Us>(move);
	}

	return nodes;
}

//A variant of perft, listing all moves and for each move, the perft of the decremented depth
//It is used solely for debugging
template<Color Us>
void perftdiv(Position& p, unsigned int depth) {
	unsigned long long nodes = 0, pf;

	MoveList<Us> list(p);

	for (Move move : list) {
		std::cout << move;

		p.play<Us>(move);
		pf = perft<~Us>(p, depth - 1);
		std::cout << ": " << pf << " moves\n";
		nodes += pf;
		p.undo<Us>(move);
	}

	std::cout << "\nTotal: " << nodes << " moves\n";
}

void test_perft() {
	Position p;
	Position::set("rnbqkbnr/pppppppp/8/8/8/8/PPPP1PPP/RNBQKBNR w KQkq -", p);
	std::cout << p;

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	auto n = perft<WHITE>(p, 6);
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	auto diff = end - begin;

	std::cout << "Nodes: " << n << "\n";
	std::cout << "NPS: "
		<< int(n * 1000000.0 / std::chrono::duration_cast<std::chrono::microseconds>(diff).count())
		<< "\n";
	std::cout << "Time difference = "
		<< std::chrono::duration_cast<std::chrono::microseconds>(diff).count() << " [microseconds]\n";
}

const int mate_score = 10000;

template<Color sideToMove, Color side>
int eval(Position& p) {
	MoveList<sideToMove> list(p);
	int adjust = sideToMove == side ? 1 : -1;
	if (list.size() > 0 )
	  return list.size() * adjust;
	else
	  return mate_score * adjust;
}

vector<uint64_t> previous_hash;

httplib::Server svr;

int main() {
  using namespace httplib;

    initialise_all_databases();
    zobrist::initialise_zobrist_keys();
/*	
    string input;
    cin >> input;

  if (0 == input.compare("uci") ){
      cout << "id name stronkchess\n"
        << "id author Joseph Huang\n"
        << "uciok" << endl;
        
      cin >> input;
      if (0 == input.compare("isready") ){
        cout << "readyok";
      }
      return 0;
  }
  else if (0 == input.compare("test") ){
    
      Position p;
      Position::set("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -", p);
      cout << p;
    
      MoveList<WHITE> list(p);
    
      for(Move m : list) {
          cout << m << "\n";
      }
      
      cout << "eval score: " << list.size();

      previous_hash.push_back(p.get_hash());
	  test_perft();
  }
  else if (0 == input.compare("server") )*/{
    svr.Get("/about", [](const httplib::Request &, httplib::Response &res) {
      res.set_content("stronkchess by Joseph Huang", "text/plain");
    });
    
    svr.Get(R"(/(([a-h][1-8][a-h][1-8])*))", [&](const Request& req, Response& res) {
      string s = req.matches[1];
      Position p;
      Position::set("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -", p);
      int ev = eval<WHITE, WHITE>(p);
      
      auto m = s.substr(0, 4);
      
      while (s.length() >= 4){
        p.play<WHITE>(Move(m));
        ev = eval<BLACK, BLACK>(p);
        s = s.substr(4);
        if (s.length() >= 4){
          p.play<BLACK>(Move(s.substr(0,4)));
          ev = eval<WHITE, WHITE>(p);
          s = s.substr(4);
        }
        m = s.substr(0, 4);
      }
      res.set_content(to_string(ev), "text/plain");
      
      cout << p;
    });

    svr.listen("0.0.0.0", 8080);
}
  
    return 0;
}
