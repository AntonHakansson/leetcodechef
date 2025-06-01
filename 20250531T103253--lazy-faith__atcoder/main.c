#if IN_SHELL /* $ bash main.c
cc main.c -o main -fsanitize=undefined -g3 -O0 -Wall -Wextra -Wconversion -Wno-sign-conversion $@
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
I64 nextlong(void);
U8 *nextstr(Arena *arena, Iz *out_len);

#define min(a, b) (((a) < (b)) ? (a) : (b))
static Iz lower_bound(I64 *sorted_list, Iz n, I64 bound) {
  Iz lb = n;
  Iz l = 0, r = n - 1;
  while (l <= r) {
    Iz m = l + (r - l) / 2;
    if (bound <= sorted_list[m]) { r = m - 1; lb = m; }
    else                         { l = m + 1; }
  }
  return lb;
}

void run(Arena *arena) {
  I64 a_count = nextlong();
  I64 b_count = nextlong();
  I64 q_count = nextlong();
  I64 *a = new(arena, I64, a_count);
  I64 *b = new(arena, I64, b_count);
  I64 *q = new(arena, I64, b_count);
  for (Iz i = 0; i < a_count; i++) {a[i] = nextlong();}
  for (Iz i = 0; i < b_count; i++) {b[i] = nextlong();}
  for (Iz i = 0; i < q_count; i++) {q[i] = nextlong();}

  I64 solve_recursive(I64 pos, I64 dist_acc, _Bool try_shrine, _Bool try_temple) {
    if (!try_shrine && !try_temple) return dist_acc;

    I64 candidates[4] = {};
    Iz candidates_count = 0;

    if (try_shrine) {
      I64 nearest_shrine_i = lower_bound(a, a_count, pos);
      if (nearest_shrine_i > 0) {
        I64 npos = a[nearest_shrine_i - 1];
        candidates[candidates_count++] = solve_recursive(npos, dist_acc + __builtin_llabs(pos - npos), 0, try_temple);
      }
      if (nearest_shrine_i < a_count) {
        I64 npos = a[nearest_shrine_i - 0];
        candidates[candidates_count++] = solve_recursive(npos, dist_acc + __builtin_llabs(pos - npos), 0, try_temple);
      }
    }
    if (try_temple) {
      I64 nearest_temple_i = lower_bound(b, b_count, pos);
      if (nearest_temple_i > 0) {
        I64 npos = b[nearest_temple_i - 1];
        candidates[candidates_count++] = solve_recursive(npos, dist_acc + __builtin_llabs(pos - npos), try_shrine, 0);
      }
      if (nearest_temple_i < b_count) {
        I64 npos = b[nearest_temple_i - 0];
        candidates[candidates_count++] = solve_recursive(npos, dist_acc + __builtin_llabs(pos - npos), try_shrine, 0);
      }
    }

    I64 result = (1ll << 62);
    for (Iz i = 0; i < candidates_count; i ++) result = min(result, candidates[i]);
    return result;
  }

  for (Iz q_i = 0; q_i < q_count; q_i++) {
    printf("%lld\n", solve_recursive(q[q_i], 0, 1, 1));
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
      .input = "2 3 4\n100\n600\n400\n900\n1000\n150\n2000\n899\n799\n",
      .expected = "350\n1400\n301\n399\n",
    },
    {
      .input = "1 1 3\n1\n10000000000\n2\n9999999999\n5000000000\n",
      .expected = "10000000000\n10000000000\n14999999998\n",
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

I64 nextlong(void) {
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

U8 *nextstr(Arena *arena, Iz *out_len) {
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
