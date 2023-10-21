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
        uint32_t load_ts, access_ts;
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

        bool isHit(uint32_t tag, int index);

        bool isMiss(uint32_t tag);
    };

    vector<Set> sets;
    int numBlocks;
    int loadHits;
    int loadMisses;
    int storeHits;
    int storeMisses;
    int blockSize;
    bool lru;
    int numLoads;
    int numStores;
    int numCycles;
    unsigned offset;
    bool writeAlloc;
    bool writeThru;
    unsigned indexBits;

    void getValue(uint32_t tag, uint32_t index, std::string memAddress);


    public:

    Cache(int blockSize, int numBlocks, bool writeThru, bool writeAlloc);
    void memStore(std::string memAddress);
    void memLoad(std::string memAddress);
    void printStats();

};

#endif 