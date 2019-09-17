#include "stack.h"

template<typename T>
class Queue {
 private:
  // s1 -> input stack
  // s2 -> output stack
  Stack<T> s1, s2;
 public:
  Queue() = default;
  virtual ~Queue() {}
  Queue(int m) : s1(m), s2(m) {}
  Queue(const Queue &q) : s1(q.s1), s2(q.s2) {}
  Queue(Queue &&q) : s1(std::move(q.s1)), s2(std::move(q.s2)) {}
  virtual operator int() const { return int(s1) + int(s2); }
  virtual int full() const { return int(s1) == s1.size() || int(s2) == s2.size(); }
  virtual T operator[](int x) const {
    if (x >= int(s2))return s1[x - int(s2)];
    return s2[int(s2) - x - 1];
  }
  virtual Queue &operator<<(T t) {
    if (int(s1) >= s1.size()) {
      while (int(s1) > 0) {
        T e;
        s1 >> e;
        s2 << e;
      }
    }
    s1 << t;
    return *this;
  }
  virtual Queue &operator>>(T &t) {
    if (int(s2) == 0) {
      while (int(s1) > 0) {
        T e;
        s1 >> e;
        s2 << e;
      }
    }
    s2 >> t;
    return *this;
  }
  virtual Queue &operator=(const Queue &q) {
    if (&q == this)return *this;
    s1 = q.s1;
    s2 = q.s2;
    return *this;
  }
  virtual Queue &operator=(Queue &&q) {
    if (&q == this)return *this;
    s1 = std::move(q.s1);
    s2 = std::move(q.s2);
    return *this;
  }
  virtual void print() const {
    for (int i = int(s2) - 1; i >= 0; --i) {
      std::cout << s2[i] << "  ";
    }
    s1.print();
  }

};

using QUEUE = Queue<int>;

enum status {
  Init, Input, Output, Assign, Copy, Now, Get
};
class Executor {
  QUEUE s;
  char **argv;
  int argc;
  status st;
 public:
  Executor(int c, char **v) : argv(v), argc(c) {}
  void try_print_stack() {
    switch (st) {
      case Input:
      case Output:
      case Copy:
      case Assign:s.print();
      default: {}
    }
  }
  void Exec() {
    try {
      for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-S") == 0) {
          st = Init;
          printf("S  ");
        } else if (strcmp(argv[i], "-I") == 0) {
          try_print_stack();
          st = Input;
          printf("I  ");
        } else if (strcmp(argv[i], "-O") == 0) {
          try_print_stack();
          st = Output;
          printf("O  ");
        } else if (strcmp(argv[i], "-C") == 0) {
          try_print_stack();
          st = Copy;
          printf("C  ");
          QUEUE n(s);
          s = n;
        } else if (strcmp(argv[i], "-A") == 0) {
          try_print_stack();
          st = Assign;
          printf("A  ");
        } else if (strcmp(argv[i], "-N") == 0) {
          try_print_stack();
          st = Now;
          printf("N  ");
          printf("%d  ", int(s));
        } else if (strcmp(argv[i], "-G") == 0) {
          try_print_stack();
          st = Get;
          printf("G  ");
        } else {
          Do(atoi(argv[i]));
          continue;
        }
      }
      try_print_stack();
    } catch (std::runtime_error &err) {
      printf("E  ");
      return;
    }

  }
  void Do(int i) {
    if (st == Init) {
      s = QUEUE(i);
      printf("%d  ", i);
    } else if (st == Input) {
      s << i;
    } else if (st == Output) {
      while (i-- > 0) {
        int j = 0;
        s >> j;
      }
    } else if (st == Assign) {
      QUEUE n = s;
      s = n;
    } else if (st == Get) {
      printf("%d  ", s[i]);
    }
  }
};

int main(int argc, char *argv[]) {
  Executor e(argc, argv);
  e.Exec();
}