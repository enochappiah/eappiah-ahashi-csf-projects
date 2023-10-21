#include "cache-rep.h"
#include <cstdint>
#include <iostream>
#include <math.h>

Cache::Cache(int slotSize, int numSlots, int numSets, bool writeThru, bool writeAlloc) {
    for(int i = 0; i < numSets; i++) {
        sets.push_back(Set(numSlots));
    }
    indexBits = log2(numSets);
    offset = log2(slotSize);
    loadHits = 0;
    loadMisses = 0;
    storeHits = 0;
    storeMisses = 0;
    this->lru = lru;
    numLoads = 0;
    numStores = 0;
    numCycles = 0;
    this->writeAlloc = writeAlloc;
    this->writeThru = writeThru;
}

Cache::Set::Set(int numSlots) {
    for (int i = 0; i < numSlots; i++) {
        Slot slot;
        slot.valid = false;
        slot.dirty = false;
        slots.push_back(slot);
    }
    this->numInserts = 0;
    this->max_ts = 0;
    this->numSlots= numSlots;
}

void Cache::getValue(uint32_t tag, uint32_t index, std::string memAddress) {

}

int Cache::Set::evictLRU(uint32_t tag) {

}

int Cache::Set::evictFIFO(uint32_t tag) {

}

bool Cache::Set::isHit(uint32_t tag, int index) {

}

bool Cache::Set::isMiss(uint32_t tag) {

}

void Cache::memStore(std::string memAddress) {

}

void Cache::memLoad(std::string memAddress) {

}

void Cache::printStats() {
    std::cout << "Total loads: " << numLoads << std::endl;
    std::cout << "Total stores: " << numStores << std::endl;
    std::cout << "Load hits: " << loadHits << std::endl;
    std::cout << "Load misses: " << loadMisses << std::endl;
    std::cout << "Store hits: " << storeHits << std::endl;
    std::cout << "Store misses: " << storeMisses << std::endl;
    std::cout << "Total cycles: " << numCycles << std::endl;
}