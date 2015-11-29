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
typedef char AirCount;

typedef char PointState;
const PointState BLACK_POINT = 0;
const PointState WHITE_POINT = 1;
const PointState EMPTY_POINT = 2;

const int MAIN_BOARD_SIZE = 13;

enum Force {
  BLACK_FORCE = 0,
  WHITE_FORCE = 1
};


constexpr BoardSizeSquare(const int board_size){
    return board_size * board_size;
}

}

#endif