#ifndef CacheRepresentation_H
#define CacheRepresentation_H

#include <cstdint>
#include <string>
#include <vector>

using std::vector;

/*
Sets within Cache
Slots within Set
*/

class Cache {
    private:

    struct Slot {
        uint32_t tag;
        bool valid;
        int insert_ts, access_ts;
        bool dirty;
    };

    class Set {
        private:
        // private variables here
        int numSlots;
        int numInserts;
        int max_ts;

        public:
        vector<Slot> slots;

        Set(int numSlots);

        int evictLRU(uint32_t tag);

        int evictFIFO(uint32_t tag);

        bool isHit(uint32_t tag, int &index);

        bool isFull(uint32_t tag);
    };

    vector<Set> sets;
    int numSlots;
    int loadHits;
    int loadMisses;
    int storeHits;
    int storeMisses;
    int slotSize;
    bool evictMethod;
    int numLoads;
    int numStores;
    int numCycles;
    unsigned offsetBits;
    bool writeAlloc;
    bool writeThru;
    unsigned indexBits;

    void getValue(uint32_t &tag, uint32_t &index, std::string memAddress);

    public:

    Cache(int slotSize, int numSlots, int numSets, bool writeThru, bool writeAlloc, bool evictMethod);
    void memStore(std::string memAddress);
    void memLoad(std::string memAddress);
    void printStats();

};

#endif 