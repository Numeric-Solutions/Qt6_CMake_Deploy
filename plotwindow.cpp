#include "plotwindow.h"
#include "ui_plotwindow.h"

#include <cmath>
#include <cstdlib>

// DataWorker implementation
DataWorker::DataWorker(int windowId) : QObject(), count(-1), windowId(windowId)
{
  workerTimer = new QTimer(this);
  connect(workerTimer, &QTimer::timeout, this, &DataWorker::generateData);
}

void DataWorker::startWork()
{
  workerTimer->start(TAU);
}

void DataWorker::stopWork()
{
  workerTimer->stop();
}

void DataWorker::generateData()
{
  QMutexLocker locker(&dataMutex);
  
  // Generate data with slight variations per window to show async behavior
  double x = double(++count);
  double phaseOffset = windowId * 0.1; // Small phase offset per window
  double yL = 10.0 * std::sin(0.05 * x + phaseOffset) + (std::rand() % 100) / 100.0;
  double yR = 10.0 * std::cos(0.03 * x + phaseOffset) + (std::rand() % 100) / 100.0;
  
  emit dataReady(x, yL, yR);
}

// PlotWindow implementation

PlotWindow::PlotWindow(QWidget *parent) : QWidget(parent), ui(new Ui::PlotWindow), windowId(0)
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

  /* Initialize frame rate counter */
  fpsTextItem = new QGraphicsTextItem();
  fpsTextItem->setPlainText("FPS: 0.0");
  fpsTextItem->setDefaultTextColor(Qt::white);
  fpsTextItem->setPos(10, 10); // Position in top-left corner
  ui->chartView->scene()->addItem(fpsTextItem);
  
  frameCount = 0;
  lastFpsUpdate = 0;
  frameTimer.start();

  /* set axis ranges */
  axisX->setRange(0.0, 0.0);
  axisYL->setRange(-12.0, 12.0);
  axisYR->setRange(-12.0, 12.0);

  /* Setup based on compile-time threading setting */
  if constexpr (ENABLE_THREADING) {
    // Setup threading - but don't start worker yet (wait for setWindowId)
    workerThread = new QThread(this);
    dataWorker = nullptr; // Will be created in setWindowId
    
    // No regular timer needed in threaded mode
    timer = nullptr;
  } else {
    // Setup single-threaded mode
    workerThread = nullptr;
    dataWorker = nullptr;
    
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &PlotWindow::onNewData);
    timer->start(TAU); // frequency = 1/TAU Hz
  }
}

PlotWindow::~PlotWindow()
{
  if constexpr (ENABLE_THREADING) {
    if (dataWorker) {
      dataWorker->stopWork();
    }
    if (workerThread && workerThread->isRunning()) {
      workerThread->quit();
      workerThread->wait(3000);
    }
  }
  delete ui;
}

void PlotWindow::setWindowId(int id)
{
  windowId = id;
  if constexpr (ENABLE_THREADING) {
    if (workerThread) {
      // Stop any existing worker
      if (dataWorker) {
        dataWorker->stopWork();
        if (workerThread->isRunning()) {
          workerThread->quit();
          workerThread->wait(1000);
        }
        dataWorker->deleteLater();
        dataWorker = nullptr;
      }
      
      // Create new worker with correct window ID
      dataWorker = new DataWorker(windowId);
      dataWorker->moveToThread(workerThread);
      
      // Set up connections
      connect(workerThread, &QThread::started, dataWorker, &DataWorker::startWork);
      connect(dataWorker, &DataWorker::dataReady, this, &PlotWindow::onThreadedData, Qt::QueuedConnection);
      connect(this, &PlotWindow::destroyed, workerThread, &QThread::quit);
      connect(workerThread, &QThread::finished, dataWorker, &DataWorker::deleteLater);
      
      // Start the worker thread
      workerThread->start();
    }
  }
}

void PlotWindow::onThreadedData(double x, double yL, double yR)
{
  if constexpr (ENABLE_THREADING) {

    // Debug output (remove after testing)
    static int debugCount = 0;
    // if (++debugCount % 100 == 0) {
      qDebug() << "Window" << windowId << "received threaded data point" << x;
    // }

    QMutexLocker locker(&updateMutex);
    updateChart(x, yL, yR);
  }
}

void PlotWindow::updateChart(double x, double yL, double yR)
{
  /* Update frame rate counter */
  frameCount++;
  qint64 currentTime = frameTimer.elapsed();
  
  // Update FPS display every 1000ms (1 second)
  if (currentTime - lastFpsUpdate >= 1000)
  {
    double fps = frameCount * 1000.0 / (currentTime - lastFpsUpdate);
    QString mode = ENABLE_THREADING ? "MT" : "ST"; // MT = Multi-threaded, ST = Single-threaded
    fpsTextItem->setPlainText(QString("FPS: %1 | W%2 (%3)").arg(fps, 0, 'f', 1).arg(windowId + 1).arg(mode));
    frameCount = 0;
    lastFpsUpdate = currentTime;
  }

  /* add point to buffer */
  bufferL.buffer.append(QPointF(x, yL));
  bufferR.buffer.append(QPointF(x, yR));

  /* calculate the window-indicies in the buffer */
  int imax = std::min(BUFFSIZE-1, static_cast<int>(x));
  int imin = std::max(0, imax - WINDOWSIZE+1);

  /* copy data from buffer to QVector to QLineSeries */
  seriesL->replace(bufferL.buffer.toQVector(imin, imax));
  seriesR->replace(bufferR.buffer.toQVector(imin, imax));

  // Update axis range for auto-scroll mode
  double xmax = x;
  double xmin = static_cast<double>(std::max(0, static_cast<int>(x) - WINDOWSIZE+1));
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

void PlotWindow::onNewData()
{
  if constexpr (ENABLE_THREADING) {
    return; // Skip if compiled for threaded mode
  }
  
  /* generate some fake data */
  double x  = double(++count);
  double yL = 10.0 * std::sin(0.05 * x) + (std::rand() % 100) / 100.0;
  double yR = 10.0 * std::cos(0.03 * x) + (std::rand() % 100) / 100.0;

  updateChart(x, yL, yR);
}
