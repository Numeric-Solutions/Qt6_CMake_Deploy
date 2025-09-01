#include "plotwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  a.setWindowIcon(QIcon(":/icons/ns256.png"));

  PlotWindow w;
  w.show();
  return a.exec();
}
