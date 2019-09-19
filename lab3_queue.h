#ifndef CPP_LAB__LAB3_QUEUE_H_
#define CPP_LAB__LAB3_QUEUE_H_

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
    // s2是用栈模拟的队列出端，假设elems指向[1,2,3,4,5]（对栈来说5先出栈，因此对模拟出来的队列来说5先出队列），那么对于栈来说，它的顺序就是[1,2,3,4,5]第i个元素就是elems[i]，但队列的顺序则是[5,4,3,2,1]，对应的元素则是s2.pos - x - 1
    return s2[int(s2) - x - 1];
  }
  virtual Queue &operator<<(T t) {
    // s1满时将元素放进s2
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
    // s2为空时将元素从s1中取出来
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
    // 栈和队列的顺序相反，因此是倒着打印
    for (int i = int(s2) - 1; i >= 0; --i) {
      std::cout << s2[i] << "  ";
    }
    s1.print();
  }

};

#endif
