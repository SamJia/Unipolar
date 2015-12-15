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
	// srand(time(NULL));
	board.StartMC();
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
	// 	playable_pos[0] = board.GetPlayablePosition(force);
	// 	position[0] = rand() % playable_pos[0].size();
	// 	move[0].position = position[0];
	// 	board.PlayMove(move[0]);

	// 	playable_pos[1] = board.GetPlayablePosition(1 - force);
	// 	position[1] = rand() % playable_pos[1].size();
	// 	move[1].position = position[1];
	// 	board.PlayMove(move[1]);
	// } while(!playable_pos[0].empty() && !playable_pos[1].empty());
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
			// if (last_pass)
			break;
			last_pass = true;
			continue;
		}
		std::set<PositionIndex>::iterator it = playable_pos.begin();
		// printf("start advance\n");
		std::advance(it, 0);
		printf("%d, %d\n", next_state^1, *it);
		// printf("advance done\n");
		mv.state = next_state ^ 1;
		mv.position = *it;
		// printf("PlayMove\n");
		board.PlayMove(mv);
		last_pass = false;
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
	return board.GetPieceCount(state) > board.GetPieceCount(state ^ 1);
	// int piece_count[] = {board.GetPieceCount(0), board.GetPieceCount(1)};
	// return float(piece_count[state]) / (piece_count[0] + piece_count[1]);
}

#endif
