#ifndef UNIPOLAR_UCT_H_
#define UNIPOLAR_UCT_H_
#include "board.h"
#include "def.h"
#include "mc.h"
#include <cstring>
#include <cassert>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <thread>
#include <mutex>
#include <set>
#include <iostream>
using namespace unipolar;

double test_bonus[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
	0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
	0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0,
	0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0,
	0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0,
	0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0,
	0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0,
	0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
	0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

class UCT {
private:
	struct Node {
		PositionIndex pos;
		Count num;
		Value val, bon;
		Node *son, *bro;
		Node(PositionIndex po = -1, Node *so = nullptr, Node *br = nullptr) : pos(po), num(0), val(0), bon(0), son(so), bro(br) {}
		~Node() {
			if (son)
				delete son;
			if (bro)
				delete bro;
		}
	};
public:
	double joseki_bonus[BoardSizeSquare(BOARD_SIZE)];
	UCT(double* bonus = NULL) : root(new Node()) {
		if (bonus) {
			// 	memcpy(joseki_bonus, bonus, sizeof(bonus));
			for (PositionIndex i = 0; i < BoardSizeSquare(BOARD_SIZE); ++i)
				joseki_bonus[i] = bonus[i];
		}
		else {
			for (PositionIndex i = 0; i < BoardSizeSquare(BOARD_SIZE); ++i)
				joseki_bonus[i] = 0;
		}
	};
	~UCT() {
		delete root;
	};
	void Task(Board &board, PointState state, Node *node, int &num, int t);
	void GenChild(Node *node, Board &board, PointState state);
	double UCB(Node *node, Count totalnum);
	double Score(Node *node);
	Node *FindBestChild(Node *node);
	Node *FindBestUCT(Node *node);
	void MCSimulation(Board &board, Node *node, PointState state);
	Move GenMove(Board &board, PointState state);
	void CopyUCT(Node *node, double **valueboard, int **numboard, double **bonusboard);
	void PrintUCT();

private:
	Node *root;
};

void UCT::Task(Board &board, PointState state, Node *node, int &num, int t) {
	// std::cout<<end_time<<clock()<<std::endl;
	while (clock() < t) {
		mtx.lock();
		++num;
		mtx.unlock();
		Board board_copy(board);
		// MC().Simulate(board_copy, state);
		MCSimulation(board_copy, node, state);

		//std::cout << std::this_thread::get_id() << std::endl;
		// printf("one MCSimulation done\n");
		// exit(0);
	}
}

void UCT::GenChild(Node *node, Board &board, PointState state) {
	mtx.lock();
	if (node->son == nullptr) {
		std::vector<PositionIndex> playable = board.GetPlayablePosition(state);
		if (playable.size() == 0) {
			node->son = new Node(POSITION_PASS, nullptr, nullptr);
			//printf("no playable\n");
		}
		for (int i = 0, size = playable.size(); i < size; ++i)
			node->son = new Node(playable[i], nullptr, node->son);
	}
	mtx.unlock();

	//WaitForMultipleObjects(playable.size(), Thread, true, INFINITE);
	// for (std::vector<PositionIndex>::iterator it = playable.begin(); it != playable.end(); it++)
	// node->son = new Node(*it, nullptr, node->son);
}

double UCT::UCB(Node *node, Count totalnum) {
	if (node->num * 1000 <= totalnum)
		return 1000000000;
	return node->bon * bonus_ratio + node->val / node->num + uctconst * sqrt(log(totalnum) / node->num);
}

double UCT::Score(Node *node) {
	if (node->num == 0)
		return 0;
	return node->bon * bonus_ratio + node->val / node->num;
}

UCT::Node *UCT::FindBestChild(Node *node) {
	if (node->son == nullptr) {
		std::cout << "no child" << std::endl;
	exit(0);
	}
	double maxUCB = UCB(node->son, node->num - 1);
	double actUCB;
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
	if (node->son == nullptr) {
		std::cout << "no child" << std::endl;
		exit(0);
	}
	// add joseki bonus.
	double maxScore = Score(node->son) + joseki_bonus[node->son->pos] + test_bonus[node->son->pos] / 100;
	double actScore;
	Node *maxNode = node->son;
	for (Node *p = node->son->bro; p; p = p->bro) {
		actScore = Score(p)  + joseki_bonus[p->pos] + test_bonus[p->pos] / 100;
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
	bool flag = true;
	double once_val = 0;
	double once_bon = 0;
	while (act->son != nullptr) {
		idx += 1;
		act = record[idx] = FindBestChild(act);
		once_bon = board.PlayMove(Move(state, act->pos));
		if (act->pos == POSITION_PASS && record[idx - 1]->pos == POSITION_PASS) {
			once_val = MC().Evaluate(board, state);
			flag = false;
			break;
		}
		state = 1 - state;
	}
	if (flag)
	{
		if (act->num >= 1) {
			idx += 1;
			GenChild(act, board, state);
			if (act->son == nullptr)
				std::cout << "gen failed" << std::endl;
			act = record[idx] = FindBestChild(act);
			once_bon = board.PlayMove(Move(state, act->pos));
			state = 1 - state;
		}
		once_val = MC().Simulate(board, state);
	}
	mtx.lock();
	record[idx]->bon = once_bon;
	for (int i = idx; i >= 0; --i)
	{
		record[i]->val += once_val;
		record[i]->num += 1;
		once_val = 1 - once_val;
	}
	mtx.unlock();
}

Move UCT::GenMove(Board &board, PointState state) {
	// printf("state %d\n", state);
	// printf("GenMove\n");
	int t = clock();
	Move nextstep;
	root->num = 1;
	// printf("GenChild\n");
	GenChild(root, board, state);
	// std::cout << root->son << std::endl;
	if (root->son == nullptr)
		return Move(EMPTY_POINT, POSITION_PASS);
	int count = 0;
	int end_time = t + CLOCKS_PER_SEC * 3;
	std::vector<std::thread> threads;
	threads.reserve(10);
	for (int i = 0; i < 8; ++i)
		threads.push_back(std::thread([&]() {this->Task(board, state, root, count, end_time);}));
	for (auto& th : threads)
		th.join();
	std::cout << "totally " << count << " times of MC" << std::endl;
	PrintUCT();
	nextstep.state = state;
	nextstep.position = FindBestUCT(root)->pos;
	// PrintUCT();
	return nextstep;
}

void UCT::PrintUCT() {
	double uctval[BOARD_SIZE][BOARD_SIZE];
	double uctbon[BOARD_SIZE][BOARD_SIZE];
	double uctaddi[BOARD_SIZE][BOARD_SIZE];
	int uctnum[BOARD_SIZE][BOARD_SIZE];
	int i, j;
	Node *act = root->son;
	double totalnum = root->num;
	while (act != nullptr) {
		uctval[act->pos / BOARD_SIZE][act->pos % BOARD_SIZE] = act->val / act->num;
		uctbon[act->pos / BOARD_SIZE][act->pos % BOARD_SIZE] = act->bon;
		uctnum[act->pos / BOARD_SIZE][act->pos % BOARD_SIZE] = act->num;
		uctaddi[act->pos / BOARD_SIZE][act->pos % BOARD_SIZE] = uctconst * sqrt(log(totalnum) / act->num);
		act = act->bro;
	}
	printf("The total simulation time is %d\n", root->num);
	printf("---THE VALUE MATRIX---\n");
	printf("   0    1    2    3    4    5    6    7    8    9    10   11   12\n");
	for (int x = 0; x < BOARD_SIZE; ++x) {
		printf("%02d ", x);
		for (int y = 0; y < BOARD_SIZE; ++y) {
			printf("%.3f ", uctval[x][y]);
		}
		printf("\n");
	}
	printf("---THE ADDI MATRIX---\n");
	printf("   0    1    2    3    4    5    6    7    8    9    10   11   12\n");
	for (int x = 0; x < BOARD_SIZE; ++x) {
		printf("%02d ", x);
		for (int y = 0; y < BOARD_SIZE; ++y) {
			printf("%.3f ", uctaddi[x][y]);
		}
		printf("\n");
	}
	printf("---THE NUMBER MATRIX---\n");
	printf("   0    1    2    3    4    5    6    7    8    9    10   11   12\n");
	for (int x = 0; x < BOARD_SIZE; ++x) {
		printf("%02d ", x);
		for (int y = 0; y < BOARD_SIZE; ++y) {
			printf("%4d ", uctnum[x][y]);
		}
		printf("\n");
	}
	printf("---THE BONUS MATRIX---\n");
	printf("   0    1    2    3    4    5    6    7    8    9    10   11   12\n");
	for (int x = 0; x < BOARD_SIZE; ++x) {
		printf("%02d ", x);
		for (int y = 0; y < BOARD_SIZE; ++y) {
			printf("%.3f ", uctbon[x][y]);
		}
		printf("\n");
	}
	act = FindBestUCT(root);
	i = act->pos / BOARD_SIZE;
	j = act->pos % BOARD_SIZE;
	printf("The best next step: (%d, %d)\n", i, j);
}

#endif
