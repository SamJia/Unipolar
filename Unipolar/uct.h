#ifndef UNIPOLAR_UCT_H_
#define UNIPOLAR_UCT_H_
#include "board.h"
#include "def.h"
#include <assert.h>
#include <stdlib.h>
#include <math.h>
using namespace unipolar;

template <int BOARD_SIZE>
class UCT {
private:
	struct Node {
		PositionIndex pos;
		Count num;
		Value val, MCval;
		Result res;
		Node *pa, *son, *bro;
		Node(PositionIndex po = -1, Node *pa = nullptr, Node *so = nullptr, Node *br = nullptr) : pos(po), num(0), val(0), MCval(0), pa(pa), son(so), bro(br) {}
	};
public:
	UCT() = default;
	~UCT() = default;
	void AddNode(Node *newnode);
	void DelNode(Node *oldnode);
	Count GetNum(Node *node) const;
	Value GetVal(Node *node) const;
	Value GetUCB(Node *node);
	Result GetRes(Node *node) const;
	Result GenMC(const Board &board, PointState state);
	void Recompute(Node *pa);
	void UpdatePa(Node *pa, Value val);
	//Move GenMove(const Board &board, PointState state);

private:
	Node root;
};

void UCT::AddNode(Node *newnode) {
    newnode->bro = this->son;
    this->son = newnode;
    newnode->pa = this;
}

void UCT::DelNode(Node *oldnode) {
    Node *tmp = oldnode;
    if(oldnode->pa->son->pos == oldnode->pos) {
        this->son = oldnode->bro;
        delete oldnode;
    }
    else {
        tmp = this->son;
        while(tmp->bro != NULL && tmp->bro->pos != oldnode->pos)
            tmp = tmp->bro;
        tmp->bro = oldnode->bro;
        delete oldnode;
    }
}

Count UCT::GetNum(Node *node) const {
    return node.num;
}

Value UCT::GetVal(Node *node) const {
    return node.val;
}

Value UCT::GetUCB(Node *node) {
    assert(node.pa);
    return node.val / node.num + sqrt(2 * logf(node.pa.num) / node.num);
}

Result UCT::GetRes(Node *node) const {
    return node.res;
}

PointState GenMC(const Board &board, PointState state) {

}

void UCT::Recompute(Node *pa) {
    Node *tmp;
    pa.num = 1;
    pa.val = MCval;
    tmp = pa.son;
    while(tmp != NULL) {
        if(tmp.res != LOOSER) {
            pa.num += tmp.num;
            pa.val += tmp.val;
        }
        tmp = tmp.bro;
    }
    if(pa.pa)
        Recompute(pa.pa);
}

void UCT::UpdatePa(Node *pa, Value val) {
    if(node.pa) {
        node.pa.num += 1;
        node.pa.val += (1 - val);
        node.pa.UpdatePa(1 - value);
    }
}

//Move GenMove(const Board &board, PointState state) {}

#endif
