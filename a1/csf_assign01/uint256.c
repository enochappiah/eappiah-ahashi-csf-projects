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

  int curStringLength = 8;
  int start = strlen(hex) - curStringLength;
  int hexLen = strlen(hex);


  for (int i = 0; i < 8; i++) {
    char curString[9];
    // populate with the spare segements
    if (start < 0) {
        char spareSegments[hexLen + 1];
        strncpy(spareSegments, hex, hexLen);

        spareSegments[hexLen] = '\0';
        result.data[i] = strtoul(spareSegments, NULL, BASE16);
        break;
    }
    strncpy(curString, hex + start, curStringLength);
    curString[curStringLength] = '\0'; // need to null terminate string to avoid an invalid read

    result.data[i] = strtoul(curString, NULL, BASE16);
    hexLen = start;
    start = strlen(hex) - curStringLength * (i + 2);
  }
  return result;
}

// Return a dynamically-allocated string of hex digits representing the
// given UInt256 value.
char *uint256_format_as_hex(UInt256 val) {
  char *hex;
  int index = 7;
  
  while (index >= 0) { // finds most significant val
    if(val.data[index] != 0U) {
      break;
    }
    index--;
  }

  if (index == -1) { 
    hex = malloc(2); 
    strcpy(hex, "0");
    return hex;
  }

  // create the buffer
  int placeholderLen = 8 * (index + 1) + 1;
  char placeholder[placeholderLen]; 
  char* start = placeholder;

  for (int i = index; i >= 0; i--) {
    sprintf(start, "%08x", val.data[i]); 
    start += 8;
  }

  int buffPointer = 0;
  // finds most signficiant char
  while (placeholder[buffPointer] == '0') {
    buffPointer++;
  } 

  hex = malloc(placeholderLen - buffPointer);
  // chop leading zeros
  strcpy(hex, placeholder + buffPointer); 
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
// should be shifted back into the least significant bits
UInt256 uint256_rotate_left(UInt256 val, unsigned nbits) {
    if (nbits % 256 == 0) {
        return val;
    }

    unsigned windowsLeftToShift = (nbits / 32) % 8;
    unsigned bitsLeftToShift = nbits % 32;
    UInt256 result = uint256_create_from_u32(0U);

    for (int i = 0; i < 8; i++) {
        int index = (i + windowsLeftToShift) % 8;
        int overflownIndex = (index + 1) % 8;

        // take care of shifted part
        result.data[index] |= val.data[i] << bitsLeftToShift;

        // take care of overflown bits and handle wrapping
        result.data[overflownIndex] |= val.data[i] >> (32 - bitsLeftToShift);
    }

    return result;
}

// Return the result of rotating every bit in val nbits to
// the right. Any bits shifted past the least significant bit
// should be shifted back into the most significant bits.
UInt256 uint256_rotate_right(UInt256 val, unsigned nbits) {
    if (nbits % 256 == 0) {
        return val;
    }
    
    unsigned windowsLeftToShift = (nbits / 32) % 8;
    unsigned bitsLeftToShift = nbits % 32;
    UInt256 result = uint256_create_from_u32(0U);

    for (int i = 0; i < 8; i++) {
      // adding 8 to account for unintended modulo operations like negative
        int index = (i - windowsLeftToShift + 8) % 8;
        int overflownIndex = (index - 1 + 8) % 8;

        // take care of shifted part
        result.data[index] |= val.data[i] >> bitsLeftToShift;

        // take care of overflown bits and handle wrapping
        if(bitsLeftToShift != 0) { // handle potential undefined behavior with shifts
            result.data[overflownIndex] |= val.data[i] << (32 - bitsLeftToShift);
        }
    }
    return result;
}