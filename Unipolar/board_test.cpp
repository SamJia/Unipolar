#include "controller.h"
#include <string>
#include <cstdlib>
#include <ctime>

int main() {
	srand(time(0));
	Board::Init();
	double value = 0;
	Board board;
	for (int i = 0; i < 1000; ++i) {
		board.ClearBoard();
		MC().Simulate(board, 0);
	}

	// Move move = UCT().GenMove(board, 0);
	// printf("%d, %d\n", move.position, move.state);
	// const Board board3(board);
	// Board board2(board3);
	// std::string command;
	// int tmp1, tmp2, tmp3;
	// while(true){
	// 	printf("command:");
	// 	std::cin >> command;
	// 	if(command == "disp")
	// 		board2.Print();
	// 	else if(command == "clear")
	// 		board2.ClearBoard();
	// 	else if(command == "play"){
	// 		std::cin >> tmp1 >> tmp2 >> tmp3;
	// 		board2.PlayMove(Move(tmp1, tmp2 * 13 + tmp3));
	// 	}
	// }
	// // uint64_t a = -1;
	// // printf("%llu\n", a);
	// // printf("%d\n", __builtin_ctzll(a));
	// BitSet bitset;
	// bitset.Set(77);
	// // bitset.Set(27);
	// printf("%d\n", bitset.GetAirPos());
}