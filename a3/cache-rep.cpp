#include "cache-rep.h"
#include <cstdint>

Cache::Cache(int blockSize, int numBlocks, bool writeThru, bool writeAlloc) {

}

Cache::Set::Set(int numBlocks) {

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

}