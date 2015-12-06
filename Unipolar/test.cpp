#include <vector>
#include <cstdio>
#include <cstdint>
#include <cstring>
// #include <intrin.h>
#include <ctime>
// #include <bitset>
#include <set>
#define SIZE 10000000
using namespace std;

int Hakmen(unsigned int n)
{
	unsigned int tmp = n - ((n >> 1) & 033333333333) - ((n >> 2) & 011111111111);
	return ((tmp + (tmp >> 3)) & 030707070707) % 63;
}

class BitSet
{
public:
	BitSet() {
		Reset();
	};
	~BitSet() = default;
	void Set() {
		// data_[0] = data_[1] = data_[2] = 0xffffffffffffffff;
		memset(data_, 0xff, sizeof(data_));
	}
	void Set(int pos) {
		data_[pos >> 6] |= 1 << (pos & 63);
	}
	void Reset() {
		memset(data_, 0, sizeof(data_));
	}
	void Reset(int pos) {
		data_[pos >> 6] &= ~(1 << (pos & 63));
	}
	bool operator[](int pos){
		return data_[pos >> 6] & (1 << (pos & 63));
	}
	int Count() {
		return __builtin_popcountll(data_[0]) + __builtin_popcountll(data_[1]) + __builtin_popcountll(data_[2]);
	}
	//should only be called when a chain has only one air!!!
	int GetAirPos() {
		uint64_t tmp1 = data_[0] | data_[1];
		uint64_t tmp2 = tmp1 | data_[2];
		return ((__builtin_popcountll(~data_[0]) + __builtin_popcountll(~tmp1)) & 192) + __builtin_ctzll(tmp2);
	}
	void Merge(const BitSet &bitset2) {
		data_[0] |= bitset2.data_[0];
		data_[1] |= bitset2.data_[1];
		data_[2] |= bitset2.data_[2];
	}
// private:
	uint64_t data_[4];
};


int main() {
	int begin = clock();
	int bit_count = 0;
	// uint64_t a = 12;
	// printf("%d\n", __builtin_clzll(a));
	// uint64_t a = 1, b = 0, c;
	// printf("%x\n", ~1);
	// for (int i = 0; i < SIZE; ++i){
	// 	c = a | b;
	// bit_count = __builtin_popcount(i);
	// bit_count = _mm_popcnt_u32(i);
	// bit_count = Hakmen(i);
	// }

	BitSet bit_set;
	// printf("%d\n", sizeof(bit_set.data_));
	// bit_set.Set(0);
	// bit_set.Set(1);
	// bit_set.Set(2);
	// printf("%lld\n", bit_set.data_[0]);
	uint64_t a[3];
	// set<int> s;
	// for (int i = 0; i < 100; ++i){
	// 	s.insert(i);
	// 	bit_set.Set(i);
	// }
	// for(int i = 0; i < 100; ++i)
	for (int i = 0; i < SIZE; ++i) {
		bit_set.Set();
		// memset(a, 0xff, sizeof(a));
		// if (bit_set[i % 200]){
			// s.insert(i % 100);
			// bit_set.Reset(i % 200);
		// }
	}
	printf("%d\n", clock() - begin);
	// for (auto i : s)
	// 	printf("%d\n", i);
}