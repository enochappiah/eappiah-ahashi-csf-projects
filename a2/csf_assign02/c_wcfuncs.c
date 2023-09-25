// Important: do not add any additional #include directives!
// The only C library functions that may be used are
//
// - fgetc
// - malloc
// - free

#include <stdint.h>
#include <stdlib.h>
#include "wcfuncs.h"


size_t provideLength(unsigned char *w ) {
  size_t length = 0;
  unsigned char *p = w;

  //move pointer to the end of the string's null terminator
  while (*p != '\0') {
    p++;
    length++;
  }

  //move pointer once backwards to last char in string
  p--;
  length--;

  return length;
}

//provides pointer to the end of a string
unsigned char* provideEndPtr(unsigned char *w ) {
  size_t i = 0;
  unsigned char *p = w;

  //move pointer to the end of the string's null terminator
  while(*p != '\0') {
    p++;
  }
  //move pointer once backwards to last char in string
  p--;
  
  return p;
}


// Compute a hash code for the given NUL-terminated
// character string.
//
// The hash algorithm should be implemented as follows:
//
// uint32_t hash_code = 5381
// for each character c of w in order {
//   hash_code = hash_code * 33 + c
// }
//
// Note that the character values should be treated as
// being unsigned (in the range 0..255)
uint32_t wc_hash(const unsigned char *w) {
  uint32_t hash_code = 5381;

  while (*w) {
    hash_code = hash_code * 33 + *w;
    w++;
  }
  return hash_code;
}

// Compare two strings lexicographically. Return
//
// - a negative value if lhs string is less than rhs string
// - 0 if lhs string is identical to rhs string
// - a positive value if lhs string is greater than rhs string
//
// Lexicographical comparison is a generalization of alphabetical
// order, but using character codes. If one string is a prefix
// of the other, it is considered as "less than". E.g.,
// "hi" would compare as less than "high".
int wc_str_compare(const unsigned char *lhs, const unsigned char *rhs) {
  while (*lhs && (*lhs == *rhs)) {
    lhs++;
    rhs++;
  }
  
  return (int)(*lhs) - (int)(*rhs);
}

// Copy NUL-terminated source string to the destination buffer.
void wc_str_copy(unsigned char *dest, const unsigned char *source) {
  while(*source) {
    *dest = *source;
    dest++;
    source++;
  }
  *dest = '\0';
}

// Return 1 if the character code in c is a whitespace character,
// false otherwise.
//
// For the purposes of this function, a whitespace character is one of
//
//   ' '
//   '\t'
//   '\r'
//   '\n'
//   '\f'
//   '\v'
int wc_isspace(unsigned char c) {
  if (c == ' ' || c == '\t'|| c== '\r' || c == '\n' || c == '\f' || c == '\v' ) {
    return 1;
  }
  return 0;
}

// Return 1 if the character code in c is an alphabetic character
// ('A' through 'Z' or 'a' through 'z'), 0 otherwise.
int wc_isalpha(unsigned char c) {
  if ( (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ) {
    return 1;
  }
  return 0;
}

// Read the next word from given input stream, storing
// the word in the array pointed-to by w. (This array should be
// assumed to be MAX_WORDLEN+1 elements in size.) Return 1
// if a word is read successfully, 0 otherwise.
//
// For the purposes of this function, a word is a sequence of
// 1 or more non-whitespace characters.
//
// If a sequence of non-whitespace characters has more than
// MAX_WORDLEN characters, then only the first MAX_WORDLEN
// characters in the sequence should be stored in the array.
int wc_readnext(FILE *in, unsigned char *w) {
    int character;
    unsigned int wLen = 0;

    // Skip initial whitespace characters
    while ((character = fgetc(in)) != EOF && wc_isspace((unsigned char)character)) {
        // EMPTY
    }

    // Sanity check if end of file
    if (character == EOF) {
        return 0;
    }

    // Found start of the word by this point. Need a do-while to make sure we don't skip first letter
    do {
        // If space, add
        if (wLen < MAX_WORDLEN) {
            w[wLen] = (unsigned char)character;
            wLen++;
        }
    } while ((character = fgetc(in)) != EOF && !wc_isspace((unsigned char)character));

    w[wLen] = '\0';

    return 1;
}


// Convert the NUL-terminated character string in the array
// pointed-to by w so that every letter is lower-case.
void wc_tolower(unsigned char *w) {
  size_t i = 0; //conventional datatype for array sizes
  while(w[i] != '\0') { //looping until end of string
    if(w[i] >= 'A' && w[i] <= 'Z') { //if ASCII value are any of the capital letters 
      w[i] = w[i] + ('a' - 'A'); //convert the char using the ASCII offset 32
    }
    i++;
  }
}

// //A recursive call for wc_trim_non_alpha function
// unsigned char * recursiveTrim(unsigned char *p) {
//   if (wc_isalpha(*p)) {       //Base case: if the char is a letter, func should return because first alphabetic char is found
//     p++;                        //p is currently at alphabetic char, so addition to move it back to previously non-alphabetic char
//     *p = '\0';                  //insert null terminator at that position to end the string early
//     return p;
//   }
//   p--;                          //keep iterating toward front of string
//   return recursiveTrim(p);      //recurive call with updated string array index
// }



// // Remove any non-alphaabetic characters from the end of the
// // NUL-terminated character string pointed-to by w.
// void wc_trim_non_alpha(unsigned char *w) {
//   unsigned char *p = provideEndPtr(w); //provides pointer to the end of parameter string
//   w = recursiveTrim(p); 
// }

void wc_trim_non_alpha(unsigned char *w) {
  unsigned char *end = w;

  while (*end) {
    end++;
  }

  if (end != w) {
    end--;
  }

  while (end != w && !wc_isalpha(*end)) {
    end--;
  }

  // If last valid character is not alphabetic set cur position to null terminator
    if (!wc_isalpha(*end)) {
        *end = '\0';
    } else {
        *(end + 1) = '\0';  // Place null terminator after last valid alphabetic character
    }
}

// Search the specified linked list of WordEntry objects for an object
// containing the specified string.
//
// If a matching object is found, set the int variable pointed-to by
// inserted to 0 and return a pointer to the matching object.
//
// If a matching object is not found, allocate a new WordEntry object,
// set its next pointer to point to what head points to (i.e., so the
// new object is at the head of the list), set the variable pointed-to
// by inserted to 1, and return a pointer to the new node. Note that
// the new node should have its count value set to 0. (It is the caller's
// job to update the count.)
struct WordEntry *wc_find_or_insert(struct WordEntry *head, const unsigned char *s, int *inserted) {
  struct WordEntry *curr = head; 

  while (curr) {
    if (wc_str_compare(curr->word, s) == 0) { 
      // Found word
      *inserted = 0;
      return curr;
    }
    curr = curr->next;
  }

  // At this point we did not find the word, so we insert
  struct WordEntry *newEntry = (struct WordEntry *)malloc(sizeof(struct WordEntry));
  if (!newEntry) {
    return NULL;
  }
  wc_str_copy(newEntry->word, s);
  newEntry->count = 0;
  newEntry->next = head;

  *inserted = 1;
  return newEntry;

}

// Find or insert the WordEntry object for the given string (s), returning
// a pointer to it. The head of the linked list which contains (or should
// contain) the entry for s is the element of buckets whose index is the
// hash code of s mod num_buckets.
//
// Returns a pointer to the WordEntry object in the appropriate linked list
// which represents s.
struct WordEntry *wc_dict_find_or_insert(struct WordEntry *buckets[], unsigned num_buckets, const unsigned char *s) {
  unsigned index = wc_hash(s) % num_buckets;
  int inserted; 
  
  // Find or insert WordEntry
  struct WordEntry *entry = wc_find_or_insert(buckets[index], s, &inserted);

  // If we inserted a word, update head
  if (inserted) {
    entry->next = buckets[index];
    buckets[index] = entry;
  }
  return entry;
}

// Free all of the nodes in given linked list of WordEntry objects.
void wc_free_chain(struct WordEntry *p) {
  while (p) {
    struct WordEntry *temp = p;
    p = p->next;
    free(temp);
  }
}
