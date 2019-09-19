#include "lab5/transport_display.h"
#include <QtWidgets/QApplication>



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