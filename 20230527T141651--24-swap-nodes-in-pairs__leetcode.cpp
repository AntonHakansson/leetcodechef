#include <cstdio>
#include <iostream>
#include <stack>
#include <string.h>
#include <sys/types.h>
#include <unordered_map>
#include <vector>

using namespace std;

/**
 * Definition for singly-linked list.
 */
struct ListNode {
  int val;
  ListNode *next;
  ListNode() : val(0), next(nullptr) {}
  ListNode(int x) : val(x), next(nullptr) {}
  ListNode(int x, ListNode *next) : val(x), next(next) {}
};

class Solution {
public:
  ListNode *swapPairs(ListNode *head) {
    if (!head || !head->next)
      return head;

    ListNode dummy[1] = {ListNode()};

    ListNode *prev = dummy;
    ListNode *cur = head;
    while (cur && cur->next) {
      prev->next =cur->next;
      cur->next = cur->next->next;
      prev->next->next = cur;

      prev = cur;
      cur = cur->next;
    }

    return dummy->next;
  }
};

int main(int argc, char **argv) {
  Solution sol;
  {}
}
