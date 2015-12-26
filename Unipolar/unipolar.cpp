#include "controller.h"

int main(int argc, char const *argv[]) {
	srand(time(0));
	Controller controller;
	Board::Init();
	Board board;
	board.ClearBoard();
	controller.Run(board);
	return 0;
}