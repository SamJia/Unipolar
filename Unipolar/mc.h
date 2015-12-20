#ifndef UNIPOLAR_MC_H_
#define UNIPOLAR_MC_H_
#include <stdlib.h>
#include <time.h>
#include <iterator>
#include "board.h"
#include "def.h"
using namespace unipolar;

class Board;

class MC {
public:
	MC() = default;
	~MC() = default;
	float Simulate(Board &board, PointState state);
private:
	float Evaluate(Board &Board, PointState state);
};

float MC::Simulate(Board &board, PointState state) {
	/*
	Basic idea:
		Copy the board.
		Randomly select among playable positions and play in turn.
		Until it ends.
		Evaluate the situation and return a heuristic number for that.

	Here it goes.
	*/
	board.StartMC();
	PointState next_state = state;
	Move mv;
	int count = 0;
	bool last_pass = false;
	while (true/*!playable_pos.empty()*/) {
		++count;
		if (count > 1000) {
			board.Print();
			printf("too much MC\n");
			exit(0);
		}
		std::set<PositionIndex> &playable_pos = board.GetPlayablePositionMC(next_state);
		next_state ^= 1;
		// printf("playable_pos_set size %d\n", playable_pos.size());
		if (playable_pos.size() == 0) {
			if (last_pass)
				break;
			last_pass = true;
			continue;
		}
		std::set<PositionIndex>::iterator it = playable_pos.begin();
		// printf("start advance\n");
		std::advance(it, rand() * playable_pos.size() / (RAND_MAX + 1));
		// printf("%d, %d\n", next_state^1, *it);
		// printf("advance done\n");
		mv.state = next_state ^ 1;
		mv.position = *it;
		// printf("PlayMove\n");
		board.PlayMove(mv);
		last_pass = false;
		// if(count > 30)
		// 	board.Print();
		// printf("static_cast\n");
		// printf("GetPlayablePosition\n");
		// playable_pos = board.GetPlayablePosition(force);
		// printf("one loop done\n");
	}
	// printf("Evaluate\n");
	// board.Print();
	// printf("totally %d times of play_move\n", count);
	return Evaluate(board, state ^ 1);
}

/*
Some ideas:
	Only return binary val.
	Return a ratio of black territory.
Currently we choose the second method, but the number of real eyes is missing.
Better if we can record the number of black force / black_real_eye within board.h.
*/

float MC::Evaluate(Board &board, PointState state) {
	// return board.GetPieceCount(state) > board.GetPieceCount(state ^ 1);
	// int piece_count[] = {board.GetPieceCount(0), board.GetPieceCount(1)};
	int piece_count[] = {board.GetAreaCount(0), board.GetAreaCount(1)};
	return 2*piece_count[state] > (piece_count[0] + piece_count[1])? 1 : 0;
}

#endif
