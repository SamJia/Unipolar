#ifndef UNIPOLAR_MC_H_
#define UNIPOLAR_MC_H_
#include <stdlib.h>
#include <time.h>
#include <iterator>
#include <thread>
#include <iostream>
#include <mutex>
#include <fstream>
#include "board.h"
#include "def.h"
using namespace unipolar;

std::mutex mtx;
class Board;

class MC {
public:
	MC() = default;
	~MC() = default;
	double Simulate(Board &board, PointState state);
	double Evaluate(Board &Board, PointState state);
// private:
};

double MC::Simulate(Board &board, PointState state) {
	PointState next_state = state;
	Move mv;
	int count = board.GetPieceCount(0) + board.GetPieceCount(1);
	bool last_pass = false;
	int playable_count;
	std::vector<PositionIndex> playable_pos;
	while (count < 200/*!playable_pos.empty()*/) {
		++count;
		if (count > 1000) {
			board.Print();
			printf("too much MC\n");
			exit(0);
		}
		mv.state = next_state;
		mv.position = POSITION_PASS;//board.SpecialPointTest(next_state);
		if (mv.position == POSITION_PASS) {
			playable_count = board.GetPlayableCount(next_state);
			if (playable_count == 0) {
				// board.empty_[state].Print();
				// board.suiside_[state].Print();
				// board.safe_eye_[state].Print();
				// board.dangerous_empty_[state].Print();
				// board.dangerous_empty_[state^1].Print();
				// (board.empty_[state] - board.suiside_[state] - board.safe_eye_[state] - board.dangerous_empty_[state] - board.dangerous_empty_[state^1]).Print();
				break;
			}
			mv.position = board.GetPlayable(mv.state, rand() * playable_count / (RAND_MAX + 1));
		}
		// printf("PlayMove\n");
		// fout << ';' << (mv.state == BLACK_POINT ? 'B' : 'W') << '[' << (char)('a' + mv.position / 13) << (char)(('a' + mv.position % 13)) << ']';
		board.PlayMove(mv);
		next_state ^= 1;
	}
	return Evaluate(board, state ^ 1);
}

double MC::Evaluate(Board &board, PointState state) {
	double piece_count[2];
	piece_count[WHITE_POINT] = board.GetAreaCount(WHITE_POINT) + komi;
	piece_count[BLACK_POINT] = board.GetAreaCount(BLACK_POINT);
	return piece_count[state] > piece_count[state ^ 1];
}

#endif
