#ifndef MISC_H
#define MISC_H
#include <QPointF>

//Round 'in_val' up or down to nearest multiple of 'step'.
inline int roundTo(float in_val, int step) {
    return std::roundf(in_val / step) * step;
}

//Round x and y values of 'in_point' to nearest multiple of 'step'.
inline QPointF roundTo(const QPointF& in_point, int step) {
    if (!step) return in_point;
    return QPointF(roundTo(in_point.x(), step),
                   roundTo(in_point.y(), step));
}

#endif //MISC_H
