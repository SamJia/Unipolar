#ifndef UNIPOLAR_CHAIN_SET_H_
#define UNIPOLAR_CHAIN_SET_H_
#include "def.h"
using namespace unipolar;

template<int BOARD_SIZE>
class ChainSet {
public:
	ChainSet = default;
	~ChainSet = default;
	// DISALLOW_COPY_AND_ASSIGN_AND_MOVE(ChainSet)
private:
	AirCount GetInitAirOfPoint(PositionIndex pos);
	struct List {
		PositionIndex father, next, tail;
		AirCount air_count;
		PositionState state;
		List(PositionIndex pos = 0) : father(BoardSizeSquare(BOARD_SIZE)), next(pos), tail(pos), state(EMPTY_POINT) {
			air_count = GetInitAirOfPoint(pos);
		}
	};

	List chain_board[BoardSizeSquare(BOARD_SIZE)];
};


AirCount ChainSet::GetInitAirOfPoint(PositionIndex pos){
	AirCount result = 4;
	int tmp = pos % BOARD_SIZE;
	result -= (tmp == 0 || tmp == BOARD_SIZE - 1);
	tmp = pos / BOARD_SIZE;
	result -= (tmp == 0 || tmp == BOARD_SIZE - 1);
	return result;
}

#endif