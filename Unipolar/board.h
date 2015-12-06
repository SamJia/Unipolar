#ifndef UNIPOLAR_BOARD_H_
#define UNIPOLAR_BOARD_H_
// #include <intrin.h>
#include <cstring>
#include <set>
#include <iostream>

#include "def.h"
using namespace unipolar;

struct Move {
	PointState state;
	PositionIndex position;
	Move(PointState sta = EMPTY_POINT, PositionIndex pos = POSITION_PASS) : state(sta), position(pos) {}
};

class BitSet
{
public:
	BitSet() {
		Reset();
	};
	~BitSet() = default;
	void Set() {
		data_[0] = data_[1] = data_[2] = 0xffffffffffffffff;
	}
	void Set(int pos) {
		data_[pos >> 6] |= 1 << (pos & 63);
	}
	void Reset() {
		data_[0] = data_[1] = data_[2] = 0;
	}
	void Reset(int pos) {
		data_[pos >> 6] &= ~(1 << (pos & 63));
	}
	bool operator[](int pos) {
		return data_[pos >> 6] & (1 << (pos & 63));
	}
	int Count() {
		return __builtin_popcountll(data_[0]) + __builtin_popcountll(data_[1]) + __builtin_popcountll(data_[2]);
	}
	//should only be called when a chain has only one air!!!
	PositionIndex GetAirPos() {
		uint64_t tmp1 = data_[0] | data_[1];
		uint64_t tmp2 = tmp1 | data_[2];
		return ((__builtin_popcountll(~data_[0]) + __builtin_popcountll(~tmp1)) & 192) + __builtin_ctzll(tmp2);
	}
	void Merge(const BitSet &bitset2) {
		data_[0] |= bitset2.data_[0];
		data_[1] |= bitset2.data_[1];
		data_[2] |= bitset2.data_[2];
	}
private:
	uint64_t data_[3];
};

template<int BOARD_SIZE>
class Board {
private:
	struct List { //list dimension, disjoint set dimension, air_set dimension and state dimension.
		PositionIndex father, next, tail;
		AirCount air_count;
		BitSet air_set;
		PointState state;
		List() : state(EMPTY_POINT) {}
	};
public:
	Board() = default;
	~Board() = default;
	static void Init();
	void ClearBoard();
	void PlayMove(const Move &move);
	std::set<PositionIndex> GetPlayablePosition(PointState state);
	double Evaluate(double (*evaluate_function)(List board[BoardSizeSquare(BOARD_SIZE)]));
	// DISALLOW_COPY_AND_ASSIGN_AND_MOVE(Board)

private:
	PositionIndex GetFather(PositionIndex pos);
	void Merge(PositionIndex pos1, PositionIndex pos2);
	void InitEmptyChain(PositionIndex pos);
	void InitChain(PositionIndex pos, PointState state);
	void RemoveChain(PositionIndex pos);
	PositionIndex AdjacentPosition(PositionIndex position, int adj_direction);
	void CheckEyeShape(PositionIndex pos);
	void CheckOnlyOneAir(PositionIndex pos, PointState state);
	void CheckNotOnlyOneAir(PositionIndex pos, PointState state);


	List board_[BoardSizeSquare(BOARD_SIZE)];
	std::set<PositionIndex> playable_pos[2]/*, suiside_pos[2]*/;
	BitSet playable_bit[2];
	static PositionIndex ADJ_POS[BoardSizeSquare(BOARD_SIZE)][5];
};

template<int BOARD_SIZE>
void Board<BOARD_SIZE>::Init() {
	memset(ADJ_POS, 0, sizeof(ADJ_POS));
	for (int i = 0; i < BoardSizeSquare(BOARD_SIZE); ++i) {
		if (i >= BOARD_SIZE)
			ADJ_POS[i][++ADJ_POS[i][0]] = i - BOARD_SIZE;
		if (i % BOARD_SIZE != 0)
			ADJ_POS[i][++ADJ_POS[i][0]] = i - 1;
		if ((i + 1) % BOARD_SIZE != 0)
			ADJ_POS[i][++ADJ_POS[i][0]] = i + 1;
		if (i + BOARD_SIZE < BoardSizeSquare(BOARD_SIZE))
			ADJ_POS[i][++ADJ_POS[i][0]] = i + BOARD_SIZE;
	}
}

template<int BOARD_SIZE>
void Board<BOARD_SIZE>::ClearBoard() {
	playable_pos[0].clear();
	playable_pos[1].clear();
	playable_bit[0].Set();
	playable_bit[1].Set();
	// suiside_pos[0].clear();
	// suiside_pos[1].clear();
	for (PositionIndex i = 0; i < BoardSizeSquare(BOARD_SIZE); ++i) {
		playable_pos[0].insert(i);
		playable_pos[1].insert(i);
		board_[i].state = EMPTY_POINT;
	}
	for (PositionIndex i = 0; i < BoardSizeSquare(BOARD_SIZE); ++i)
		InitEmptyChain(i);
}

template<int BOARD_SIZE>
void Board<BOARD_SIZE>::PlayMove(const Move &move) {
	if (move.position == POSITION_PASS)
		return;
	InitChain(move.position, move.state);
	PositionIndex pos = move.position;
	PositionIndex adj_pos;
	for (int i = 1; i <= ADJ_POS[pos][0]; ++i) {
		adj_pos = ADJ_POS[pos][i];
		if (board_[adj_pos].state == board_[pos].state)
			Merge(adj_pos, pos);
		else {
			board_[adj_pos].air_set.Reset(pos);
			board_[adj_pos].air_count = board_[adj_pos].air_set.Count();
			if (board_[adj_pos].air_count == 0) {
				if (board_[adj_pos].state == EMPTY_POINT) {
					CheckEyeShape(adj_pos);
				}
				else
					RemoveChain(adj_pos);
			}
			else if (board_[adj_pos].air_count == 1) {
				CheckOnlyOneAir(board_[adj_pos].air_set.GetAirPos(), board_[adj_pos].state);
			}
		}
	}
	if (board_[pos].air_count == 0)
		RemoveChain(pos);
	else if (board_[pos].air_count == 1)
		CheckOnlyOneAir(pos, move.state);
}

template<int BOARD_SIZE>
std::set<PositionIndex> Board<BOARD_SIZE>::GetPlayablePosition(PointState state) {
	return playable_pos[state];
}

template<int BOARD_SIZE>
double Board<BOARD_SIZE>::Evaluate(double (*evaluate_function)(List board[BoardSizeSquare(BOARD_SIZE)])) {
	return evaluate_function(board_);
}


template<int BOARD_SIZE>
PositionIndex Board<BOARD_SIZE>::GetFather(PositionIndex pos) {
	PositionIndex result;
	for (result = pos; board_[result].father >= 0; result = board_[result].father);
	for (int i = pos, tmp; board_[i].father >= 0;) {
		tmp = i;
		i = board_[i].father;
		board_[tmp].father = result;
	}
	return result;
}

template<int BOARD_SIZE>
void Board<BOARD_SIZE>::Merge(PositionIndex pos1, PositionIndex pos2) {
	PositionIndex father1 = GetFather(pos1), father2 = GetFather(pos2);

	//make sure tree1 is greater.
	if (board_[pos1].father > board_[father2].father)
		Swap(father1, father2);
	board_[father1].father += board_[father2].father;
	board_[father2].father = father1;

	board_[board_[father1].tail].next = father2;
	board_[father1].tail = board_[father2].tail;

	board_[father1].air_set.Merge(board_[father2].air_set);
	board_[father1].air_count = board_[father1].air_set.Count();
}

template<int BOARD_SIZE>
void Board<BOARD_SIZE>::InitEmptyChain(PositionIndex pos) {
	board_[pos].state = EMPTY_POINT;
	board_[pos].air_set.Reset();
	PositionIndex adj_pos;
	for (int i = 1; i <= ADJ_POS[pos][0]; ++i) {
		adj_pos = ADJ_POS[pos][i];
		if (adj_pos >= 0 && board_[adj_pos].state == EMPTY_POINT)
			board_[pos].air_set.Set(adj_pos);
	}
	board_[pos].air_count = board_[pos].air_set.Count();
}


template<int BOARD_SIZE>
void Board<BOARD_SIZE>::InitChain(PositionIndex pos, PointState state) {
	InitEmptyChain(pos);
	board_[pos].father = -1;
	board_[pos].next = board_[pos].tail = pos;
	board_[pos].state = state;
}

template<int BOARD_SIZE>
void Board<BOARD_SIZE>::RemoveChain(PositionIndex pos) {
	PositionIndex father = GetFather(pos);
	for (PositionIndex i = father, tail = board_[father].tail; i != tail; i = board_[i].next)
		board_[i].state = EMPTY_POINT;
	for (PositionIndex i = father, tail = board_[father].tail; i != tail; i = board_[i].next) {
		PositionIndex adj_pos;
		for (int i = 1; i <= ADJ_POS[pos][0]; ++i) {
			adj_pos = ADJ_POS[pos][i];
			board_[adj_pos].air_set.set(pos);
			board_[adj_pos].air_count = board_[adj_pos].air_set.Count();
			if (board_[adj_pos].air_count == 2 && board_[adj_pos].state != EMPTY_POINT)
				CheckNotOnlyOneAir(adj_pos, board_[adj_pos].state);
		}
	}
}

template<int BOARD_SIZE>
void Board<BOARD_SIZE>::CheckEyeShape(PositionIndex pos) {
	PointState state = board_[ADJ_POS[pos][1]].state;
	for (int i = 1; i <= ADJ_POS[pos][0]; ++i)
		if (board_[ADJ_POS[pos][i]].state != state || board_[ADJ_POS[pos][i]].air_count == 1)
			return;
	playable_pos[state ^ 1].erase(pos);
	playable_bit[state ^ 1].Reset(pos);
	// suiside_pos[state ^ 1].insert(pos);
}

template<int BOARD_SIZE>
void Board<BOARD_SIZE>::CheckOnlyOneAir(PositionIndex pos, PointState state) {
	if (board_[pos].air_count > 0)
		return;
	PointState op_state = state ^ 1;
	// if (suiside_pos[op_state].find(pos) != suiside_pos[op_state].end()) {
	// 	suiside_pos[op_state].erase(pos);
	// 	playable_pos[op_state].insert(pos);
	// }
	if (!playable_bit[op_state][pos]) {
		playable_pos[op_state].insert(pos);
		playable_bit[op_state].Set(pos);
	}
	for (int i = 1; i <= ADJ_POS[pos][0]; ++i )
		if (board_[ADJ_POS[pos][i]].state == state && board_[ADJ_POS[pos][i]].air_count > 1)
			return;
	playable_pos[state].erase(pos);
	playable_bit[state].Reset(pos);
	// suiside_pos[state].insert(pos);
}

template<int BOARD_SIZE>
void Board<BOARD_SIZE>::CheckNotOnlyOneAir(PositionIndex pos, PointState state) {
	if (board_[pos].air_count > 0)
		return;
	// if (suiside_pos[state].find(pos) != suiside_pos[state].end()) {
	// 	suiside_pos[state].erase(pos);
	// 	playable_pos[state].insert(pos);
	// }
	if (!playable_bit[state][pos]) {
		playable_pos[state].insert(pos);
		playable_bit[state].Set(pos);
	}
	for (int i = 1; i <= ADJ_POS[pos][0]; ++i )
		if (board_[ADJ_POS[pos][i]].state != state && board_[ADJ_POS[pos][i]].air_count > 1
		        || board_[ADJ_POS[pos][i]].state == state && board_[ADJ_POS[pos][i]].air_count == 1)
			return;
	playable_pos[state ^ 1].erase(pos);
	playable_bit[state ^ 1].Reset(pos);
	// suiside_pos[state ^ 1].insert(pos);
}


#endif