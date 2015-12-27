#include "def.h"
#include <intrin.h>
#include <xmmintrin.h>
#include <immintrin.h>
#include <cstdio>
#include <cstdint>
#include <ctime>
#include <cstdlib>
#include <string>
#include <iostream>
#define TIMES 100000000
using namespace unipolar;
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
int main() {
	BitSet a, b;
	a.set(0);
	b.set(1);
	uint64_t data[3];
	data[0] = 3;
	data[1] = 5;
	data[0] &= data[0] ^ data[1];
	printf("%lld\n", data[0]);
	std::string s;
	while(true){
		std::cin >> s;
		printf("%d\n", s == "name");
	}
}