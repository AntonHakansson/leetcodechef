#if IN_SHELL /* $ bash main.c
 cc main.c -fsanitize=undefined,address -g3 -O0 -Wall -Wextra -Wconversion -Wno-sign-conversion $@
exit # */
#endif

#define assert(c)     while((!(c))) __builtin_trap()
#define breakpoint(c) ((c) ? ({ asm volatile ("int3; nop"); }) : 0)
#define new(a, t, n)  ((t *) arena_alloc(a, (Iz)sizeof(t), (Iz)_Alignof(t), (n)))
#define countof(s)    (Iz)(sizeof((s)) / sizeof(*(s)))

typedef unsigned char       U8;
typedef signed   long long  I64;
typedef typeof((char *)0-(char *)0)  Iz;
typedef typeof((sizeof(0)))          Uz;

typedef struct {U8 *beg, *end;} Arena;
static U8 *arena_alloc(Arena *a, Iz objsize, Iz align, Iz count);

#include <stdio.h>
static I64 nextlong(void);

#define swap(t, a, b) do { t tmp = a; a = b; b = tmp; } while(0);
static Iz union_find(I64 *parent, Iz i) {
  while (i != parent[i]) {
    parent[i] = parent[parent[i]];
    i = parent[i];
  }
  return i;
}
static I64 union_join(I64 *restrict parent, I64 *restrict rank, I64 a, I64 b) {
  if (a == b) return rank[a];
  if (rank[a] < rank[b]) { swap(I64, a, b); }
  parent[b] = a;
  rank[a] += rank[b];
  return rank[a];
}

void run(Arena *arena) {
  I64 N = nextlong();
  I64 M = nextlong();
  I64 *A = new(arena, I64, M);
  I64 *B = new (arena, I64, M);
  for (Iz i = 0; i < M; i++) {
    A[i] = nextlong();
    B[i] = nextlong();
  }

  I64 *parent = new(arena, I64, N + 1);
  I64 *rank = new(arena, I64, N + 1);
  for (Iz i = 1; i <= N; i++) { parent[i] = i; rank[i] = 1; }

  // dp(i) :: inconvenience for broken bridges up to i (inclusive)
  I64 *dp = new(arena, I64, M);
  I64 ans = N * (N - 1) / 2; // No edges -> N choose 2;
  for (Iz i = M - 1; i >= 0; i--) {
    dp[i] = ans;

    I64 union_a = union_find(parent, A[i]);
    I64 union_b = union_find(parent, B[i]);
    if (union_a != union_b) {
      ans -= rank[union_a] * rank[union_b];
      union_join(parent, rank, union_a, union_b);
    }
  }

  for (Iz i = 0; i < M; i++) {
    printf("%lld\n", dp[i]);
  }
}

#include <stdlib.h>
#ifdef ONLINE_JUDGE
int main() {
  enum { HEAP_CAP = 1 << 28, };
  Arena arena[1] = { { .beg = malloc(HEAP_CAP), .end = arena->beg + HEAP_CAP, } };
  run(arena);
  return 0;
}
#else
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
int main() {
  enum { HEAP_CAP = 1 << 28, };
  Arena arena[1] = { { .beg = malloc(HEAP_CAP), .end = arena->beg + HEAP_CAP, } };
  Arena arena_rewind = *arena;

  int input_pipe[2], output_pipe[2];
  assert(pipe(input_pipe) == 0 && pipe(output_pipe) == 0);
  dup2(input_pipe[0], STDIN_FILENO);
  dup2(output_pipe[1], STDOUT_FILENO);

  struct {
    char *input;
    char *expected;
  } tests[] = {
    {
      .input = "4 5\n1 2\n3 4\n1 3\n2 3\n1 4\n",
      .expected = "0\n0\n4\n5\n6\n",
    },
    {
      .input = "6 5\n2 3\n1 2\n5 6\n3 4\n4 5\n",
      .expected = "8\n9\n12\n14\n15\n",
    },
    {
      .input = "2 1\n1 2\n",
      .expected = "1\n",
    },
  };

  for (Iz test_i = 0; test_i < countof(tests); test_i++) {
    *arena = arena_rewind;

    write(input_pipe[1], tests[test_i].input, strlen(tests[test_i].input));
    close(input_pipe[1]);
      run(arena);
    close(output_pipe[1]);
    fflush(NULL);

    char buf[4096] = {0};
    Iz l = read(output_pipe[0], buf, sizeof(buf) - 1);
    buf[l] = 0;

    if (__builtin_strcmp(tests[test_i].expected, buf) != 0) {
      fprintf(stderr, "==Failed case %ld==\n%sExpected: %s\nGot: %s",
              test_i, tests[test_i].input, tests[test_i].expected, buf);
      breakpoint(1);
    }
    close(input_pipe[0]);
    assert(pipe(input_pipe) == 0);
    dup2(input_pipe[0], STDIN_FILENO);
    close(output_pipe[0]);
    assert(pipe(output_pipe) == 0);
    dup2(output_pipe[1], STDOUT_FILENO);
  }

  return 0;
}
#endif

static U8 *arena_alloc(Arena *a, Iz objsize, Iz align, Iz count) {
  a->beg += -(Uz)(a->beg) & (align - 1); // align
  U8 *p = __builtin_memset(a->beg, 0, objsize * count);
  a->beg += objsize * count;
  return p;
}

static I64 nextlong(void) {
  char c = (char)getchar_unlocked();
  while (c != '-' && (c < '0' || '9' < c)) c = (char)getchar_unlocked();
  int s = 0;
  if (c == '-') {
    s = 1;
    c = (char)getchar_unlocked();
  }
  I64 x = 0;
  while ('0' <= c && c <= '9') {
    x = x * 10 + c - '0';
    c = (char)getchar_unlocked();
  }
  return s ? -x : x;
}
