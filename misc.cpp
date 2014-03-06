#include "misc.h"

//Global
int roundTo(float in_val, int step) {
    return std::roundf(in_val / step) * step;
}

QPoint roundTo(const QPointF& in_point, int step) {
return QPoint(roundTo(in_point.x(), step),
              roundTo(in_point.y(), step));
}
