#ifndef UNIPOLAR_SRC_DEF_H_
#define UNIPOLAR_SRC_DEF_H_

#include <cstdint>

#define CANNOT_RUN_HERE() {\
            assert(false);\
        }

#define DISALLOW_COPY_AND_ASSIGN(ClassName) \
    ClassName(const ClassName &) = delete; \
    ClassName &operator =(const ClassName &) = delete;

#define DISALLOW_COPY_AND_ASSIGN_AND_MOVE(ClassName) \
    ClassName(const ClassName &) = delete; \
    ClassName &operator =(const ClassName &) = delete; \
    ClassName(ClassName &&) = delete; \
    ClassName &operator =(ClassName &&) = delete;

namespace unipolar {

typedef uint32_t HashKey;
typedef int16_t PositionIndex;
const PositionIndex POSITION_PASS = 169;
typedef int AirCount;

typedef char PointState;
const PointState BLACK_POINT = 0;
const PointState WHITE_POINT = 1;
const PointState EMPTY_POINT = 2;

const int BOARD_SIZE = 13;

enum Force {
    BLACK_FORCE = 0,
    WHITE_FORCE = 1
};


constexpr int BoardSizeSquare(const int board_size) {
    return board_size * board_size;
}

template<typename T>
inline void Swap(T &a, T &b){
    T tmp = a;
    a = b;
    b = tmp;
}

const double uctconst = 0.4;
const double amafconst = 1;
typedef double Value;
typedef int Count;

// for joseki theshold
const int JOSEKI_STEP = 12;

const int TIME_PER_STEP = 3;

const int THREAD_NUM = 8;

// for expand in uct
const int EXPAND_NUM = 8;

const double save_dangerous_eye = 2;
const double save_dangerous_chain = 5;
const double eat_chain = 4;
const double threaten_oppose = 2;
const double threaten_self = -5;
const double make_eye = 1.8;
const double make_safe_eye = 3.5;
const double tolerate_upper_bound = 30;
const double match_range = 6;

double komi = 0.0;
double bonus_ratio = 0.2;
int step_count = 0;
}

#endif
