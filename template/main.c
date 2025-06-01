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
#define putchar putchar_unlocked
static I64 nextlong(void);
static U8 *nextstr(Arena *arena, Iz *out_len);

void run(Arena *arena) {
  I64 N = nextlong();
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
      .input = "",
      .expected = "",
    },
    {
      .input = "",
      .expected = "",
    },
  };

  for (Iz test_i = 0; test_i < 1; test_i++) {
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

static U8 *nextstr(Arena *arena, Iz *out_len) {
  int c = getchar_unlocked();
  while (c == ' ' || c == '\n') c = getchar_unlocked();
  Iz len = 0;
  while (c != ' ' && c != '\n' && c != EOF) {
    *(arena->beg)++ = (char)c;
    len++;
    c = getchar_unlocked();
  }
  *(arena->beg)++ = '\0';
  if (out_len) {
    *out_len = len;
  }
  return arena->beg - 1 - len;
}
