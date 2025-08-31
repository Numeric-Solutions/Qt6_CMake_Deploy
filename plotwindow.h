#ifndef PLOTWINDOW_H
#define PLOTWINDOW_H

#include <QWidget>

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QMainWindow>
#include <QTimer>
#include <QVector>
#include <QPointF>
#include <QValueAxis>
#include <QVBoxLayout>
#include <QWidget>
#include <QFile>
#include <QDir>
#include "ringbuffer.h"

// #define constants here
constexpr int BUFFSIZE    = 25*60*20;  // 
constexpr int TAU         = 10;        // period between data points in ms
constexpr int WINDOWSIZE  = 25*60*5;   // 

QT_BEGIN_NAMESPACE
namespace Ui {
class PlotWindow;
}
QT_END_NAMESPACE

class PlotWindow : public QWidget
{
  Q_OBJECT

public:
  PlotWindow(QWidget *parent = nullptr);
  ~PlotWindow();

private slots:
  void onNewData();

private:
  QLineSeries         *seriesL;
  QLineSeries         *seriesR;
  QChartView          *chartView;
  QTimer              *timer;
  QLineSeriesBuffer    bufferL{BUFFSIZE, QPen(Qt::red, 1)};  /* faster */
  QLineSeriesBuffer    bufferR{BUFFSIZE, QPen(Qt::blue, 1)}; /* faster */
  int                  count = -1;  /* first point increments to zero */
  QValueAxis          *axisX;       /* axis X */
  QValueAxis          *axisYL;      /* axis Y (left) */
  QValueAxis          *axisYR;      /* axis Y (right) */
  QVBoxLayout *layout;

private:
  Ui::PlotWindow *ui;
};
#endif // PLOTWINDOW_H
