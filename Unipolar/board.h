#ifndef UNIPOLAR_BOARD_H_
#define UNIPOLAR_BOARD_H_
#ifdef _MSC_VER
#include <intrin.h>
#include <immintrin.h>
#define __builtin_popcountll __popcnt64
#define __builtin_ctzll _tzcnt_u64
#endif
// #include <intrin.h>
#include <cstring>
#include <set>
#include <vector>
#include <iostream>
#include <cstdio>

#include "def.h"
using namespace unipolar;

struct Move {
	PointState state;
	PositionIndex position;
	Move(PointState sta = EMPTY_POINT, PositionIndex pos = POSITION_PASS) : state(sta), position(pos) {}
};

class Board;

class BitSet
{
public:
	BitSet() {
		reset();
	};
	~BitSet() = default;
	void set() {
		data_[0] = data_[1] = 0xffffffffffffffff;
		data_[2] = 0x1ffffffffff;
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
	BitSet &operator=(const BitSet &bitset2) {
		data_[0] = bitset2.data_[0];
		data_[1] = bitset2.data_[1];
		data_[2] = bitset2.data_[2];
		return (*this);
	}
	BitSet operator-(const BitSet &bitset2) const {
		BitSet result;
		result.data_[0] = data_[0] & (data_[0] ^ bitset2.data_[0]);
		result.data_[1] = data_[1] & (data_[1] ^ bitset2.data_[1]);
		result.data_[2] = data_[2] & (data_[2] ^ bitset2.data_[2]);
		return result;
	}
	BitSet &operator-=(const BitSet &bitset2) {
		data_[0] &= data_[0] ^ bitset2.data_[0];
		data_[1] &= data_[1] ^ bitset2.data_[1];
		data_[2] &= data_[2] ^ bitset2.data_[2];
		return (*this);
	}
	BitSet operator+(const BitSet &bitset2) const {
		BitSet result;
		result.data_[0] = data_[0] | bitset2.data_[0];
		result.data_[1] = data_[1] | bitset2.data_[1];
		result.data_[2] = data_[2] | bitset2.data_[2];
		return result;
	}
	BitSet &operator+=(const BitSet &bitset2) {
		data_[0] |= bitset2.data_[0];
		data_[1] |= bitset2.data_[1];
		data_[2] |= bitset2.data_[2];
		return (*this);
	}
	BitSet operator*(const BitSet &bitset2) const {
		BitSet result;
		result.data_[0] = data_[0] & bitset2.data_[0];
		result.data_[1] = data_[1] & bitset2.data_[1];
		result.data_[2] = data_[2] & bitset2.data_[2];
		return result;
	}
	BitSet &operator*=(const BitSet &bitset2) {
		data_[0] &= bitset2.data_[0];
		data_[1] &= bitset2.data_[1];
		data_[2] &= bitset2.data_[2];
		return (*this);
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
		for (int i = 0; i < BoardSizeSquare(BOARD_SIZE); ++i)
			if ((*this)[i])
				printf("%d ", i);
		printf("\n");
	}
	void PrintBinary() {
		// printf("%x %x %x\n", data_[0], data_[1], data_[2]);
		for (int i = 0; i < BoardSizeSquare(BOARD_SIZE); ++i)
			printf("%d", (bool)(*this)[i]);
		printf("\n");
	}
	friend class Board;
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
	friend class MC;

	List board_[BoardSizeSquare(BOARD_SIZE)];
	BitSet empty_[2], eye_[2], safe_eye_[2], dangerous_[2], suiside_[2], dangerous_empty_[2];
	PositionIndex piece_count_[2];
	PositionIndex ko_, last_move, last_atari[2];
	static PositionIndex ADJ_POS_[BoardSizeSquare(BOARD_SIZE)][5];
	double score;
	bool mc_;
	int delta[8] = {-1-BOARD_SIZE,0-BOARD_SIZE,1-BOARD_SIZE,1,1+BOARD_SIZE,0+BOARD_SIZE,-1+BOARD_SIZE,-1};


	Board() = default;
	~Board() = default;
	static void Init();
	void ClearBoard();
	double PlayMove(const Move &move);
	std::vector<PositionIndex> GetPlayablePosition(PointState state) {
		int base = 0;
		BitSet tmp = empty_[state] - suiside_[state] - safe_eye_[state] - dangerous_empty_[state];
		std::vector<PositionIndex> playable_pos;
		playable_pos.reserve(tmp.count());
		for (int j = 0; j < 3; ++j) {
			while (tmp.data_[j]) {
				playable_pos.push_back(base + __builtin_ctzll(tmp.data_[j]));
				tmp.data_[j] &= tmp.data_[j] - 1;
			}
			base += 64;
		}
		return playable_pos;
	}
	int GetPlayableCount(PointState state) {
		return (empty_[state] - suiside_[state] - safe_eye_[state]).count();
	}
	PositionIndex GetPlayable(PointState state, int number) {
		// printf("number:%d\n", number);
		int base = 0;
		BitSet tmp = empty_[state] - suiside_[state] - safe_eye_[state];
		for (int j = 0; j < 3; ++j) {
			if (__builtin_popcountll(tmp.data_[j]) <= number)
				number -= __builtin_popcountll(tmp.data_[j]);
			else {
				for (; number; --number) {
					tmp.data_[j] &= tmp.data_[j] - 1;
				}
				return __builtin_ctzll(tmp.data_[j]) + base;
			}
			base += 64;
		}
		return POSITION_PASS;
	}
	void GetSafePoint(PositionIndex &last_safe, PositionIndex &best_save, PointState state);
	void GetEatPoint(PositionIndex &last_eat, PositionIndex &best_eat, PointState state);
	void PrintVector(std::vector<PositionIndex> v) {
		for (auto i : v)
			printf("%d ", i);
		printf("\n");
	}
	PositionIndex GetPieceCount(PointState state) {
		return piece_count_[state];
	}
	PositionIndex GetAreaCount(PointState state) {
		return piece_count_[state] + eye_[state].count();
	}
	bool Playable(PositionIndex pos, PointState state) {
		return (empty_[state] - suiside_[state])[pos];
	}

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
	}

	PositionIndex GetFather(PositionIndex pos);
	void Merge(PositionIndex pos1, PositionIndex pos2);
	void ToEmpty(PositionIndex pos);
	void FromEmpty(PositionIndex pos, PointState state);
	void RemoveChain(PositionIndex pos);
	PositionIndex AdjacentPosition(PositionIndex position, int adj_direction);
	void CheckSpecialPoint(PositionIndex pos, bool add_score = false);

	void SetEye(PositionIndex pos, PointState state) {
		eye_[state].set(pos);
	}
	void RemoveEye(PositionIndex pos, PointState state) {
		eye_[state].reset(pos);
	}
	void SetSafeEye(PositionIndex pos, PointState state) {
		safe_eye_[state].set(pos);
	}
	void RemoveSafeEye(PositionIndex pos, PointState state) {
		safe_eye_[state].reset(pos);
	}
	void SetDangerous(PositionIndex pos, PointState state) {
		dangerous_[state].set(pos);
	}
	void RemoveDangerous(PositionIndex pos, PointState state) {
		dangerous_[state].reset(pos);
	}
	void SetSuiside(PositionIndex pos, PointState state) {
		suiside_[state].set(pos);
	}
	void RemoveSuiside(PositionIndex pos, PointState state) {
		suiside_[state].reset(pos);
	}
	void SetEmpty(PositionIndex pos, PointState state) {
		empty_[state].set(pos);
	}
	void RemoveEmpty(PositionIndex pos, PointState state) {
		empty_[state].reset(pos);
	}
	void SetDangerousEmpty(PositionIndex pos, PointState state) {
		dangerous_empty_[state].set(pos);
	}
	void RemoveDangerousEmpty(PositionIndex pos, PointState state) {
		dangerous_empty_[state].reset(pos);
	}

	bool OnBoard(PositionIndex pos) {
		return (pos >= 0 && pos < BoardSizeSquare(BOARD_SIZE));
	}


	// below are the pattern functions, implemented in pattern.cpp

	PositionIndex GetMogoPattern(PointState state);
	bool MatchCut(PositionIndex pos, PointState state);
	bool MatchHane(PositionIndex pos, PointState state);
	bool MatchBorder(PositionIndex pos, PointState state);
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
	// last_atari[0] = last_atari[1] = last_move = POSITION_PASS;

	// for (int i = 0; i < BoardSizeSquare(BOARD_SIZE); ++i) {
	// 	printf("{%d,%d,%d,%d,%d},\n", ADJ_POS_[i][0], ADJ_POS_[i][1], ADJ_POS_[i][2], ADJ_POS_[i][3], ADJ_POS_[i][4]);
	// }
}

void Board::ClearBoard() {
	empty_[0].set();
	empty_[1].set();
	eye_[0].reset();
	eye_[1].reset();
	safe_eye_[0].reset();
	safe_eye_[1].reset();
	dangerous_[0].reset();
	dangerous_[1].reset();
	suiside_[0].reset();
	suiside_[1].reset();
	for (PositionIndex i = 0; i < BoardSizeSquare(BOARD_SIZE); ++i) {
		board_[i].father = -1;
		board_[i].state = EMPTY_POINT;
		board_[i].air_set.reset();
		for (int j = 1; j <= ADJ_POS_[i][0]; ++j)
			board_[i].air_set.set(ADJ_POS_[i][j]);
		board_[i].air_count = board_[i].air_set.count();
	}
	piece_count_[0] = piece_count_[1] = 0;
	ko_ = -1;
	mc_ = false;
}

void Board::GetSafePoint(PositionIndex &last_safe, PositionIndex &best_save, PointState state) {
	BitSet tmp = (dangerous_[state] - suiside_[state]) * empty_[state];
	BitSet merged_air_set;
	double max_score = 0, score;
	
	last_safe = best_save = POSITION_PASS;
	PositionIndex pos;
	
	if (ko_ >= 0 && tmp[ko_]) {
		printf("ko_ occur in my dangerous\n");
		exit(0);
	}

	for (int i = 0, base = 0; i < 3; ++i) {
		while (tmp.data_[i]) {
			PositionIndex adj_chain[4], tmp_chain;
			int adj_count = 0;
			score = 1;
			
			pos = base + __builtin_ctzll(tmp.data_[i]);
			merged_air_set = board_[pos].air_set;
			tmp.data_[i] &= tmp.data_[i] - 1;
			// printf("check dangerous pos %d\n", pos);

			for (int j = 1; j <= ADJ_POS_[pos][0]; ++j) {
				if (board_[ADJ_POS_[pos][j]].state == state) {
					tmp_chain = GetFather(ADJ_POS_[pos][j]);
					bool flag = true;
					for (int k = 0; k < adj_count; ++k) {
						if (tmp_chain == adj_chain[k]) {
							flag = false;
						}
					}
					if (flag) {
						adj_chain[adj_count++] = tmp_chain;
						merged_air_set += board_[tmp_chain].air_set;
						score += -board_[tmp_chain].father;
					}
				}
			}
			// if the merged string still dangerous
			if (merged_air_set.count() - 1 <= 1)
				continue;
			if (score > max_score) {
				max_score = score;
				best_save = pos;
			}
			if (pos == last_atari[state])
				last_safe = pos;
		}
		base += 64;
	}
}
void Board::GetEatPoint(PositionIndex &last_eat, PositionIndex &best_eat, PointState state) {
	PositionIndex best_pos = POSITION_PASS, pos, adj_chain[4], tmp_chain;
	BitSet tmp = dangerous_[state ^ 1] * empty_[state];
	last_eat = best_eat = POSITION_PASS;
	// printf("test: %d,%d\n", last_atari[state^1]);
	double max_score = 0, score;
	if (ko_ >= 0 && tmp[ko_]) {
		printf("ko_ occur in oppose dangerous\n");
		exit(0);
	}

	for (int i = 0, base = 0; i < 3; ++i) {
		while (tmp.data_[i]) {
			int adj_count = 0;
			score = 0;
			pos = base + __builtin_ctzll(tmp.data_[i]);
			tmp.data_[i] &= tmp.data_[i] - 1;

			for (int j = 1; j <= ADJ_POS_[pos][0]; ++j) {
				if (board_[ADJ_POS_[pos][j]].state == (state ^ 1)) {
					tmp_chain = GetFather(ADJ_POS_[pos][j]);
					bool flag = true;
					for (int k = 0; k < adj_count; ++k) {
						if (tmp_chain == adj_chain[k]) {
							flag = false;
						}
					}
					if (flag) {
						adj_chain[adj_count++] = tmp_chain;
						score += -board_[tmp_chain].father;
					}					
				}
			}
			if (score > max_score) {
				max_score = score;
				best_eat = pos;
			}
			if (pos == last_atari[state^1])
				last_eat = pos;
		}
		base += 64;
	}
}

double Board::PlayMove(const Move &move) {
	// printf("play move %d %d\n", move.position, move.state);
	if (ko_ >= 0) {
		SetEmpty(ko_, move.state);
		CheckSpecialPoint(ko_);
		// CheckEyeShape(ko_);
		ko_ = -1;
	}
	if (move.position == POSITION_PASS)
		return 0;
	score = 0;
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
	PositionIndex v[4];
	int count = 0;
	double save_score = 0;
	if ((eye_[move.state] - safe_eye_[move.state])[move.position])
		score += save_dangerous_eye;
	for (int i = 1; i <= ADJ_POS_[pos][0]; ++i) {
		adj_chain = GetFather(ADJ_POS_[pos][i]);
		board_[adj_chain].air_set.reset(pos);
		board_[adj_chain].air_count = board_[adj_chain].air_set.count();
		// printf("adj_chain state : %d\n", board_[adj_chain].state);
		if (board_[adj_chain].state == board_[pos].state) {
			// printf("merge: %d %d\n", adj_chain, pos);
			// printf("merge %d %d\n", adj_chain, pos);
			PositionIndex father = GetFather(pos);
			if(adj_chain != father){
				if(board_[adj_chain].air_count == 1)
					score += -(board_[adj_chain].father * save_dangerous_chain);
				Merge(adj_chain, father);
			}
		}
		else {
			for (int j = 0; j < count; ++j)
				if (v[j] == adj_chain)
					continue;
			v[count++] = adj_chain;
		}
	}
	for (int i = 0; i < count; ++i) {
		if (board_[v[i]].air_count == 0) {
			if (board_[v[i]].state == EMPTY_POINT) {
				RemoveDangerousEmpty(v[i], 0);
				RemoveDangerousEmpty(v[i], 1);
				CheckSpecialPoint(v[i], true);
			}
			else {
				score += -(board_[v[i]].father * eat_chain);
				RemoveChain(v[i]);
			}
		}
		else if (board_[v[i]].air_count == 1) {
			if (board_[v[i]].state != EMPTY_POINT) {
				score += -(board_[v[i]].father * threaten_oppose);
				CheckSpecialPoint(board_[v[i]].air_set.GetAirPos());
				last_atari[board_[v[i]].state] = board_[v[i]].air_set.GetAirPos();
			}
			else
				CheckSpecialPoint(v[i]);
		}
	}
	// printf("for done\n");
	pos = GetFather(pos);
	if (board_[pos].air_count == 0) {
		Print();
		printf("suiside at pos %d\n", pos);
		exit(0);
		RemoveChain(pos);
	}
	else if (board_[pos].air_count == 1) {
		score += -(board_[pos].father * threaten_self);
		CheckSpecialPoint(board_[pos].air_set.GetAirPos());
		last_atari[board_[pos].state] = board_[pos].air_set.GetAirPos();
	}
	if (board_[pos].father != -1)
		ko_ = -1;
	if (ko_ >= 0)
		RemoveEmpty(ko_, move.state ^ 1);

	last_move = move.position;
	return score;
}

PositionIndex Board::GetFather(PositionIndex pos) {
	PositionIndex result;
	if (board_[pos].father < 0)
		return pos;
	else if (board_[board_[pos].father].father < 0)
		return board_[pos].father;
	for (result = board_[board_[pos].father].father; board_[result].father >= 0; result = board_[result].father);
	for (int i = pos, tmp; board_[i].father >= 0;) {
		tmp = i;
		i = board_[i].father;
		board_[tmp].father = result;
	}
	return result;
}

void Board::Merge(PositionIndex pos1, PositionIndex pos2) {
	// printf("merge %d %d\n", pos1, pos2);
	PositionIndex father1 = pos1, father2 = pos2;
	// PositionIndex father1 = GetFather(pos1), father2 = GetFather(pos2);
	// printf("merge father %d %d\n", father1, father2);
	if (father1 == father2) {
		// printf("father %d and %d are equal\n", father1, father2);
		return;
	}
	// make sure tree1 is greater.
	if (board_[father1].father > board_[father2].father)
		Swap(father1, father2);
	board_[father1].father += board_[father2].father;
	board_[father2].father = father1;
	// printf("new father %d %d\n", board_[father1].father, board_[father2].father);
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
	SetEmpty(pos, 0);
	SetEmpty(pos, 1);
	// RemoveEye(pos, 0);
	// RemoveEye(pos, 1);
}

void Board::FromEmpty(PositionIndex pos, PointState state) {
	board_[pos].next = board_[pos].tail = pos;
	board_[pos].state = state;
	RemoveEmpty(pos, 0);
	RemoveEmpty(pos, 1);
	RemoveEye(pos, 0);
	RemoveEye(pos, 1);
	RemoveSafeEye(pos, state);
	RemoveDangerous(pos, 0);
	RemoveDangerous(pos, 1);
	RemoveDangerousEmpty(pos, 0);
	RemoveDangerousEmpty(pos, 1);
}

void Board::RemoveChain(PositionIndex pos) {
	PositionIndex father = GetFather(pos);
	piece_count_[board_[father].state] += board_[father].father;
	if (board_[father].state == EMPTY_POINT) {
		Print();
		printf("remove empty piece at pos %d\n", pos);
		exit(0);
	}
	// printf("for 1 of remove\n");
	//turn to_remove pieces to empty piece and set air_count to 0.
	int remove_pos[BoardSizeSquare(BOARD_SIZE)][6];
	int count = 0;
	for (PositionIndex i = father, last_i = -1; i != last_i; last_i = i, i = board_[i].next) {
		remove_pos[count][0] = i;
		remove_pos[count++][1] = 1;
		ToEmpty(i);
	}
	PositionIndex adj_chain;
	//check not only on air chain
	// printf("for 2 of remove\n");
	for (int i = 0; i < count; ++i) {
		for (int j = 1; j <= ADJ_POS_[remove_pos[i][0]][0]; ++j) {
			adj_chain = GetFather(ADJ_POS_[remove_pos[i][0]][j]);
			remove_pos[i][++remove_pos[i][1]] = adj_chain;
			if (board_[adj_chain].air_count == 1) {
				++board_[adj_chain].air_count;
				score += -(board_[adj_chain].father * save_dangerous_chain);
				CheckSpecialPoint(board_[adj_chain].air_set.GetAirPos());
			}
		}
	}
	//add air for adjacent chain.
	// printf("for 3 of remove\n");
	for (int i = 0; i < count; ++i) {
		// for (int j = 1; j <= ADJ_POS_[remove_pos[i][0]][0]; ++j) {
		for (int j = 2; j <= remove_pos[i][1]; ++j) {
			// adj_chain = GetFather(ADJ_POS_[remove_pos[i][0]][j-1]);
			adj_chain = remove_pos[i][j];
			board_[adj_chain].air_set.set(remove_pos[i][0]);
			board_[adj_chain].air_count = board_[adj_chain].air_set.count();
		}
	}
	for (int i = 0; i < count; ++i) {
		if (board_[remove_pos[i][0]].air_count == 1)
			CheckSpecialPoint(remove_pos[i][0]);
	}
	if (board_[father].air_count == 0) {
		ko_ = father;
		CheckSpecialPoint(father, true);
	}
// Print();
}


void Board::CheckSpecialPoint(PositionIndex pos, bool add_score) {
	// printf("check suiside of %d\n", pos);
	if (board_[pos].state != EMPTY_POINT) {
		Print();
		printf("check suiside point %d for exist piece\n", pos);
		exit(0);
	}
	bool air[3][2] = {}; //[state][>1?]
	PositionIndex adj_chain;
	for (int i = 1; i <= ADJ_POS_[pos][0]; ++i) {
		adj_chain = GetFather(ADJ_POS_[pos][i]);
		air[board_[adj_chain].state][board_[adj_chain].air_count > 1] = true;
	}
	if (board_[pos].air_count > 0) {
		// printf("air_count at pos %d is %d, greater than zero\n", pos, board_[pos].air_count);
		if (board_[pos].air_count == 1) {
			if ((air[BLACK_POINT][0] | air[BLACK_POINT][1] | air[WHITE_POINT][0]) == 0)
				SetDangerousEmpty(pos, BLACK_POINT);
			else if ((air[WHITE_POINT][0] | air[WHITE_POINT][1] | air[BLACK_POINT][0]) == 0)
				SetDangerousEmpty(pos, WHITE_POINT);
		}
		air[0][1] = air[1][1] = true;
	}
	// printf("%d %d %d %d\n", air[0][0], air[0][1], air[1][0], air[1][1]);
	//white eat black or white connect to air
	if (air[BLACK_POINT][0] | air[WHITE_POINT][1]) {
		RemoveSuiside(pos, WHITE_POINT);
	}
	else {
		if ((air[WHITE_POINT][0] | air[WHITE_POINT][1]) == 0) {
			if (add_score)
				score += make_safe_eye;
			SetSafeEye(pos, BLACK_POINT);
		}
		SetSuiside(pos, WHITE_POINT);
	}
	//black eat white or black connect to air
	if (air[WHITE_POINT][0] | air[BLACK_POINT][1]) {
		RemoveSuiside(pos, BLACK_POINT);
	}
	else {
		if ((air[BLACK_POINT][0] | air[BLACK_POINT][1]) == 0) {
			if (add_score)
				score += make_safe_eye;
			SetSafeEye(pos, WHITE_POINT);
		}
		SetSuiside(pos, BLACK_POINT);
	}

	if ((air[WHITE_POINT][0] | air[WHITE_POINT][1]) == 0) {
		if (add_score)
			score += make_eye;
		SetEye(pos, BLACK_POINT);
	}
	if ((air[BLACK_POINT][0] | air[BLACK_POINT][1]) == 0) {
		if (add_score)
			score += make_eye;
		SetEye(pos, WHITE_POINT);
	}
	if (air[WHITE_POINT][0]) {
		SetDangerous(pos, WHITE_POINT);
		RemoveSafeEye(pos, WHITE_POINT);
	}
	else {
		RemoveDangerous(pos, WHITE_POINT);
	}
	if (air[BLACK_POINT][0]) {
		SetDangerous(pos, BLACK_POINT);
		RemoveSafeEye(pos, BLACK_POINT);
	}
	else {
		RemoveDangerous(pos, BLACK_POINT);
	}
}


// match situation like this:
// 		. o .
// 		x . x
// 		_ _ _
// suppose we are 'o', '_' means not opposite.
// can be rotated in clockwisely.
bool Board::MatchCut(PositionIndex pos, PointState my_state) {
	for(int dir = 0; dir < 8; dir += 2) {
		PositionIndex around[8];
		for(int i = 0; i < 8; ++i) {
			around[i] = pos + delta[(dir+i) % 8];
		}
		if(!OnBoard(around[1]) || !OnBoard(around[3]) || !OnBoard(around[5]) || !OnBoard(around[6]) || !OnBoard(around[7]))
			continue;
		if(board_[around[1]].state == EMPTY_POINT)
			continue;
		PointState state = board_[around[1]].state;
		if(board_[around[7]].state != 1-state)
			continue;
		if(board_[around[3]].state != 1-state)
			continue;
		if(board_[around[4]].state == 1-state)
			continue;
		if(board_[around[5]].state == 1-state)
			continue;
		if(board_[around[6]].state == 1-state)
			continue;
		return true;
		// if(board_[around[1]].state == state 	&&
		//    board_[around[3]].state == 1-state 	&&
		//    board_[around[7]].state == 1-state 	&&
		//    board_[around[4]].state != state 	&&
		//    board_[around[5]].state != state 	&&
		//    board_[around[6]].state != state)
		// 	return true;
	}
	return false;
}
//
bool Board::MatchHane(PositionIndex pos, PointState my_state) {
	for(int dir = 0; dir < 8; dir += 2) {
		PositionIndex around[8];
		for(int i = 0; i < 8; ++i) {
			around[i] = pos + delta[(dir+i) % 8];
		}
		if(!OnBoard(around[0]) || board_[around[0]].state == EMPTY_POINT)
			continue;
		PointState state = board_[around[0]].state;
		if(!OnBoard(around[1]) || board_[around[1]].state != 1-state)
			continue;

		if(OnBoard(around[7])) {
			if(board_[around[7]].state == 1-state) {
				if(!OnBoard(around[5]) || !OnBoard(around[3]))
					continue;
				if(!(!board_[around[5]].state == EMPTY_POINT && board_[around[3]].state == 1-state) 
				&& !(!board_[around[3]].state == EMPTY_POINT && board_[around[5]].state == 1-state))
					return true;
			}
			if(board_[around[7]].state == state && !OnBoard(around[3]) && !OnBoard(around[5]))
				return true;
		}
		if(OnBoard(around[2]) && OnBoard(around[3]) &&
		   board_[around[2]].state == state && board_[around[3]].state == EMPTY_POINT)
			return true;
		if(!OnBoard(around[2]) && !OnBoard(around[3]) && !OnBoard(around[5]))
			return true;
		if(!OnBoard(around[2]) && !OnBoard(around[3]) && OnBoard(around[2]) 
		&& board_[around[2]].state == 1-state && state == my_state)
			return true;

		// if(!OnBoard(around[0]) || ! OnBoard(around[1]))
		// 	continue;
		// if(board_[around[0]].state != 1-board_[around[1]].state)
		// 	continue;
		// if(OnBoard(around[7]) && OnBoard(around[3]) && OnBoard(around[5])) {
		// 	if(board_[around[7]].state == 1-state) {
		// 		if(board_[around[3]].state == 1-state && board_[around[5]].state == 1-state)
		// 			return false;
		// 		return true;
		// 	} else {
		// 		if(board_[around[3]].state == EMPTY_POINT && board_[around[5]].state == EMPTY_POINT)
		// 			return true;
		// 	}
		// }
		// if(OnBoard(around[2]) && OnBoard(around[3])) {
		// 	if(board_[around[2]].state == state && board_[around[3]].state != 1-state)
		// 		return true;
		// }

		// symmetric ones.
		for(int i = 0; i < 8; ++i) {
			around[i] = pos + delta[(dir-i+8) % 8];
		}
		if(!OnBoard(around[0]) || ! OnBoard(around[1]))
			continue;
		state = board_[around[0]].state;
		if(board_[around[1]].state != 1-state)
			continue;
		if(OnBoard(around[7]) && board_[around[7]].state == state
		&& !OnBoard(around[3]) && !OnBoard(around[5]))
			return true;
		if(!OnBoard(around[2]) && !OnBoard(around[3]) && !OnBoard(around[5]))
			return true;
		if(!OnBoard(around[2]) && !OnBoard(around[3]) && OnBoard(around[2]) 
		&& board_[around[2]].state == 1-state && state == my_state)
			return true;

		// if(!OnBoard(around[0]) || ! OnBoard(around[1]))
		// 	continue;
		// if(board_[around[0]].state != 1-board_[around[1]].state)
		// 	continue;
		// if(OnBoard(around[7]) && OnBoard(around[3]) && OnBoard(around[5])) {
		// 	if(board_[around[7]].state == 1-state) {
		// 		if(board_[around[3]].state == 1-state && board_[around[5]].state == 1-state)
		// 			return false;
		// 		return true;
		// 	} else {
		// 		if(board_[around[3]].state == EMPTY_POINT && board_[around[5]].state == EMPTY_POINT)
		// 			return true;
		// 	}
		// }
		// if(OnBoard(around[2]) && OnBoard(around[3])) {
		// 	if(board_[around[2]].state == state && board_[around[3]].state != 1-state)
		// 		return true;
		// }

	}

	return false;
}

bool Board::MatchBorder(PositionIndex pos, PointState my_state) {
	int i = pos / BOARD_SIZE, j = pos % BOARD_SIZE;
	int dir = 0, around[8];
	if (!i)
		dir = 2;
	if (i == BOARD_SIZE-1)
		dir = 6;
	if (!j)
		dir = 4;
	if (j == BOARD_SIZE-1)
		dir = 0;
	for(int k = 0; k < 8; ++k) 
		around[k] = pos + delta[(dir+k)%8];

	if(OnBoard(around[0]) && OnBoard(around[1]) && OnBoard(around[7])) {
		if(board_[around[1]].state == EMPTY_POINT 
		&& board_[around[0]].state + board_[around[7]].state == 1)
			return true;
	}

	if(OnBoard(around[1]) && OnBoard(around[2]) && OnBoard(around[3])) {
		if(board_[around[1]].state == EMPTY_POINT 
		&& board_[around[2]].state + board_[around[3]].state == 1)
			return true;
	}

	if(OnBoard(around[1]) && OnBoard(around[3]) && OnBoard(around[7])) {
		if(board_[around[7]].state == board_[around[1]].state 
		&& board_[around[1]].state + board_[around[3]].state == 1)
			return true;
	}

	if(OnBoard(around[1]) && OnBoard(around[3]) && OnBoard(around[7])) {
		if(board_[around[3]].state == board_[around[1]].state 
		&& board_[around[1]].state + board_[around[7]].state == 1)
			return true;
	}

	if(OnBoard(around[1]) && OnBoard(around[2])) {
		if(board_[around[1]].state == my_state 
		&& board_[around[1]].state + board_[around[2]].state == 1)
			return true;
	}

	if(OnBoard(around[0]) && OnBoard(around[1])) {
		if(board_[around[1]].state == my_state 
		&& board_[around[0]].state + board_[around[1]].state == 1)
			return true;
	}

	if(OnBoard(around[1]) && OnBoard(around[2]) && OnBoard(around[3])) {
		if(board_[around[3]].state != board_[around[1]].state
		&& board_[around[2]].state == my_state 
		&& board_[around[1]].state + board_[around[2]].state == 1)
			return true;
	}

	if(OnBoard(around[1]) && OnBoard(around[0]) && OnBoard(around[7])) {
		if(board_[around[7]].state != board_[around[1]].state
		&& board_[around[0]].state == my_state 
		&& board_[around[1]].state + board_[around[0]].state == 1)
			return true;
	}

	if(OnBoard(around[1]) && OnBoard(around[2]) && OnBoard(around[3]) && OnBoard(around[7])) {
		if(board_[around[1]].state != board_[around[3]].state
		&& board_[around[2]].state == my_state 
		&& board_[around[1]].state + board_[around[2]].state == 1
		&& board_[around[3]].state + board_[around[7]].state == 1)
			return true;
	}

	if(OnBoard(around[1]) && OnBoard(around[0]) && OnBoard(around[3]) && OnBoard(around[7])) {
		if(board_[around[1]].state != board_[around[7]].state
		&& board_[around[0]].state == my_state 
		&& board_[around[1]].state + board_[around[0]].state == 1
		&& board_[around[7]].state + board_[around[3]].state == 1)
			return true;
	}
	return false;
}

PositionIndex Board::GetMogoPattern(PointState state) {
	PositionIndex matches[8];
	int count = 0;
	for(int i = 0; i < 8; ++i) {
		PositionIndex pos = last_move + delta[i];
		int bi = pos / BOARD_SIZE;
		int bj = pos % BOARD_SIZE;
		if(!OnBoard(pos) || !Playable(pos,state))
			continue;
		// not at corner
		if ((!bi && !bj) || (bi == BOARD_SIZE-1 && !bj) || (bi == BOARD_SIZE-1 && bj == BOARD_SIZE-1) || (bi == 0 && bj == BOARD_SIZE - 1)) // at corner
			continue;

		// match patterns begin.
		if (bi > 0 && bi < BOARD_SIZE-1 && bj > 0 && bj < BOARD_SIZE-1) {
			if(MatchHane(pos, state)) {
				matches[count++] = pos;
				continue;
			}
			if(MatchCut(pos, state)) {
				matches[count++] = pos;
				continue;
			}
		} else {
			if(MatchBorder(pos, state)) {
				matches[count++] = pos;
				continue;
			}
		}
	}
	return count? matches[rand()*count/(RAND_MAX+1)] : POSITION_PASS;

}


#endif
