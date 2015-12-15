#include "controller.h"
#include <string>
#include <cstdlib>
#include <ctime>

int main(){
	srand(time(0));
	Board::Init();
	double value = 0;
	Board board;
	board.ClearBoard();
	// board.PlayMove(Move(0, 0));
	Board board_copy(board);
	printf("%f", MC().Simulate(board_copy, 1));
	board_copy.Print();
	// for(int i = 0; i < 1000; ++i){
	// 	Board board_copy(board);
	// 	value += MC().Simulate(board_copy, 1);
	// }
	// printf("value:%f\n", value);
	// board.ClearBoard();
	// board.PlayMove(Move(0, 56));
	// value = 0;
	// for(int i = 0; i < 1000; ++i){
	// 	Board board_copy(board);
	// 	value += MC().Simulate(board_copy, 1);
	// }
	// printf("value:%f\n", value);
	
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