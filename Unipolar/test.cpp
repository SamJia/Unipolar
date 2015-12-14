#include <vector>
#include <cstdio>
#include <cstdint>
#include <cstring>
// #include <intrin.h>
#include <ctime>
// #include <bitset>
// #include "board_.h"
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

 class Test
 {
 public:
 	Test(){}
 	Test(const Test &t2){
 		printf("copy\n");
 	}
	bool operator <(const Test &t2) const{
		return true;
	}
 	~Test(){}
 };

 class Test2
 {
 public:
 	Test2(){
 		s.insert(Test());
 	}
 	~Test2(){}
 	set<Test> s;
 	set<Test> &fun(){
 		return s;
 	}
 };

 int main() {
 	Test2 t;
 	set<Test> &s = t.fun();
 	printf("%d\n", s.size());

	// unipolar::Force bf = unipolar::BLACK_FORCE;
	// Move mv;
	// mv.state = 1 - bf;
	// // printf("%d\n", mv.state);
	// Board board;
	// MC mc;
	// printf("%f\n", mc.Simulate(board, bf));


 }

