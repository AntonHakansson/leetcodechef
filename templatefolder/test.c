static struct { char *in; char *out; } test_cases[] = {
  {
    .in = "\n",
    .out = "\n",
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
    Arena temp = arena;
    Stream reader = {0}; {
      reader.beg = reader.at = test_cases[i].in;
      reader.end = reader.beg + __builtin_strlen(test_cases[i].in);
    }
    Stream writer = stream(&temp, (1ll << 13), flush);
    run(temp, &reader, &writer);
    flush(&writer);
    if (__builtin_strcmp(writer.beg, test_cases[i].out) != 0) {
      fprintf(stderr, "==Failed case %d==\n%sExpected: %sGot: %s", i, test_cases[i].in, test_cases[i].out, writer.beg);
    }
  }
  free(heap);
  return 0;
}
