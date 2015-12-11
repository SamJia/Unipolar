#ifndef UNIPOLAR_UCT_H_
#define UNIPOLAR_UCT_H_
#include "board.h"
#include "def.h"
#include "mc.h"
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <set>
using namespace unipolar;

template <int BOARD_SIZE>
class UCT {
private:
    struct Node {
        PositionIndex pos;
        Count num;
        Val val;
        Node *pa, *son, *bro;
        Node(PositionIndex po = -1, Node *pa = nullptr, Node *so = nullptr, Node *br = nullptr) : pos(po), num(0), val(0), pa(pa), son(so), bro(br) {}
    };
public:
    UCT() = default;
    ~UCT() = default;
    static void GenChild(Node *node, Board &board, PointState state);
    static float UCB(Node *node);
    static void FindBestChild(Node *node);
    static int MCSimulation(Board &board, Node *node, PointState state);
    static Move GenMove(const Board &board, PointState state);

private:
    Node root;
};

template <int BOARD_SIZE>
void UCT<BOARD_SIZE>::GenChild(Node *node, Board &board, PointState state) {
    Board actBoard(board);
    std::set<PositionIndex> playable;
    playable = actBoard.GetPlayablePosition(state);
    std::set<PositionIndex>::iterator it;
    for(it = playable.begin(); it != playable.end(); it++) {
        Node *newchild = new Node(*it, nullptr, nullptr, nullptr);
        newchild.bro = node.son;
        node.son = newchild;
    }
}

template <int BOARD_SIZE>
float UCT<BOARD_SIZE>::UCB(Node *node) {
    return node.val + sqrt(2 * logf(node.pa.num - 1) / node.num);
}

template <int BOARD_SIZE>
void UCT<BOARD_SIZE>::FindBestChild(Node *node) {
    Node *act = node.son;
    Node *prebr;
    float maxval = UCB(act);
    while(act.bro != NULL) {
        prebr = act;
        act = act.bro;
        if(UCB(act) > maxval) {
            prebr.bro = act.bro;
            act.bro = node.son;
            node.son = act;
        }
    }
}

template <int BOARD_SIZE>
int UCT<BOARD_SIZE>::MCSimulation(Board &board, Node *node, PointState state) {
    Board actBoard(board);
    Move move;
    FindBestChild(node);
    Node *act = node.son;
    move.state = state;
    move.position = act.pos;
    actBoard.PlayMove(move);
    act.num += 1;
    if(act.son == NULL) {
        if(act.num > 1) {
            GenChild(act, actBoard, 1 - state);
            act.val = 1 - MCSimulation(actBoard, act, 1 - state);
        }
        act.val = MC.Simulate(actBoard, 1 - state);
        return act.val;
    }
    else
        act.val = 1 - MCSimulation(actBoard, act, 1 - state);
}

template <int BOARD_SIZE>
Move UCT<BOARD_SIZE>::GenMove(const Board &board, PointState state) {
    Board uctBoard(board);
    root.num = 1;
    GenChild(root, uctBoard, state);
    while(root.num != 100) {
        MCSimulation(root);
    }
    FindBestChild(root);
    return Move(1 - state, root->son.pos);
}
