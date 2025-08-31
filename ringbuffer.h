#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <QObject>
#include <QPointF>
#include <vector>
#include <stdexcept>
#include <QPen>
#include <QList>
#include <limits> // for std::numeric_limits

class RingBuffer : public QObject
{
    Q_OBJECT
public:
    explicit RingBuffer(size_t capacity, QObject* parent = nullptr);
    void append(const QPointF &point);
    size_t size() const;
    int buffSize() const { return capacity; }        // Get the maximum capacity of the buffer
    bool isFull() const;
    const QPointF &at(size_t index) const;
    QList<QPointF> toQVector()    const;                 // Convert to QVector for QLineSeries
    QList<QPointF> toQVector(int imin, int imax) const;  // Convert to QVector for QLineSeries
    QPointF front() const;                               // Get the oldest point [imin]
    QPointF back()  const;                               // Get the newest point [imax]
    QPointF get(size_t index) const;                     // Get a point at a specific index [i]
    
    // Min/Max tracking methods
    double getYMin() const { return yMin; }
    double getYMax() const { return yMax; }
    void resetMinMax();                                  // Force recalculation if needed
    void clear();                                        // Clear all data from the buffer

private:
    std::vector<QPointF> data;
    size_t head;                                        // Index for next write
    size_t tail;                                        // Index for oldest data
    size_t capacity;
    size_t count;                                       // Current number of elements
    
    // Min/Max tracking
    double yMin = std::numeric_limits<double>::max();
    double yMax = std::numeric_limits<double>::lowest();
    bool minMaxValid = false;
    
    void updateMinMax(const QPointF& newPoint, const QPointF* removedPoint = nullptr);
    void recalculateMinMax();  // Fallback when we can't track incrementally
};


// use to buffer data instead of QLineSeries
class QLineSeriesBuffer
{
public:
    QLineSeriesBuffer(size_t bufferSize, const QPen& pen) : buffer(bufferSize), pen(pen) {}

    RingBuffer buffer;
    void clear() {buffer.clear();}
    double getYMin()  const {return buffer.getYMin();}
    double getYMax()  const {return buffer.getYMax();}
    QPointF get(size_t index) const {return buffer.get(index);}
    size_t size()     const {return buffer.size();   }
    size_t capacity() const {return buffer.buffSize();}
    QPointF front() const {return buffer.front();};
    QPointF back()  const {return buffer.back(); };
    
    QList<QPointF> toQVector() const {return buffer.toQVector();};
    QList<QPointF> toQVector(int imin, int imax) const {return buffer.toQVector(imin, imax);};
    QPen pen;

private:
    // size_t bufferSize; // Size of the buffer

};
#endif // RINGBUFFER_H
