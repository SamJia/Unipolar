#ifndef UNIPOLAR_MC_H_
#define UNIPOLAR_MC_H_
#include "board.h"
#include "def.h"
using namespace unipolar;

template <int BOARD_SIZE>
class MC {
public:
	MC() = default;
	~MC() = default;
	static int Simulate(const Board &board, Force force);
};

#endif