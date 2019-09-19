#ifndef CPP_LAB_LAB5_TRANSPORT_DISPLAY_H_
#define CPP_LAB_LAB5_TRANSPORT_DISPLAY_H_
#include "transport_system.h"
#include <QWidget>
#include <QtGui/QPainter>
#include <QMouseEvent>
#include <QtWidgets/QCheckBox>
class TransportDisplay : public QWidget {
  const TransportSystem &system;
  MinRouteInfo min_route;
  // 选中 -> 最短路径 未选中-> 最少换乘
  QCheckBox strategy;
  int from_x, from_y, to_x, to_y;
  bool set_from = true;
 public:
  TransportDisplay(const TransportSystem &s) : system(s) {
    resize(1024, 768);
    setWindowTitle("Transport Display");
    strategy.setParent(this);
    strategy.setGeometry(800, 600, 25, 25);
    strategy.setCheckState(Qt::CheckState::Checked);
    QObject::connect(&strategy, &QCheckBox::stateChanged, this, &TransportDisplay::paintOnCheckBoxChanged);
    strategy.show();
  }
  void paintOnCheckBoxChanged(int) {
    repaint();
  }
  void debugInfo() const {
    std::cout << "最短距离: " << min_route.dist << std::endl;
    std::cout << "换乘线路: ";
    for (int i = 0; i < min_route.route.size(); ++i) {
      std::cout << "[" << min_route.orient[i] << ":" << min_route.route[i].from << " --" << min_route.route[i].stop
                << "--> " << min_route.route[i].to
                << "]";
    }
    std::cout << "\n";
  }
  void mousePressEvent(QMouseEvent *event) override {
    // 当设置好起点和终点后，开始计算
    if (set_from) {
      from_x = event->x();
      from_y = event->y();
      set_from = false;
    } else {
      to_x = event->x();
      to_y = event->y();
      min_route = system.SearchRoute(
          Point{float(from_x), float(from_y)},
          Point{float(to_x), float(to_y)},
          strategy.checkState() == Qt::CheckState::Checked ? TransportSystem::Distance : TransportSystem::Transport);
      set_from = true;
      debugInfo();
    }
    repaint();
  }
  void paintEvent(QPaintEvent *event) override {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPen line(Qt::black, 3);
    QPen point(Qt::yellow, 6);
    QPen trans(Qt::red, 5);
    painter.drawText(100, 575, QString(set_from ? "点击设置起点" : "点击设置终点"));
    painter.drawText(100, 600, QString("起点 X: %1 Y: %2").arg(from_x).arg(from_y));
    painter.drawText(100, 625, QString("终点 X: %1 Y: %2").arg(to_x).arg(to_y));
    painter.drawText(100, 675, QString("最短距离: %1").arg(min_route.dist));
    painter.drawText(825, 625, strategy.checkState() == Qt::Checked ? "最短距离" : "最少换乘");
    QString stops_string;
    int prev = -1;
    for (int i = 0; i < min_route.all_stop.size(); ++i) {
      if (prev == min_route.all_stop[i])stops_string += "(转乘)";
      else stops_string += QString::number(min_route.all_stop[i]);
      prev = min_route.all_stop[i];
      if (i != min_route.all_stop.size() - 1)stops_string += "->";

    }
    QString trans_string(min_route.route.empty() ? "无需转乘" : "于以下站点转乘: ");
    for (auto &t : min_route.route) {
      trans_string += QString("乘坐%1号线至%2站转乘%3号线 ").arg(t.from).arg(t.stop).arg(t.to);
    }
    painter.drawText(200, 700, stops_string);
    painter.drawText(200, 725, trans_string);
    // 绘制线路
    for (auto &route : system.routes) {
      if (!route.second.C.empty()) {
        auto cur_stop = route.second.C.at(1);
        painter.setPen(line);
        //painter.drawText(int(cur_stop.point.x), int(cur_stop.point.y), QString::number(route.first) + 'C');
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
        //painter.drawText(int(cur_stop.point.x), int(cur_stop.point.y), QString::number(route.first) + 'R');
        for (auto &stop: route.second.R) {
          painter.drawLine(int(cur_stop.point.x),
                           int(cur_stop.point.y),
                           int(stop.second.point.x),
                           int(stop.second.point.y));
          cur_stop = stop.second;
        }
      }
      painter.setPen(trans);
      if (!min_route.all_stop.empty()) {
        auto cur_stop = min_route.from;
        painter.drawLine(from_x,
                         from_y,
                         int(system.stops.at(cur_stop).point.x),
                         int(system.stops.at(cur_stop).point.y));
        for (auto &stop: min_route.all_stop) {
          painter.drawLine(int(system.stops.at(cur_stop).point.x),
                           int(system.stops.at(cur_stop).point.y),
                           int(system.stops.at(stop).point.x),
                           int(system.stops.at(stop).point.y));
          cur_stop = stop;
        }
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
#endif //CPP_LAB_LAB5_TRANSPORT_DISPLAY_H_
