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
	float Simulate(Board &board, PointState state);
	float Evaluate(Board &Board, PointState state);
// private:
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
	// board.StartMC();
	PointState next_state = state;
	Move mv;
	int count = board.GetPieceCount(0) + board.GetPieceCount(1);
	bool last_pass = false;
	int playable_count;
	std::vector<PositionIndex> playable_pos;
// 	std::ofstream fout2("num.txt", std::ios::app);
// 	std::ofstream fout("test.sgf");
// 	fout << "(;FF[4]CA[UTF-8]AP[GoGui:1.4.9]SZ[13]\
// KM[6.5]PB[gogui-twogtp]PW[gogui-twogtp]DT[2015-12-16]";
	while (count < 200/*!playable_pos.empty()*/) {
		++count;
		if (count > 1000) {
			board.Print();
			printf("too much MC\n");
			exit(0);
		}
		mv.state = next_state;
		mv.position = board.SpecialPointTest(next_state);
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
	// fout << ')';
	// fout.close();
	// fout2 << count << ' ';
	// fout2.close();
	return Evaluate(board, state ^ 1);
}

float MC::Evaluate(Board &board, PointState state) {
	float piece_count[2];
	piece_count[WHITE_POINT] = board.GetAreaCount(WHITE_POINT) + 6.5;
	piece_count[BLACK_POINT] = board.GetAreaCount(BLACK_POINT);
	return piece_count[state] > piece_count[state ^ 1];
}

#endif
