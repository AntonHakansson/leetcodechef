static struct { char *in; char *out; } test_cases[] = {
  {
    .in =
    "3 1\n"
    "1 2 1\n",
    .out = "2\n",
  },
  {
    .in =
"6 5\n"
"1 2 1\n"
"2 3 2\n"
"1 3 3\n"
"4 5 4\n"
"5 6 5\n",
    .out = "2\n",
  },
  {
    .in =
     "100000 1\n"
     "1 100000 100\n",
    .out = "99999\n",
  },
  {
    .in =
"3 1\n"
"1 2 1\n",
    .out = "2\n",
  },
  {
    .in =
     "10 5\n"
     "5 7 1\n"
     "1 2 0\n"
     "2 3 1\n"
     "10 1 1\n"
     "7 8 1\n",
    .out = "5\n",
  },
  {
    .in =
     "4 3\n"
     "1 2 2\n"
     "2 3 2\n"
     "3 4 2\n",
    .out = "1\n",
  },
  {
    .in =
     "5 4\n"
     "5 4 2\n"
     "1 2 2\n"
     "1 3 2\n"
     "1 4 2\n",
    .out = "1\n",
  },
};

static void pprint2D(U64 *a, Size count, Size stride) {
  for (Size row = 0; row < count; row++) {
    for (Size col = 0; col < stride; col++) {
      printf("%lu ", a[row * stride + col]);
    }
    printf("\n");
  }
  printf("\n");
}

int main(int argc, char **argv)
{
  void *heap = malloc(HEAP_CAP);
  Arena arena = (Arena){heap, heap + HEAP_CAP};
  for (Size i = 0; i < count_of(test_cases); i++) {
    Write_Buffer reader = { .buf = test_cases[i].in, .at = test_cases[i].in };
    Write_Buffer writer = { .cap = 1ll << 13 };
    writer.buf = writer.at = new (&arena, U8, writer.cap);
    run(arena, &reader, &writer);
    if (__builtin_strcmp(writer.buf, test_cases[i].out) != 0) {
      fprintf(stderr, "==Failed case %d==\n%sExpected: %sGot: %s", i, test_cases[i].in, test_cases[i].out, writer.buf);
    }
  }
  return 0;
}
