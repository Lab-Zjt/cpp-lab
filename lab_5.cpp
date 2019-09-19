#include <vector>
#include <cmath>
#include <set>
#include <iostream>
#include <fstream>
#include <map>
#include <memory>
#include <QWidget>
#include <QtGui/QPainter>
#include <QMouseEvent>
#include <QtWidgets/QApplication>

// 表示地图上的一个点
struct Point {
  float x, y;
  float distance(const Point &rhs) const {
    return std::sqrt(powf(x - rhs.x, 2) + powf(y - rhs.y, 2));
  }
};

// 判断浮点数是否相等
template<typename Float>
bool equal(Float x, Float y) {
  return std::abs(x - y) < 1e-4;
}

// 站点
struct Stop {
  // number -> 表中的站点序号
  int number;
  // 站点位置
  Point point;
  // 在线路X上，int表示线路编号，char表示方向
  std::set<std::pair<int, char>> in_route;
  float distance(const Stop &rhs) const { return point.distance(rhs.point); }
  friend std::ostream &operator<<(std::ostream &os, const Stop &s) {
    std::cout << "[" << s.number << "](" << s.point.x << "," << s.point.y << ") Route:[";
    for (const auto &i : s.in_route) {
      std::cout << i.first << i.second << " ";
    }
    std::cout << "]";
    return os;
  }
};

// 线路
struct Route {
  // 线路编号
  int number;
  // map: int -> 该路线上的站点序号
  // 顺时针
  std::map<int, Stop> C;
  // 逆时针
  std::map<int, Stop> R;
 public:
  // 计算顺时针线路上两个站点的距离
  std::pair<float, std::vector<int>> distanceAtC(int from_stop, int to_stop) const {
    float dist = -1;
    Stop prev;
    std::vector<int> stops;
    for (const auto &p : C) {
      // 找到开始站点，开始计算距离
      if (p.second.number == from_stop && dist < 0) {
        dist = 0;
        prev = p.second;
      }
      if (dist >= 0) {
        dist += prev.distance(p.second);
        prev = p.second;
        stops.emplace_back(prev.number);
        // 找到到达站点，返回
        if (p.second.number == to_stop) {
          return {dist, stops};
        }
      }
    }
    // 没有完成计算
    return {std::numeric_limits<float>::max(), {}};
  }
  // 计算逆时针线路上两个站点的距离
  std::pair<float, std::vector<int>> distanceAtR(int from_stop, int to_stop) const {
    float dist = -1;
    Stop prev;
    std::vector<int> stops;
    for (const auto &p : R) {
      // 找到开始站点，开始计算距离
      if (p.second.number == from_stop && dist < 0) {
        dist = 0;
        prev = p.second;
      }
      if (dist >= 0) {
        dist += prev.distance(p.second);
        prev = p.second;
        stops.emplace_back(prev.number);
        // 找到到达站点，返回
        if (p.second.number == to_stop) {
          return {dist, stops};
        }
      }
    }
    // 没有完成计算
    return {std::numeric_limits<float>::max(), {}};
  }
  friend std::ostream &operator<<(std::ostream &os, const Route &s) {
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
};

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
TransportRouteInfo operator+(const TransportRouteInfo &lhs, const TransportRouteInfo &rhs) {
  TransportRouteInfo res;
  for (const auto &l : lhs) {
    res.emplace_back(l);
  }
  for (const auto &r : rhs) {
    res.emplace_back(r);
  }
  return res;
}

// 用std::set来保证线路唯一，该函数用于比较std::set的比较
bool operator<(const TransportRouteInfo &lhs, const TransportRouteInfo &rhs) {
  if (lhs.size() < rhs.size()) return true;
  if (lhs.size() > rhs.size()) return false;
  for (int i = 0; i < lhs.size(); i++) {
    if (lhs[i] < rhs[i])return true;
    if (lhs[i] > rhs[i]) return false;
  }
  return false;
}

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
    for (const auto &l : lhs.route) {
      res.route[l.first].insert(l.second.begin(), l.second.end());
    }
    for (const auto &r : rhs.route) {
      res.route[r.first].insert(r.second.begin(), r.second.end());
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

// 转乘矩阵
class TransportMatrix {
 private:
  // 由x号线到y号线的转乘信息
  std::map<int, std::map<int, TransportRoute>> data;
 public:
  TransportMatrix() = default;
  TransportMatrix(const std::map<int, Stop> &stops, const std::map<int, Route> &routes) {
    // 初始化矩阵，i==j时设置为存在一条由自己到自己的转乘线路
    for (int i = 1; i <= routes.size(); ++i) {
      for (int j = 1; j <= routes.size(); ++j) {
        if (i == j) data[i][j].route[1] = {{TransportStopInfo{i, j, 0}}};
        else data[i][j].route = {};
      }
    }
    // 遍历所有路线，对于路线上的某个站点，如果还有其他路线经过该站点，则将转乘信息加入到转乘线路中
    for (const auto &route: routes) {
      // 对于顺时针方向上的所有站点
      for (const auto &stop : route.second.C) {
        // 对于所有经过该站点的线路
        for (const auto &in_route : stops.at(stop.second.number).in_route) {
          if (data[route.first][in_route.first].route.empty()) { data[route.first][in_route.first].route[1] = {}; }
          // 如果该线路不是当前线路
          if (in_route.first != route.first) {
            // 加入转乘信息
            data[route.first][in_route.first].route[1]
                .emplace(std::vector<TransportStopInfo>{TransportStopInfo{route.first, in_route.first, stop.second.number}});
          }
        }
      }
      for (const auto &stop : route.second.R) {
        for (const auto &in_route : stops.at(stop.second.number).in_route) {
          if (data[route.first][in_route.first].route.empty()) { data[route.first][in_route.first].route[1] = {}; }
          if (in_route.first != route.first) {
            data[route.first][in_route.first].route[1]
                .emplace(std::vector<TransportStopInfo>{TransportStopInfo{route.first, in_route.first, stop.second.number}});
          }
        }
      }
    }
  }
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
// 最短换乘信息
struct MinRouteInfo {
  float dist;
  int from;
  int to;
  std::vector<int> all_stop;
  std::vector<TransportStopInfo> route;
  std::vector<char> orient;
};

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
  MinRouteInfo CalculateDistance(int from, int to) const {
    // 从站点列表获得站点信息
    auto &from_stop = stops.at(from);
    auto &to_stop = stops.at(to);
    // 初始化
    float min_distance = std::numeric_limits<float>::max();
    std::vector<TransportStopInfo> min_route;
    std::vector<char> min_orient;
    std::vector<int> min_stop;
    // 计算所有可能的换乘线路，即计算从起点的所在线路到终点所在的线路可能的换乘线路
    for (const auto &from_route : from_stop.in_route) {
      for (const auto &to_route : to_stop.in_route) {
        float cur_dist = 0;
        // 从转乘矩阵获得从线路x到线路y的转乘信息
        const auto &route_info = trans.at(from_route.first, to_route.first);
        // 对于换乘次数1～x
        for (auto &info : route_info.route) {
          // 对于该换乘次数下的所有线路
          for (auto &route : info.second) {
            // 初始化当前站点
            auto cur_stop = from_stop.number;
            // 初始化当前线路
            auto cur_route = from_route.first;
            // 初始化方向信息
            std::vector<char> cur_orient;
            std::vector<int> all_stop;
            // 对于该换乘线路上的所有转乘站点
            for (auto &stop : route) {
              // 求出当前站点到转乘站点的距离
              auto[c_dist, cs] = routes.at(cur_route).distanceAtC(cur_stop, stop.stop);
              auto[r_dist, rs] = routes.at(cur_route).distanceAtR(cur_stop, stop.stop);
              if (cs.empty() && rs.empty()) {
                cur_dist = -1;
                break;
              }
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
            // 计算当前站点到终点的距离
            auto[c_dist, cs] = routes.at(cur_route).distanceAtC(cur_stop, to_stop.number);
            auto[r_dist, rs] = routes.at(cur_route).distanceAtR(cur_stop, to_stop.number);
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
            if (cur_dist >= 0 && cur_dist < min_distance) {
              min_distance = cur_dist;
              min_route = route;
              min_orient = cur_orient;
              min_stop = all_stop;

            }
          }
        }
      }
    }
    return {min_distance, from, to, min_stop, min_route, min_orient};
  }
  MinRouteInfo SearchRoute(Point from, Point to) const {
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
    return CalculateDistance(min_from_stop, min_to_stop);
  }
};

class TransportDisplay : public QWidget {
  const TransportSystem &system;
  MinRouteInfo min_route;
  int from_x, from_y, to_x, to_y;
  bool set_from = true;
 public:
  TransportDisplay(const TransportSystem &s) : system(s) {
    resize(1024, 768);
    setWindowTitle("Transport Display");
  }
  void mousePressEvent(QMouseEvent *event) override {
    if (set_from) {
      from_x = event->x();
      from_y = event->y();
      set_from = false;
    } else {
      to_x = event->x();
      to_y = event->y();
      min_route = system.SearchRoute(
          Point{float(from_x), float(from_y)},
          Point{float(to_x), float(to_y)});
      std::cout << "最短距离: " << min_route.dist << std::endl;
      std::cout << "换乘线路: ";
      for (int i = 0; i < min_route.route.size(); ++i) {
        std::cout << "[" << min_route.orient[i] << ":" << min_route.route[i].from << " --" << min_route.route[i].stop
                  << "--> " << min_route.route[i].to
                  << "]";
      }
      std::cout << "\n";
      set_from = true;
    }
    repaint();
  }
  void paintEvent(QPaintEvent *event) override {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPen line(Qt::black, 3);
    QPen point(Qt::yellow, 6);
    QPen trans(Qt::red, 5);
    painter.drawText(800, 600, QString("起点 X: %1 Y: %2").arg(from_x).arg(from_y));
    painter.drawText(800, 625, QString("终点 X: %1 Y: %2").arg(to_x).arg(to_y));
    painter.drawText(800, 650, QString(set_from ? "点击设置起点" : "点击设置终点"));
    for (auto &route : system.routes) {
      if (!route.second.C.empty()) {
        auto cur_stop = route.second.C.at(1);
        painter.setPen(line);
        painter.drawText(int(cur_stop.point.x), int(cur_stop.point.y), QString::number(route.first) + 'C');
        for (auto &stop: route.second.C) {
          painter.drawLine(int(cur_stop.point.x),
                           int(cur_stop.point.y),
                           int(stop.second.point.x),
                           int(stop.second.point.y));
          cur_stop = stop.second;
        }
      }
      if (!route.second.R.empty()) {
        auto cur_stop = route.second.R.at(1);
        painter.drawText(int(cur_stop.point.x), int(cur_stop.point.y), QString::number(route.first) + 'R');
        for (auto &stop: route.second.C) {
          painter.drawLine(int(cur_stop.point.x),
                           int(cur_stop.point.y),
                           int(stop.second.point.x),
                           int(stop.second.point.y));
          cur_stop = stop.second;
        }
      }
      painter.setPen(trans);
      if (!min_route.route.empty()) {
        auto cur_stop = min_route.from;
        std::cout << cur_stop << "->";
        for (auto &stop: min_route.all_stop) {
          painter.drawLine(int(system.stops.at(cur_stop).point.x),
                           int(system.stops.at(cur_stop).point.y),
                           int(system.stops.at(stop).point.x),
                           int(system.stops.at(stop).point.y));
          cur_stop = stop;
          std::cout << cur_stop << "->";
        }
        std::cout << "\n";
        painter.drawLine(
            int(system.stops.at(cur_stop).point.x),
            int(system.stops.at(cur_stop).point.y),
            to_x, to_y);
      }
      for (auto &stop : system.stops) {
        painter.setPen(point);
        painter.drawPoint(int(stop.second.point.x), int(stop.second.point.y));
        painter.setPen(line);
        painter.drawText(int(stop.second.point.x), int(stop.second.point.y), QString::number(stop.second.number));
      }
    }
  }
};

int main(int argc, char *argv[]) {
  TransportSystem ts("../stops.txt", "../lines.txt");
//  auto p = ts.CalculateDistance(atoi(argv[1]), atoi(argv[2]));
//  //auto p = ts.CalculateDistance(13, 25);
//  if (p.dist < 0) {
//    std::cerr << "没有合适的线路！\n";
//    return 0;
//  }
//  std::cout << "最短距离: " << p.dist << std::endl;
//  std::cout << "换乘线路: ";
//  for (int i = 0; i < p.route.size(); ++i) {
//    std::cout << "[" << p.orient[i] << ":" << p.route[i].from << " --" << p.route[i].stop << "--> " << p.route[i].to
//              << "]";
//  }
//  std::cout << "\n";
  QApplication app(argc, argv);
  TransportDisplay dis(ts);
  dis.show();
  return app.exec();
}