#ifndef CPP_LAB__LAB1_STACK_H_
#define CPP_LAB__LAB1_STACK_H_
struct STACK {
  // 元素数组头指针
  int *elems;
  // 最大容纳个数
  int max;
  // 当前位置
  int pos;
};

enum ErrorCode {
  Success = 0, Failed = -1
};

int Error = Success;

inline void initSTACK(STACK *const p, int m) {
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

inline void initSTACK(STACK *const p, const STACK &s) {
  initSTACK(p, s.max);
  p->pos = s.pos;
  memcpy(p->elems, s.elems, sizeof(int) * p->pos);
}

inline int size(const STACK *const p) {
  return p->max;
}

inline int howMany(const STACK *const p) {
  return p->pos;
}

inline int getelem(const STACK *const p, int x) {
  if (x >= p->pos) {
    throw std::runtime_error("");
  }
  return p->elems[x];
}

inline STACK *const push(STACK *const p, int e) {
  if (p->pos >= p->max) {
    throw std::runtime_error("");
  }
  p->elems[p->pos++] = e;
  return p;
}

inline STACK *const pop(STACK *const p, int &e) {
  if (p->pos <= 0) {
    throw std::runtime_error("");
  }
  e = p->elems[--p->pos];
  return p;
}

inline STACK *const assign(STACK *const p, const STACK &s) {
  initSTACK(p, s);
  return p;
}

inline void print(const STACK *p) {
  for (int i = 0; i < p->pos; ++i) {
    printf("%d  ", p->elems[i]);
  }
}

inline void destroySTACK(STACK *const p) {
  p->pos = p->max = 0;
  free(p->elems);
  p->elems = nullptr;
}
#endif
