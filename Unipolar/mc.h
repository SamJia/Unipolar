#ifndef UNIPOLAR_MC_H_
#define UNIPOLAR_MC_H_
#include <stdlib.h>
#include <time.h>
#include <iterator>
#include <thread>
#include <iostream>
#include <mutex>
#include <fstream>
#include "board.h"
#include "def.h"
#include "joseki.h"
using namespace unipolar;

std::mutex mtx;
class Board;
class UCT;
struct Node {
	PositionIndex pos;
	Count num;
	Value val, bon;
	Node *son, *bro;
	std::vector<Value> son_val_a, son_num_a;
	Node(PositionIndex po = -1, Node *so = nullptr, Node *br = nullptr) : pos(po), num(0), val(0), bon(0), son(so), bro(br) {}
	~Node() {
		if (son)
			delete son;
		if (bro)
			delete bro;
	}
};

struct Amaf {
	Amaf() {
		memset(data, EMPTY_POINT, sizeof(data));
	}
	PointState operator[](int idx) {
		return data[idx];
	}
	void set(PositionIndex pos, PointState state) {
		if (data[pos] == EMPTY_POINT){
			// printf("%d %d\n", pos, state);
			data[pos] = state;
		}
	}
	PointState data[170];
};

class MC {
public:
	MC() = default;
	~MC() = default;
	double Simulate(Board &board, PointState state, Amaf &amaf, TireTree &joseki, string &joseki_seq);
	double Evaluate(Board &Board, PointState state);
// private:
};

double MC::Simulate(Board &board, PointState state, Amaf &amaf, TireTree &joseki, string &joseki_seq) {

	PointState next_state = state;
	Move mv;
	int count = board.GetPieceCount(0) + board.GetPieceCount(1);
	bool joseki_valid = (step_count <= JOSEKI_STEP);
	// printf("step_count: %d\n", step_count);
	int playable_count;
	std::vector<PositionIndex> playable_pos;
	while (count < 200/*!playable_pos.empty()*/) {
		++count;
		mv.state = next_state;
		mv.position = POSITION_PASS;
		if(joseki_valid) {
			mv.position = joseki.findBest(joseki_seq);
			// printf("The best found is: %d\n", mv.position);
			joseki.updateSeq(joseki_seq, mv.position);
			if(mv.position == POSITION_PASS) {
				joseki_valid = false;
				// printf("joseki_valid: %d\n", joseki_valid);
			}
		} 
		if(mv.position == POSITION_PASS) {
			PositionIndex last_eat, best_eat, last_safe, best_safe;
			last_eat = last_safe = best_safe = best_eat = POSITION_PASS;
			board.GetSafePoint(last_safe, best_safe, next_state);
			board.GetEatPoint(last_eat, best_eat, next_state);
			mv.position = last_eat;
			if(mv.position == POSITION_PASS)
				mv.position = last_safe;
			if(mv.position == POSITION_PASS)
				mv.position = best_safe;
			if(mv.position == POSITION_PASS)
				mv.position = board.GetMogoPattern(next_state);
			if(mv.position == POSITION_PASS)
				mv.position = best_eat;

		}
		if (mv.position == POSITION_PASS) {
			playable_count = board.GetPlayableCount(next_state);
			if (playable_count == 0) {
				break;
			}
			mv.position = board.GetPlayable(mv.state, rand() * playable_count / (RAND_MAX + 1));
		}
		board.PlayMove(mv);
		amaf.set(mv.position, mv.state);
		next_state ^= 1;
	}
	// fout << ')';
	// fout.close();
	// fout2 << count << ' ';
	// fout2.close();
	return Evaluate(board, state ^ 1);
}

double MC::Evaluate(Board &board, PointState state) {
	double piece_count[2];
	piece_count[WHITE_POINT] = board.GetAreaCount(WHITE_POINT) + komi;
	piece_count[BLACK_POINT] = board.GetAreaCount(BLACK_POINT);
	return piece_count[state] > piece_count[state ^ 1];
}

#endif
