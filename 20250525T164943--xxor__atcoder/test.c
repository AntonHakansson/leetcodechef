#if IN_SHELL /* $ bash test.c
cc test.c -o test -fsanitize=undefined -g3 -O0 -Wall -Wextra -Wconversion -Wno-sign-conversion $@
exit # */
#endif

#include "main.c"

static struct {
  char *in;
  char *out;
}
test_cases[] = {
  {
    .in  = "3 7\n"
           "1 6 3\n",
    .out = "14\n",
  },
  {
    .in  = "4 9\n7 4 0 3\n",
    .out = "46\n",
  },
  {
    .in  = "1 0\n1000000000000\n",
    .out = "1000000000000\n",
  },
  {
    .in  = "4 6\n1 5 4 7\n",
    .out = "21\n",
  },
};

#include <stdio.h>
int main() {
  enum { HEAP_CAP = 1ll << 30 };
  void *heap = malloc(HEAP_CAP);
  Arena arena = (Arena){heap, heap + HEAP_CAP};
  for (Size i = 0; i < countof(test_cases); i++) {
    if (__builtin_strlen(test_cases[i].in) == 0) continue;
    Arena temp = arena;
    Stream reader = {0}; {
      reader.beg = reader.at = (U8 *)test_cases[i].in;
      reader.end = reader.beg + __builtin_strlen(test_cases[i].in);
    }
    Stream writer = stream(&temp, (1ll << 13), flush);
    run(temp, &reader, &writer);
    flush(&writer);
    if (__builtin_strcmp((char *)writer.beg, test_cases[i].out) != 0) {
      fprintf(stderr, "==Failed case %ld==\n%sExpected: %sGot: %s", i, test_cases[i].in, test_cases[i].out, writer.beg);
    }
  }
  free(heap);
  return 0;
}
