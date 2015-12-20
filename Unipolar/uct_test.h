#ifndef UNIPOLAR_UCT_H_
#define UNIPOLAR_UCT_H_
#include "board.h"
#include "def.h"
#include "mc.h"
#include <cstring>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <float.h>
#include <set>
#include <vector>

using namespace unipolar;

#define CONFIDENCE_CONST 50

class UCT {
private:
	struct Node {
		PositionIndex pos;
		Count num;
		Value val;
		std::vector<Node *> sons;		// use pointer or not?
		// Node *son, *bro;
		// here we use the even-game heuristic!
		Node(PositionIndex po = -1, int n = CONFIDENCE_CONST, float v = 25) : pos(po), num(n), val(v) {}
		~Node() = default;
	};
public:
	UCT() : root(new Node()) {};
	~UCT(){
		delete root;
	};
	void GenChild(Node *node, Board &board, PointState state);
	float UCB(Node *node, Count totalnum);
	float Score(Node *node);
	Node *FindBestChild(Node *node);
	Node *FindBestUCT(Node *node);
	float MCSimulation(Board &board, Node *node, PointState state);
	Move GenMove(Board &board, PointState state);
	void CopyUCT(Node *node, float **valueboard, int **numboard);
	void PrintUCT();

private:
	Node *root;
};

void UCT::GenChild(Node *node, Board &board, PointState state) {
	std::vector<PositionIndex> playable = board.GetPlayablePosition(state);
	node->sons.reserve(playable.size()+1);
	PositionIndex i = 0;
	for (; i < playable.size(); ++i) {
		node->sons.push_back(new Node(playable[i]));
	}
	node->sons.push_back(new Node(POSITION_PASS));
}

float UCT::UCB(Node *node, Count totalnum) {
	if (node->num == 0)
		return DBL_MAX;
	return node->val / node->num + uctconst * sqrt(logf(totalnum) / node->num);
}

float UCT::Score(Node *node){
	if (node->num == 0)
		return 0;
	return node->val / node->num;
}

UCT::Node *UCT::FindBestChild(Node *node) {
	float maxUCB = -1, actUCB;
	Node *maxNode, *p;
	for(PositionIndex i = 0; i < node->sons.size(); ++i) {
		p = node->sons[i];
		actUCB = UCB(p, node->num);
		if(actUCB > maxUCB) {
			maxUCB = actUCB;
			maxNode = p;
		}
	}	
	return maxNode;
}

UCT::Node *UCT::FindBestUCT(Node *node) {
	float maxUCB = -1, actUCB;
	Node *maxNode, *p;
	for(PositionIndex i = 0; i < node->sons.size(); ++i) {
		p = node->sons[i];
		actUCB = Score(p);
		if(actUCB > maxUCB) {
			maxUCB = actUCB;
			maxNode = p;
		}
	}	
	return maxNode;
}

float UCT::MCSimulation(Board &board, Node *node, PointState state) {
	// board.Print();
	Node *act = FindBestChild(node);
	// printf("PlayMove %d %d\n", state, act->pos);
	board.PlayMove(Move(state, act->pos));
	// printf("playmovedone\n");
	act->num += 1;
	Value value_once;

	if(node->pos == POSITION_PASS && act->pos == POSITION_PASS) {
		value_once = MC().Evaluate(board, state);
	}
	else if (act->sons.empty()) {
		if (act->num > 1 + CONFIDENCE_CONST) { // modified!
			GenChild(act, board, 1 - state);
			value_once = 1 - MCSimulation(board, act, 1 - state);
		}
		else {
			// printf("Mc simulate\n");
			value_once = MC().Simulate(board, 1 - state);
		}
	}
	else {
		value_once = 1 - MCSimulation(board, act, 1 - state);
	}
	act->val += value_once;
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
	int end_time = t + CLOCKS_PER_SEC * 1;
	while (clock() < end_time) {
		++count;
		root->num += 1;
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

void UCT::CopyUCT(Node *node, float **valueboard, int **numboard) {
    Node *act;
	for(PositionIndex i = 0; i < node->sons.size(); ++i) {
		act = node->sons[i];
        valueboard[act->pos / BOARD_SIZE][act->pos % BOARD_SIZE] = Score(act);
        numboard[act->pos / BOARD_SIZE][act->pos % BOARD_SIZE] = act->num;
	}
}

void UCT::PrintUCT() {
    float **uctval;
    int **uctnum;
    uctval = new float*[BOARD_SIZE];
    uctnum = new int*[BOARD_SIZE];
    for(int k = 0; k < BOARD_SIZE; ++k) {
        uctval[k] = new float[BOARD_SIZE];
        memset(uctval[k],0,sizeof(uctval[k]));
        uctnum[k] = new int[BOARD_SIZE];
        memset(uctnum[k],0,sizeof(uctnum[k]));
    }
    int i, j;
    printf("The total simulation time is %d\n", root->num);
    CopyUCT(root, uctval, uctnum);
    printf("---THE VALUE MATRIX---\n");
    printf("   0    1    2    3    4    5    6    7    8    9    10   11   12\n");
    for(int x = 0; x < BOARD_SIZE; ++x) {
        printf("%02d ", x);
        for(int y = 0; y < BOARD_SIZE; ++y) {
        	if(uctval[x][y] > 1000)
        		printf("0.00 ");
        	else
	            printf("%.2f ", uctval[x][y]);
        }
        printf("\n");
    }
    printf("---THE NUMBER MATRIX---\n");
    printf("   0    1    2    3    4    5    6    7    8    9    10   11   12\n");
    for(int x = 0; x < BOARD_SIZE; ++x) {
        printf("%02d ", x);
        for(int y = 0; y < BOARD_SIZE; ++y) {
        	if(uctnum[x][y] > 1000)
        		printf("00  ");
        	else
	            printf("%02d   ", uctnum[x][y]);
        }
        printf("\n");
    }
    Node *act = FindBestUCT(root);
    i = act->pos / BOARD_SIZE;
    j = act->pos % BOARD_SIZE;
    printf("The best next step: (%d, %d)\n", i, j);
    delete(uctval);
    delete(uctnum);
}

#endif
