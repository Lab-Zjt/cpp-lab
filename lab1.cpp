#include <iostream>
#include <cstring>
#include "lab1_stack.h"

// 解释器状态
enum status {
  Init, Input, Output, Assign, Copy, Now, Get
};

// 解释器
class Executor {
  STACK s{nullptr, 0, 0};
  char **argv;
  int argc;
  status st;
 public:
  Executor(int c, char **v) : argv(v), argc(c) {}
  // 尝试打印栈，当前一个操作是I、O、A、C时打印
  void try_print_stack() {
    switch (st) {
      case Input:
      case Output:
      case Copy:
      case Assign:print(&s);
      default: {}
    }
  }
  // 执行
  void Exec() {
    // 遇到Error直接抛出异常，接住然后打印E即可
    try {
      // 根据不同参数改变状态或执行命令
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
          STACK n{nullptr, 0, 0};
          initSTACK(&n, s);
          destroySTACK(&s);
          s = n;
        } else if (strcmp(argv[i], "-A") == 0) {
          try_print_stack();
          st = Assign;
          printf("A  ");
        } else if (strcmp(argv[i], "-N") == 0) {
          try_print_stack();
          st = Now;
          printf("N  ");
          printf("%d  ", howMany(&s));
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
  // 当前参数是一个数字，根据解释器状态执行相应函数
  void Do(int i) {
    if (st == Init) {
      initSTACK(&s, i);
      printf("%d  ", i);
    } else if (st == Input) {
      push(&s, i);
    } else if (st == Output) {
      while (i-- > 0) {
        int j = 0;
        pop(&s, j);
      }
    } else if (st == Assign) {
      STACK n{nullptr, 0, 0};
      initSTACK(&n, i);
      assign(&n, s);
      s = n;
    } else if (st == Get) {
      printf("%d  ", getelem(&s, i));
    }
  }
};

int main(int argc, char *argv[]) {
  Executor e(argc, argv);
  e.Exec();
  return 0;
}