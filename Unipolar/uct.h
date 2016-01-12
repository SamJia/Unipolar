#ifndef UNIPOLAR_UCT_H_
#define UNIPOLAR_UCT_H_
#include "board.h"
#include "def.h"
#include "mc.h"
#include "joseki.h"
#include <cstring>
#include <cassert>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <thread>
#include <mutex>
#include <set>
#include <iostream>
#include <stdio.h>
#include <algorithm>

FILE *f = fopen("unipolar.txt", "w");

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
public:

public:
	string joseki_seq;	
	TireTree *joseki_p;
	UCT(string seq, TireTree *jose) : root(new Node()), joseki_seq(seq) {
		joseki_p = jose;
	};
	UCT() : root(new Node()) {
		joseki_seq = "";	
	};
	~UCT() {
		delete root;
		// delete joseki;
	};
	void Task(Board &board, PointState state, Node *node, int &num, int t, int seed);
	void GenChild(Node *node, Board &board, PointState state);
	double UCB(Node *node, Node *parent);
	double Score(Node *node, Node *parent);
	Node *FindBestChild(Node *node);
	Node *FindBestUCT(Node *node);
	void MCSimulation(Board &board, Node *node, PointState state, string joseki_seq);
	Move GenMove(Board &board, PointState state);
	void CopyUCT(Node *node, double **valueboard, int **numboard, double **bonusboard);
	void PrintUCT();

private:
	Node *root;
};

void UCT::Task(Board &board, PointState state, Node *node, int &num, int t, int seed) {
	// std::cout<<end_time<<clock()<<std::endl;
	srand(seed);
	while (clock() < t) {
		mtx.lock();
		++num;
		mtx.unlock();
		Board board_copy(board);
		string seq_copy = joseki_seq;
		// MC().Simulate(board_copy, state);
		MCSimulation(board_copy, node, state, seq_copy);
		//std::cout << std::this_thread::get_id() << std::endl;
		// printf("one MCSimulation done\n");
		// exit(0);
	}
}

void UCT::GenChild(Node *node, Board &board, PointState state) {
	mtx.lock();
	// node->val.resize(170, 0);
	// node->num.resize(170, 0);
	// printf("resize\n");
	// printf("node:%d\n", node);
	node->son_val_a.resize(170);
	node->son_num_a.resize(170);
	// printf("resize done\n");
	// node->val_p.resize(170, 0);
	if (node->son == nullptr) {
		std::vector<PositionIndex> playable = board.GetPlayablePosition(state);
		if (playable.size() == 0) {
			node->son = new Node(POSITION_PASS, nullptr, nullptr);
			//printf("no playable\n");
		}
		for (int i = 0, size = playable.size(); i < size; ++i) {
			node->son_val_a[playable[i]] = 0;
			node->son_num_a[playable[i]] = 0;
			node->son = new Node(playable[i], nullptr, node->son);
		}
	}
	mtx.unlock();

	//WaitForMultipleObjects(playable.size(), Thread, true, INFINITE);
	// for (std::vector<PositionIndex>::iterator it = playable.begin(); it != playable.end(); it++)
	// node->son = new Node(*it, nullptr, node->son);
}

double UCT::UCB(Node *node, Node *parent) {
	const double BIAS = 1.0 / 4500;
	if (node->num) {
		if (parent->son_num_a[node->pos]) {
			double beta = parent->son_num_a[node->pos] / (parent->son_num_a[node->pos] + node->num + parent->son_num_a[node->pos] * node->num * BIAS);
			return (1.0 - beta) * node->val / node->num + beta * parent->son_val_a[node->pos] / parent->son_num_a[node->pos];
		} else {
			return node->val / node->num;
		}
	} else if (parent->son_num_a[node->pos]) {
		return parent->son_val_a[node->pos] / parent->son_num_a[node->pos];
	}
	return 1.0;
	// if (node->num * 1000 <= parent->num)
	// 	return 1000000000;
	// return node->bon * bonus_ratio + node->val / node->num + uctconst * sqrt(log(parent->num) / node->num);
}

double UCT::Score(Node *node, Node *parent) {
	if (node->num == 0)
		return 0;
	return node->bon * bonus_ratio + /*(node->val + parent->son_val_a[node->pos]) /*/ (node->num + parent->son_num_a[node->pos]);
}

Node *UCT::FindBestChild(Node *node) {
	if (node->son == nullptr) {
		std::cout << "no child" << std::endl;
		exit(0);
	}
	double maxUCB = -100;
	double actUCB;
	Node *maxNode = nullptr;
	for (Node *p = node->son; p; p = p->bro) {
		actUCB = UCB(p, node)/* + node->son_val_a[p->pos] / std::max(0.000001, node->son_num_a[p->pos])*/;
		if (actUCB > maxUCB) {
			maxUCB = actUCB;
			maxNode = p;
		}
	}
	return maxNode;
}

Node *UCT::FindBestUCT(Node *node) {
	if (node->son == nullptr) {
		std::cout << "no child" << std::endl;
		exit(0);
	}
	// add joseki bonus.
	double maxScore = -100;
	double actScore;
	Node *maxNode = nullptr;
	for (Node *p = node->son; p; p = p->bro) {
		actScore = Score(p, node)/* + node->son_val_a[p->pos] / std::max(0.000001, node->son_num_a[p->pos])*/;
		if (actScore > maxScore) {
			maxScore = actScore;
			maxNode = p;
		}
	}
	return maxNode;
}

int max_depth = 0;
void UCT::MCSimulation(Board &board, Node *node, PointState state, string joseki_seq) {
	// printf("MCSimulation\n");
	std::vector<Node *> record;
	Amaf amaf;
	record.reserve(220);
	//Node *record[170];
	Node *act = node;
	record.push_back(node);
	int idx = 0;
	bool flag = true;
	double once_val = 0;
	double once_bon = 0;
	while (act->son != nullptr) {
		idx += 1;
		act = FindBestChild(act);
		record.push_back(act);
		amaf.set(act->pos, state);
		once_bon = board.PlayMove(Move(state, act->pos));
		if (joseki_seq != "") {
			joseki_p->updateSeq(joseki_seq, act->pos);
		}
		if (act->pos == POSITION_PASS && record[idx - 1]->pos == POSITION_PASS) {
			once_val = MC().Evaluate(board, state);
			flag = false;
			break;
		}
		state = 1 - state;
	}
	if (flag) {
		if (act->num >= EXPAND_NUM) {
			idx += 1;
			GenChild(act, board, state);
			if (act->son == nullptr)
				std::cout << "gen failed" << std::endl;
			act = FindBestChild(act);
			// add to seq;
			record.push_back(act);
			amaf.set(act->pos, state);
			once_bon = board.PlayMove(Move(state, act->pos));
			if (joseki_seq != "") {
				joseki_p->updateSeq(joseki_seq, act->pos);
			}
			state = 1 - state;
		}
		once_val = MC().Simulate(board, state, amaf, *joseki_p, joseki_seq);
	}
	mtx.lock();
	idx = record.size()-1;
	max_depth = std::max(max_depth, idx);
	record[idx]->bon = once_bon;
	for (int i = idx; i > 0; --i) {
		record[i]->val += once_val;
		record[i]->num += 1;
		state = 1 - state;
		for (Node *p = record[i - 1]->son; p; p = p->bro) {
			if (amaf[p->pos] == state) {
				record[i - 1]->son_val_a[p->pos] += once_val;
				record[i - 1]->son_num_a[p->pos] += 1.0;
			}
		}
		once_val = 1 - once_val;
	}
	record[0]->num += 1;
	mtx.unlock();
}

Move UCT::GenMove(Board &board, PointState state) {
	// printf("state %d\n", state);
	// printf("GenMove\n");
	int t = clock();
	Move nextstep;
	root->num = 1;
	printf("GenChild\n");
	GenChild(root, board, state);
	std::cout << root->son << std::endl;
	if (root->son == nullptr)
		return Move(EMPTY_POINT, POSITION_PASS);
	int count = 0;
	int end_time = t + CLOCKS_PER_SEC * 3;
	std::vector<std::thread> threads;
	threads.reserve(10);
	for (int i = 0; i < THREAD_NUM; ++i){
		int seed = rand();
		threads.push_back(std::thread([&]() {this->Task(board, state, root, count, end_time, seed);}));
	}
	for (auto& th : threads)
		th.join();
	printf("max_depth:%d\n", max_depth);
	std::cout << "totally " << count << " times of MC" << std::endl;
	// PrintUCT();
	nextstep.state = state;
	nextstep.position = FindBestUCT(root)->pos;
	PrintUCT();
	std::cout << joseki_seq << std::endl;
	return nextstep;
}

void UCT::PrintUCT() {
	double uctval[BOARD_SIZE][BOARD_SIZE] = {{0}};
	double uctbon[BOARD_SIZE][BOARD_SIZE] = {{0}};
	double uctaddi[BOARD_SIZE][BOARD_SIZE] = {{0}};
	int uctnum[BOARD_SIZE][BOARD_SIZE] = {{0}};
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
	fprintf(f, "---THE VALUE MATRIX---\n");
	printf("   0    1    2    3    4    5    6    7    8    9    10   11   12\n");
	fprintf(f, "   0    1    2    3    4    5    6    7    8    9    10   11   12\n");
	for (int x = 0; x < BOARD_SIZE; ++x) {
		printf("%02d ", x);
		fprintf(f, "%02d ", x);
		for (int y = 0; y < BOARD_SIZE; ++y) {
			printf("%.3f ", uctval[x][y]);
			fprintf(f, "%.3f ", uctval[x][y]);
		}
		printf("\n");
		fprintf(f, "\n");
	}
	// printf("---THE ADDI MATRIX---\n");
	// fprintf(f, "---THE ADDI MATRIX---\n");
	// printf("   0    1    2    3    4    5    6    7    8    9    10   11   12\n");
	// fprintf(f,"   0    1    2    3    4    5    6    7    8    9    10   11   12\n");
	// for (int x = 0; x < BOARD_SIZE; ++x) {
	// 	printf("%02d ", x);
	// 	fprintf(f,"%02d ", x);
	// 	for (int y = 0; y < BOARD_SIZE; ++y) {
	// 		printf("%.3f ", uctaddi[x][y]);
	// 		fprintf(f, "%.3f ", uctaddi[x][y]);
	// 	}
	// 	printf("\n");
	// 	fprintf(f, "\n");
	// }
	printf("---THE NUMBER MATRIX---\n");
	fprintf(f,"---THE NUMBER MATRIX---\n");
	printf("   0    1    2    3    4    5    6    7    8    9    10   11   12\n");
	fprintf(f,"   0    1    2    3    4    5    6    7    8    9    10   11   12\n");
	for (int x = 0; x < BOARD_SIZE; ++x) {
		printf("%02d ", x);
		fprintf(f, "%02d ", x);
		for (int y = 0; y < BOARD_SIZE; ++y) {
			printf("%4d ", uctnum[x][y]);
			fprintf(f,"%4d ", uctnum[x][y]);
		}
		printf("\n");
		fprintf(f,"\n");
	}
	// printf("---THE VAL_A MATRIX---\n");
	// printf("   0    1    2    3    4    5    6    7    8    9    10   11   12\n");
	// for (int x = 0; x < BOARD_SIZE; ++x) {
	// 	printf("%02d ", x);
	// 	for (int y = 0; y < BOARD_SIZE; ++y) {
	// 		printf("%.3f ", root->son_val_a[x * 13 + y] / std::max(0.0001, root->son_num_a[x * 13 + y]));
	// 	}
	// 	printf("\n");
	// }
	printf("---THE NUM_A MATRIX---\n");
	printf("   0    1    2    3    4    5    6    7    8    9    10   11   12\n");
	for (int x = 0; x < BOARD_SIZE; ++x) {
		printf("%02d ", x);
		for (int y = 0; y < BOARD_SIZE; ++y) {
			printf("%.3f ", root->son_num_a[x * 13 + y]);
		}
		printf("\n");
	}
	// printf("---THE BONUS MATRIX---\n");
	// fprintf(f,"---THE BONUS MATRIX---\n");
	// printf("   0    1    2    3    4    5    6    7    8    9    10   11   12\n");
	// fprintf(f,"   0    1    2    3    4    5    6    7    8    9    10   11   12\n");
	// for (int x = 0; x < BOARD_SIZE; ++x) {
	// 	printf("%02d ", x);
	// 	fprintf(f,"%02d ", x);
	// 	for (int y = 0; y < BOARD_SIZE; ++y) {
	// 		printf("%.3f ", uctbon[x][y]);
	// 		fprintf(f,"%.3f ", uctbon[x][y]);
	// 	}
	// 	printf("\n");
	// 	fprintf(f,"\n");
	// }
	act = FindBestUCT(root);
	i = act->pos / BOARD_SIZE;
	j = act->pos % BOARD_SIZE;
	printf("The best next step: (%d, %d)\n", i, j);
	fprintf(f,"The best next step: (%d, %d)\n", i, j+1);
	char i_char = (i > 7 ? i + 1 : i) + 'A';
	fprintf(f, "the next step is %c%d\n", i_char,j+1);
	fprintf(f, "\n");
	fprintf(f, "\n");
	fprintf(f, "\n");
	fprintf(f, "\n");
}

#endif
