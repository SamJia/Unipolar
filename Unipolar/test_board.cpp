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
	TireTree joseki;
	ifstream in("static_20.dic");
	string a;
	while (getline(in, a)) {
		joseki.insert(a);
	}

	float j[169];
	for(int i = 0; i < 169; ++i)
		j[i] = 0;
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
}

// class Test2
// {
// public:
// 	Test2() {
// 		s.insert(Test());
// 	}
// 	~Test2() {}
// 	set<Test> s;
// 	set<Test> &fun() {
// 		return s;
// 	}
// };

// std::vector<int>::iterator find_max_element(std::vector<int> &v) {
// 	std::vector<int>::iterator max_ele;
// 	int max_value;
// 	for (auto it = v.begin(); it != v.end(); ++it)
// 		if (*it > max_value) {
// 			max_value = *it;
// 			max_ele = it;
// 		}
// 	return max_ele;
// }

// std::vector<int> fun() {
// 	vector<int>  v;
// 	for (int j = 0; j < 100; ++j)
// 		v.push_back(rand());
// 	return v;
// }

// struct Node {
// 	Node(int da = 0, Node *ne = NULL) : data(da), next(ne) {}
// 	int data;
// 	Node *next;
// };

// void test_fun(){
// 	std::vector<Test> v;
// 	v.push_back(Test(1));
// 	v.push_back(Test(2));
// 	v.push_back(Test(3));
// 	int a;
// }

// int main()
// {
// 	// test_fun();
// 	int a;
// 	int begin = clock();
// 	// for(int i = 0; i < SIZE; ++i){
// 		vector<int>  v;
// 		// v.resize(100);
// 		for(int j = 0; j < 1000; ++j){
// 			v.push_back(rand());
// 			printf("size:%d\n", v.capacity());
// 		}
// 	// 	// std::vector<int> v = fun();
// 		// std::vector<Test> v2(v.begin(), v.end());
// 	// 	// std::vector<Test> v2;
// 	// 	// for(int j = 0; j < 100; ++j)
// 	// 	// 	v2.push_back(rand());
// 	// }
// 	// vector<int> v;
// 	// v.resize(100);
// 	// for (int i = 0; i < SIZE; ++i)
// 	// 	for (int j = 0; j < 100; ++j)
// 	// 		a = v[j];

// 	Node *head = new Node();
// 	for (int i = 0; i < 100; ++i)
// 		head->next = new Node(rand(), head->next);
// 	for (int i = 0; i < SIZE; ++i)
// 		for (Node *p = head->next; p; p = p->next)
// 			a = p->data;

// 	// std::vector<int>::iterator it;
// 	// for(int i = 0; i < SIZE; ++i){
// 	// 	it = find_max_element(v);
// 	// 	// it = max_element(v.begin(), v.end());
// 	// }
// 	printf("%d\n", clock() - begin);
// 	return 0;
// }
