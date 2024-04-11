/*
 * #+title: Codechef: Event Organizer
 * #+author: anton@hakanssn.comn
 * #+ref: https://www.codechef.com/practice/course/dynamic-programming/INTDP01/problems/MAXCOMP
 * #+licence: This is free and unencumbered software released into the public domain.
 *
 * Chef Po has given an online advertisement to provide *Event organizing services*. Chef
 * got a huge response for his advertisement. He got various orders to conduct the events
 * from different organizations. In turn, Chef will receive a compensation depend upon the
 * type of event and the total numbers of persons in the event. Chef has received *N*
 * orders for conducting events in this weekend in all. As weekend consists of two days
 * all events will take place during the period of *48* hours. For the *i*-th order the
 * corresponding event will start at *S_{i}* hours, ends at *E_{i}* hours and Chef will
 * receive a compensation *C_{i}* for this event. For example, if *S_{i} = 17* and *E_{i}
 * = 22* then duration of event is *22 -- 17 = 5* hours and its time period is *17:00 --
 * 22:00* of Saturday. Hours of Sunday are numbered by numbers from *24* to *48*. So, for
 * example, *10:00* of Sunday will be represented as *10 + 24 = 34*. Because Chef is a
 * newbie, the organizations had put a condition that Chef will receive a compensation for
 * the event if and only if he is available for the entire duration of the event. It means
 * that he can not choose overlapping events. Note, however, that if some event starts
 * just in the moment another event has finished the Chef can safely conduct them both.
 *
 * In general Chef will obey the orders on first come first serve basis. But on weekends
 * Chef will select the orders in such a way that the total compensation for all the
 * events he will conduct will be the maximal. Now your task is to help Chef and find this
 * maximal total compensation.
 *
 *
 * * Input
 *
 * The first line of the input contains an integer *T*, the number of test cases. *T* test
 * cases follow. The first line of each test case contains an integer *N*, the number of
 * received orders for conducting events. Each of the next *N* lines contains three space
 * separated integers *S_{i}*, *E_{i}*, *C_{i}*, the parameters of the *i*-th event
 * described in the problem statement.
 *
 * * Constraints
 *
 * - 1 ≤ T ≤ 10
 * - 1 ≤ N ≤ 2000
 * - 0 ≤ S_{i} < E_{i} ≤ 48
 * - 0 ≤ C_{i} ≤ 10^{6}
 *
 * * Output
 *
 * Output for each test case should contain a single integer in a separate line, the
 * maximal compensation Chef Po can get.
 *
 * ** Sample 1:
 *
 * #+begin_example
 * 2
 * 4
 * 1 2 100
 * 2 3 200
 * 3 4 1600
 * 1 3 2100
 * 3
 * 1 10 2000
 * 2 5 100
 * 6 9 400
 * #+end_example
 *
 * #+begin_example
 * 3700
 * 2000
 * #+end_example
 *
 * ** Explanation:
 *
 * *Case 1.* The best choice here is to conduct *3*rd and *4*th events. The total
 * compensation is equal to *1600 + 2100 = 3700*. These events do not overlap since *3*rd
 * event starts just after the finish of the *4*th one. Alternatively we can conduct first
 * three events that also do not overlap. But in this case compensation will be only
 * *100 + 200 + 1600 = 1900*.
 *
 * *Case 2.* Note that first event overlaps with both second and third events, while the
 * last two events do not overlap. Hence there are two reasonable choices available for
 * Chef. One is to take just the first order with total compensation *2000* and the second
 * one is to take the last two orders with total compensation *100 + 400 = 500*. Clearly
 * the first choice is better. Hence the answer is *2000*.
 *
 * * Solution
 *
 * Sort items by end time such that \( E_i \leq E_{i+1} \) for \( i \in \{ 0 \leq i < n \;|\; i \in \mathem{N} \} \)
 *
 * Then we can define the optimization problem as
 * - \( \mathrm{OPT}(i) \) :: Maximum compensation for events \( [0, i) \).
 * We are interested in computing the solution \( \mathrm{OPT}(n) \).
 *
 * \begin{align*}
 * \mathrm{OPT}(i) = \max \begin{cases}
 *   \mathrm{OPT}(j) + C_i  & \text{pick event } i \\
 *   \mathrm{OPT}(i - 1)    & \text{otherwise}
 * \end{cases}
 * \end{align*}
 *
 * where \( j \) is the latest possible event for \( i \) such that they are non-overlapping i.e. \( E_j \leq  S_i \).
 *
 * \begin{gather*}
 * \mathrm{OPT}_0 \gets C_0 \\
 * for i \in [0, n):    \\
 *   j \gets \text{latest non-overlapping event} \\
 *   pick \gets \mathrm{OPT}_j + C_i \\
 *   otherwise \gets \mathrm{OPT}_{i-1} \\
 *   OPT_i \gets \max \{pick, otherwise\}
 * \end{gather*}
 *
*/

#include <stdint.h>
#include <stddef.h>
typedef unsigned char Byte;
typedef uint8_t   U8;
typedef int32_t   I32;
typedef int64_t   I64;
typedef uint64_t  U64;
typedef ptrdiff_t Size;
typedef I32    B32;

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define size_of(s)  (Size)sizeof(s)
#define align_of(s) (Size)_Alignof(s)
#define count_of(s) (size_of((s)) / size_of(*(s)))

#ifndef assert
# define assert(c)  while((!(c))) __builtin_unreachable()
#endif


////////////////////////////////////////////////////////////////////////////////
//- Arena Allocator

#define new(a, t, n) (t *) arena_alloc(a, size_of(t), align_of(t), (n))

typedef struct Arena Arena;
struct Arena {
  Size capacity;
  Size at;
  Byte *backing;
};

__attribute((malloc, alloc_size(2,4), alloc_align(3)))
static Byte *arena_alloc(Arena *a, Size objsize, Size align, Size count)
{
  Size avail = a->capacity - a->at;
  Size padding = -(uintptr_t)(a->backing + a->at) & (align - 1);
  Size total   = padding + objsize * count;
  assert(total < avail);
  Byte *p = (a->backing + a->at) + padding;
  memset(p, 0, objsize * count);
  a->at += total;
  return p;
}


////////////////////////////////////////////////////////////////////////////////
//- Program

typedef struct Event Event;
struct Event {
  Size start;
  Size end;
  U64 compensation;
};

typedef struct Test Test;
struct Test {
  Event *items;
  Size count;
};

int sort_events_by_end_time(const void *a, const void *b)
{
  const Event *A = (const Event *)a;
  const Event *B = (const Event *)b;
  return (A->end - B->end);
}

#define max(a, b) ((a) > (b)) ? (a) : (b)
void solve(Arena arena, Test test)
{
  qsort(test.items, test.count, sizeof(test.items[0]), &sort_events_by_end_time);

  U64 *dp = new (&arena, U64, test.count + 1);
  dp += 1; // dp[-1] = 0

  for (Size i = 0; i < test.count; i++) {
    Size latest_non_overlapping_event = -1; {
      Size k = i;
      while (--k >= 0) {
        if (test.items[k].end <= test.items[i].start) {
          latest_non_overlapping_event = k;
          break;
        }
      }
    }
    U64 pick      = dp[latest_non_overlapping_event] + test.items[i].compensation;
    U64 otherwise = dp[i - 1];
    dp[i] = max(pick, otherwise);
  }

  printf("%ld\n", dp[test.count - 1]);
}

int main(int argc, char **argv)
{
  Arena arena[1] = {0};
  arena->capacity = 1 << 30;
  arena->backing = malloc(arena->capacity);

  Size T = 0;
  scanf("%ld\n", &T);

  for (Size t = 0; t < T; t++) {
    Arena temp = *arena;
    Test test = {0};
    scanf("%ld\n", &test.count);
    test.items = new(&temp, typeof(*test.items), test.count);
    for (Size i = 0; i < test.count; i++) {
      scanf("%ld ", &test.items[i].start);
      scanf("%ld ", &test.items[i].end);
      scanf("%ld\n", &test.items[i].compensation);
    }
    solve(temp, test);
  }

  free(arena->backing);
  return 0;
}


/* Local Variables: */
/* compile-command: "cc main.c -fsanitize=undefined -g3" */
/* End: */
