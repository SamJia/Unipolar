#ifndef UNIPOLAR_MC_H_
#define UNIPOLAR_MC_H_
#include <stdlib.h>
#include <time.h>
#include <iterator>
#include "board_.h"
#include "def.h"
using namespace unipolar;

class Board;

class MC {
public:
	MC() = default;
	~MC() = default;
	double Simulate(const Board &board, Force force);
private:
	double Evaluate(const Board &Board, Force force);
};

double MC::Simulate(const Board &board, Force force) {
	/*
	Basic idea:
		Copy the board.
		Randomly select among playable positions and play in turn.
		Until it ends.
		Evaluate the situation and return a heuristic number for that.

	Here it goes.
	*/
	srand(time(NULL));
	Board mcBoard = board;
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
	Move mv;
	playable_pos = mcBoard.GetPlayablePosition(force);

	while(!playable_pos.empty()) {
		std::set<PositionIndex>::iterator it = playable_pos.begin();
		std::advance(it, rand() % playable_pos.size());
		mv.state = force;
		mv.position = *it;
		mcBoard.PlayMove(mv);
		force = static_cast<Force>(force ^ 1);
		playable_pos = mcBoard.GetPlayablePosition(force);
	}
	return Evaluate(mcBoard, force);
}

/*
Some ideas:
	Only return binary val.
	Return a ratio of black territory.
Currently we choose the second method, but the number of real eyes is missing.
Better if we can record the number of black force / black_real_eye within board.h.
*/

double MC::Evaluate(const Board &mcBoard, Force force) {
	int black_count = 0;
	for(PositionIndex i = 0; i < BoardSizeSquare(MAIN_BOARD_SIZE); ++i) {
		if(mcBoard.board_[i] == BLACK_POINT)
			black_count++;
	}
	double black_ratio = double(black_count) / BoardSizeSquare(MAIN_BOARD_SIZE);
	return force == BLACK_FORCE ? black_ratio : 1 - black_ratio;
}

#endif
