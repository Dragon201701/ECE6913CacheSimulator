#include "Cache.h"

Cache::Cache(int block_size, int set_size, int cache_size){
    blocksize = block_size;
    assoc = set_size; // number of ways 0 - fully-assoc 1-direct map >1 - n way
    numblocks = (set_size > 1)? set_size : 1;
    cachesize = cache_size; // in kb
    numsets = cachesize * 1024 / (blocksize * numblocks);
    cout<<"Creating cache. Associativity: "<<assoc<<" Block size: "<<blocksize<<" Number of sets: "<<numsets<<endl;
    cache = new Entry[numsets];
    capcity_counter = 0;
    evict_counter = 0;
    cache = (Entry *)calloc(numsets, sizeof(Entry));
    for(int i = 0; i < numsets; i++){
        cache[i].valid = false;
        cache[i].dirty = false;
        cache[i].blocks = (int *)calloc(numblocks, sizeof(int));
        cache[i].evict_counter = 0;
    }
}

/*
    Check if the given address is in the cache. 
    For fully-associate cache, loop through all sets. 
    For set-assocate cache, go to the set and loop through all blocks.
*/
bool Cache::exist(bitset<32> addr){
    bool hit = false;
    int blockbits = (int)log2(blocksize);
    int setbits = (int)log2(numsets);
    int setindex = (int)((addr<<(32 - setbits - blockbits)) >> (32 - setbits)).to_ulong();
    int * tag;
    if(assoc == 0){
        tag = new int((addr>>blockbits).to_ulong());
        
        for(int i = 0; i < numsets; i++){
            if(cache[i].valid && *cache[i].blocks == *tag){
                // Hit!
                cout<<"Fully-assoc tag: "<<hex<<tag<<" exist.\n";
                return true;
            }
        }
    }
    else{
        tag = new int((addr>>(setbits + blockbits)).to_ulong());
        cout<<"setbits: "<<dec<<setbits<<"blockbits: "<<blockbits<<endl;
        for(int i = 0; i < numblocks; i++){
            cout<<"Set-assoc check "<<hex<<addr.to_ulong()<<" tag: "<<*tag<<" at set: "<<setindex<<" way "<<i<<" get: "<<
                hex<<cache[setindex].blocks[i]<<":";
            if(cache[setindex].blocks[i] == *tag){
                // Hit!
                cout<<" Hit.\n";
                return true;
            } else
                cout<<" miss.\n";
            
        }
    }
    return false;
}
/*
    For set-associate cache, read whatever its there.
*/
Entry *Cache::access_read(bitset<32> addr){
    int blockbits = (int)log2(blocksize);
    int setbits = (int)log2(numsets);
    int setindex = ((addr<<(32 - setbits - blockbits - 2)) >> (32 - setbits)).to_ulong();
    // if not valid, return false
    if(!cache[setindex].valid)
        return nullptr;
    else
        // if valid, just return whatever is there.
        return &cache[setindex];
}
/*
    Use to Move data from other cache or the memory
    If fully-assoc
        check if full capicity_counter == numsets.
            evict if full, otherwise return null.
    If set-assoc, 
        check if the set is full. If full, than evict the block, otherwise return null.
*/
bitset<32> Cache::access_write(bitset<32> addr){
    Entry *entry;
    int blockbits =(int)log2(blocksize);
    int setbits = (int)log2(numsets);
    int setindex = (int)((addr<<(32 - setbits - blockbits)) >> (32 - setbits)).to_ulong();
    bitset<32> blockoffset = (addr<<(32 - blockbits))>>(32 - blockbits);
    int * tag;
    
    bool hit = false;
    if(assoc == 0){
        // Check if full. 
        tag = new int((addr>>blockbits).to_ulong());
        cout<<"Fully associate Access write tag: "<<hex<<tag<<endl;
        if(capcity_counter == numsets){
            // Full. Evict the entry at evict_counter;
            entry = &cache[evict_counter];
            bitset<32> evict_tag(*cache[evict_counter].blocks);
            bitset<32> evict_addr = evict_tag << blockbits;
            cache[evict_counter] = {true, false, tag, 0};
            evict_counter = (evict_counter + 1) % numsets;
            return evict_addr;
        } 
        else {
            cache[capcity_counter] = {true, false, tag, 0};
            capcity_counter++;
        }
        return 0;
    }
    else{
        // Set Associtivity.
        // Check if set is full.
        tag = new int((addr>>(setbits + blockbits)).to_ulong());
        
        for(int i = 0; i < numblocks; i++){
            if(cache[setindex].blocks[i] == 0){
                // Empty space
                
                cache[setindex].blocks[i] = *tag;
                cout<<"Set associate Access write "<<hex<<addr.to_ulong()<<" tag: "<<hex<<*tag<<" to set: "<<
                    setindex<<" at way "<<i<<": "<<hex<<cache[setindex].blocks[i]<<endl;
                return 0;
            }
        }

        // that set is full, so evict the block at evict_counter;
        bitset<32> evict_tag = bitset<32>(cache[setindex].blocks[cache[setindex].evict_counter]) << (blockbits + setbits);
        bitset<32> evict_set = bitset<32>(setindex)<<blockbits;
        bitset<32> evict_addr = evict_tag | evict_set;
        cache[setindex].blocks[evict_counter] = *tag;
        cache[setindex].evict_counter = (cache[setindex].evict_counter + 1) % numblocks;
        return evict_addr;
    }
    return 0;
}

/*
    Find, remove, and return the entry at that address
*/
bitset<32> Cache::remove(bitset<32> addr){
    int blockbits = (int)log2(blocksize);
    int setbits = (int)log2(numsets);
    int setindex = ((addr<<(32 - setbits - blockbits)) >> (32 - setbits)).to_ulong();
    int * tag;
    cout<<"Cache remove: "<<hex<<addr.to_ulong();
    if(assoc == 0){
        tag = new int((addr>>blockbits).to_ulong());
        for(int i = 0; i < numsets; i++){
            if(cache[i].valid && *cache[i].blocks == *tag){
                
                cache[i] = {false, false, (int *)calloc(numblocks, sizeof(int)), 0};
                bitset<32> evict_tag(cache[i].blocks);
                bitset<32> evict_addr = evict_tag << blockbits;
                return evict_addr;
            }
        }
    }
    else{
        tag = new int((addr>>(setbits + blockbits)).to_ulong());
        cout<<" set-assoc tag: "<<hex<<*tag<<endl;
        for(int i = 0; i < numblocks; i++){
            if(cache[setindex].blocks[i] == *tag){
                // Hit!
                cout<<" Find the block with tag: "<<hex<<cache[setindex].blocks[i]<<" at index: "<<setindex<<endl;
                cache[i] = {false, false, (int *)calloc(numblocks, sizeof(int)), 0};
                bitset<32> evict_tag(cache[setindex].blocks[i]);
                bitset<32> evict_addr = evict_tag << (blockbits + setbits);
                return evict_addr;
            }
        }
    }
    return 0;
}