#include <vector>
#include <cstdio>
#include <cstdint>
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

class Test2
{
public:
	Test2(){
		printf("default\n");
	}
	~Test2() = default;
	Test2(const Test2 &test2){
		printf("copy\n");
	}
	bool operator<(const Test2 &test2) const{
		return true;
	}
};

class Test
{
public:
	Test() = default;
	~Test() = default;
	set<Test2> s;
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

	// printf("%d\n", clock() - begin);
	// Test t;
	// t.s.insert(Test2());
	// Test t2(t);
	// t.s.clear();
	// printf("%d %d\n", t.s.size(), t2.s.size());
	set<int> s;
	s.insert(1);
	s.insert(2);
	s.insert(3);
	s.erase(4);
	s.insert(3);
	for(auto i : s)
		printf("%d\n", i);
}