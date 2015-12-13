#ifndef UNIPOLAR_UCT_H_
#define UNIPOLAR_UCT_H_
#include "board.h"
#include "def.h"
#include "mc.h"
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <set>
using namespace unipolar;

template <int BOARD_SIZE>
class UCT {
private:
    struct Node {
        PositionIndex pos;
        Count num;
        Value val, mcval;
        Node *son, *bro;
        Node(PositionIndex po = -1, Node *so = nullptr, Node *br = nullptr) : pos(po), num(0), val(0), mcval(0), son(so), bro(br) {}
    };
public:
    UCT();
    ~UCT();
    void deleteUCT(Node *node);
    void GenChild(Node *node, Board<BOARD_SIZE> &board, PointState state);
    float UCB(Node *node);
    void FindBestChild(Node *node);
    void FindBestUCT(Node *node);
    int MCSimulation(Board<BOARD_SIZE> &board, Node *node, PointState state);
    Move GenMove(const Board<BOARD_SIZE> &board, PointState state);

private:
    Node root;
};

template <int BOARD_SIZE>
UCT<BOARD_SIZE>::UCT() {
    Node root = new Node();
}

template <int BOARD_SIZE>
UCT<BOARD_SIZE>::~UCT() {
    deleteUCT(root);
}

template <int BOARD_SIZE>
void UCT<BOARD_SIZE>::deleteUCT(Node *node) {
    Node *act = node->son;
    Node *del;
    if(act == NULL) {
        act = node;
        while(act != NULL) {
            del = act;
            act = act->bro;
            delete(del);
        }
    }
    else
        deleteUCT(act);
}

template <int BOARD_SIZE>
void UCT<BOARD_SIZE>::GenChild(Node *node, Board<BOARD_SIZE> &board, PointState state) {
    Board<BOARD_SIZE> actBoard(board);
    std::set<PositionIndex> playable;
    playable = actBoard.GetPlayablePosition(state);
    std::set<PositionIndex>::iterator it;
    for(it = playable.begin(); it != playable.end(); it++) {
        Node *newchild = new Node(*it, nullptr, nullptr);
        newchild->bro = node->son;
        node->son = newchild;
    }
}

template <int BOARD_SIZE>
float UCT<BOARD_SIZE>::UCB(Node *node) {
    return node->val / node->num + uctconst * sqrt(logf(root->num) / node->num);
}

template <int BOARD_SIZE>
void UCT<BOARD_SIZE>::FindBestChild(Node *node) {
    Node *act = node->son;
    Node *prebr = node;
    float maxUCB = UCB(act);
    float actUCB;
    while(act != NULL) {
        actUCB = UCB(act);
        if(actUCB > maxUCB) {
            maxUCB = actUCB;
            prebr->bro = act->bro;
            act->bro = node->son;
            node->son = act;
        }
        prebr = act;
        act = act->bro;
    }
}

template <int BOARD_SIZE>
void UCT<BOARD_SIZE>::FindBestUCT(Node *node) {
    Node *act = node->son;
    Node *prebr = node;
    float maxval = act->val / act->num;
    float actval = 0;
    while(act != NULL) {
        actval = act->val / act->num;
        if(actval > maxval) {
            maxval = actval;
            prebr->bro = act->bro;
            act->bro = node->son;
            node->son = act;
        }
        prebr = act;
        act = act->bro;
    }
}

template <int BOARD_SIZE>
int UCT<BOARD_SIZE>::MCSimulation(Board<BOARD_SIZE> &board, Node *node, PointState state) {
    Board<BOARD_SIZE> actBoard(board);
    Move move;
    FindBestChild(node);
    Node *act = node->son;
    move.state = state;
    move.position = act->pos;
    actBoard.PlayMove(move);
    act->num += 1;
    if(act->son == NULL ) {
        if(act->num > 1) {
            GenChild(act, actBoard, 1 - state);
            act->mcval = 1 - MCSimulation(actBoard, act, 1 - state);
            act->val += act->mcval;
        }
        else
            act->mcval = act->val = MC.Simulate(actBoard, 1 - state);
    }
    else {
        act->mcval = 1 - MCSimulation(actBoard, act, 1 - state);
        act->val += act->mcval;
    }
    return act->mcval;
}

template <int BOARD_SIZE>
Move UCT<BOARD_SIZE>::GenMove(const Board<BOARD_SIZE> &board, PointState state) {
    int t = clock();
    UCT<BOARD_SIZE> uct;
    Move nextstep;
    Board<BOARD_SIZE> uctBoard(board);
    uct->root->num = 1;
    GenChild(uct->root, uctBoard, state);
    while(clock() - t / CLOCKS_PER_SEC < 2) {
        uct->root->num += 1;
        MCSimulation(uct->root);
    }
    FindBestUCT(uct->root);
    nextstep.state = 1 - state;
    nextstep.position = uct->root->son->pos;
    return nextstep;
}

#endif
