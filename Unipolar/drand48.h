#ifndef DRAND48_H
#define DRAND48_H

#include <stdlib.h>

#define mmmm 0x100000000LL
#define cccc 0xB16
#define aaaa 0x5DEECE66DLL

static unsigned long long seed = 1;

double drand48(void)
{
	seed = (aaaa * seed + cccc) & 0xFFFFFFFFFFFFLL;
	unsigned int x = seed >> 16;
    return 	((double)x / (double)mmmm);
	
}

void srand48(unsigned int i)
{
    seed  = (((long long int)i) << 16) | rand();
}

#endif