#ifndef UNIPOLAR_BOARD_H_
#define UNIPOLAR_BOARD_H_
// #include <intrin.h>
#include <cstring>
#include <set>
#include <iostream>

// #include "mc.h"
#include "def.h"
using namespace unipolar;

struct Move {
	PointState state;
	PositionIndex position;
	Move(PointState sta = EMPTY_POINT, PositionIndex pos = POSITION_PASS) : state(sta), position(pos) {}
};

class Board {
private:
	friend class MC;
	std::set<PositionIndex> playable_pos[2];/*, suiside_pos[2]*/
public:
	Board() {
		for(PositionIndex i = 0; i < BoardSizeSquare(MAIN_BOARD_SIZE); ++i) {
			board_[i] = EMPTY_POINT;
		}
		playable_pos[0].clear();
		playable_pos[1].clear();
	}
	~Board() = default;
	void PlayMove(const Move &move);
	int board_[BoardSizeSquare(MAIN_BOARD_SIZE)];
	std::set<PositionIndex> GetPlayablePosition(PointState state);
};

void Board::PlayMove(const Move &move) {
	if (move.position == POSITION_PASS)
		return;
	PositionIndex pos = move.position;
	board_[pos] = move.state;
	return;
}

std::set<PositionIndex> Board::GetPlayablePosition(PointState state) {
	playable_pos[0].clear();
	playable_pos[1].clear();
	for(PositionIndex i = 0; i < BoardSizeSquare(MAIN_BOARD_SIZE); ++i) {
		if(board_[i] == EMPTY_POINT) {
			playable_pos[0].insert(i);
			playable_pos[1].insert(i);
		}
	}
	return playable_pos[state];
}


#endif
