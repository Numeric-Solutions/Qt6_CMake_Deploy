#include "ringbuffer.h"
#include <QVector>
#include <QDebug>

/* constructor: allocate memory for data and set parameters */
RingBuffer::RingBuffer(size_t capacity, QObject* parent)
    : QObject(parent), data(capacity)
{
    if (capacity == 0) throw std::invalid_argument("Capacity must be greater than 0");
    head  = 0;
    tail  = 0;
    count = 0;
    this->capacity = capacity;
}

/* add point to buffer */
void RingBuffer::append(const QPointF &point)
{
  QPointF* removedPoint = nullptr;
  QPointF oldPoint;
  
  // If buffer is full, we'll be overwriting a point
  if (count == capacity) {
    oldPoint = data[head];
    removedPoint = &oldPoint;
  }
  
  data[head] = point;
  head = (head + 1) % capacity;

  if(count < capacity)
  {
    count++;
  }
  else
  {
    /* else, increment the tail */
    tail = (tail + 1) % capacity;
  }
  
  // Update min/max tracking
  updateMinMax(point, removedPoint);
}

size_t RingBuffer::size() const
{
  return count;
}

bool RingBuffer::isFull() const
{
  return count == capacity;
}

const QPointF &RingBuffer::at(size_t index) const
{
  if (index >= count) throw std::out_of_range("Index out of range");
  return data[(tail + index) % capacity];
}

/* oldest point [imin] */
QPointF RingBuffer::front() const
{
  if (count == 0) throw std::out_of_range("Buffer is empty");
  return data[tail];
}

/* newest point [imax] */
QPointF RingBuffer::back() const
{
  if (count == 0) throw std::out_of_range("Buffer is empty");
  return data[(head + capacity - 1) % capacity];
}

/* get values at point [index] */
QPointF RingBuffer::get(size_t index) const
{
  if (index >= count) throw std::out_of_range("Index out of range");
  return at(index);
}

QList<QPointF> RingBuffer::toQVector() const
{
  QList<QPointF> vout;
  vout.reserve(static_cast<int>(count));
  for (size_t i = 0; i < count; ++i)
  {
    vout.append(at(i));
  }
  return vout;
}

QList<QPointF> RingBuffer::toQVector(int imin, int imax) const
{
  // Safety checks to prevent crashes
  if (count == 0) {
    return QList<QPointF>();  // Return empty list for empty buffer
  }
  
  // Clamp indices to valid range
  imin = std::max(0, imin);
  imax = std::min(imax, static_cast<int>(count) - 1);
  
  // Ensure valid range
  if (imin > imax) {
    return QList<QPointF>();  // Return empty list for invalid range
  }
  
  int winsize = imax - imin + 1;
  QList<QPointF> vout;
  vout.reserve(static_cast<int>(winsize));
  
  for (int i = imin; i <= imax; ++i)
  {
    vout.append(at(i));
  }
  return vout;
}

void RingBuffer::updateMinMax(const QPointF& newPoint, const QPointF* removedPoint)
{
  double newY = newPoint.y();
  
  // Handle first point or invalid state
  if (!minMaxValid || count <= 1)
  {
    yMin = yMax = newY;
    minMaxValid = true;
    return;
  }
  
  // Check if new point extends current range
  if (newY < yMin) yMin = newY;
  if (newY > yMax) yMax = newY;
  
  // If we removed a point, check if it was a min/max value
  if (removedPoint)
  {
    double removedY = removedPoint->y();
      
    // If we removed the current min or max, we need to recalculate
    if (removedY == yMin || removedY == yMax)
    {
      recalculateMinMax();
    }
  }
}

void RingBuffer::recalculateMinMax()
{
  if (count == 0)
  {
    minMaxValid = false;
    return;
  }
    
  yMin = yMax = at(0).y();
    
  for (size_t i = 1; i < count; ++i)
  {
    double y = at(i).y();
    if (y < yMin) yMin = y;
    if (y > yMax) yMax = y;
  }
    
  minMaxValid = true;
}

void RingBuffer::resetMinMax(){recalculateMinMax();}

void RingBuffer::clear()
{
  head = 0;
  tail = 0;
  count = 0;
  minMaxValid = false;
  yMin = std::numeric_limits<double>::max();
  yMax = std::numeric_limits<double>::lowest();
}
