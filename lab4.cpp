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
  virtual explicit operator int() const { return int(static_cast<Stack<T> &>(*this)) + int(s); }
  virtual int full() const { return this->pos >= this->max || int(s) >= s.size(); }
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

};