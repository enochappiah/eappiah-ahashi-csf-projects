#include <stdio.h>
#include <stdlib.h>
#include "tctest.h"

#include "uint256.h"

typedef struct {
  UInt256 zero; // the value equal to 0
  UInt256 one;  // the value equal to 1
  UInt256 max;  // the value equal to (2^256)-1
  UInt256 msb_set; // the value equal to 2^255
  UInt256 rot; // value used to test rotations
} TestObjs;

// Helper functions for implementing tests
void set_all(UInt256 *val, uint32_t wordval);

#define ASSERT_SAME(expected, actual) \
do { \
  ASSERT(expected.data[0] == actual.data[0]); \
  ASSERT(expected.data[1] == actual.data[1]); \
  ASSERT(expected.data[2] == actual.data[2]); \
  ASSERT(expected.data[3] == actual.data[3]); \
  ASSERT(expected.data[4] == actual.data[4]); \
  ASSERT(expected.data[5] == actual.data[5]); \
  ASSERT(expected.data[6] == actual.data[6]); \
  ASSERT(expected.data[7] == actual.data[7]); \
} while (0)

#define INIT_FROM_ARR(val, arr) \
do { \
  for (unsigned i = 0; i < 8; ++i) \
    val.data[i] = arr[i]; \
} while (0)

// Functions to create and cleanup the test fixture object
TestObjs *setup(void);
void cleanup(TestObjs *objs);

// Declarations of test functions
void test_get_bits(TestObjs *objs);
void test_create_from_u32(TestObjs *objs);
void test_create(TestObjs *objs);
void test_create_from_hex(TestObjs *objs);
void test_format_as_hex(TestObjs *objs);
void test_add(TestObjs *objs);
void test_sub(TestObjs *objs);
void test_negate(TestObjs *objs);
void test_rotate_left(TestObjs *objs);
void test_rotate_right(TestObjs *objs);
void test_zero_addition(TestObjs *objs);
void test_zero_sub(TestObjs *objs);
void test_add_overflow(TestObjs *objs);
void test_sub_overflow(TestObjs *objs);
void test_add_with_fact(TestObjs *objs);
void test_sub_with_fact(TestObjs *objs);



int main(int argc, char **argv) {
  if (argc > 1) {
    tctest_testname_to_execute = argv[1];
  }

  TEST_INIT();

  TEST(test_get_bits); // DONE
  TEST(test_create_from_u32); //DONE
  TEST(test_create); //DONE
  TEST(test_create_from_hex);
  TEST(test_format_as_hex);
  TEST(test_add);
  TEST(test_sub);
  TEST(test_negate);
  TEST(test_rotate_left);
  TEST(test_rotate_right);
  TEST(test_zero_addition);
  TEST(test_zero_sub);
  TEST(test_add_overflow);
  TEST(test_sub_overflow);
  TEST(test_add_with_fact);
  TEST(test_sub_with_fact);

  TEST_FINI();
}

// Set all of the "words" of a UInt256 to a specific initial value
void set_all(UInt256 *val, uint32_t wordval) {
  for (unsigned i = 0; i < 8; ++i) {
    val->data[i] = wordval;
  }
}

TestObjs *setup(void) {
  TestObjs *objs = (TestObjs *) malloc(sizeof(TestObjs));

  // initialize several UInt256 values "manually"
  set_all(&objs->zero, 0);
  set_all(&objs->one, 0);
  objs->one.data[0] = 1U;
  set_all(&objs->max, 0xFFFFFFFFU);

  // create a value with only the most-significant bit set
  uint32_t msb_set_data[8] = { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0x80000000U };
  INIT_FROM_ARR(objs->msb_set, msb_set_data);

  // value with nonzero values in least significant and most significant words
  // Note that the hex representation of this value is
  //   CD000000 00000000 00000000 00000000 00000000 00000000 00000000 000000AB
  uint32_t rot_data[8] = { 0x000000ABU, 0U, 0U, 0U, 0U, 0U, 0U, 0xCD000000U };
  INIT_FROM_ARR(objs->rot, rot_data);

  return objs;
}

void cleanup(TestObjs *objs) {
  free(objs);
}

void test_get_bits(TestObjs *objs) { 
  ASSERT(0U == uint256_get_bits(objs->zero, 0));
  ASSERT(0U == uint256_get_bits(objs->zero, 1));
  ASSERT(0U == uint256_get_bits(objs->zero, 2));
  ASSERT(0U == uint256_get_bits(objs->zero, 3));
  ASSERT(0U == uint256_get_bits(objs->zero, 4));
  ASSERT(0U == uint256_get_bits(objs->zero, 5));
  ASSERT(0U == uint256_get_bits(objs->zero, 6));
  ASSERT(0U == uint256_get_bits(objs->zero, 7));

  ASSERT(1U == uint256_get_bits(objs->one, 0));
  ASSERT(0U == uint256_get_bits(objs->one, 1));
  ASSERT(0U == uint256_get_bits(objs->one, 2));
  ASSERT(0U == uint256_get_bits(objs->one, 3));
  ASSERT(0U == uint256_get_bits(objs->one, 4));
  ASSERT(0U == uint256_get_bits(objs->one, 5));
  ASSERT(0U == uint256_get_bits(objs->one, 6));
  ASSERT(0U == uint256_get_bits(objs->one, 7));

  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 0));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 1));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 2));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 3));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 4));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 5));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 6));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 7));
}

void test_create_from_u32(TestObjs *objs) {
  UInt256 zero = uint256_create_from_u32(0U);
  UInt256 one = uint256_create_from_u32(1U);

  ASSERT_SAME(objs->zero, zero);
  ASSERT_SAME(objs->one, one);
}

void test_create(TestObjs *objs) {
  (void) objs;

  uint32_t data1[8] = { 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U };
  UInt256 val1 = uint256_create(data1);
  ASSERT(1U == val1.data[0]);
  ASSERT(2U == val1.data[1]);
  ASSERT(3U == val1.data[2]);
  ASSERT(4U == val1.data[3]);
  ASSERT(5U == val1.data[4]);
  ASSERT(6U == val1.data[5]);
  ASSERT(7U == val1.data[6]);
  ASSERT(8U == val1.data[7]);
}

void test_create_from_hex(TestObjs *objs) { //TODO
  UInt256 zero = uint256_create_from_hex("0");
  ASSERT_SAME(objs->zero, zero);

  UInt256 one = uint256_create_from_hex("1");
  ASSERT_SAME(objs->one, one);

  UInt256 max = uint256_create_from_hex("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
  ASSERT_SAME(objs->max, max);
}

void test_format_as_hex(TestObjs *objs) { //TODO
  char *s;

  s = uint256_format_as_hex(objs->zero);
  ASSERT(0 == strcmp("0", s));
  free(s);

  s = uint256_format_as_hex(objs->one);
  ASSERT(0 == strcmp("1", s));
  free(s);

  s = uint256_format_as_hex(objs->max);
  ASSERT(0 == strcmp("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", s));
  free(s);
}

void test_add(TestObjs *objs) { //TODO
  UInt256 result;

  result = uint256_add(objs->zero, objs->zero);
  ASSERT_SAME(objs->zero, result);

  result = uint256_add(objs->zero, objs->one);
  ASSERT_SAME(objs->one, result);

  uint32_t two_data[8] = { 2U };
  UInt256 two;
  INIT_FROM_ARR(two, two_data);
  result = uint256_add(objs->one, objs->one);
  ASSERT_SAME(two, result);

  result = uint256_add(objs->max, objs->one);
  ASSERT_SAME(objs->zero, result);
}



void test_sub(TestObjs *objs) { //TODO
  UInt256 result;

  result = uint256_sub(objs->zero, objs->zero);
  ASSERT_SAME(objs->zero, result);

  result = uint256_sub(objs->one, objs->one);
  ASSERT_SAME(objs->zero, result);

  result = uint256_sub(objs->zero, objs->one);
  ASSERT_SAME(objs->max, result);
}

void test_negate(TestObjs *objs) { //TODO
  UInt256 result;

  result = uint256_negate(objs->zero);
  ASSERT_SAME(objs->zero, result);

  result = uint256_negate(objs->one);
  ASSERT_SAME(objs->max, result);

  result = uint256_negate(objs->max);
  ASSERT_SAME(objs->one, result);
}

void test_rotate_left(TestObjs *objs) { //TODO
  UInt256 result;

  // rotating the value with just the most significant bit set
  // one position to the left should result in the value equal to 1
  // (i.e., the value with only the least significant bit set)
  result = uint256_rotate_left(objs->msb_set, 1);
  ASSERT_SAME(objs->one, result);

  // after rotating the "rot" value left by 4 bits, the resulting value should be
  //   D0000000 00000000 00000000 00000000 00000000 00000000 00000000 00000ABC
  result = uint256_rotate_left(objs->rot, 4);
  ASSERT(0x00000ABCU == result.data[0]);
  ASSERT(0U == result.data[1]);
  ASSERT(0U == result.data[2]);
  ASSERT(0U == result.data[3]);
  ASSERT(0U == result.data[4]);
  ASSERT(0U == result.data[5]);
  ASSERT(0U == result.data[6]);
  ASSERT(0xD0000000U == result.data[7]);

  //try to rotate by 0
  result = uint256_rotate_left(objs->msb_set, 0);
  ASSERT_SAME(objs->msb_set, result);

  // try to rotate by 256
  result = uint256_rotate_left(objs->msb_set, 256);
  ASSERT_SAME(objs->msb_set, result);

  // try to rotate the number zero
  result = uint256_rotate_left(objs->zero, 123); // You can pick any number for rotation
  ASSERT_SAME(objs->zero, result);

  // try to rotate one by 255 bits
  result = uint256_rotate_left(objs->one, 255);
  ASSERT_SAME(objs->msb_set, result);

  // try to rotate by 1024 which is like rotating by 256 four different times
  result = uint256_rotate_left(objs->msb_set, 1024);
  ASSERT_SAME(objs->msb_set, result);
}

void test_rotate_right(TestObjs *objs) { //TODO
  UInt256 result;

  // rotating 1 right by 1 position should result in a value with just
  // the most-significant bit set
  result = uint256_rotate_right(objs->one, 1);
  ASSERT_SAME(objs->msb_set, result);

  // after rotating the "rot" value right by 4 bits, the resulting value should be
  //   BCD00000 00000000 00000000 00000000 00000000 00000000 00000000 0000000A
  result = uint256_rotate_right(objs->rot, 4);
  ASSERT(0x0000000AU == result.data[0]);
  ASSERT(0U == result.data[1]);
  ASSERT(0U == result.data[2]);
  ASSERT(0U == result.data[3]);
  ASSERT(0U == result.data[4]);
  ASSERT(0U == result.data[5]);
  ASSERT(0U == result.data[6]);
  ASSERT(0xBCD00000U == result.data[7]);

  // try to rotate by 0
result = uint256_rotate_right(objs->msb_set, 0);
ASSERT_SAME(objs->msb_set, result);

// try to rotate by 256
result = uint256_rotate_right(objs->msb_set, 256);
ASSERT_SAME(objs->msb_set, result);

// try to rotate the number zero
result = uint256_rotate_right(objs->zero, 123); // You can pick any number for rotation
ASSERT_SAME(objs->zero, result);

// try to rotate by 1024 which is like rotating by 256 four different times
result = uint256_rotate_right(objs->msb_set, 1024);
ASSERT_SAME(objs->msb_set, result);

}

void test_zero_addition(TestObjs *objs) {
  UInt256 result;

  result = uint256_add(objs->one, objs->zero);
  ASSERT_SAME(objs->one, result);
  
  result = uint256_add(result, objs->zero);
  ASSERT_SAME(objs->one, result); //testing 1 + 0 + 0 = 1

  result = uint256_add(objs->zero, objs->one);
  
  ASSERT(1U == result.data[0]);
  ASSERT(0U == result.data[1]);
  ASSERT(0U == result.data[2]);
  ASSERT(0U == result.data[3]);
  ASSERT(0U == result.data[4]);
  ASSERT(0U == result.data[5]);
  ASSERT(0U == result.data[6]);
  ASSERT(0U == result.data[7]);
  ASSERT_SAME(objs->one, result);

  uint32_t two_data[8] = { 2U };
  UInt256 two;
  INIT_FROM_ARR(two, two_data);
  result = uint256_add(two, objs->zero);
  ASSERT_SAME(two, result);

  result = uint256_add(objs->max, objs->zero);
  ASSERT_SAME(objs->max, result);

}

void test_zero_sub(TestObjs *objs) {
  UInt256 result;

  result = uint256_sub(objs->one, objs->zero);
  ASSERT_SAME(objs->one, result);
  
  result = uint256_sub(result, objs->zero);
  ASSERT_SAME(objs->one, result); //testing 1 - 0 - 0 = 1

  result = uint256_sub(objs->zero, objs->one);
  
  ASSERT(0xFFFFFFFFU == result.data[0]);
  ASSERT(0xFFFFFFFFU == result.data[1]);
  ASSERT(0xFFFFFFFFU == result.data[2]);
  ASSERT(0xFFFFFFFFU == result.data[3]);
  ASSERT(0xFFFFFFFFU == result.data[4]);
  ASSERT(0xFFFFFFFFU == result.data[5]);
  ASSERT(0xFFFFFFFFU == result.data[6]);
  ASSERT(0xFFFFFFFFU == result.data[7]);
  ASSERT_SAME(objs->max, result);

  uint32_t two_data[8] = { 2U };
  UInt256 two;
  INIT_FROM_ARR(two, two_data);
  result = uint256_sub(two, objs->zero);
  ASSERT_SAME(two, result);

  result = uint256_sub(objs->max, objs->zero);
  ASSERT_SAME(objs->max, result);
}

// ensure overflow happens when adding one to maxVal
void test_add_overflow(TestObjs *objs) {
  UInt256 result = uint256_add(objs->max, objs->one);

  ASSERT(result.data[7] == 0U);
  ASSERT(result.data[6] == 0U);
  ASSERT(result.data[5] == 0U);
  ASSERT(result.data[4] == 0U);
  ASSERT(result.data[3] == 0U);
  ASSERT(result.data[2] == 0U);
  ASSERT(result.data[1] == 0U);
  ASSERT(result.data[0] == 0U);
}

// ensure overflow happens when subtracting one from zero
void test_sub_overflow(TestObjs *objs) {
  UInt256 result = uint256_sub(objs->zero, objs->one);
  ASSERT(0xFFFFFFFFU == result.data[0]);
  ASSERT(0xFFFFFFFFU == result.data[1]);
  ASSERT(0xFFFFFFFFU == result.data[2]);
  ASSERT(0xFFFFFFFFU == result.data[3]);
  ASSERT(0xFFFFFFFFU == result.data[4]);
  ASSERT(0xFFFFFFFFU == result.data[5]);
  ASSERT(0xFFFFFFFFU == result.data[6]);
  ASSERT(0xFFFFFFFFU == result.data[7]);

}

// used genfact to test addition
void test_add_with_fact(TestObjs *objs) {
  UInt256 left, right, result;

  left.data[0] = 0xbd3f2275U;
  left.data[1] = 0xdaade3feU;
  left.data[2] = 0x8f8991d5U;
  left.data[3] = 0x4b5feaa9U;
  left.data[4] = 0x19448805U;
  left.data[5] = 0x525c1526U;
  left.data[6] = 0x4719744bU;
  left.data[7] = 0x50a3b0bcU;
  right.data[0] = 0x90066acbU;
  right.data[1] = 0xf3e33d14U;
  right.data[2] = 0x39edfa60U;
  right.data[3] = 0xb3da172dU;
  right.data[4] = 0x2b92cedeU;
  right.data[5] = 0x2594beccU;
  right.data[6] = 0x0d571731U;
  right.data[7] = 0x996fc7bdU;
  result = uint256_add(left, right);
  ASSERT(0x4d458d40U == result.data[0]);
  ASSERT(0xce912113U == result.data[1]);
  ASSERT(0xc9778c36U == result.data[2]);
  ASSERT(0xff3a01d6U == result.data[3]);
  ASSERT(0x44d756e3U == result.data[4]);
  ASSERT(0x77f0d3f2U == result.data[5]);
  ASSERT(0x54708b7cU == result.data[6]);
  ASSERT(0xea137879U == result.data[7]);
}

// used genfact to test with subtraction
void test_sub_with_fact(TestObjs *objs) {
  UInt256 left, right, result;

  left.data[0] = 0x4d458d40U;
  left.data[1] = 0xce912113U;
  left.data[2] = 0xc9778c36U;
  left.data[3] = 0xff3a01d6U;
  left.data[4] = 0x44d756e3U;
  left.data[5] = 0x77f0d3f2U;
  left.data[6] = 0x54708b7cU;
  left.data[7] = 0xea137879U;
  right.data[0] = 0x90066acbU;
  right.data[1] = 0xf3e33d14U;
  right.data[2] = 0x39edfa60U;
  right.data[3] = 0xb3da172dU;
  right.data[4] = 0x2b92cedeU;
  right.data[5] = 0x2594beccU;
  right.data[6] = 0x0d571731U;
  right.data[7] = 0x996fc7bdU;
  result = uint256_sub(left, right);
  ASSERT(0xbd3f2275U == result.data[0]);
  ASSERT(0xdaade3feU == result.data[1]);
  ASSERT(0x8f8991d5U == result.data[2]);
  ASSERT(0x4b5feaa9U == result.data[3]);
  ASSERT(0x19448805U == result.data[4]);
  ASSERT(0x525c1526U == result.data[5]);
  ASSERT(0x4719744bU == result.data[6]);
  ASSERT(0x50a3b0bcU == result.data[7]);
}