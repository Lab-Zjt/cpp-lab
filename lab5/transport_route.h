#ifndef CPP_LAB_LAB5_TRANSPORT_ROUTE_H_
#define CPP_LAB_LAB5_TRANSPORT_ROUTE_H_
#include "transport_info.h"
#include "route.h"
// 从线路x到线路y的换乘信息
struct TransportRoute {
  // 换乘次数 -> 该换乘次数下的换乘线路，可能有多条
  std::map<int, std::set<TransportRouteInfo>> route;
 public:
  size_t method_num() const {
    size_t size = 0;
    for (const auto &r : route) {
      size += r.second.size();
    }
    return size;
  }
  // 连接两部分换乘信息，如lhs是线路1->线路3与rhs是线路3->线路2，连接后即为线路1->线路2
  // 之所以用×是因为它类似于矩阵乘法里每个单元相乘
  friend TransportRoute operator*(const TransportRoute &lhs, const TransportRoute &rhs) {
    TransportRoute res;
    // 遍历换乘线路
    // i,j即是换乘次数，route_infos_*是换乘i,j次下的换乘线路
    for (int i = 1; i <= lhs.route.size(); ++i) {
      std::set<TransportRouteInfo> route_infos_l;
      if (auto route_infos_l_it = lhs.route.find(i); route_infos_l_it != lhs.route.end()) {
        route_infos_l = route_infos_l_it->second;
      }
      for (int j = 1; j <= rhs.route.size(); ++j) {
        std::set<TransportRouteInfo> route_infos_r;
        if (auto route_infos_r_it = rhs.route.find(j); route_infos_r_it != rhs.route.end()) {
          route_infos_r = route_infos_r_it->second;
        }
        for (auto &stop_info_l : route_infos_l) {
          for (auto &stop_info_r : route_infos_r) {
            // 换乘i+j次下的换乘线路，即是左边部分的换乘线路与右边部分的换乘线路连接起来
            res.route[i + j].insert(stop_info_l + stop_info_r);
          }
        }
      }
    }
    return res;
  }
  // 合并换乘信息，lhs、rhs都是从线路x到线路y的换乘信息，将两者合并起来，并去掉重复的那部分
  friend TransportRoute operator+(const TransportRoute &lhs, const TransportRoute &rhs) {
    TransportRoute res;
    for (const auto &lr : {lhs.route, rhs.route}) {
      for (auto &l : lr) {
        res.route[l.first].insert(l.second.begin(), l.second.end());
      }
    }
    return res;
  }
  // 调试用，输出换乘信息
  friend std::ostream &operator<<(std::ostream &os, const TransportRoute &r) {
    for (const auto &route_info : r.route) {
      std::cout << "换乘次数" << route_info.first << ":\n";
      int i = 1;
      for (const auto &route : route_info.second) {
        std::cout << "\t路线" << i << ": ";
        for (const auto &stop : route) {
          std::cout << "[" << stop.from << " --" << stop.stop << "--> " << stop.to << "]";
        }
        std::cout << "\n";
        i++;
      }
      std::cout << "\n";
    }
    return os;
  }
};

#endif //CPP_LAB_LAB5_TRANSPORT_ROUTE_H_
