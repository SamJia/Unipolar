#include "def.h"
#include <intrin.h>
#include <xmmintrin.h>
#include <immintrin.h>
#include <cstdio>
#include <cstdint>
#include <ctime>
#include <cstdlib>
#include <string>
#include <iostream>
#include <cmath>
#define TIMES 100000000
using namespace unipolar;

int main() {
	printf("%f\n", 0.2 * sqrt(log(100000)/(100000 / 340)));
	printf("%f\n", 0.2 * sqrt(log(100000)/(100000 / 80)));
}