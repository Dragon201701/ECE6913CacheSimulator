#include "CacheSystem.h"

CacheSystem::CacheSystem(int l1blocksize, int l1setsize, int l1size, int l2blocksize, int l2setsize, int l2size){

    L1blocksize = l1blocksize;
    L1setsize = l1setsize;
    L1size = l1size;
    L2blocksize = l2blocksize;
    L2setsize = l2setsize;
    L2size = l2size;
    L1AcceState = NA;
    L2AcceState = NA;
    L1 = new Cache(l1blocksize, l1setsize, l1size);
    L2 = new Cache(l2blocksize, l2setsize, l2size);

}
bool CacheSystem::read(bitset<32> addr){
    
    // Read L1 and if 
    if(L1->exist(addr)){
        L1AcceState = RH;
        L2AcceState = NA;
        cout<<"L1 Read: "<<hex<<addr.to_ulong()<<" Hit.\n";
        return true;
    }
    else{
        // L1 miss
        L1AcceState = RM;
        cout<<"L1 Read: "<<hex<<addr.to_ulong()<<" Miss.\n";
        if(L2->exist(addr)){
            // L2 Hit
            cout<<"L2 Read: "<<hex<<addr.to_ulong()<<" Hit.\n";
            L2AcceState = RH;
            bitset<32> evict_addr(L2->remove(addr));
            cout<<"L2 evict: "<<hex<<evict_addr.to_ulong()<<endl;
            bitset<32> l1_evict_addr = L1->access_write(evict_addr);
            cout<<"L1 write: "<<hex<<evict_addr.to_ulong()<<" from L2. Get: "<<hex<<l1_evict_addr.to_ulong()<<endl;
            if(l1_evict_addr != 0){
                L2->access_write(l1_evict_addr);
                cout<<"L2 write: "<<hex<<l1_evict_addr.to_ulong()<<" from L1."<<endl;
            }
            return true;
        } else {
            // both miss, write to L1 and evict to L2
            cout<<"L2 Read: "<<hex<<addr.to_ulong()<<" Miss.\n";
            L2AcceState = RM;
            bitset<32> evict_addr(L1->access_write(addr));
            cout<<"L1 write: "<<hex<<addr.to_ulong()<<" from Memory. Get: "<<hex<<evict_addr.to_ulong()<<endl;
            if(evict_addr != 0){
                L2->access_write(evict_addr);
                cout<<"L2 write: "<<hex<<evict_addr.to_ulong()<<" from L1.\n";
            }
        }
    }
    
    return false;
}
bool CacheSystem::write(bitset<32> addr){
    if(L1->exist(addr)){
        L1AcceState = WH;
        L2AcceState = NA;
        return true;
    }
    else{
        L1AcceState = WM;
        if(L2->exist(addr)){
            // L2 Hit
            L2AcceState = WH;
            return true;
        }
        else{
            // L2 Miss.
            L2AcceState = WM;
        }
    }
    return false;
}