#ifndef UNIPOLAR_BOARD_H_
#define UNIPOLAR_BOARD_H_
// #include <intrin.h>
#include <cstring>
#include <set>
#include <iostream>
#include <cstdio>

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
		reset();
	};
	~BitSet() = default;
	void set() {
		data_[0] = data_[1] = data_[2] = 0xffffffffffffffff;
	}
	void set(int pos) {
		data_[pos >> 6] |= (uint64_t)1 << (pos & 63);
	}
	void reset() {
		data_[0] = data_[1] = data_[2] = 0;
	}
	void reset(int pos) {
		data_[pos >> 6] &= ~((uint64_t)1 << (pos & 63));
	}
	bool operator[](int pos) {
		return data_[pos >> 6] & ((uint64_t)1 << (pos & 63));
	}
	int count() {
		return __builtin_popcountll(data_[0]) + __builtin_popcountll(data_[1]) + __builtin_popcountll(data_[2]);
	}
	//should only be called when a chain has only one air!!!
	PositionIndex GetAirPos() {
		uint64_t tmp1 = data_[0] | data_[1];
		uint64_t tmp2 = tmp1 | data_[2];
		return (__builtin_popcountll(~data_[0]) & 64) + (__builtin_popcountll(~tmp1) & 64) + __builtin_ctzll(tmp2);
	}
	void merge(const BitSet &bitset2) {
		data_[0] |= bitset2.data_[0];
		data_[1] |= bitset2.data_[1];
		data_[2] |= bitset2.data_[2];
	}
	void Print() {
		// printf("%x %x %x\n", data_[0], data_[1], data_[2]);
		for (int i = 0; i < 169; ++i)
			if ((*this)[i])
				printf("%d ", i);
		printf("\n");
	}
private:
	uint64_t data_[3];
};

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
	std::set<PositionIndex> GetPlayablePosition(PointState state) {
		return playable_pos_[state];
	}
	PositionIndex GetPieceCount(PointState state) {
		return piece_count_[state];
	}
	friend class MC;
	// DISALLOW_COPY_AND_ASSIGN_AND_MOVE(Board)
	void Print() {
		printf("   0  1  2  3  4  5  6  7  8  9  10 11 12\n");
		for (int i = 0 ; i < BOARD_SIZE; ++i) {
			if (i < 10)
				printf("%d  ", i);
			else
				printf("%d ", i);
			for (int j = 0; j < BOARD_SIZE; ++j)
				if (board_[i * 13 + j].state == EMPTY_POINT)
					printf(".  ");
				else if (board_[i * 13 + j].state == BLACK_POINT)
					printf("X  ");
				else if (board_[i * 13 + j].state == WHITE_POINT)
					printf("O  ");
			printf("\n");
		}
		printf("   0  1  2  3  4  5  6  7  8  9  10 11 12\n");
		for (int i = 0 ; i < BOARD_SIZE; ++i) {
			if (i < 10)
				printf("%d  ", i);
			else
				printf("%d ", i);
			for (int j = 0; j < BOARD_SIZE; ++j)
					printf("%c  ", board_[i*13+j].father < 0 ? (board_[i*13+j].air_count + '0') : 'X');
			printf("\n");
		}
		printf("BLACK playable_pos_:\n");
		for (std::set<PositionIndex>::iterator it = playable_pos_[BLACK_POINT].begin(); it != playable_pos_[BLACK_POINT].end(); ++it)
			printf("%d ", *it);
		printf("\n");
		printf("BLACK playable_bit_:\n");
		playable_bit_[BLACK_POINT].Print();
		printf("\n");
		printf("WHITE playable_pos_:\n");
		for (std::set<PositionIndex>::iterator it = playable_pos_[WHITE_POINT].begin(); it != playable_pos_[WHITE_POINT].end(); ++it)
			printf("%d ", *it);
		printf("\n");
		printf("WHITE playable_bit_:\n");
		playable_bit_[WHITE_POINT].Print();
		printf("\n");
		printf("Chain_set:\n");
		for (int i = 0; i < BoardSizeSquare(BOARD_SIZE); ++i) {
			if (board_[i].father < 0 && board_[i].state != EMPTY_POINT) {
				printf("state:%d, ", board_[i].state);
				printf("air_count:%d, air_set:", board_[i].air_count);
				board_[i].air_set.Print();
				printf(", points:[");
				for (PositionIndex j = i, last_j = -1; j != last_j; last_j = j, j = board_[j].next)
					printf("%d, ", j);
				printf("]\n");
			}
		}
		printf("\n");
		printf("Ko:%d\n", ko_);
		printf("BLACK piece count%d\n", piece_count_[BLACK_POINT]);
		printf("WHITE piece count%d\n", piece_count_[WHITE_POINT]);
	}

private:
	PositionIndex GetFather(PositionIndex pos);
	void Merge(PositionIndex pos1, PositionIndex pos2);
	void ToEmpty(PositionIndex pos);
	void FromEmpty(PositionIndex pos, PointState state);
	void RemoveChain(PositionIndex pos);
	PositionIndex AdjacentPosition(PositionIndex position, int adj_direction);
	void CheckSuisidePoint(PositionIndex pos);

	void Able(PositionIndex pos, PointState state) {
		printf("able %d %d\n", pos, state);
		if (!playable_bit_[state][pos]) {
			playable_pos_[state].insert(pos);
			playable_bit_[state].set(pos);
		}
	}
	void DisAble(PositionIndex pos, PointState state) {
		printf("disable %d %d\n", pos, state);
		if (playable_bit_[state][pos]) {
			playable_pos_[state].erase(pos);
			playable_bit_[state].reset(pos);
		}
	}

	List board_[BoardSizeSquare(BOARD_SIZE)];
	std::set<PositionIndex> playable_pos_[2]/*, suiside_pos[2]*/;
	BitSet playable_bit_[2];
	PositionIndex piece_count_[2];
	PositionIndex ko_;
	static PositionIndex ADJ_POS_[BoardSizeSquare(BOARD_SIZE)][5];
};

PositionIndex Board::ADJ_POS_[BoardSizeSquare(BOARD_SIZE)][5];


void Board::Init() {
	memset(ADJ_POS_, 0, sizeof(ADJ_POS_));
	for (int i = 0; i < BoardSizeSquare(BOARD_SIZE); ++i) {
		if (i >= BOARD_SIZE)
			ADJ_POS_[i][++ADJ_POS_[i][0]] = i - BOARD_SIZE;
		if (i % BOARD_SIZE != 0)
			ADJ_POS_[i][++ADJ_POS_[i][0]] = i - 1;
		if ((i + 1) % BOARD_SIZE != 0)
			ADJ_POS_[i][++ADJ_POS_[i][0]] = i + 1;
		if (i + BOARD_SIZE < BoardSizeSquare(BOARD_SIZE))
			ADJ_POS_[i][++ADJ_POS_[i][0]] = i + BOARD_SIZE;
	}
}

void Board::ClearBoard() {
	playable_bit_[0].set();
	playable_bit_[1].set();
	for (PositionIndex i = 0; i < BoardSizeSquare(BOARD_SIZE); ++i) {
		playable_pos_[0].insert(i);
		playable_pos_[1].insert(i);
		board_[i].father = -1;
		board_[i].state = EMPTY_POINT;
		board_[i].air_set.reset();
		for(int j = 1; j <= ADJ_POS_[i][0]; ++j)
			board_[i].air_set.set(ADJ_POS_[i][j]);
		board_[i].air_count = board_[i].air_set.count();
	}
	piece_count_[0] = piece_count_[1] = 0;
	ko_ = -1;
}

void Board::PlayMove(const Move &move) {
	if (ko_ >= 0) {
		CheckSuisidePoint(ko_);
		// CheckEyeShape(ko_);
		ko_ = -1;
	}
	if (move.position == POSITION_PASS)
		return;
	if (board_[move.position].state != EMPTY_POINT) {
		Print();
		printf("pos %d has piece\n", move.position);
		exit(0);
	}
	if (move.position == ko_) {
		Print();
		printf("pos %d is ko\n", move.position);
		exit(0);
	}
	++piece_count_[move.state];
	FromEmpty(move.position, move.state);
	PositionIndex pos = move.position;
	PositionIndex adj_chain;
	for (int i = 1; i <= ADJ_POS_[pos][0]; ++i) {
		adj_chain = GetFather(ADJ_POS_[pos][i]);
		board_[adj_chain].air_set.reset(pos);
		board_[adj_chain].air_count = board_[adj_chain].air_set.count();
		if (board_[adj_chain].state == board_[pos].state) {
			printf("merge: %d %d\n", adj_chain, pos);
			Merge(adj_chain, pos);
		}
	}
	for (int i = 1; i <= ADJ_POS_[pos][0]; ++i) {
		adj_chain = GetFather(ADJ_POS_[pos][i]);
		if (board_[adj_chain].state != move.state) {
			if (board_[adj_chain].air_count == 0) {
				if (board_[adj_chain].state == EMPTY_POINT)
					CheckSuisidePoint(adj_chain);
				else
					RemoveChain(adj_chain);
			}
			else if (board_[adj_chain].air_count == 1 && board_[adj_chain].state != EMPTY_POINT)
				CheckSuisidePoint(board_[adj_chain].air_set.GetAirPos());
		}
	}
	pos = GetFather(pos);
	if (board_[pos].air_count == 0) {
		Print();
		printf("suiside at pos %d\n", pos);
		exit(0);
		RemoveChain(pos);
	}
	else if (board_[pos].air_count == 1)
		CheckSuisidePoint(board_[pos].air_set.GetAirPos());
	if(board_[pos].father != -1)
		ko_ = -1;
	if (ko_ >= 0)
		DisAble(ko_, move.state ^ 1);
}

PositionIndex Board::GetFather(PositionIndex pos) {
	PositionIndex result;
	for (result = pos; board_[result].father >= 0; result = board_[result].father);
	for (int i = pos, tmp; board_[i].father >= 0;) {
		tmp = i;
		i = board_[i].father;
		board_[tmp].father = result;
	}
	return result;
}

void Board::Merge(PositionIndex pos1, PositionIndex pos2) {
	PositionIndex father1 = GetFather(pos1), father2 = GetFather(pos2);

	//make sure tree1 is greater.
	if (board_[father1].father > board_[father2].father)
		Swap(father1, father2);
	board_[father1].father += board_[father2].father;
	board_[father2].father = father1;

	board_[board_[father1].tail].next = father2;
	board_[father1].tail = board_[father2].tail;

	board_[father1].air_set.merge(board_[father2].air_set);
	board_[father1].air_count = board_[father1].air_set.count();
}

void Board::ToEmpty(PositionIndex pos) {
	board_[pos].father = -1;
	board_[pos].air_count = 0;
	board_[pos].air_set.reset();
	board_[pos].state = EMPTY_POINT;
	Able(pos, 0);
	Able(pos, 1);
}

void Board::FromEmpty(PositionIndex pos, PointState state) {
	board_[pos].next = board_[pos].tail = pos;
	board_[pos].state = state;
	DisAble(pos, 0);
	DisAble(pos, 1);
}

void Board::RemoveChain(PositionIndex pos) {
	PositionIndex father = GetFather(pos);
	piece_count_[board_[father].state] += board_[father].father;
	if (board_[father].state == EMPTY_POINT) {
		Print();
		printf("remove empty piece at pos %d\n", pos);
		exit(0);
	}
	//turn to_remove pieces to empty piece and set air_count to 0.
	for (PositionIndex i = father, last_i = -1; i != last_i; last_i = i, i = board_[i].next)
		ToEmpty(i);
	PositionIndex adj_chain;
	//check not only on air chain
	for (PositionIndex i = father, last_i = -1; i != last_i; last_i = i, i = board_[i].next) {
		for (int j = 1; j <= ADJ_POS_[i][0]; ++j) {
			adj_chain = GetFather(ADJ_POS_[i][j]);
			if (board_[adj_chain].air_count == 1){
				++board_[adj_chain].air_count;
				CheckSuisidePoint(board_[adj_chain].air_set.GetAirPos());
			}
		}
	}
	//add air for adjacent chain.
	for (PositionIndex i = father, last_i = -1; i != last_i; last_i = i, i = board_[i].next) {
		for (int j = 1; j <= ADJ_POS_[i][0]; ++j) {
			adj_chain = GetFather(ADJ_POS_[i][j]);
			board_[adj_chain].air_set.set(i);
			board_[adj_chain].air_count = board_[adj_chain].air_set.count();
		}
	}
	if (board_[father].air_count == 0){
		ko_ = father;
		CheckSuisidePoint(father);
	}
}


void Board::CheckSuisidePoint(PositionIndex pos) {
	printf("check suiside of %d\n", pos);
	if(board_[pos].state != EMPTY_POINT){
		Print();
		printf("check suiside point %d for exist piece\n", pos);
		exit(0);
	}
	if (board_[pos].air_count > 0) {
		Able(pos, BLACK_POINT);
		Able(pos, WHITE_POINT);
		return;
	}
	bool air[2][2] = {}; //[state][>1?]

	PositionIndex adj_pos;
	for (int i = 1; i <= ADJ_POS_[pos][0]; ++i) {
		adj_pos = ADJ_POS_[pos][i];
		air[board_[adj_pos].state][board_[adj_pos].air_count > 1] = true;
	}
	//white eat black or white connect to air
	printf("%d %d %d %d\n", air[0][0], air[0][1], air[1][0], air[1][1]);
	if (air[BLACK_POINT][0] | air[WHITE_POINT][1])
		Able(pos, WHITE_POINT);
	else
		DisAble(pos, WHITE_POINT);

	//black eat white or black connect to air
	if (air[WHITE_POINT][0] | air[BLACK_POINT][1])
		Able(pos, BLACK_POINT);
	else
		DisAble(pos, BLACK_POINT);
}

#endif
