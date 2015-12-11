#ifndef UNIPOLAR_MC_H_
#define UNIPOLAR_MC_H_
#include <stdlib.h>
#include <time.h>
#include "board.h"
#include "def.h"
using namespace unipolar;

template <int BOARD_SIZE>
class MC {
public:
	MC() = default;
	~MC() = default;
	static int Simulate(const Board &board, Force force);
private:
	double EvalFunc(List Board[BoradSizeSquare(BOARD_SIZE)], Force force);
};

int Simulate(const Board &board, Force force) {
	/*
	Basic idea:
		Copy the board.
		Randomly select among playable positions and play in turn.
		Until it ends.
		Evaluate the situation and return a heuristic number for that.

	Here it goes.
	*/
	srand(time(NULL));
	Board mcBoard(board);
	// PointState state[2];
	// PositionIndex position[2];
	// Move move[2];
	// state[0] = force;		// Can we do this?
	// state[1] = 1 - force;
	// move[0].state = state[0];
	// move[1].state = state[1];
	// std::set<PositionIndex> playable_pos[2];
	// // Do we have to wait for both players to give PASS?
	// do {
	// 	playable_pos[0] = mcBoard.GetPlayablePosition(force);
	// 	position[0] = rand() % playable_pos[0].size();
	// 	move[0].position = position[0];
	// 	mcBoard.PlayMove(move[0]);

	// 	playable_pos[1] = mcBoard.GetPlayablePosition(1 - force);
	// 	position[1] = rand() % playable_pos[1].size();
	// 	move[1].position = position[1];
	// 	mcBoard.PlayMove(move[1]);
	// } while(!playable_pos[0].empty() && !playable_pos[1].empty());
	PointState state;
	PositionIndex position;
	std::set<PositionIndex> playable_pos;
	Move move;
	do {
		playable_pos = mcBoard.GetPlayablePosition(force);
		position = rand() % playable_pos.size();
		move.state = force;
		move.position = position;
		mcBoard.PlayMove(move);

		force ^= 1;
	} while(!playable_pos.empty());


	return mcBoard.Evaluate(&EvalFunc, force);
}

/*
Some ideas:
	Only return binary val.
	Return a ratio of black territory.
Currently we choose the second method, but the number of real eyes is missing.
Better if we can record the number of black force / black_real_eye within board.h.
*/
double EvalFunc(List board[BoradSizeSquare(BOARD_SIZE)], Force force) {
	int black_count = 0;
	for(int i = 0; i < BoardSizeSquare(BOARD_SIZE); ++i) {
		if(board[i].state == BLACK_POINT)
			black_count++;
	}
	double black_ratio = double(black_count) / BoardSizeSquare(BOARD_SIZE);
	return force == BLACK_FORCE ? black_ratio : 1 - black_ratio;
}

#endif
