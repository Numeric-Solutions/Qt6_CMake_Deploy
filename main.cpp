#include "plotwindow.h"

#include <QApplication>
#include <QScreen>
#include <QRect>
#include <QDebug>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  a.setWindowIcon(QIcon("ns256.png"));

  // Get the primary screen geometry
  QScreen *screen = QApplication::primaryScreen();
  QRect screenGeometry = screen->geometry();
  
  // Calculate quartile dimensions
  int halfWidth = screenGeometry.width() / 2;
  int halfHeight = screenGeometry.height() / 2;
  int startX = screenGeometry.x();
  int startY = screenGeometry.y();

  PlotWindow w1;
  PlotWindow w2;
  PlotWindow w3;
  PlotWindow w4;
  
  // Set unique window IDs for async behavior
  w1.setWindowId(0);
  w2.setWindowId(1);
  w3.setWindowId(2);
  w4.setWindowId(3);
  
  // Position windows in 2x2 grid (quartiles)
  // Top-left quartile
  w1.setGeometry(startX, startY, halfWidth, halfHeight);
  w1.setWindowTitle("Plot Window 1 - Top Left");
  
  // Top-right quartile
  w2.setGeometry(startX + halfWidth, startY, halfWidth, halfHeight);
  w2.setWindowTitle("Plot Window 2 - Top Right");
  
  // Bottom-left quartile
  w3.setGeometry(startX, startY + halfHeight, halfWidth, halfHeight);
  w3.setWindowTitle("Plot Window 3 - Bottom Left");
  
  // Bottom-right quartile
  w4.setGeometry(startX + halfWidth, startY + halfHeight, halfWidth, halfHeight);
  w4.setWindowTitle("Plot Window 4 - Bottom Right");
  
  w1.show();
  w2.show();
  w3.show();
  w4.show();
  
  return a.exec();
}
