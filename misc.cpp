#include "misc.h"

//Global
int roundTo(float in_val, int step) {
    return std::roundf(in_val / step) * step;
}

QPointF roundTo(const QPointF& in_point, int step) {
return QPointF(roundTo(in_point.x(), step),
              roundTo(in_point.y(), step));
}
