#include "cache-rep.h"
#include <cstdint>
#include <iostream>
#include <math.h>
#include <sys/types.h>

Cache::Cache(int slotSize, int numSlots, int numSets, bool writeThru, bool writeAlloc, bool evictMethod) {
    for(int i = 0; i < numSets; i++) {
        sets.push_back(Set(numSlots));
    }
    indexBits = log2(numSets);
    offsetBits = log2(slotSize);
    loadHits = 0;
    loadMisses = 0;
    storeHits = 0;
    storeMisses = 0;
    this->evictMethod = evictMethod;
    numLoads = 0;
    numStores = 0;
    numCycles = 0;
    this->writeAlloc = writeAlloc;
    this->writeThru = writeThru;
    this->numSlots = numSlots;
    this->slotSize = slotSize;
}

Cache::Set::Set(int numSlots) {
    // this->numInserts = 0;
    // this->max_ts = 0;
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

void Cache::getValue(uint32_t &tag, uint32_t &index, std::string memAddress) {
    // hexadecmial to integer
    uint32_t initialMem = std::stoul(memAddress,nullptr,16);
    // chop off offset bits
    initialMem = initialMem >> offsetBits;
    uint32_t indexFilter = (1 << indexBits) - 1;
    index = initialMem & indexFilter;
    tag = initialMem >> indexBits;
}

int Cache::Set::evictLRU(uint32_t tag) {
    int lruSlot = 0;
    int curMinimum = max_ts;
    // by the end of the for loop we will have the least recently used slot
    for (int i = 0; i < numSlots; i++) {
        if (slots[i].access_ts < curMinimum) {
            curMinimum = slots[i].access_ts;
            lruSlot = i;
        }
    }
    this->slots[lruSlot].access_ts = max_ts;
    max_ts++;
    this->slots[lruSlot].tag = tag;
    return lruSlot;
}

int Cache::Set::evictFIFO(uint32_t tag) {
    int lruSlot = 0;
    int firstEntry = numInserts;
    // by the end of the for loop we will have the first insertion
    for (int i = 0; i < numSlots; i++) {
        if (slots[i].insert_ts < firstEntry) {
            firstEntry = slots[i].insert_ts;
            lruSlot = i;
        }
    }
    this->slots[lruSlot].insert_ts = numInserts;
    numInserts++;
    this->slots[lruSlot].tag = tag;

    return lruSlot;
}

bool Cache::Set::isHit(uint32_t tag, int &hitLocation) {
    for (int i = 0; i < numSlots; i++) {
        if (slots[i].tag == tag && slots[i].valid) {
            slots[i].access_ts = max_ts;
            max_ts++;
            hitLocation = i;
            return true;
        }
    }
    return false;
}

bool Cache::Set::isFull(uint32_t tag) {
    for (int i = 0; i < numSlots; i++) {
        if (!slots[i].valid) {
            slots[i].valid = true;
            slots[i].access_ts = max_ts;
            max_ts++;
            slots[i].tag = tag;
            slots[i].insert_ts = numInserts;
            numInserts++;
            return false;
        }
    }
    return true;
}

void Cache::memStore(std::string memAddress) {
    numStores++;
    uint32_t index, tag;
    getValue(tag, index, memAddress);
    int hitLocation = 0;
    Set &curSet = sets[index];

    // In the event of a hit
    if (curSet.isHit(tag, hitLocation)) {
        storeHits++;
        numCycles++;

        // WRITE-THROUGH
        if (writeThru) {
            numCycles += 100;

        // WRITE-BACK
        } else {
            curSet.slots[hitLocation].dirty = true;
        }

    // In the event of a miss
    } else {
        storeMisses++;
        // WRITE-ALLOCATE
        if (writeAlloc) {
            numCycles += 100 * (slotSize / 4);
            numCycles++;
            // If it is full that means we have to evict by some method (either FIFO or LRU)
            if (curSet.isFull(tag)) {
                int evictIndex =  evictMethod ? curSet.evictLRU(tag) : curSet.evictFIFO(tag); // curSet.evictLRU(tag); // CHANGE LATER
                if (curSet.slots[evictIndex].dirty) {
                    numCycles += 100 * (slotSize / 4);
                }
            }
        // NO-WRITE-ALLOCATE
        } else {
            numCycles += 100;
        }
    }
}

void Cache::memLoad(std::string memAddress) {
    uint32_t index, tag;
    numLoads++;
    getValue(tag, index, memAddress);
    int hitLocation = 0;
    Set &curSet = sets[index];
    if (curSet.isHit(tag, hitLocation)) {
        numCycles++;
        loadHits++;
    } else {
        numCycles += 100 * (slotSize / 4);
        numCycles++;
        loadMisses++;
        // If it is full that means we have to evict by some method (either FIFO or LRU)
        if (curSet.isFull(tag)) {
            evictMethod ? curSet.evictLRU(tag) : curSet.evictFIFO(tag);
            //evictMethod = curSet.evictFIFO(tag);
        }
    }
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