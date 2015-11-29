#ifndef UNIPOLAR_UCT_H_
#define UNIPOLAR_UCT_H_
#include "board.h"
#include "def.h"
using namespace unipolar;

template <int BOARD_SIZE>
class UCT {
public:
	UCT() = default;
	~UCT() = default;
	static Move GenMove(const Board &board, Force force);
};

#endif