#ifndef CPP_LAB__STACK_H_
#define CPP_LAB__STACK_H_
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <cstring>
template<typename T>
class Stack {
 protected:
  T *const elems;
  const int max;
  int pos;
 public:
  Stack() : elems(nullptr), max(0), pos(0) {}
  Stack(int m) : elems(new T[m]), max(m), pos(0) {}
  virtual ~Stack() { delete[]elems; }
  Stack(const Stack &s) : elems(new T[s.max]), max(s.max), pos(s.pos) {
    std::copy(s.elems, s.elems + s.pos, elems);
  }
  Stack(Stack &&s) : elems(s.elems), max(s.max), pos(s.pos) {
    const_cast<T *&>(s.elems) = nullptr;
    s.pos = const_cast<int &>(s.max) = 0;
  }
  virtual int size() const { return max; }
  virtual explicit operator int() const { return pos; }
  virtual T operator[](int x) const {
    if (x >= pos)throw std::runtime_error("index out of range");
    return elems[x];
  }
  virtual Stack &operator<<(T e) {
    if (pos >= max)throw std::runtime_error("stack is full");
    elems[pos++] = e;
    return *this;
  }
  virtual Stack &operator>>(T &e) {
    if (pos <= 0) throw std::runtime_error("stack is empty");
    e = elems[--pos];
    return *this;
  }
  virtual Stack &operator=(const Stack &s) {
    if (&s == this)return *this;
    const_cast<int &>(max) = s.max;
    pos = s.pos;
    const_cast<T *&>(elems) = new T[s.max];
    std::copy(s.elems, s.elems + s.pos, elems);
    return *this;
  }
  virtual Stack &operator=(Stack &&s) {
    if (&s == this)return *this;
    const_cast<int &>(max) = s.max;
    pos = s.pos;
    const_cast<T *&>(elems) = s.elems;
    const_cast<T *&>(s.elems) = nullptr;
    s.pos = 0;
    const_cast<int &>(s.max) = 0;
    return *this;
  }
  virtual void print() const {
    for (int i = 0; i < pos; ++i) {
      std::cout << elems[i] << "  ";
    }
  }
};
#endif //CPP_LAB__STACK_H_
