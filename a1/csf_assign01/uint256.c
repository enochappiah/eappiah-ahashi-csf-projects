#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include "uint256.h"

#define BASE16 16

// Create a UInt256 value from a single uint32_t value.
// Only the least-significant 32 bits are initialized directly,
// all other bits are set to 0.
UInt256 uint256_create_from_u32(uint32_t val) {
  UInt256 result;
  // COMPLETE
  result.data[0] = val;
  for (int i = 1; i < 8; i++) {
    result.data[i] = 0;
  }
  return result;
}

// Create a UInt256 value from an array of NWORDS uint32_t values.
// The element at index 0 is the least significant, and the element
// at index 3 is the most significant.
UInt256 uint256_create(const uint32_t data[8]) {
  UInt256 result;
  // COMPLETE
  for(int i = 0; i <= 8; i++) {
    result.data[i] = data[i];
  }
  return result;
}

// Create a UInt256 value from a string of hexadecimal digits.
UInt256 uint256_create_from_hex(const char *hex) {
  
  UInt256 result = uint256_create_from_u32(0);

  int i = 0;
  int windowLen = 8;
  int start = strlen(hex) - windowLen;
  int hexLen = strlen(hex);

  while(i < 8) {  
    char window[9];
    if(start < 0) {
        char remainingWindow[hexLen + 1];
        strncpy(remainingWindow, hex, hexLen);

        remainingWindow[hexLen] = '\0';
        result.data[i] = strtoul(remainingWindow, NULL, BASE16);
        break;
    }
    
    strncpy(window, hex + start, windowLen);
    window[windowLen] = '\0';

    result.data[i] = strtoul(window, NULL, BASE16);
    hexLen = start;
    start = strlen(hex) - windowLen * (i + 2);
    i++;
  }
  return result;
}

// Return a dynamically-allocated string of hex digits representing the
// given UInt256 value.
char *uint256_format_as_hex(UInt256 val) {
   char *hex = NULL;
  int index = 7;
  //Search for the most significant value
  while(index >= 0) {
    if(val.data[index] != 0UL) {
      break;
    }
    index--;
  }
  //Edge case in which all values are insignificant 
  if(index == -1) {
    hex = (char *) malloc(2);
    strcpy(hex, "0");
    return hex;
  }

  int buffLength = 8 * (index + 1) + 1;
  char buff[buffLength]; //buffer to copy hex without formating
  char* startPointer = buff;

  for(int i = index; i >= 0; i--) {
    sprintf(startPointer, "%08x", val.data[i]);
    startPointer += 8;
  }

  int buffPointer = 0;
  while(buff[buffPointer] == '0') { //Searches for position of most significant char
    buffPointer++;
  } 
  hex = (char *) malloc(buffLength - buffPointer);
  strcpy(hex, buff + buffPointer); //removes leading zeros from beggining of string
  return hex;
}

// Get 32 bits of data from a UInt256 value.
// Index 0 is the least significant 32 bits, index 3 is the most
// significant 32 bits.
uint32_t uint256_get_bits(UInt256 val, unsigned index) {
  uint32_t bits;
  // COMPLETE
  return val.data[index];
}

// Compute the sum of two UInt256 values.
UInt256 uint256_add(UInt256 left, UInt256 right) {
  UInt256 sum;
  uint32_t carryValue = 0;

  for (int i = 0; i < 8; i++) {
    uint64_t indSum = (uint64_t)right.data[i] + left.data[i] + carryValue;
    
    carryValue = (uint32_t)(indSum >> 32);
    sum.data[i] = (uint32_t)indSum;
  }
  return sum;
}

// Compute the difference of two UInt256 values.
UInt256 uint256_sub(UInt256 left, UInt256 right) {
  UInt256 result;

  return result = uint256_add(left, uint256_negate(right));
}

// Return the two's-complement negation of the given UInt256 value.
UInt256 uint256_negate(UInt256 val) {
  UInt256 result;
  // TODO: implement
  // for (int i = 0; i < 8; i++) {
  //   val.data[i] == 0U ? val.data[i] = 1U : val.data[i] = 0U;
  // }
  uint32_t data1[8] = { 1U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };
  UInt256 val1 = uint256_create(data1);

  for (int i = 0; i < 8; i++) {
    val.data[i] = ~val.data[i];
  }
  result = uint256_add(val, val1);
  return result;
}

// Return the result of rotating every bit in val nbits to
// the left.  Any bits shifted past the most significant bit
// should be shifted back into the least significant bits.
UInt256 uint256_rotate_left(UInt256 val, unsigned nbits) {
  UInt256 result = uint256_create_from_u32(0UL);
  
  int overFlows = nbits / 32;
  nbits %= 32;
  
  // Shift u64 buckets to the left
  for(int i = 0; i < overFlows; i++) {
    val.data[7]= val.data[6];
    val.data[6]= val.data[5];
    val.data[5]= val.data[4];
    val.data[4] = val.data[3];
    val.data[3] = val.data[2];
    val.data[2] = val.data[1];
    val.data[1] = val.data[0];
    val.data[0] = 0UL;
  }

  //Return early if no more shifts required
  if (nbits == 0) {
    return val;
  }

  //left shift all u64 values
  for(int i = 0; i < 8; i++) {
     result.data[i] = (val.data[i] << nbits); 
  }

  //get overflowed values that should wrap into higher bucket
  UInt256 overFlow = uint256_create_from_u32(0UL);
  for (int i = 0; i < 7 ; i++) {
    overFlow.data[i] = val.data[i] >> (32 - nbits);
  }

  //Wrap overflowed values into higher bucket
  for (int i = 0; i < 7; i++) {
       result.data[i + 1] = result.data[i + 1] | overFlow.data[i];
  }

  return result;
}

// Return the result of rotating every bit in val nbits to
// the right. Any bits shifted past the least significant bit
// should be shifted back into the most significant bits.
UInt256 uint256_rotate_right(UInt256 val, unsigned nbits) {
  UInt256 result;
  // TODO: implement
  return result;
}
