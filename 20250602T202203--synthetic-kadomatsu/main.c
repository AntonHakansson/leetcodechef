#if IN_SHELL /* $ bash main.c
cc main.c -fsanitize=undefined -g3 -O0 -Wall -Wextra -Wconversion -Wno-sign-conversion $@
exit # */
#endif

#define assert(c)     while((!(c))) __builtin_trap()
#define breakpoint(c) ((c) ? ({ asm volatile ("int3; nop"); }) : 0)
#define new(a, t, n)  ((t *) arena_alloc(a, (Iz)sizeof(t), (Iz)_Alignof(t), (n)))
#define countof(s)    (Iz)(sizeof((s)) / sizeof(*(s)))

typedef unsigned char       U8;
typedef signed   long long  I64;
typedef typeof((char *)0-(char *)0)  Iz;

typedef struct { U8 *beg, *end; } Arena;
static U8 *arena_alloc(Arena *a, Iz objsize, Iz align, Iz count);

#include <stdio.h>
static I64 nextlong(void);

#define min(a, b) (((a) < (b)) ? (a) : (b))

void run(Arena *arena) {
  I64 N = nextlong();
  I64 A = nextlong();
  I64 B = nextlong();
  I64 C = nextlong();
  I64 *ls = new(arena, I64, N);
  for (Iz i = 0; i < N; i++) { ls[i] = nextlong(); }

  I64 solve(Iz i, I64 a, I64 b, I64 c) {
    if (i >= N) {
      if (min(min(a, b), c) > 0) {
        // Optimally use extensian/shortening magic to satisfy bamboo of lenghts A, B, C
        return __builtin_llabs(a - A) + __builtin_llabs(b - B) + __builtin_llabs(c - C) - 30;
      } else {
        return (1ll << 62);
      }
    }

    I64 candidates[4] = {
      solve(i + 1, a, b, c) + 0, // don't use
      solve(i + 1, a + ls[i], b,         c        ) + 10, // use as "material" for A
      solve(i + 1, a,         b + ls[i], c        ) + 10, // use as "material" for B
      solve(i + 1, a,         b,         c + ls[i]) + 10, // use as "material" for C
    };

    I64 result = (1ll << 62);
    for (Iz i = 0; i < countof(candidates); i++) result = min(result, candidates[i]);
    return result;
  }

  I64 result = solve(0, 0, 0, 0);
  printf("%lld\n", result);
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
      .input = "5 100 90 80\n98\n40\n30\n21\n80\n",
      .expected = "23\n",
    },
    {
      .input = "8 100 90 80\n100\n100\n90\n90\n90\n80\n80\n80\n",
      .expected = "0\n",
    },
    {
      .input = "8 1000 800 100\n300\n333\n400\n444\n500\n555\n600\n666\n",
      .expected = "243\n",
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
  a->beg += -(unsigned long long)(a->beg) & (align - 1); // align
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
