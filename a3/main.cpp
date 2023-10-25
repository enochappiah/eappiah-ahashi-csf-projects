#include <cctype>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include "cache-rep.h"

void convertToLower(std::string &string);
void setStringToBoolean(bool *choicesArray, std::string allocateString, std::string writeString, std::string evictMethodString);
void getInputs(std::string line, std::string& operation, std::string &memAddress);

int main(int argc, char* argv[]) {

  if (argc != 7) {
    std::cerr << "Invalid number of variables" << std::endl;
    return 1;
  }

  int sets; 
  try {
    //convert sets arg to int
    sets = std::stoi(argv[1]);
  } catch (const std::invalid_argument& err) {
    std::cerr << "Invalid number of sets: Needs to be a number" <<std::endl;
    return 1;
  } catch (const std::out_of_range& err) {
    std::cerr << "Sets argument is out of range" << std::endl;
    return 1;
  
  } 

  if (((sets - 1) & sets) || sets < 1) {
    std::cerr << "Invalid number of sets: Needs to be a power of 2" <<std::endl;
    return 1;
  }

  int blocks;
  try {
    blocks = std::stoi(argv[2]);
  } catch (const std::invalid_argument& err) {
    std::cerr << "Invalid number of blocks: Needs to be a number" <<std::endl;
    return 1;
  } catch (const std::out_of_range& err) {
    std::cerr << "Blocks argument is out of range" << std::endl;
    return 1;
  
  } 
  if (((blocks - 1) & blocks) || blocks < 1) {
    std::cerr << "Invalid number of blocks: Needs to be a power of 2" <<std::endl;
    return 1;
  }

  int blockSize;
  try {
    blockSize = std::stoi(argv[3]);
  } catch (const std::invalid_argument& err) {
    std::cerr << "Invalid number for blockSize: Needs to be a number" <<std::endl;
    return 1;
  } catch (const std::out_of_range& err) {
    std::cerr << "BlockSize argument is out of range" << std::endl;
    return 1;
  
  } 
  if (((blockSize - 1) & blockSize) || blockSize < 4) {
    std::cerr << "Invalid number for blockSize: Needs to be a power of 2 and at least 4" <<std::endl;
    return 1;
  }

  std::string allocateString = argv[4];
  std::string writeString = argv[5];
  std::string strategyString = argv[6];

  convertToLower(allocateString);
  convertToLower(writeString);
  convertToLower(strategyString);

  bool choices[4];
  setStringToBoolean(choices, allocateString, writeString, strategyString);

  if (!choices[3]) { //cchecking boolean array to make sure variables are correct
    return 1;
  }

  bool allocateBool = choices[0];
  bool writeBool = choices[1];
  bool evictMethodBool = choices[2];

  Cache cache(blockSize, blocks, sets, writeBool, allocateBool, evictMethodBool);
  

  std::string line;
  while (std::getline(std::cin,line)) {
    std::string operation;
    std::string memAddress;
    getInputs(line, operation, memAddress);

    if (operation == "l") {
      cache.memLoad(memAddress);
    } else if (operation == "s") {
      cache.memStore(memAddress);
    } else {
      std::cerr << "Invalid process argument" << std:: endl;
      return 1;
      }
    }
    cache.printStats();
    return 0;
  }
  
  void convertToLower(std::string &string) {
    for (char& c: string) {
      c = std::tolower(c);
    }
  }

  void setStringToBoolean(bool *choices, std::string allocateChoice, std::string writeChoice, std::string evictMethodChoice) {

    choices[3] = true; //error signal index

    if (allocateChoice == "write-allocate") {
      choices[0] = true;
    } else if (allocateChoice == "no-write-allocate") {
      choices[0] = false;
    } else {
      std::cerr << "Invalid allocate argument" <<std::endl;
      choices[3] = false; //signals error
      return;
    }

    if (writeChoice == "write-through") {
      choices[1] = true;
    } else if (writeChoice == "write-back" ) {
      if (choices[0] == false) {
        std::cerr << "Invalid arguments: Cannot choose 'no-write-allocate' and 'write-back' together" <<std::endl;
        choices[3] = false;
      }
      choices[1] = false;
    } else {
      std::cerr << "Invalid write-back argument" <<std::endl;
      choices[3] = false; //signals error
      return;
    }

    if (evictMethodChoice == "lru") {
      choices[2] = true;
    } else if (evictMethodChoice == "fifo") {
      choices[2] = false;
    } else {
      std::cerr << "Invalid eviction strategy argument" <<std::endl;
      choices[3] = false; //signals error
      return;
    }
  }

  void getInputs(std::string line, std::string& operation, std::string& memAddress) {
    std::stringstream ss(line);
    ss >> operation;
    ss >> memAddress;
  }

