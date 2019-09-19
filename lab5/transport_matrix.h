#ifndef CPP_LAB_LAB5_TRANSPORT_MATRIX_H_
#define CPP_LAB_LAB5_TRANSPORT_MATRIX_H_
#include "transport_route.h"
// 转乘矩阵
class TransportMatrix {
 private:
  // 由x号线到y号线的转乘信息
  std::map<int, std::map<int, TransportRoute>> data;
 public:
  TransportMatrix() = default;
  TransportMatrix(const std::map<int, Stop> &stops, const std::map<int, Route> &routes) {
    // 初始化矩阵//，i==j时设置为存在一条由自己到自己的转乘线路
    for (int i = 1; i <= routes.size(); ++i) {
      for (int j = 1; j <= routes.size(); ++j) {
        /*if (i == j) data[i][j].route[1] = {{TransportStopInfo{i, j, 0}}};
        else*/ data[i][j].route = {};
      }
    }
    // 遍历所有路线，对于路线上的某个站点，如果还有其他路线经过该站点，则将转乘信息加入到转乘线路中
    for (const auto &route: routes) {
      // 对于顺时针方向上的所有站点
      for (const auto &cr : {route.second.C, route.second.R}) {
        for (const auto &stop : cr) {
          // 对于所有经过该站点的线路
          for (const auto &in_route : stops.at(stop.second.number).in_route) {
            if (data[route.first][in_route.first].route.empty()) { data[route.first][in_route.first].route[1] = {}; }
            // 如果该线路不是当前线路
            if (in_route.first != route.first) {
              // 加入转乘信息
              data[route.first][in_route.first].route[1]
                  .emplace(std::vector<TransportStopInfo>{
                      TransportStopInfo{route.first, in_route.first, stop.second.number}});
            }
          }
        }
      }
    }
  }
  // 矩阵乘法
  friend TransportMatrix operator*(const TransportMatrix &lhs, const TransportMatrix &rhs) {
    TransportMatrix res;
    if (lhs.data.size() != rhs.data.size())throw std::runtime_error("matrix match failed.");
    size_t size = lhs.data.size();
    for (size_t i = 1; i <= size; ++i) {
      for (size_t j = 1; j <= size; ++j) {
        res.data[i][j] = lhs.data.at(i).at(j) + rhs.data.at(i).at(j);
        for (size_t k = 1; k <= size; ++k) {
          if (k == i || k == j)continue;
          res.data[i][j] = res.data[i][j] + lhs.data.at(i).at(k) * rhs.data.at(k).at(j);
        }
      }
    }
    return res;
  }
  TransportRoute &at(int x, int y) { return data[x][y]; }
  const TransportRoute &at(int x, int y) const { return data.at(x).at(y); }
};
#endif //CPP_LAB_LAB5_TRANSPORT_MATRIX_H_
