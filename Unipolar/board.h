#ifndef UNIPOLAR_BOARD_H_
#define UNIPOLAR_BOARD_H_

#include "def.h"
#include "chanset.h"
using namespace unipolar;

struct Move {
	Force force;
	PositionIndex position;
	Move(Force fo = BLACE_FORCE, PositionIndex po = 0) : force(fo), position(po){} 
};

template<int BOARD_SIZE>
class Board {
public:
	Board() = default;
	~Board() = default;
	void PlayMove(const Move &move);
	std::vector<PositionIndex> GetPlayablePosition();
	// DISALLOW_COPY_AND_ASSIGN_AND_MOVE(Board)

private:
	char board_[BoardSizeSquare(BOARD_SIZE)];
	ChanSet<BOARD_SIZE> chanset;
};

#endif