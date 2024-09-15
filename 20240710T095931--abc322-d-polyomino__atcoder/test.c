static struct { char *in; char *out;
} test_cases[] = {
    {
        .in = "....\n###.\n.#..\n....\n....\n.###\n.##.\n....\n..#.\n.##.\n.##."
              "\n.##.\n",
        .out = "Yes\n",
    },
    {
      .in = "###.\n#.#.\n##..\n....\n....\n..#.\n....\n....\n####\n##..\n#...\n#...\n",
      .out = "Yes\n",
    },
};

#include <stdio.h>
int main(int argc, char **argv)
{
  void *heap = malloc(HEAP_CAP);
  Arena arena = (Arena){heap, heap + HEAP_CAP};
  for (Size i = 0; i < countof(test_cases); i++) {
    Arena temp = arena;
    Stream reader = {0}; {
      reader.beg = reader.at = test_cases[i].in;
      reader.end = reader.beg + __builtin_strlen(test_cases[i].in);
    }
    Stream writer = stream(&temp, (1ll << 13), flush);
    run(temp, &reader, &writer);
    flush(&writer);
    if (__builtin_strcmp(writer.beg, test_cases[i].out) != 0) {
      fprintf(stderr, "==Failed case %ld==\n%sExpected: %sGot: %s", i, test_cases[i].in, test_cases[i].out, writer.beg);
    }
  }
  free(heap);
  return 0;
}
