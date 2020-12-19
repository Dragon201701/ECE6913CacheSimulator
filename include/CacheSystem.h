#include <bitset>
#include <vector>
#include <string>
#include <iostream>
#include <math.h>
#include "Cache.h"

#define NA 0 // no action
#define RH 1 // read hit
#define RM 2 // read miss
#define WH 3 // Write hit
#define WM 4 // write miss

using namespace std;

class CacheSystem{
    private:
        int L1blocksize, L1setsize, L1size, L2blocksize, L2setsize, L2size;
        
        Cache *L1;
        Cache *L2;
    public:
        int L1AcceState, L2AcceState;
        CacheSystem(int l1blocksize, int l1setsize, int l1size, int l2blocksize, int l2setsize, int l2size);
        bool read(bitset<32> addr);
        bool write(bitset<32> addr);
        
};