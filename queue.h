#ifndef CPP_LAB__QUEUE_H_
#define CPP_LAB__QUEUE_H_

#include "stack.h"

template<typename T>
class Queue : Stack<T> {
  // *this -> input stack
  // s -> output stack
  Stack<T> s;
 public:
  Queue() = default;
  ~Queue() {}
  Queue(int m) : Stack<T>(m), s(m) {}
  Queue(const Queue &q) : Stack<T>(q), s(q.s) {}
  Queue(Queue &&q) : Stack<T>(std::move(q)), s(std::move(q.s)) {}
  virtual explicit operator int() const { return Stack<T>::operator int() + int(s); }
  virtual int full() const { return Stack<T>::operator int() >= Stack<T>::size() || int(s) >= s.size(); }
  virtual T operator[](int x) const {
    if (x >= int(s)) {
      return Stack<T>::operator[](x - int(s));
    }
    return s[x];
  }
  virtual Queue &operator<<(T t) {
    Stack<T>::operator<<(t);
    return *this;
  }
  virtual Queue &operator>>(T &t) {
    if (int(s) == 0) {
      while (this->pos > 0) {
        T e;
        Stack<T>::operator>>(e);
        s << e;
      }
    }
    s >> t;
    return *this;
  }
  virtual Queue &operator=(const Queue &q) {
    if (&q == this) return *this;
    static_cast<Stack<T> &>(*this) = q;
    s = q.s;
    return *this;
  }
  virtual Queue &operator=(Queue &&q) {
    if (&q == this) return *this;
    static_cast<Stack<T> &>(*this) = std::move(q);
    s = std::move(q.s);
    return *this;
  }
  virtual void print() {
    for (int i = int(s) - 1; i >= 0; --i) {
      std::cout << s[i] << "  ";
    }
    Stack<T>::print();
  }
};

#endif //CPP_LAB__QUEUE_H_
