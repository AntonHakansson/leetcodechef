// O(N^3)
static I64 is_palindrome(I64 *a, Size i, Size j) {
  Size len = j - i + 1;
  for (Size it = 0; it < (len / 2); it++) {
    if (a[i + it] != a[j - it]) return 0;
  }
  return 1;
}
static void run_slow(Arena arena, Write_Buffer * restrict reader, Write_Buffer * restrict writer) {
  Size N = expect_i64(reader);
  I64 *a = new(&arena, typeof(*a), N);
  for (Size i = 0; i < N; i++) { a[i] = expect_i64(reader); }

  // dp[i..N] :: lowest C that a[i..N] can be broken into C contiguous sub-arrays which
  // are palindromes.
  U64 *dp = new (&arena, U64, N + 1) + 1;
  for (Size i = 0; i < N; i++) {
    dp[i] = (1ll << 62);
    for (Size j = 0; j < i + 1; j++) {
      I64 is_palin = is_palindrome(a, j, i);
      U64 ans = (1ll << 62);
      if (is_palin) { ans = dp[j - 1] + 1; }
      dp[i] = min(dp[i], ans);
    }
  }
  I64 result = dp[N - 1];
  append_i64(writer, result, '\n');
}

static struct { char *in; char *out; } test_cases[] = {
  {
    .in =
        "5\n"
        "34 45 34 56 34\n",
    .out = "3\n",
  },
  {
    .in =
        "7\n"
        "1 2 1 3 3 1 2\n",
    .out = "2\n",
  },
  {
    .in =
        "7\n"
        "2 1 3 3 1 2 1\n",
    .out = "2\n",
  },
  {
    .in =
        "13\n"
        "1 2 1 3 3 1 2 1 1 1 3 1 2\n",
    .out = "4\n",
  },
  {
    .in =
        "13\n"
        "1 2 1 3 3 1 2 1 1 1 3 3 3\n",
    .out = "3\n",
  },
  {
    .in =
        "100\n"
        "1 2 1 3 3 1 2 1 1 7 1 7 7 2 9 6 6 4 1 2 3 1 9 1 3 2 1 4 6 6 9 2 7 7 1 7 1 1 1 2 1 3 3 1 2 7 4 8 4 9 4 8 4 7 1 2 1 3 3 1 2 7 9 9 4 2 9 3 9 2 4 9 9 7 1 2 1 3 3 1 2 3 7 8 2 6 5 7 7 7 5 6 2 8 7 3 2 1 3 3\n",
    .out = "10\n",
  },
};

void pprint2D(U64 *a, Size count, Size stride) {
  for (Size row = 0; row < count; row++) {
    for (Size col = 0; col < stride; col++) {
      printf("%lu ", a[row * stride + col]);
    }
    printf("\n");
  }
  printf("\n");
}

int main(int argc, char **argv) {
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
