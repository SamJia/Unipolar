#ifndef UNIPOLAR_UCT_H_
#define UNIPOLAR_UCT_H_
#include "board.h"
#include "def.h"
#include "mc.h"
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <set>
#include <float.h>
using namespace unipolar;

class UCT {
private:
	struct Node {
		PositionIndex pos;
		Count num;
		Value val;
		Node *son, *bro;
		Node(PositionIndex po = -1, Node *so = nullptr, Node *br = nullptr) : pos(po), num(0), val(0), son(so), bro(br) {}
	};
public:
	UCT() : root(new Node()) {};
	~UCT() = default;
	void GenChild(Node *node, Board &board, PointState state);
	float UCB(Node *node, Count totalnum);
	Node *FindBestChild(Node *node);
	Node *FindBestUCT(Node *node);
	int MCSimulation(Board &board, Node *node, PointState state);
	Move GenMove(Board &board, PointState state);

private:
	Node *root;
};

void UCT::GenChild(Node *node, Board &board, PointState state) {
	std::set<PositionIndex> &playable = board.GetPlayablePosition(state);
	for (std::set<PositionIndex>::iterator it = playable.begin(); it != playable.end(); it++)
		node->son = new Node(*it, nullptr, node->son);
	node->son = new Node(POSITION_PASS, nullptr, node->son);
}

float UCT::UCB(Node *node, Count totalnum) {
	if (node->num == 0)
		return DBL_MAX;
	return node->val / node->num + uctconst * sqrt(logf(totalnum) / node->num);
}

UCT::Node *UCT::FindBestChild(Node *node) {
	float maxUCB = UCB(node->son, node->num);
	float actUCB;
	Node *maxNode = node->son;
	for (Node *p = node->son->bro; p; p = p->bro) {
		actUCB = UCB(p, node->num);
		if (actUCB > maxUCB) {
			maxUCB = actUCB;
			maxNode = p;
		}
	}
	return maxNode;
}

UCT::Node *UCT::FindBestUCT(Node *node) {
	float maxUCB = UCB(node->son, node->num);
	float actUCB;
	Node *maxNode = node->son;
	for (Node *p = node->son->bro; p; p = p->bro) {
		actUCB = UCB(p, node->num);
		if (actUCB > maxUCB) {
			maxUCB = actUCB;
			maxNode = p;
		}
	}
	return maxNode;
}

int UCT::MCSimulation(Board &board, Node *node, PointState state) {
	// board.Print();
	Node *act = FindBestChild(node);
	// printf("PlayMove %d %d\n", state, act->pos);
	board.PlayMove(Move(state, act->pos));
	// printf("playmovedone\n");
	act->num += 1;
	Value value_once;
	if (act->son == nullptr) {
		if (act->num > 1) {
			GenChild(act, board, 1 - state);
			value_once = 1 - MCSimulation(board, act, 1 - state);
			act->val += value_once;
		}
		else {
			// printf("Mc simulate\n");
			value_once = act->val = MC().Simulate(board, 1 - state);			
		}
	}
	else {
		value_once = 1 - MCSimulation(board, act, 1 - state);
		act->val += value_once;
	}
	return value_once;
}

Move UCT::GenMove(Board &board, PointState state) {
	// printf("state %d\n", state);
	// printf("GenMove\n");
	int t = clock();
	Move nextstep;
	root->num = 1;
	// printf("GenChild\n");
	GenChild(root, board, state);
	int count = 0;
	int end_time = t + CLOCKS_PER_SEC * 2.9;
	while (clock() < end_time) {
		++count;
		root->num += 1;
		// printf("once\n");
		Board board_copy(board);
		MCSimulation(board_copy, root, state);
		// printf("one MCSimulation done\n");
		// exit(0);
	}
	// printf("totally %d times of MC\n", count);
	nextstep.state = state;
	nextstep.position = FindBestUCT(root)->pos;
	return nextstep;
}

#endif
