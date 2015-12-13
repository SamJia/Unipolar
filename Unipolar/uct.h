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

//template <int BOARD_SIZE>
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
    UCT() = default;
    ~UCT() = default;
    static void GenChild(Node *node, Board &board, PointState state);
    static float UCB(Node *node, Count totalnum);
    static void FindBestChild(Node *node, Count totalnum);
    static void FindBestUCT(Node *node);
    static int MCSimulation(Board &board, Node *node, PointState state, Count totalnum);
    static Move GenMove(const Board &board, PointState state);

private:
    Node *root;
};

void UCT::GenChild(Node *node, Board &board, PointState state) {
    Board actBoard(board);
    std::set<PositionIndex> playable;
    playable = actBoard.GetPlayablePosition(state);
    std::set<PositionIndex>::iterator it;
    for(it = playable.begin(); it != playable.end(); it++) {
        Node *newchild = new Node(*it, nullptr, nullptr);
        newchild->bro = node->son;
        node->son = newchild;
    }
}

float UCT::UCB(Node *node, Count totalnum) {
    return node->val / node->num + uctconst * sqrt(logf(totalnum) / node->num);
}

void UCT::FindBestChild(Node *node, Count totalnum) {
    Node *act = node->son;
    Node *prebr = node;
    float maxUCB = UCB(act, totalnum);
    float actUCB;
    while(act != NULL) {
        actUCB = UCB(act, totalnum);
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

void UCT::FindBestUCT(Node *node) {
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

int UCT::MCSimulation(Board &board, Node *node, PointState state, Count totalnum) {
    Board actBoard(board);
    Move move;
    FindBestChild(node, totalnum);
    Node *act = node->son;
    move.state = state;
    move.position = act->pos;
    actBoard.PlayMove(move);
    act->num += 1;
    if(act->son == NULL ) {
        if(act->num > 1) {
            GenChild(act, actBoard, 1 - state);
            act->mcval = 1 - MCSimulation(actBoard, act, 1 - state, totalnum);
            act->val += act->mcval;
        }
        else
            act->mcval = act->val = MC::Simulate(actBoard, Force(1 - state));
    }
    else {
        act->mcval = 1 - MCSimulation(actBoard, act, 1 - state, totalnum);
        act->val += act->mcval;
    }
    return act->mcval;
}

Move UCT::GenMove(const Board &board, PointState state) {
    int t = clock();
    UCT uct;
    Move nextstep;
    Board uctBoard(board);
    uct.root->num = 1;
    GenChild(uct.root, uctBoard, state);
    while(clock() - t / CLOCKS_PER_SEC < 2) {
        uct.root->num += 1;
        MCSimulation(uctBoard, uct.root, state, uct.root->num);
    }
    FindBestUCT(uct.root);
    nextstep.state = 1 - state;
    nextstep.position = uct.root->son->pos;
    return nextstep;
}

#endif
