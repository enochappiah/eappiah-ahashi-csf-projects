#include <stdio.h>
#include <stdint.h>
#include "wcfuncs.h"

// Suggested number of buckets for the hash table
#define HASHTABLE_SIZE 13249


int main(int argc, char **argv) {
  // stats (to be printed at end)
  uint32_t total_words = 0;
  uint32_t unique_words = 0;
  const unsigned char *best_word = (const unsigned char *) "";
  uint32_t best_word_count = 0;

  FILE *file = (argc > 1) ? fopen(argv[1], "r") : stdin;
  if (!file) {
    fprintf(stderr, "Error in opening file\n");
    return 1;
  }

  // Array representing the hash table (each element is like head of linked list)
  struct WordEntry *buckets[HASHTABLE_SIZE] = {0};
  unsigned char word[MAX_WORDLEN + 1];

  while (wc_readnext(file, word)) {
    total_words++;
    wc_tolower(word);
    wc_trim_non_alpha(word);

    struct WordEntry *entry = wc_dict_find_or_insert(buckets, HASHTABLE_SIZE, word);
    entry->count++;

    if (entry->count > best_word_count) {
      best_word_count = entry->count;
      best_word = entry->word;
    } else if (entry->count == best_word_count) {
      if ((wc_str_compare(entry->word, best_word)) < 0) {
        best_word = entry->word;
      }
    }
  }

  for (int i = 0; i < HASHTABLE_SIZE; i++) {
    struct WordEntry *curr = buckets[i];

    // linked list traversal
    while (curr) {
      unique_words++;
      curr = curr->next;
    }
  }


  printf("Total words read: %u\n", (unsigned int) total_words);
  printf("Unique words read: %u\n", (unsigned int) unique_words);
  printf("Most frequent word: %s (%u)\n", (const char *) best_word, best_word_count);

  fclose(file);

  for (int i = 0; i < HASHTABLE_SIZE; i++) {
    wc_free_chain(buckets[i]);
  }

  return 0;
}

