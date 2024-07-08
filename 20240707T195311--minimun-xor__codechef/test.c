static struct { char *in; char *out; } test_cases[] = {
  {
    .in = "3\n4\n2 4 3 6\n2\n4 4\n5\n1 3 5 17 9\n",
    .out = "0\n0\n14\n",
  },
};

static void pprint2D(Stream *writer, U64 *a, Size count, Size stride) {
  for (Size row = 0; row < count; row++) {
    for (Size col = 0; col < stride; col++) {
      write_i64(writer, a[row * stride + col], ' ');
    }
    *(writer->at++) = '\n';
  }
  *(writer->at++) = '\n';
}

#include <stdio.h>
int main(int argc, char **argv)
{
  void *heap = malloc(HEAP_CAP);
  Arena arena = (Arena){heap, heap + HEAP_CAP};
  for (Size i = 0; i < count_of(test_cases); i++) {
    Stream reader = {0}; {
      reader.beg = reader.at = test_cases[i].in;
      reader.end = reader.beg + __builtin_strlen(test_cases[i].in);
    }
    Stream writer = stream(&arena, (1ll << 13), flush);
    run(arena, &reader, &writer);
    flush(&writer);
    if (__builtin_strcmp(writer.beg, test_cases[i].out) != 0) {
      fprintf(stderr, "==Failed case %d==\n%sExpected: %sGot: %s", i, test_cases[i].in, test_cases[i].out, writer.beg);
    }
  }
  free(heap);
  return 0;
}
