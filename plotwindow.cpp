#include "plotwindow.h"
#include "ui_plotwindow.h"

#include <cmath>
#include <cstdlib>

PlotWindow::PlotWindow(QWidget *parent) : QWidget(parent), ui(new Ui::PlotWindow)
{
  ui->setupUi(this);

  /* set window and chart theme */
  QString qssfilepth = QCoreApplication::applicationDirPath() + "/QTdark_PlotWin.qss";
  QFile qssfile(qssfilepth);  
  if(!qssfile.open(QFile::ReadOnly)){qDebug() << qssfilepth + " not found.";}
  else
  {
    QString StyleSheet = QLatin1String(qssfile.readAll());
    this->setStyleSheet(StyleSheet);
    qssfile.close();
  }
  ui->chartView->chart()->setTheme(QChart::ChartThemeDark);

  seriesL = new QLineSeries();
  seriesR = new QLineSeries();

  ui->chartView->setRenderHint(QPainter::Antialiasing, false);        
  ui->chartView->setDragMode(QGraphicsView::RubberBandDrag);          
  ui->chartView->chart()->setAnimationOptions(QChart::NoAnimation);   
  seriesL->setUseOpenGL(true);                                        
  seriesR->setUseOpenGL(true);                                        

  /* check if OpenGL is actually working */
  if (!seriesL->useOpenGL())
  {
    qWarning() << "OpenGL not enabled.";
    seriesL->setUseOpenGL(false);
    seriesR->setUseOpenGL(false);
  }

  /* make pen and set series */
  seriesL->setPen(bufferL.pen);
  seriesR->setPen(bufferR.pen);

  /* make axes */
  axisX  = new QValueAxis(this);
  axisYL = new QValueAxis(this);
  axisYR = new QValueAxis(this);

  /* add axes to chart */
  ui->chartView->chart()->addAxis(axisX,   Qt::AlignBottom);
  ui->chartView->chart()->addAxis(axisYL,  Qt::AlignLeft);
  ui->chartView->chart()->addAxis(axisYR,  Qt::AlignRight);

  /* add series to chart */
  ui->chartView->chart()->addSeries(seriesL);
  ui->chartView->chart()->addSeries(seriesR);

  /* add axes to series */
  seriesL->attachAxis(axisX);
  seriesL->attachAxis(axisYL);
  seriesR->attachAxis(axisX);
  seriesR->attachAxis(axisYR);

  /* set Title, hide legend and sets the main content area */
  ui->chartView->chart()->setTitle("Real-Time Sliding Plot with Buffer");
  ui->chartView->chart()->legend()->hide();

  /* set axis ranges */
  axisX->setRange(0.0, 0.0);
  axisYL->setRange(-12.0, 12.0);
  axisYR->setRange(-12.0, 12.0);

  /* create, connect, and start timer */
  timer   = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &PlotWindow::onNewData);
  timer->start(TAU); // frequency = 1/TAU Hz
}

PlotWindow::~PlotWindow()
{
  delete ui;
}

void PlotWindow::onNewData()
{
  /* generate some fake data */
  double x  = double(++count);
  double yL = 10.0 * std::sin(0.05 * x) + (std::rand() % 100) / 100.0;
  double yR = 10.0 * std::cos(0.03 * x) + (std::rand() % 100) / 100.0;

  /* add point to buffer */
  bufferL.buffer.append(QPointF(x, yL));
  bufferR.buffer.append(QPointF(x, yR));

  /* calculate the window-indicies in the buffer */
  int imax = std::min(BUFFSIZE-1, count);
  int imin = std::max(0, imax - WINDOWSIZE+1);

  /* copy data from buffer to QVector to QLineSeries */
  seriesL->replace(bufferL.buffer.toQVector(imin, imax));
  seriesR->replace(bufferR.buffer.toQVector(imin, imax));

  // Update axis range for auto-scroll mode
  double xmax = static_cast<double>(count);
  double xmin = static_cast<double>(std::max(0, count - WINDOWSIZE+1));
  axisX->setRange(xmin, xmax);

  // Update Y axis ranges based on buffer min/max
  double yMinL = bufferL.buffer.getYMin();
  double yMaxL = bufferL.buffer.getYMax();
  double yMinR = bufferR.buffer.getYMin();
  double yMaxR = bufferR.buffer.getYMax();
  
  if (yMinL < axisYL->min() || yMaxL > axisYL->max())
  {
    axisYL->setRange(yMinL, yMaxL);
  }
  if (yMinR < axisYR->min() || yMaxR > axisYR->max())
  {
    axisYR->setRange(yMinR, yMaxR);
  }
}
