#if IN_SHELL /* $ bash main.c
cc main.c -fsanitize=undefined -g3 -O0 -Wall -Wextra -Wconversion -Wno-sign-conversion $@
exit # */
#endif

#define assert(c)     while((!(c))) __builtin_trap()
#define breakpoint(c) ((c) ? ({ asm volatile ("int3; nop"); }) : 0)
#define new(a, t, n)  ((t *) arena_alloc(a, (Iz)sizeof(t), (Iz)_Alignof(t), (n)))
#define countof(s)    (Iz)(sizeof((s)) / sizeof(*(s)))

typedef unsigned char       U8;
typedef unsigned long long  U64;
typedef signed   long long  I64;
typedef typeof((char *)0-(char *)0)  Iz;
typedef typeof((sizeof(0)))          Uz;

typedef struct { U8 *beg, *end; } Arena;
static U8 *arena_alloc(Arena *a, Iz objsize, Iz align, Iz count);

#include <stdio.h>
static I64 nextlong(void);

void run(Arena *arena) {
  Iz N = nextlong();
  enum { MOD = 1000000007, };
  U64 *_mem = new(arena, U64, N * 32 * 32 * 32);
  #define mem(n, l3) _mem[ (n * 32 * 32 * 32) + ((l3[0] - 'A') * 32 * 32) + ((l3[1] - 'A') * 32) + (l3[2] - 'A')]
  U64 solve(Iz n, char last3[3]) {
    if (n == N) { return 1; }
    U64 *m = &mem(n, last3);
    if (*m) { return *m; }
    U64 result = 0;
    char options[4] = "ACGT";
    for (Iz i = 0; i < countof(options); i++) {
      char last4[4] = { last3[0], last3[1], last3[2], options[i], };
      {
        if (last4[1] == 'A' && last4[2] == 'G' && last4[3] == 'C')
          continue;
        if (last4[1] == 'A' && last4[2] == 'C' && last4[3] == 'G')
          continue;
        if (last4[1] == 'G' && last4[2] == 'A' && last4[3] == 'C')
          continue;
        if (last4[0] == 'A' && last4[2] == 'G' && last4[3] == 'C')
          continue;
        if (last4[0] == 'A' && last4[1] == 'G' && last4[3] == 'C')
          continue;
      }
      result = (result + solve(n + 1, last4 + 1)) % MOD;
    }
    *m = result;
    return result;
  }
  printf("%lld\n", solve(0, "TTT"));
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
      .input = "3\n",
      .expected = "61\n",
    },
    {
      .input = "4\n",
      .expected = "230\n",
    },
    {
      .input = "100\n",
      .expected = "388130742\n",
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
