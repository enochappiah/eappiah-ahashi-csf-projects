#include <stdio.h>
#include <stdlib.h>
#include "tctest.h"
#include "wcfuncs.h"

// Test fixture object type
typedef struct {
  const unsigned char *test_str_1;
  const unsigned char *test_str_1_upper; 
  const unsigned char *test_str_2;
  const unsigned char *test_str_3;
  const unsigned char *test_str_4;
  const unsigned char *test_str_4_upper;
  const unsigned char *test_str_5_trim;
  const unsigned char *test_str_6_trim;

  const unsigned char *test_str_1_copy;

  const unsigned char *words_1;
  const unsigned char *words_2;
} TestObjs;

// Functions to create and clean up the test fixture object
TestObjs *setup(void);
void cleanup(TestObjs *objs);

// Helper functions for tests
FILE *create_input_file(const unsigned char *text);

// Prototypes of test functions
void test_hash(TestObjs *objs);
void test_str_compare(TestObjs *objs);
void test_str_copy(TestObjs *objs);
void test_isspace(TestObjs *objs);
void test_isalpha(TestObjs *objs);
void test_readnext(TestObjs *objs);
void test_readnext_1(TestObjs *objs);
void test_tolower(TestObjs *objs);
void test_trim_non_alpha(TestObjs *objs);
void test_trim_non_alpha_1(TestObjs *objs);
void test_trim_non_alpha_2(TestObjs *objs);
void test_find_or_insert(TestObjs *objs);
void test_find_or_insert_1(TestObjs *objs);
void test_find_or_insert_trim(TestObjs *objs);
void test_dict_find_or_insert(TestObjs *objs);
void test_dict_find_or_insert_1(TestObjs *objs);
void test_free_chain(TestObjs *objs);

int main(int argc, char **argv) {
  // If a command line argument is provided, use it as the
  // name of the test function to run
  if (argc > 1) {
    tctest_testname_to_execute = argv[1];
  }

  TEST_INIT();

  TEST(test_hash);
  TEST(test_str_compare);
  TEST(test_str_copy);
  TEST(test_isspace);
  TEST(test_isalpha);
  TEST(test_readnext);
  TEST(test_readnext_1);
  TEST(test_tolower);
  TEST(test_trim_non_alpha);
  TEST(test_trim_non_alpha_1);
  TEST(test_trim_non_alpha_2);
  TEST(test_find_or_insert);
  TEST(test_find_or_insert_1);
  TEST(test_find_or_insert_trim);
  TEST(test_dict_find_or_insert);
  TEST(test_dict_find_or_insert_1);
  TEST(test_free_chain);

  TEST_FINI();
}

TestObjs *setup(void) {
  TestObjs *objs;

  objs = malloc(sizeof(TestObjs));

  objs->test_str_1 = (const unsigned char *) "hello";
  objs->test_str_1_upper = (const unsigned char *) "HELLO!";
  objs->test_str_2 = (const unsigned char *) "This is A SeNtEnCe with_MiXeD cASe.";
  objs->test_str_3 = (const unsigned char *) "O_O...";
  objs->test_str_4 = (const unsigned char *) "hello, world";
  objs->test_str_4_upper = (const unsigned char *) "HElLO, WoRlD";

  objs->test_str_5_trim = (const unsigned char *) "testing....!3112323!!";
  objs->test_str_6_trim = (const unsigned char *) "t3$t1ng";

  objs->test_str_1_copy = (const unsigned char *) "hello";

  objs->words_1 = (const unsigned char *) "A strong smell of petroleum prevails throughout.";
  objs->words_2 = (const unsigned char *) "This is a test.";

  //printf("%u\n", wc_hash("Burris"));
  //printf("%u\n", wc_hash("Burt's"));

  return objs;
}


void cleanup(TestObjs *objs) {
  free(objs);
}

FILE *create_input_file(const unsigned char *text) {
  FILE *out;

  // open a temporary file
  out = tmpfile();
  if (out == NULL) {
    fprintf(stderr, "tmpfile failed\n");
    exit(1);
  }

  // write text
  size_t len = strlen((const char *) text);
  if (fwrite(text, 1, len, out) != len) {
    fprintf(stderr, "couldn't write text to temp file\n");
    exit(1);
  }

  // fseek back to the beginning of the file
  if (fseek(out, 0, SEEK_SET) == -1) {
    fprintf(stderr, "fseek failed\n");
    exit(1);
  }

  return out;
}

void test_hash(TestObjs *objs) {
  uint32_t hash;

  hash = wc_hash(objs->test_str_1);
  ASSERT(261238937U == hash);
}

void test_str_compare(TestObjs *objs) {
  ASSERT(wc_str_compare(objs->test_str_1, objs->test_str_1) == 0);
  ASSERT(wc_str_compare(objs->test_str_1, objs->test_str_4) < 0);
  ASSERT(wc_str_compare(objs->test_str_4, objs->test_str_1) > 0);

  // ADDED TESTS
  ASSERT(wc_str_compare(objs->test_str_4, objs->test_str_4) == 0);
  ASSERT(wc_str_compare(objs->test_str_2, objs->test_str_4) < 0);
  ASSERT(wc_str_compare(objs->test_str_3, objs->test_str_4) < 0);
  ASSERT(wc_str_compare(objs->test_str_3, objs->test_str_1) < 0);
  ASSERT(wc_str_compare(objs->test_str_1, objs->test_str_2) > 0);
  ASSERT(wc_str_compare(objs->test_str_2, objs->test_str_1) < 0);
  
}


void test_str_copy(TestObjs *objs) {
  unsigned char buf[256];

  wc_str_copy(buf, objs->test_str_1);
  ASSERT(0 == strcmp((const char *) objs->test_str_1, (const char *) buf));


  // ADDED TESTS
  wc_str_copy(buf, objs->test_str_2);
  ASSERT(0 == strcmp((const char *) objs->test_str_2, (const char *) buf));

  wc_str_copy(buf, objs->test_str_3);
  ASSERT(0 == strcmp((const char *) objs->test_str_3, (const char *) buf));

  wc_str_copy(buf, objs->test_str_4);
  ASSERT(0 == strcmp((const char *) objs->test_str_4, (const char *) buf));
}



void test_isspace(TestObjs *objs) {
  // this test function doesn't use the test fixture
  (void) objs;

  ASSERT(1 == wc_isspace(' '));
  ASSERT(1 == wc_isspace('\t'));

  ASSERT(0 == wc_isspace('a'));
  ASSERT(0 == wc_isspace('.'));
  ASSERT(0 == wc_isspace('*'));

  // ADDED TESTS
  ASSERT(1 == wc_isspace('\n'));
  ASSERT(1 == wc_isspace('\r'));
  ASSERT(1 == wc_isspace('\f'));
  ASSERT(1 == wc_isspace('\v'));

  ASSERT(0 == wc_isspace('z'));
  ASSERT(0 == wc_isspace('!'));
  

}

void test_isalpha(TestObjs *objs) {
  // this test function doesn't use the test fixture
  (void) objs;

  ASSERT(1 == wc_isalpha('x'));
  ASSERT(1 == wc_isalpha('F'));

  ASSERT(0 == wc_isalpha('0'));
  ASSERT(0 == wc_isalpha(','));

  ASSERT(1 == wc_isalpha('Z'));
  ASSERT(1 == wc_isalpha('A'));
  ASSERT(1 == wc_isalpha('z'));
  ASSERT(1 == wc_isalpha('a'));

  ASSERT(0 == wc_isalpha('\n'));
  ASSERT(0 == wc_isalpha('7'));
  ASSERT(0 == wc_isalpha('?'));
  ASSERT(0 == wc_isalpha('!'));
  
}

void test_readnext(TestObjs *objs) {
  FILE *in;
  unsigned char buf[MAX_WORDLEN + 1];

  in = create_input_file(objs->words_1);

  ASSERT(1 == wc_readnext(in, buf));
  ASSERT(0 == strcmp("A", (const char *) buf));

  ASSERT(1 == wc_readnext(in, buf));
  ASSERT(0 == strcmp("strong", (const char *) buf));

  ASSERT(1 == wc_readnext(in, buf));
  ASSERT(0 == strcmp("smell", (const char *) buf));

  ASSERT(1 == wc_readnext(in, buf));
  ASSERT(0 == strcmp("of", (const char *) buf));

  ASSERT(1 == wc_readnext(in, buf));
  ASSERT(0 == strcmp("petroleum", (const char *) buf));

  ASSERT(1 == wc_readnext(in, buf));
  ASSERT(0 == strcmp("prevails", (const char *) buf));

  ASSERT(1 == wc_readnext(in, buf));
  ASSERT(0 == strcmp("throughout.", (const char *) buf));

  ASSERT(0 == wc_readnext(in, buf));

  fclose(in);
}

// ADDED TEST
void test_readnext_1(TestObjs *objs) {
  FILE *in;
  unsigned char buf[MAX_WORDLEN + 1];

  in = create_input_file(objs->words_2);

  ASSERT(1 == wc_readnext(in, buf));
  ASSERT(0 == strcmp("This", (const char *) buf));

  ASSERT(1 == wc_readnext(in, buf));
  ASSERT(0 == strcmp("is", (const char *) buf));

  ASSERT(1 == wc_readnext(in, buf));
  ASSERT(0 == strcmp("a", (const char *) buf));

  ASSERT(1 == wc_readnext(in, buf));
  ASSERT(0 == strcmp("test.", (const char *) buf));

  ASSERT(0 == wc_readnext(in, buf));

  fclose(in);
}


void test_tolower(TestObjs *objs) {
  unsigned char buf[256];

  strcpy((char *) buf, (char *) objs->test_str_2);
  wc_tolower(buf);
  ASSERT(0 == strcmp("this is a sentence with_mixed case.", (char *) buf));

  // ADDED TESTS
  strcpy((char *) buf, (char *) objs->test_str_4_upper);
  wc_tolower(buf);
  ASSERT(0 == strcmp("hello, world", (char *) buf));

  strcpy((char *) buf, (char *) objs->test_str_1_upper);
  wc_tolower(buf);
  ASSERT(0 == strcmp("hello!", (char *) buf));
}

void test_trim_non_alpha(TestObjs *objs) {
  unsigned char buf[256];

  strcpy((char *) buf, (const char *) objs->test_str_3);
  ASSERT(0 == strcmp("O_O...", (const char *) buf));
  printf("Before trim: %s\n", buf);
  wc_trim_non_alpha(buf);
  printf("After trim: %s\n", buf);
  ASSERT(0 == strcmp("O_O", (const char *) buf));

}

// ADDED TESTS
void test_trim_non_alpha_1(TestObjs *objs) {
  unsigned char buf[256];
  
  strcpy((char *) buf, (const char *) objs->test_str_5_trim);
  ASSERT(0 == strcmp("testing....!3112323!!", (const char *) buf));
  printf("Before trim: %s\n", buf);
  wc_trim_non_alpha(buf);
  printf("After trim: %s\n", buf);
  ASSERT(0 == strcmp("testing", (const char *) buf));
}

void test_trim_non_alpha_2(TestObjs *objs) {
  unsigned char buf[256];

  strcpy((char *) buf, (const char *) objs->test_str_6_trim);
  ASSERT(0 == strcmp("t3$t1ng", (const char *) buf));
  printf("Before trim: %s\n", buf);
  wc_trim_non_alpha(buf);
  printf("After trim: %s\n", buf);
  ASSERT(0 == strcmp("t3$t1ng", (const char *) buf));

}

void test_find_or_insert(TestObjs *objs) {
  (void) objs;

  struct WordEntry *list = NULL;
  int inserted;

  struct WordEntry *p;

  p = wc_find_or_insert(list, (const unsigned char *) "avis", &inserted);
  ASSERT(1 == inserted);
  list = p;
  ASSERT(p != NULL);
  ASSERT(0 == strcmp("avis", (const char *) p->word));
  ASSERT(0 == p->count);
  ++p->count;

  p = wc_find_or_insert(list, (const unsigned char *) "ax's", &inserted);
  ASSERT(1 == inserted);
  list = p;
  ASSERT(p != NULL);
  ASSERT(0 == strcmp("ax's", (const char *) p->word));
  ASSERT(0 == p->count);
  ++p->count;

  p = wc_find_or_insert(list, (const unsigned char *) "avis", &inserted);
  ASSERT(0 == inserted);
  ASSERT(p != NULL);
  ASSERT(0 == strcmp("avis", (const char *) p->word));
  ASSERT(1 == p->count);
  ++p->count;

  p = wc_find_or_insert(list, (const unsigned char *) "ax's", &inserted);
  ASSERT(0 == inserted);
  ASSERT(p != NULL);
  ASSERT(0 == strcmp("ax's", (const char *) p->word));
  ASSERT(1 == p->count);
  ++p->count;

  wc_free_chain(p);
}

// ADDED TESTS
void test_find_or_insert_1(TestObjs *objs) {
  (void) objs;

  struct WordEntry *list = NULL;
  int inserted;

  struct WordEntry *p;

  
  p = wc_find_or_insert(list, (const unsigned char *) "cheese", &inserted);
  ASSERT(1 == inserted);
  list = p;
  ASSERT(p != NULL);
  ASSERT(0 == strcmp("cheese", (const char *) p->word));
  ASSERT(0 == p->count);
  ++p->count;

  p = wc_find_or_insert(list, (const unsigned char *) "another", &inserted);
  ASSERT(1 == inserted);
  list = p;
  ASSERT(p != NULL);
  ASSERT(0 == strcmp("another", (const char *) p->word));
  ASSERT(0 == p->count);
  ++p->count;


  p = wc_find_or_insert(list, (const unsigned char *) "cheese", &inserted);
  ASSERT(0 == inserted);
  ASSERT(p != NULL);
  ASSERT(0 == strcmp("cheese", (const char *) p->word));
  ASSERT(1 == p->count);
  ++p->count;
  ASSERT(2 == p->count);

  p = wc_find_or_insert(list, (const unsigned char *) "another", &inserted);
  ASSERT(0 == inserted);
  ASSERT(p != NULL);
  ASSERT(0 == strcmp("another", (const char *) p->word));
  ASSERT(1 == p->count);
  ++p->count;
  ASSERT(2 == p->count);


  wc_free_chain(p);

}

// ADDED TESTS  
void test_find_or_insert_trim(TestObjs *objs) {
  (void) objs;

  struct WordEntry *list = NULL;
  int inserted;

  struct WordEntry *p;

  unsigned char word[MAX_WORDLEN + 1] = "word!";
  wc_trim_non_alpha(word);

  p = wc_find_or_insert(list, word, &inserted);
  ASSERT(1 == inserted);
  list = p;
  ASSERT(p != NULL);
  ASSERT(0 == strcmp("word", (const char *) p->word));
  ASSERT(0 == p->count);
  ++p->count;


  p = wc_find_or_insert(list, word, &inserted);
  ASSERT(0 == inserted);
  ASSERT(p != NULL);
  ASSERT(0 == strcmp("word", (const char *) p->word));
  ASSERT(1 == p->count);
  ++p->count;
  ASSERT(2 == p->count);


  wc_free_chain(p);
}

void test_dict_find_or_insert(TestObjs *objs) {
  struct WordEntry *dict[5] = { NULL, NULL, NULL, NULL, NULL };
  struct WordEntry *p;

  // "avis" and "ax's" should go in bucket 1

  p = wc_dict_find_or_insert(dict, 5, (const unsigned char *) "avis");
  ASSERT(dict[0] == NULL);
  ASSERT(dict[1] != NULL);
  ASSERT(dict[1] == p);
  ASSERT(dict[2] == NULL);
  ASSERT(dict[3] == NULL);
  ASSERT(dict[4] == NULL);
  ASSERT(p->count == 0);
  ++p->count;


  p = wc_dict_find_or_insert(dict, 5, (const unsigned char *) "ax's");
  ASSERT(dict[0] == NULL);
  ASSERT(dict[1] != NULL);
  ASSERT(dict[1] == p);
  ASSERT(dict[2] == NULL);
  ASSERT(dict[3] == NULL);
  ASSERT(dict[4] == NULL);
  ASSERT(p->count == 0);
  ++p->count;

  // "lemur" should go in bucket 0
  p = wc_dict_find_or_insert(dict, 5, (const unsigned char *) "lemur");
  ASSERT(dict[0] != NULL);
  ASSERT(dict[0] == p);
  ASSERT(dict[1] != NULL);
  ASSERT(dict[2] == NULL);
  ASSERT(dict[3] == NULL);
  ASSERT(dict[4] == NULL);
  ASSERT(p->count == 0);
  ++p->count;

  // register another occurrence of "avis"
  p = wc_dict_find_or_insert(dict, 5, (const unsigned char *) "avis");
  ASSERT(dict[0] != NULL);
  ASSERT(dict[1] != NULL);
  ASSERT(dict[2] == NULL);
  ASSERT(dict[3] == NULL);
  ASSERT(dict[4] == NULL);
  ASSERT(p->count == 1);
  ++p->count;

  // "marmoset" should go in bucket 2
  p = wc_dict_find_or_insert(dict, 5, (const unsigned char *) "marmoset");
  ASSERT(dict[0] != NULL);
  ASSERT(dict[1] != NULL);
  ASSERT(dict[2] != NULL);
  ASSERT(dict[2] == p);
  ASSERT(dict[3] == NULL);
  ASSERT(dict[4] == NULL);
  ASSERT(p->count == 0);
  ++p->count;

  // "coelacanth" should go in bucket 3
  p = wc_dict_find_or_insert(dict, 5, (const unsigned char *) "coelacanth");
  ASSERT(dict[0] != NULL);
  ASSERT(dict[1] != NULL);
  ASSERT(dict[2] != NULL);
  ASSERT(dict[3] != NULL);
  ASSERT(dict[3] == p);
  ASSERT(dict[4] == NULL);
  ASSERT(p->count == 0);
  ++p->count;

  wc_free_chain(dict[0]);
  wc_free_chain(dict[1]);
  wc_free_chain(dict[2]);
  wc_free_chain(dict[3]);
  wc_free_chain(dict[4]);

}

void test_dict_find_or_insert_1(TestObjs *objs) {
  struct WordEntry *dict[5] = { NULL, NULL, NULL, NULL, NULL };
  struct WordEntry *p;

  // test1 should go in bucket 4
  p = wc_dict_find_or_insert(dict, 5, (const unsigned char *) "test1");
  ASSERT(dict[0] == NULL);
  ASSERT(dict[1] == NULL);
  ASSERT(dict[2] == NULL);
  ASSERT(dict[4] == p);
  ASSERT(dict[3] == NULL);
  ASSERT(dict[4] != NULL);
  ASSERT(p->count == 0);
  ++p->count;


  // test300 should go in bucket 2
  p = wc_dict_find_or_insert(dict, 5, (const unsigned char *) "test300");
  ASSERT(dict[0] == NULL);
  ASSERT(dict[1] == NULL);
  ASSERT(dict[2] != NULL);
  ASSERT(dict[2] == p);
  ASSERT(dict[3] == NULL);
  ASSERT(dict[4] != NULL);
  ASSERT(p->count == 0);
  ++p->count;

  // "trucker" should go in bucket 3
  p = wc_dict_find_or_insert(dict, 5, (const unsigned char *) "trucker");
  ASSERT(dict[0] == NULL);
  ASSERT(dict[1] == NULL);
  ASSERT(dict[2] != NULL);
  ASSERT(dict[3] != NULL);
  ASSERT(dict[3] == p);
  ASSERT(dict[4] != NULL);
  ASSERT(p->count == 0);
  ++p->count;

  // register another occurrence of "test1"
  p = wc_dict_find_or_insert(dict, 5, (const unsigned char *) "test1");
  ASSERT(dict[0] == NULL);
  ASSERT(dict[1] == NULL);
  ASSERT(dict[2] != NULL);
  ASSERT(dict[3] != NULL);
  ASSERT(dict[4] != NULL);
  ASSERT(p->count == 1);
  ++p->count;


  // register another occurrence of "test300"
  p = wc_dict_find_or_insert(dict, 5, (const unsigned char *) "test300");
  ASSERT(dict[0] == NULL);
  ASSERT(dict[1] == NULL);
  ASSERT(dict[2] != NULL);
  ASSERT(dict[3] != NULL);
  ASSERT(dict[4] != NULL);
  ASSERT(p->count == 1);
  ++p->count;
  
  // register another occurrence of "trucker"
  p = wc_dict_find_or_insert(dict, 5, (const unsigned char *) "trucker");
  ASSERT(dict[0] == NULL);
  ASSERT(dict[1] == NULL);
  ASSERT(dict[2] != NULL);
  ASSERT(dict[3] != NULL);
  ASSERT(dict[4] != NULL);
  ASSERT(p->count == 1);
  ++p->count;

  // register another occurrence of "trucker"
  p = wc_dict_find_or_insert(dict, 5, (const unsigned char *) "trucker");
  ASSERT(dict[0] == NULL);
  ASSERT(dict[1] == NULL);
  ASSERT(dict[2] != NULL);
  ASSERT(dict[3] != NULL);
  ASSERT(dict[4] != NULL);
  ASSERT(p->count == 2);
  ++p->count;
  

  wc_free_chain(dict[0]);
  wc_free_chain(dict[1]);
  wc_free_chain(dict[2]);
  wc_free_chain(dict[3]);
  wc_free_chain(dict[4]);
}

void test_free_chain(TestObjs *objs) {
  (void) objs;

  // Note that there are no assertions in this
  // test function. To determine whether wc_free_chain
  // works correctly, run this unit test using valgrind
  // and make sure that there is no memory leak.

  struct WordEntry *p;

  // build a linked list
  p = calloc(1, sizeof(struct WordEntry));
  p->next = calloc(1, sizeof(struct WordEntry));
  p->next->next = calloc(1, sizeof(struct WordEntry));
  p->next->next->next = calloc(1, sizeof(struct WordEntry));

  // store some data in the nodes (not really necessary)
  strcpy((char *) p->word, "foo");
  p->count = 1;
  strcpy((char *) p->next->word, "bar");
  p->count = 2;
  strcpy((char *) p->next->next->word, "baz");
  p->count = 3;
  strcpy((char *) p->next->next->next->word, "thud");
  p->count = 4;

  wc_free_chain(p);
}
