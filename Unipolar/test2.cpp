#include <intrin.h>
#include <xmmintrin.h>
#include <immintrin.h>
#include <cstdio>
#include <cstdint>
#include <ctime>
#include <cstdlib>
#define TIMES 100000000

int main() {
	uint64_t a = 2;
	int16_t ADJ_POS_[169][4] = {};
	for (int i = 0; i < 169; ++i) {
		// ADJ_POS_[i][0] = ADJ_POS_[i][1] = ADJ_POS_[i][2] = ADJ_POS_[i][3] = 0;
		if (i >= 13)
			ADJ_POS_[i][0] = i - 13;
		if (i % 13 != 0)
			ADJ_POS_[i][1] = i - 1;
		if ((i + 1) % 13 != 0)
			ADJ_POS_[i][2] = i + 1;
		if (i + 13 < 169)
			ADJ_POS_[i][3] = i + 13;
	}
	int begin = clock();
	long long i;
	for(i = 0; i < TIMES; ++i){
		int pos = i % 169;
		for(int j = 0; j < 4; ++j)
			a += ADJ_POS_[pos][j];
		// if (pos >= 13)
		// 	a += pos - 13;
		// if (pos % 13 != 0)
		// 	a += pos - 1;
		// if ((pos + 1) % 13 != 0)
		// 	a += pos + 1;
		// if (pos + 13 < 169)
		// 	a += pos + 13;
	}
	// int b = 0;
	// for (int i = 0; i < TIMES; ++i){
	// 	// b += __builtin_popcountll(rand());
	// 	b += _tzcnt_u64((uint64_t)rand());
	// }
	printf("%d\n", clock() - begin);
	printf("%lld\n", i);
}