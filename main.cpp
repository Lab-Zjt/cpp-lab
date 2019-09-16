#include <iostream>
#include <cstring>

struct STACK {
  int *elems;
  int max;
  int pos;
};

enum ErrorCode {
  Success = 0, Failed = -1
};

int Error = Success;

void initSTACK(STACK *const p, int m) {
  p->max = m;
  if (p->elems) {
    free(p->elems);
  }
  p->elems = (int *) malloc(sizeof(int) * p->max);
  if (!p->elems) {
    throw std::runtime_error("");
  }
  p->pos = 0;
}

void initSTACK(STACK *const p, const STACK &s) {
  initSTACK(p, s.max);
  p->pos = s.pos;
  memcpy(p->elems, s.elems, sizeof(int) * p->pos);
}

int size(const STACK *const p) {
  return p->max;
}
int howMany(const STACK *const p) {
  return p->pos;
}
int getelem(const STACK *const p, int x) {
  if (x >= p->pos) {
    throw std::runtime_error("");
  }
  return p->elems[x];
}
STACK *const push(STACK *const p, int e) {
  if (p->pos >= p->max) {
    throw std::runtime_error("");
  }
  p->elems[p->pos++] = e;
  return p;
}
STACK *const pop(STACK *const p, int &e) {
  if (p->pos <= 0) {
    throw std::runtime_error("");
  }
  e = p->elems[--p->pos];
  return p;
}

STACK *const assign(STACK *const p, const STACK &s) {
  initSTACK(p, s);
  return p;
}

void print(const STACK *p) {
  for (int i = 0; i < p->pos; ++i) {
    printf("%d  ", p->elems[i]);
  }
}

void destroySTACK(STACK *const p) {
  p->pos = p->max = 0;
  free(p->elems);
  p->elems = nullptr;
}

enum status {
  Init, Input, Output, Assign, Copy, Now, Get
};

class Executor {
  STACK s{nullptr, 0, 0};
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
      case Assign:print(&s);
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