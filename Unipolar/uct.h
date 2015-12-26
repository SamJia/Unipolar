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
#include <thread>
#include <mutex>
#include <set>
#include <iostream>
#include <float.h>
using namespace unipolar;

std::mutex mtx;

class UCT {
private:
	struct Node {
		PositionIndex pos;
		Count num;
		Value val;
		Node *son, *bro;
		Node(PositionIndex po = -1, Node *so = nullptr, Node *br = nullptr) : pos(po), num(0), val(0), son(so), bro(br) {}
		~Node(){
			if(son)
				delete son;
			if(bro)
				delete bro;
		}
	};
public:
	UCT() : root(new Node()) {};
	~UCT(){
		delete root;
	};
	void Task(Board &board, PointState state, Node *node, int &num);
	void GenChild(Node *node, Board &board, PointState state);
	float UCB(Node *node, Count totalnum);
	float Score(Node *node);
	Node *FindBestChild(Node *node);
	Node *FindBestUCT(Node *node);
	void MCSimulation(Board &board, Node *node, PointState state);
	Move GenMove(Board &board, PointState state);
	void CopyUCT(Node *node, float **valueboard, int **numboard);
	void PrintUCT();

private:
	Node *root;
};

void UCT::Task(Board &board, PointState state, Node *node, int &num) {
    if (mtx.try_lock()) {
        ++num;
        // std::cout<<end_time<<clock()<<std::endl;
        node->num += 1;
        Board board_copy(board);
        //board.StartMC();
        MCSimulation(board_copy, root, state);
        //std::cout << std::this_thread::get_id() << std::endl;
        // printf("one MCSimulation done\n");
		// exit(0);
        mtx.unlock();
    }
}

void UCT::GenChild(Node *node, Board &board, PointState state) {
	node->son = new Node(POSITION_PASS, nullptr, nullptr);
	std::vector<PositionIndex> playable = board.GetPlayablePosition(state);
	for(int i = 0, size = playable.size(); i < size; ++i)
		node->son = new Node(playable[i], nullptr, node->son);
    //WaitForMultipleObjects(playable.size(), Thread, true, INFINITE);
	// for (std::vector<PositionIndex>::iterator it = playable.begin(); it != playable.end(); it++)
		// node->son = new Node(*it, nullptr, node->son);
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
	float maxUCB = UCB(node->son, node->num - 1);
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
	float maxScore = Score(node->son);
	float actScore;
	Node *maxNode = node->son;
	for (Node *p = node->son->bro; p; p = p->bro) {
		actScore = Score(p);
		if (actScore > maxScore) {
			maxScore = actScore;
			maxNode = p;
		}
	}
	return maxNode;
}

void UCT::MCSimulation(Board &board, Node *node, PointState state) {
    Node *record[170];
    Node *act = record[0] = node;
    int idx = 0;
    while(act->son != nullptr) {
        idx += 1;
        act = record[idx] = FindBestChild(act);
        board.PlayMove(Move(state, act->pos));
        state = 1 - state;
        act->num += 1;
    }
    if(act->num > 1) {
        idx += 1;
        GenChild(act, board, state);
        act = record[idx] = FindBestChild(act);
        state = 1 - state;
        act->num += 1;
    }
    if(act->pos == POSITION_PASS && record[idx - 1]->pos == POSITION_PASS)
        record[idx]->val = MC().Evaluate(board, state);
    else
        record[idx]->val = MC().Simulate(board, state);
    for(int i = 1; i < idx; ++i)
    {
        if(i % 2 == idx % 2)
            record[i]->val += record[idx]->val;
        else
            record[i]->val += (1 - record[idx]->val);
    }
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
	int end_time = t + CLOCKS_PER_SEC * 3;
	std::thread threads[10];
	while (clock() < end_time) {
        for(int i = 0; i < 10; ++i)
            threads[i] = std::thread([&]{Task(board, state, root, count);});
        for(auto& th : threads)
            th.join();
	}
	//printf("totally %d times of MC\n", count);
	// PrintUCT();
	nextstep.state = state;
	nextstep.position = FindBestUCT(root)->pos;
	return nextstep;
}

void UCT::CopyUCT(Node *node, float **valueboard, int **numboard) {
    Node *act = node->son;
    while(act != nullptr) {
        valueboard[act->pos / BOARD_SIZE][act->pos % BOARD_SIZE] = Score(act);
        numboard[act->pos / BOARD_SIZE][act->pos % BOARD_SIZE] = act->num;
        act = act->bro;
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
	            printf("%02d  ", uctnum[x][y]);
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
