#include <vector>
#include <string>
#include <iostream>
#include <math.h>
#include <bitset>

using namespace std;

struct Entry{
    bool valid;
    bool dirty;
    int * blocks;
    int evict_counter;
};

class Cache{
    public:
        Entry *cache;
        int blocksize, assoc, cachesize, numsets, numblocks, capcity_counter, evict_counter;
        Cache(int block_size, int set_size, int cache_size);
        bool exist(bitset<32> addr);
        Entry *access_read(bitset<32> addr);
        bitset<32> access_write(bitset<32> addr);
        bitset<32> remove(bitset<32> addr);
};
