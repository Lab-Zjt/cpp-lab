#ifndef CPP_LAB_LAB5_TRANSPORT_STOP_INFO_H_
#define CPP_LAB_LAB5_TRANSPORT_STOP_INFO_H_
#include <vector>
// 转乘信息
struct TransportStopInfo {
  // 从x号线路
  int from;
  // 转到y号线路
  int to;
  // 从z站点转乘
  int stop;
  friend bool operator<(const TransportStopInfo &lhs, const TransportStopInfo &rhs) {
    if (lhs.from < rhs.from) return true;
    if (lhs.from > rhs.from) return false;
    if (lhs.to < rhs.to) return true;
    if (lhs.to > rhs.to) return false;
    if (lhs.stop < rhs.stop) return true;
    return false;
  }
  friend bool operator>(const TransportStopInfo &lhs, const TransportStopInfo &rhs) {
    if (lhs.from > rhs.from) return true;
    if (lhs.from < rhs.from) return false;
    if (lhs.to > rhs.to) return true;
    if (lhs.to < rhs.to) return false;
    if (lhs.stop > rhs.stop) return true;
    return false;
  }
};

// 表示转乘线路，它由一系列的转乘点组成
using TransportRouteInfo = std::vector<TransportStopInfo>;

// 连接两部分转乘线路，即把它们的转乘点合并在一起
inline TransportRouteInfo operator+(const TransportRouteInfo &lhs, const TransportRouteInfo &rhs) {
  TransportRouteInfo res;
  // 禁止原地换乘
  if (lhs.front().stop == rhs.back().stop)return {};
  for (const auto &l : lhs) {
    res.emplace_back(l);
  }
  for (const auto &r : rhs) {
    res.emplace_back(r);
  }
  return res;
}

// 用std::set来保证线路唯一，该函数用于比较std::set的比较
inline bool operator<(const TransportRouteInfo &lhs, const TransportRouteInfo &rhs) {
  if (lhs.size() < rhs.size()) return true;
  if (lhs.size() > rhs.size()) return false;
  for (int i = 0; i < lhs.size(); i++) {
    if (lhs[i] < rhs[i])return true;
    if (lhs[i] > rhs[i]) return false;
  }
  return false;
}
// 最短换乘信息
struct MinRouteInfo {
  float dist = 0;
  int from = 0;
  int to = 0;
  std::vector<int> all_stop;
  std::vector<TransportStopInfo> route;
  std::vector<char> orient;
};

#endif //CPP_LAB_LAB5_TRANSPORT_STOP_INFO_H_
