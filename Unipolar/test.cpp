#include <vector>
#include <cstdio>
#include <cstdint>
#include <cstring>
// #include <intrin.h>
#include <ctime>
// #include <bitset>
#include "board_.h"
#include "def.h"
#include "mc.h"
#include <set>
#define SQUARE(x) ((x)*(x))
#define SIZE 10000000
using namespace std;

 int Hakmen(unsigned int n)
 {
 	unsigned int tmp = n - ((n >> 1) & 033333333333) - ((n >> 2) & 011111111111);
 	return ((tmp + (tmp >> 3)) & 030707070707) % 63;
 }

 template<int S>
 class Test
 {
 public:
 	Test();
 	~Test();
 	int a[S];
 };

 int main() {
 	// set<int> s;
 	// s.insert(1);
 	// s.insert(2);
 	// s.insert(3);
 	// s.insert(4);
 	// printf("%d\n", s[0]);

	unipolar::Force bf = unipolar::BLACK_FORCE;
	Move mv;
	mv.state = 1 - bf;
	// printf("%d\n", mv.state);
	Board board;
	MC mc;
	printf("%f\n", mc.Simulate(board, bf));


 }

