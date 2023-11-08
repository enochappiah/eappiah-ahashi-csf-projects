#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int compare_i64(const void *left_, const void *right_) {
  int64_t left = *(int64_t *)left_;
  int64_t right = *(int64_t *)right_;
  if (left < right) return -1;
  if (left > right) return 1;
  return 0;
}

void seq_sort(int64_t *arr, size_t begin, size_t end) {
  size_t num_elements = end - begin;
  qsort(arr + begin, num_elements, sizeof(int64_t), compare_i64);
}

// Merge the elements in the sorted ranges [begin, mid) and [mid, end),
// copying the result into temparr.
void merge(int64_t *arr, size_t begin, size_t mid, size_t end, int64_t *temparr) {
  int64_t *endl = arr + mid;
  int64_t *endr = arr + end;
  int64_t *left = arr + begin, *right = arr + mid, *dst = temparr;

  for (;;) {
    int at_end_l = left >= endl;
    int at_end_r = right >= endr;

    if (at_end_l && at_end_r) break;

    if (at_end_l)
      *dst++ = *right++;
    else if (at_end_r)
      *dst++ = *left++;
    else {
      int cmp = compare_i64(left, right);
      if (cmp <= 0)
        *dst++ = *left++;
      else
        *dst++ = *right++;
    }
  }
}

void fatal(const char *msg) __attribute__ ((noreturn));

void fatal(const char *msg) {
  fprintf(stderr, "Error: %s\n", msg);
  exit(1);
}

void merge_sort(int64_t *arr, size_t begin, size_t end, size_t threshold) {
  assert(end >= begin);
  size_t size = end - begin;
  size_t mid = begin + size / 2;

  if (size <= threshold) {
    seq_sort(arr, begin, end);

  } else {
    pid_t leftHalf = fork();
    if (leftHalf == -1) {
      fprintf(stderr, "Error: Left half fork failed to start child process\n");
      exit(1);
    } else if (leftHalf == 0) {
      merge_sort(arr, begin, mid, threshold);
      exit(0);
    }

    pid_t rightHalf = fork();
    if (rightHalf == -1) {
      fprintf(stderr, "Error: Right half fork failed to start child process\n");
      exit(1);
    } else if (rightHalf == 0) {
      merge_sort(arr, mid, end, threshold);
      exit(0);
    }

    int leftChildInfo;
    int rightChildInfo;
    pid_t leftWaitReturn = waitpid(leftHalf, &leftChildInfo, 0);
    pid_t rightWaitReturn = waitpid(rightHalf, &rightChildInfo, 0);
    if (leftWaitReturn == -1) {
      fprintf(stderr, "Error: Something went wrong with child process\n");
      exit(1);
    }

    if (!WIFEXITED(leftChildInfo)) {
      fprintf(stderr, "Error: Left child did not exit normally\n");
      exit(1);
    }

    if(WEXITSTATUS(leftChildInfo) != 0) {
      fprintf(stderr, "Error: Bad exit status with left child\n");
      exit(1);
    }

    if (rightWaitReturn == -1) {
      fprintf(stderr, "Error: Something went wrong with child process\n");
      exit(1);
    }

    if (!WIFEXITED(rightChildInfo)) {
      fprintf(stderr, "Error: Right child did not exit normally\n");
      exit(1);
    }

    if(WEXITSTATUS(rightChildInfo) != 0) {
      fprintf(stderr, "Error: Bad exit status with right child\n");
      exit(1);
    }
  



  // allocate temp array now, so we can avoid unnecessary work
  // if the malloc fails
  int64_t *temp_arr = (int64_t *) malloc(size * sizeof(int64_t));
  if (temp_arr == NULL)
    fatal("malloc() failed");

  // child processes completed successfully, so in theory
  // we should be able to merge their results
  merge(arr, begin, mid, end, temp_arr);

  // copy data back to main array
  for (size_t i = 0; i < size; i++)
    arr[begin + i] = temp_arr[i];

  // now we can free the temp array
  free(temp_arr);

  // success!

  }
}

int main(int argc, char **argv) {
  // check for correct number of command line arguments
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <filename> <sequential threshold>\n", argv[0]);
    return 1;
  }

  // process command line arguments
  const char *filename = argv[1];
  char *end;
  size_t threshold = (size_t) strtoul(argv[2], &end, 10);
  if (end != argv[2] + strlen(argv[2])) {
    // TODO: report an error (threshold value is invalid)
    fprintf(stderr, "Error: Threshold value is invalid\n");
    return 1;
  }

  // TODO: open the file
  int fd = open(filename, O_RDWR);
  if (fd < 0) {
    fprintf(stderr, "Error: Could not open file\n");
    return 1;
  }

  // TODO: use fstat to determine the size of the file
  struct stat statbuf;
  int rc = fstat(fd, &statbuf);
  if (rc != 0) {
    fprintf(stderr, "Error: Could not determine size of file\n");
    close(fd);
    return 1;
  }

  size_t file_size_in_bytes = statbuf.st_size;


  // TODO: map the file into memory using mmap
  int64_t *data = mmap(NULL, file_size_in_bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (data == MAP_FAILED) {
    fprintf(stderr, "Error: Could not map to memory\n");
    close(fd);
    return 1;
  }

  // TODO: sort the data!
  size_t len = file_size_in_bytes/ sizeof(int64_t);
  merge_sort(data, 0, len, threshold);

  // TODO: unmap and close the file
  if (munmap(data, file_size_in_bytes) == -1) {
    fprintf(stderr, "Error: Unmapping did not work properly\n");
    close(fd);
    return 1;
  }

  if (close(fd) == -1) {
    fprintf(stderr, "Error: Closing file produced error\n");
    return 1;
  }
  return 0;

  // TODO: exit with a 0 exit code if sort was successful
}
