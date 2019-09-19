#ifndef CPP_LAB_LAB5_ROUTE_H_
#define CPP_LAB_LAB5_ROUTE_H_

#include "stop.h"
#include <vector>
#include <map>
// 线路
struct Route {
  // 线路编号
  int number;
  // map: int -> 该路线上的站点序号
  // 顺时针线路
  std::map<int, Stop> C;
  // 逆时针线路
  std::map<int, Stop> R;
 public:
  // 计算线路上两个站点的距离，并返回经过的站点
  std::pair<float, std::vector<int>> distanceBetween(int from_stop, int to_stop, char orient) const;
  friend std::ostream &operator<<(std::ostream &os, const Route &s);
};

inline std::pair<float, std::vector<int>> Route::distanceBetween(int from_stop, int to_stop, char orient) const {
  float dist = -1;
  Stop prev;
  std::vector<int> stops;
  for (const auto &p : orient == 'C' ? C : R) {
    // 找到起点，开始计算距离
    if (p.second.number == from_stop && dist < 0) {
      dist = 0;
      prev = p.second;
    }
    if (dist >= 0) {
      dist += prev.distance(p.second);
      prev = p.second;
      stops.emplace_back(prev.number);
      // 找到终点，返回
      if (p.second.number == to_stop) {
        return {dist, stops};
      }
    }
  }
  // 没有完成计算
  return {std::numeric_limits<float>::max(), {}};
}

inline std::ostream &operator<<(std::ostream &os, const Route &s) {
  std::cout << "[" << s.number << "C" << s.C.size() << "] Stop:[";
  for (const auto &p : s.C) {
    std::cout << "(" << p.first << "->{" << p.second.number << ",(" << p.second.point.x << "," << p.second.point.y
              << ")}) ";
  }
  std::cout << "]\n";
  std::cout << "[" << s.number << "R" << s.R.size() << "] Stop:[";
  for (const auto &p : s.R) {
    std::cout << "(" << p.first << "->{" << p.second.number << ",(" << p.second.point.x << "," << p.second.point.y
              << ")}) ";
  }
  std::cout << "]";
  return os;
}
#endif //CPP_LAB_LAB5_ROUTE_H_
