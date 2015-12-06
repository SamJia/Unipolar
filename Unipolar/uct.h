#ifndef UNIPOLAR_UCT_H_
#define UNIPOLAR_UCT_H_
#include "board.h"
#include "def.h"
using namespace unipolar;

template <int BOARD_SIZE>
class UCT {
private:
	struct Node {
		PositionIndex pos;
		Node *son, *bro;
		Node(PositionIndex po = -1, Node *so = nullptr, Node *br = nullptr) : pos(po), son(so), bro(br) {}
	};
public:
	UCT() = default;
	~UCT() = default;
	Move GenMove(const Board &board, PointState state);
private:
	Node root;
};

Mode GenMove(const Board &board, PointState state) {
	
}

#endif