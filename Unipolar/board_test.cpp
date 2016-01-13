#include <vector>
#include <cstdio>
#include <cstdint>
#include <cstring>
// #include <intrin.h>
#include <ctime>
// #include <bitset>
// #include "board_.h"
#include <set>
#include <algorithm>
#include <iostream>
#include "def.h"
#include "mc.h"
#include "uct.h"
#include "joseki.h"
#include "controller.h"
// #include "uct_vector.h"
// #include "uct_vector_ptr.h"
// #include "uct_test.h"
#include <set>
#include <stdio.h>
#define SQUARE(x) ((x)*(x))
#define SIZE 100000
using namespace std;

int Hakmen(unsigned int n)
{
	unsigned int tmp = n - ((n >> 1) & 033333333333) - ((n >> 2) & 011111111111);
	return ((tmp + (tmp >> 3)) & 030707070707) % 63;
}

int main() {
	// TireTree joseki;
	// ifstream in("static_20.dic");
	// string a;
	// while (getline(in, a)) {
	// 	joseki.insert(a);
	// }

	// float j[169];
	// for(int i = 0; i < 169; ++i)
	// 	j[i] = 0;
	Board::Init();
	Board board;
	board.ClearBoard();
	int n = 20;
	int p1, p2;
	PointState s = 0;
	PositionIndex last_safe, best_safe, last_eat, best_eat;
	while(n >= 0) {
        scanf("%d %d", &p1, &p2);
        int p = p1*13+p2;
        board.PlayMove(Move(s, p));
        board.GetSafePoint(last_safe, best_safe, s);
        if(s == 0)
        	cout << "For X\n";
        else 
        	cout << "For O\n";

        cout << "safe" << last_safe << ' ' << best_safe << endl;
        board.GetEatPoint(last_eat, best_eat, s);
        cout << "eat" << last_eat << ' ' << best_eat << endl;
        // UCT uct;
        //printf("---START---\n");
        // Move np = uct.GenMove(board, 1 - s);
        // uct.PrintUCT();
        // board.PlayMove(np);
        s = 1-s;
        board.Print();
	}
}
