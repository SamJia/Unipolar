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
	Board() = default;
	~Board() = default;
	static void Init();
	void ClearBoard();
	float PlayMove(const Move &move);
	std::vector<PositionIndex> GetPlayablePosition(PointState state) {
		int base = 0;
		BitSet tmp = empty_[state] - suiside_[state] - safe_eye_[state];
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
		return (empty_[state] - suiside_[state] - safe_eye_[state] - dangerous_empty_[state] - dangerous_empty_[state^1]).count();
	}
	PositionIndex GetPlayable(PointState state, int number) {
		// printf("number:%d\n", number);
		int base = 0;
		BitSet tmp = empty_[state] - suiside_[state] - safe_eye_[state] - dangerous_empty_[state] - dangerous_empty_[state^1];
		for (int j = 0; j < 3; ++j) {
			if (__builtin_popcountll(tmp.data_[j]) <= number)
				number -= __builtin_popcountll(tmp.data_[j]);
			else
			{	// number++;
				// int lower = 0, upper = 64, mid = 32, num;
				// while (true) {
				// 	mid = (lower + upper) / 2;
				// 	num = __builtin_popcountll(tmp.data_[j] & (((uint64_t)2 << (mid)) - 1));
				// 	// printf("tmp.data_[j]:%llx, mid:%d, num:%d, yanma:%llx, and:%llx\n", tmp.data_[j], mid, num, (((uint64_t)2 << (mid)) - 1), tmp.data_[j] & (((uint64_t)1 << (mid)) - 1));
				// 	if(number == num)
				// 		return base + 63 - __builtin_clzll(tmp.data_[j] & (((uint64_t)2 << (mid)) - 1));
				// 	else if (number < num)
				// 		upper = mid;
				// 	else
				// 		lower = mid + 1;
				// }
				for (; number; --number) {
					tmp.data_[j] &= tmp.data_[j] - 1;
				}
				return __builtin_ctzll(tmp.data_[j]) + base;
				// if(__builtin_ctzll(tmp.data_[j]) != result){
				// 	printf("%d %d %d\n", number, __builtin_ctzll(tmp.data_[j]), result);
				// 	exit(0);
				// }
				// return result + base;
			}
			base += 64;
		}
	}
	PositionIndex SpecialPointTest(PointState state);
	void PrintVector(std::vector<PositionIndex> v) {
		for (auto i : v)
			printf("%d ", i);
		printf("\n");
	}
	PositionIndex GetPieceCount(PointState state) {
		return piece_count_[state];
	}
	PositionIndex GetAreaCount(PointState state) {
		return piece_count_[state] + eye_[state].count() + dangerous_empty_[state].count();
	}
	bool Playable(PositionIndex pos, PointState state) {
		return (empty_[state] - suiside_[state])[pos];
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
		/*printf("   0  1  2  3  4  5  6  7  8  9  10 11 12\n");
		for (int i = 0 ; i < BOARD_SIZE; ++i) {
			if (i < 10)
				printf("%d  ", i);
			else
				printf("%d ", i);
			for (int j = 0; j < BOARD_SIZE; ++j)
				printf("%c  ", board_[i * 13 + j].father < 0 ? (board_[i * 13 + j].air_count + '0') : 'X');
			printf("\n");
		}
		printf("   0  1  2  3  4  5  6  7  8  9  10 11 12\n");
		for (int i = 0 ; i < BOARD_SIZE; ++i) {
			if (i < 10)
				printf("%d  ", i);
			else
				printf("%d ", i);
			for (int j = 0; j < BOARD_SIZE; ++j)
				printf("%3d", board_[i * 13 + j].father);
			printf("\n");
		}
		// printf("BLACK playable_pos_:\n");
		// for (std::set<PositionIndex>::iterator it = playable_pos_[BLACK_POINT].begin(); it != playable_pos_[BLACK_POINT].end(); ++it)
		// 	printf("%d ", *it);
		// printf("\n");
		printf("BLACK playable_bit_:\n");
		playable_bit_[BLACK_POINT].Print();
		printf("BLACK eye_:\n");
		eye_[BLACK_POINT].Print();
		printf("\n");
		// printf("WHITE playable_pos_:\n");
		// for (std::set<PositionIndex>::iterator it = playable_pos_[WHITE_POINT].begin(); it != playable_pos_[WHITE_POINT].end(); ++it)
		// 	printf("%d ", *it);
		// printf("\n");
		printf("WHITE playable_bit_:\n");
		playable_bit_[WHITE_POINT].Print();
		printf("WHITE eye_:\n");
		eye_[WHITE_POINT].Print();
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
		printf("WHITE piece count%d\n", piece_count_[WHITE_POINT]);*/
	}

	int Check() {
		for (PositionIndex pos = 0; pos < BoardSizeSquare(BOARD_SIZE); ++pos) {
			if (board_[pos].state == EMPTY_POINT) {
				int tmp = 0;
				for (int i = 1; i <= ADJ_POS_[pos][0]; ++i)
					if (board_[ADJ_POS_[pos][i]].state == EMPTY_POINT)
						++tmp;
				if (tmp != board_[pos].air_count)
					return pos;
			}
		}
		for (PositionIndex pos = 0; pos < BoardSizeSquare(BOARD_SIZE); ++pos) {
			if (board_[pos].state == EMPTY_POINT && board_[pos].air_count == 0) {
				bool air[2][2] = {}; //[state][>1?]
				PositionIndex adj_chain;
				for (int i = 1; i <= ADJ_POS_[pos][0]; ++i) {
					adj_chain = GetFather(ADJ_POS_[pos][i]);
					air[board_[adj_chain].state][board_[adj_chain].air_count > 1] = true;
				}
				//white eat black or white connect to air
				// printf("%d %d %d %d\n", air[0][0], air[0][1], air[1][0], air[1][1]);
				// if (air[BLACK_POINT][0] | air[WHITE_POINT][1]) {
				// 	if (!playable_bit_[WHITE_POINT][pos])
				// 		return pos;
				// }
				// else if (playable_bit_[WHITE_POINT][pos])
				// 	return pos;

				// //black eat white or black connect to air
				// if (air[WHITE_POINT][0] | air[BLACK_POINT][1]) {
				// 	if (!playable_bit_[BLACK_POINT][pos])
				// 		return pos;
				// }
				// else if (playable_bit_[BLACK_POINT][pos])
				// 	return pos;
			}
		}
		return -1;
	}

// private:
	PositionIndex GetFather(PositionIndex pos);
	void Merge(PositionIndex pos1, PositionIndex pos2);
	void ToEmpty(PositionIndex pos);
	void FromEmpty(PositionIndex pos, PointState state);
	void RemoveChain(PositionIndex pos);
	PositionIndex AdjacentPosition(PositionIndex position, int adj_direction);
	void CheckSpecialPoint(PositionIndex pos, bool add_score = false);

	// void Able(PositionIndex pos, PointState state) {
	// 	if (mc_ && eye_[state][pos])
	// 		return;
	// 	playable_bit_[state].set(pos);
	// }

	// void DisAble(PositionIndex pos, PointState state) {
	// 	playable_bit_[state].reset(pos);
	// }

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

	// void StartMC() {
	// 	mc_ = true;
	// 	int base;
	// 	uint64_t tmp;
	// 	for (int i = 0; i < 2; ++i) {
	// 		base = 0;
	// 		for (int j = 0; j < 3; ++j) {
	// 			tmp = eye_[i].data_[j];
	// 			while (tmp) {
	// 				DisAble(base + __builtin_ctzll(tmp), i);
	// 				tmp &= tmp - 1;
	// 			}
	// 			base += 64;
	// 		}
	// 	}
	// }

	List board_[BoardSizeSquare(BOARD_SIZE)];
	BitSet empty_[2], eye_[2], safe_eye_[2], dangerous_[2], suiside_[2], dangerous_empty_[2];
	PositionIndex piece_count_[2];
	PositionIndex ko_;
	bool mc_;
	static PositionIndex ADJ_POS_[BoardSizeSquare(BOARD_SIZE)][5];
	float score;
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
		// playable_pos_[0].insert(i);
		// playable_pos_[1].insert(i);
		// playable_pos_no_eye_[0].insert(i);
		// playable_pos_no_eye_[1].insert(i);
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

PositionIndex Board::SpecialPointTest(PointState state) {
	float max_score = 1, score;
	PositionIndex best_pos = POSITION_PASS, pos, adj_chain;
	BitSet tmp = (dangerous_[state] - suiside_[state]) * empty_[state];
	if (ko_ >= 0 && tmp[ko_]) {
		printf("ko_ occur in my dangerous\n");
		exit(0);
	}
	for (int i = 0, base = 0; i < 3; ++i) {
		while (tmp.data_[i]) {
			pos = base + __builtin_ctzll(tmp.data_[i]);
			// printf("check dangerous pos %d\n", pos);
			tmp.data_[i] &= tmp.data_[i] - 1;
			BitSet merged_air_set = board_[pos].air_set;
			for (int j = 1; j <= ADJ_POS_[pos][0]; ++j) {
				if (board_[ADJ_POS_[pos][j]].state == state) {
					adj_chain = GetFather(ADJ_POS_[pos][j]);
					merged_air_set += board_[adj_chain].air_set;
				}
			}
			score = merged_air_set.count() - 1;
			if (score > max_score) {
				max_score = score;
				best_pos = pos;
			}
		}
		base += 64;
	}
	tmp = dangerous_[state ^ 1] * empty_[state];
	if (ko_ >= 0 && tmp[ko_]) {
		printf("ko_ occur in oppose dangerous\n");
		exit(0);
	}
	for (int i = 0, base = 0; i < 3; ++i) {
		while (tmp.data_[i]) {
			pos = base + __builtin_ctzll(tmp.data_[i]);
			tmp.data_[i] &= tmp.data_[i] - 1;
			score = 0;
			for (int j = 1; j <= ADJ_POS_[pos][0]; ++j) {
				if (board_[ADJ_POS_[pos][j]].state == (state ^ 1)) {
					adj_chain = GetFather(ADJ_POS_[pos][j]);
					score += -board_[adj_chain].father;
				}
			}
			if ((score * 1.1) > max_score) {
				max_score = score * 1.1;
				best_pos = pos;
			}
		}
		base += 64;
	}
	return best_pos;
}

float Board::PlayMove(const Move &move) {
	// printf("play move %d\n", move.position);
	if (ko_ >= 0) {
		SetEmpty(ko_, move.state);
		CheckSpecialPoint(ko_);
		// CheckEyeShape(ko_);
		ko_ = -1;
	}
	// int wrong_pos = Check();
	// if (wrong_pos >= 0) {
	// 	Print();
	// 	printf("Check out some Wrong at %d\n", wrong_pos);
	// 	exit(0);
	// }
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
	for (int i = 1; i <= ADJ_POS_[pos][0]; ++i) {
		adj_chain = GetFather(ADJ_POS_[pos][i]);
		board_[adj_chain].air_set.reset(pos);
		board_[adj_chain].air_count = board_[adj_chain].air_set.count();
		// printf("adj_chain state : %d\n", board_[adj_chain].state);
		if (board_[adj_chain].state == board_[pos].state) {
			// printf("merge: %d %d\n", adj_chain, pos);
			// printf("merge %d %d\n", adj_chain, pos);
			Merge(adj_chain, GetFather(pos));
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
				score += -board_[v[i]].father;
				RemoveChain(v[i]);
			}
		}
		else if (board_[v[i]].air_count == 1) {
			if (board_[v[i]].state != EMPTY_POINT) {
				score += -(board_[v[i]].father / 3);
				CheckSpecialPoint(board_[v[i]].air_set.GetAirPos());
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
		score -= -(board_[pos].father / 2);
		CheckSpecialPoint(board_[pos].air_set.GetAirPos());
	}
	if (board_[pos].father != -1)
		ko_ = -1;
	if (ko_ >= 0)
		RemoveEmpty(ko_, move.state ^ 1);
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
				score += -(board_[adj_chain].father / 2);
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
				score += 2;
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
				score += 2;
			SetSafeEye(pos, WHITE_POINT);
		}
		SetSuiside(pos, BLACK_POINT);
	}

	if ((air[WHITE_POINT][0] | air[WHITE_POINT][1]) == 0) {
		if (add_score)
			score += 1;
		SetEye(pos, BLACK_POINT);
	}
	if ((air[BLACK_POINT][0] | air[BLACK_POINT][1]) == 0) {
		if (add_score)
			score += 1;
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

#endif
