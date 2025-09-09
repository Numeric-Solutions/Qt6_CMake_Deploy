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
#include <QElapsedTimer>
#include <QGraphicsTextItem>
#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include "ringbuffer.h"

// #define constants here
constexpr int BUFFSIZE    = 25*60*20;  // 
constexpr int TAU         = 40;        // period between data points in ms
constexpr int WINDOWSIZE  = 25*60*5;   // 

// Threading control - set to true to enable multi-threading, false for single-threaded
constexpr bool ENABLE_THREADING = true;  // Change this before compiling

QT_BEGIN_NAMESPACE
namespace Ui {
class PlotWindow;
}
QT_END_NAMESPACE

// Worker thread for data generation
class DataWorker : public QObject
{
  Q_OBJECT

public:
  DataWorker(int windowId = 0);
  void startWork();
  void stopWork();

public slots:
  void generateData();

signals:
  void dataReady(double x, double yL, double yR);

private:
  QTimer *workerTimer;
  int count;
  int windowId;
  QMutex dataMutex;
};

class PlotWindow : public QWidget
{
  Q_OBJECT

public:
  PlotWindow(QWidget *parent = nullptr);
  ~PlotWindow();
  void setWindowId(int id);

private slots:
  void onNewData();
  void onThreadedData(double x, double yL, double yR);

private:
  void updateChart(double x, double yL, double yR);

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

  // Frame rate tracking
  QElapsedTimer        frameTimer;
  QGraphicsTextItem   *fpsTextItem;
  int                  frameCount;
  qint64               lastFpsUpdate;

  // Threading components
  QThread             *workerThread;
  DataWorker          *dataWorker;
  int                  windowId;
  QMutex               updateMutex;

private:
  Ui::PlotWindow *ui;
};
#endif // PLOTWINDOW_H
