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
#include "uct.h"
#include <set>
#include <stdio.h>
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
 	//Test2 t;
 	//set<Test> &s = t.fun();
 	//printf("%d\n", s.size());

	Board::Init();
	Board board;
	board.ClearBoard();
	int n = 20;
	int p1, p2;
	PointState s = 0;
	while(n >= 0) {
        scanf("%d %d", &p1, &p2);
        int p = p1*13+p2;
        board.PlayMove(Move(s, p));
        UCT uct;
        //printf("---START---\n");
        Move np = uct.GenMove(board, 1 - s);
        uct.PrintUCT();
        board.PlayMove(np);
        board.Print();
	}
	// MC mc;
	// printf("%f\n", mc.Simulate(board, bf));


 }

