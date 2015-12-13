#include "board.h"
#include <string>
#include <cstdlib>
#include <ctime>

int main(){
	Board::Init();
	Board board;
	board.ClearBoard();
	std::string command;
	int tmp1, tmp2, tmp3;
	while(true){
		printf("command:");
		std::cin >> command;
		if(command == "disp")
			board.Print();
		else if(command == "clear")
			board.ClearBoard();
		else if(command == "play"){
			std::cin >> tmp1 >> tmp2 >> tmp3;
			board.PlayMove(Move(tmp1, tmp2 * 13 + tmp3));
		}
	}
	// // uint64_t a = -1;
	// // printf("%llu\n", a);
	// // printf("%d\n", __builtin_ctzll(a));
	// BitSet bitset;
	// bitset.Set(77);
	// // bitset.Set(27);
	// printf("%d\n", bitset.GetAirPos());
}