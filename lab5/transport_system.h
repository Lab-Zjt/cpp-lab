#ifndef CPP_LAB_LAB5_TRANSPORT_SYSTEM_H_
#define CPP_LAB_LAB5_TRANSPORT_SYSTEM_H_
#include "transport_matrix.h"
#include <fstream>
// 从文件加载站点信息
std::map<int, Stop> LoadStops(const char *filename) {
  std::ifstream fs(filename);
  std::string s;
  std::map<int, Stop> stops;
  int lines;
  fs >> lines;
  for (int i = 1; i <= lines; ++i) {
    float x, y;
    fs >> x;
    fs >> y;
    stops[i] = Stop{i, Point{x, y}};
  }
  return stops;
}

// 从文件加载线路信息
std::map<int, Route> LoadRoute(const char *filename, std::map<int, Stop> &stops) {
  std::ifstream fs(filename);
  std::string s;
  fs >> s;
  std::map<int, Route> routes;
  auto lines = strtol(s.c_str(), nullptr, 10);
  for (;;) {
    int num;
    fs >> num;
    if (!fs)break;
    char orient;
    fs >> orient;
    int stops_num;
    fs >> stops_num;
    routes[num].number = num;
    if (orient == 'C') {
      for (int j = 1; j <= stops_num; ++j) {
        int stop;
        fs >> stop;
        routes[num].C[j] = stops[stop];
        stops[stop].in_route.emplace(num, 'C');

      }
    } else if (orient == 'R') {
      for (int j = 1; j <= stops_num; ++j) {
        int stop;
        fs >> stop;
        routes[num].R[j] = stops[stop];
        stops[stop].in_route.emplace(num, 'R');
      }
    }
  }
  return routes;
}

class TransportSystem {
  friend class TransportDisplay;
 private:
  // 站点列表
  std::map<int, Stop> stops;
  // 线路列表
  std::map<int, Route> routes;
  // 转乘矩阵
  TransportMatrix trans;
  // 选择策略
  enum Strategy { Transport, Distance };
  Strategy strategy = Distance;
 public:
  TransportSystem(const char *stops_file, const char *lines_file)
      : stops(LoadStops(stops_file)), routes(LoadRoute(lines_file, stops)), trans(stops, routes) {
    trans = trans * trans;
  }
  // 计算两个站点间的最短换乘信息
  MinRouteInfo CalculateDistance(int from, int to, Strategy st) const {
    // 从站点列表获得站点信息
    auto &from_stop = stops.at(from);
    auto &to_stop = stops.at(to);
    // 初始化
    float min_distance = std::numeric_limits<float>::max();
    std::vector<TransportStopInfo> min_route;
    std::vector<char> min_orient;
    std::vector<int> min_stop;
    int min_trans = std::numeric_limits<int>::max();
    // 如果策略为换乘次数最少，则找到最少换乘次数的线路后立即结束循环
    bool break_immediately = st == Strategy::Transport;
    // 计算所有可能的换乘线路，即计算从起点的所在线路到终点所在的线路可能的换乘线路
    for (const auto &from_route : from_stop.in_route) {
      for (const auto &to_route : to_stop.in_route) {
        // 在同一条线路上，可以不用换乘
        if (from_route == to_route) {
          auto[dist, s] = routes.at(from_route.first).distanceBetween(from, to_stop.number, from_route.second);
          if (dist < min_distance || (break_immediately && min_trans > 0)) {
            min_distance = dist;
            min_orient.push_back(from_route.second);
            min_stop = s;
            min_trans = 0;
          }
          // 策略为换乘次数最沙时，由于换乘次数为0，而以下循环换乘次数均大于0，因此直接跳过。
          if (break_immediately) {
            continue;
          }
        }
        // 从转乘矩阵获得从线路x到线路y的转乘信息
        const auto &route_info = trans.at(from_route.first, to_route.first);
        // 对于换乘次数1～x
        for (auto &info : route_info.route) {
          // 对于该换乘次数下的所有线路
          for (auto &route : info.second) {
            float cur_dist = 0;
            // 初始化当前站点
            auto cur_stop = from_stop.number;
            // 初始化当前线路
            auto cur_route = from_route.first;
            // 初始化方向信息
            std::vector<char> cur_orient;
            // 初始化经过站点
            std::vector<int> all_stop;
            // 对于该换乘线路上的所有转乘站点
            for (auto &stop : route) {
              // 求出当前站点到转乘站点的距离
              // c_dist 顺时针距离
              // r_dist 逆时针距离
              // cs 顺时针经过站点
              // rs 逆时针经过站点
              auto[c_dist, cs] = routes.at(cur_route).distanceBetween(cur_stop, stop.stop, 'C');
              auto[r_dist, rs] = routes.at(cur_route).distanceBetween(cur_stop, stop.stop, 'R');
              // 顺时针逆时针均不能到达，跳出循环，尝试下一条线路
              if (cs.empty() && rs.empty()) {
                cur_dist = -1;
                break;
              }
              // 如果逆时针不能到达，或者逆时针距离长于顺时针距离，则选择顺时针，反之亦然
              if (rs.empty() || r_dist >= c_dist) {
                cur_dist += c_dist;
                cur_orient.push_back('C');
                all_stop.insert(all_stop.end(), cs.begin(), cs.end());
              } else if (cs.empty() || c_dist >= r_dist) {
                cur_dist += r_dist;
                cur_orient.push_back('R');
                all_stop.insert(all_stop.end(), rs.begin(), rs.end());
              }
              // 更新当前站点、当前线路信息
              cur_route = stop.to;
              cur_stop = stop.stop;
            }
            if (cur_dist >= 0) {
              // 换乘结束，计算当前站点到终点的距离
              // 同样是计算顺时针和逆时针的距离
              auto[c_dist, cs] = routes.at(cur_route).distanceBetween(cur_stop, to_stop.number, 'C');
              auto[r_dist, rs] = routes.at(cur_route).distanceBetween(cur_stop, to_stop.number, 'R');
              if (cs.empty() && rs.empty()) {
                cur_dist = -1;
              } else {
                if (rs.empty() || r_dist >= c_dist) {
                  cur_dist += c_dist;
                  cur_orient.push_back('C');
                  all_stop.insert(all_stop.end(), cs.begin(), cs.end());
                } else {
                  cur_dist += r_dist;
                  cur_orient.push_back('R');
                  all_stop.insert(all_stop.end(), rs.begin(), rs.end());
                }
              }
              // 如果该线路的距离更短，更新最短换乘信息
              if (cur_dist >= 0 && cur_dist < min_distance && !(info.first > min_trans && break_immediately)) {
                min_distance = cur_dist;
                min_route = route;
                min_orient = cur_orient;
                min_stop = all_stop;
                min_trans = info.first;
              }
            }
          }
          if (break_immediately)break;
        }
      }
    }
    return {min_distance, from, to, min_stop, min_route, min_orient};
  }
  MinRouteInfo SearchRoute(Point from, Point to, Strategy st) const {
    // 先找到分别离起点和终点最近的车站
    float min_from_dist = std::numeric_limits<float>::max();
    float min_to_dist = std::numeric_limits<float>::max();
    int min_from_stop = -1;
    int min_to_stop = -1;
    for (auto &stop : stops) {
      auto dist = stop.second.point.distance(from);
      if (dist < min_from_dist && !stop.second.in_route.empty()) {
        min_from_dist = dist;
        min_from_stop = stop.second.number;
      }
      dist = stop.second.point.distance(to);
      if (dist < min_to_dist && !stop.second.in_route.empty()) {
        min_to_dist = dist;
        min_to_stop = stop.second.number;
      }
    }
    // 然后计算这两个车站的最短距离
    auto info = CalculateDistance(min_from_stop, min_to_stop, st);
    info.dist += min_from_dist + min_to_dist;
    return info;
  }
};
#endif //CPP_LAB_LAB5_TRANSPORT_SYSTEM_H_
