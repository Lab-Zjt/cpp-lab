#ifndef CPP_LAB_LAB5_STOP_H_
#define CPP_LAB_LAB5_STOP_H_

#include <iostream>
#include <cmath>
#include <set>
// 地图上的一个点
struct Point {
  float x, y;
  float distance(const Point &rhs) const {
    return std::sqrt(powf(x - rhs.x, 2) + powf(y - rhs.y, 2));
  }
};

// 站点
struct Stop {
  // number -> 表中的站点序号
  int number;
  // 站点坐标
  Point point;
  // 该站点在线路X上，int表示线路编号，char表示方向
  std::set<std::pair<int, char>> in_route;
  float distance(const Stop &rhs) const { return point.distance(rhs.point); }
  friend std::ostream &operator<<(std::ostream &os, const Stop &s);
};

std::ostream &operator<<(std::ostream &os, const Stop &s) {
  std::cout << "[" << s.number << "](" << s.point.x << "," << s.point.y << ") Route:[";
  for (const auto &i : s.in_route) {
    std::cout << i.first << i.second << " ";
  }
  std::cout << "]";
  return os;
}

#endif
