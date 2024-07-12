static struct { char *in; char *out; } test_cases[] = {
  {
    .in = "6 4\n-10 8 2 1 2 6\n\n",
    .out = "14\n",
  },
  {
    .in = "6 4\n-6 -100 50 -2 -5 -3\n\n",
    .out = "44\n",
  },
  {
    .in = "6 3\n-6 -100 50 -2 -5 -3\n\n",
    .out = "0\n",
  },
};

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


  /* typedef struct Node Node; */
  /* struct Node { */
  /*   Node *parent, *next, *first; */
  /*   Size k, l, opl; */
  /*   I64 result; */
  /* }; */
  /* Node *root = new (&arena, Node, 1); */
  /* root->opl = N; */

  /* Node *top = root; */
  /* while (1) { */
  /*   if (top->k == K) { */
  /*     I64 hand_sum = 0; */
  /*     for (Size i = 0; i < K; i++) */
  /*       hand_sum += hand[i]; */
  /*     top->parent->result = max(top->parent->result, hand_sum); */
  /*     top = top->next ? top->next : top->parent; */
  /*     continue; */
  /*   } */
  /*   if (top->first) { // return from children */
  /*     top->parent->result = max(top->parent->result, top->result); */
  /*     top = top->next ? top->next : top->parent; */
  /*     continue; */
  /*   } */

  /*   Node *children = new (&arena, Node, 4); */
  /*   children[0].parent = top; */
  /*   children[0].next = children + 1; */
  /*   children[1].parent = top; */
  /*   children[1].next = children + 2; */
  /*   children[2].parent = top; */
  /*   children[2].next = children + 3; */
  /*   children[3].parent = top; */

  /*   top->first = children + 0; */
  /*   top = top->first; */
  /* } */
