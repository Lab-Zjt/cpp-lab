#include "queue.h"

class Solution {
  Queue<int> mq;
  Queue<int> fq;
 public:
  Solution(int M, int F, int m, int f) : mq(M), fq(F) {
    for (int i = 1; i <= M; ++i) {
      if (m == i)mq << 2;
      else mq << 0;
    }
    for (int i = 1; i <= F; ++i) {
      if (f == i)fq << 2;
      else fq << 1;
    }
  }
  int Solute() {
    for (int i = 1;; i++) {
      int m, f;
      mq >> m;
      fq >> f;
      if (m == f)return i;
      mq << m;
      fq << f;
    }
  }
};

int main(int argc, char *argv[]) {
  if (argc != 5)exit(-1);
  Solution s(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
  std::cout << s.Solute() << std::endl;
  /*auto f = [argc, argv]() {
    int M = atoi(argv[1]), F = atoi(argv[2]), m = atoi(argv[3]), f = atoi(argv[4]);
    for (int i = 0;; ++i) {
      if (i % M == m - 1 && i % F == f - 1) {
        std::cout << i + 1 << std::endl;
        return;
      }
    }
  };
  f();*/
}