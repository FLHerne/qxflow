#ifndef MISC_H
#define MISC_H
#include <QPointF>

//Round 'in_val' up or down to nearest multiple of 'step'.
int roundTo(float in_val, int step);
//Round x and y values of 'in_point' to nearest multiple of 'step'.
QPointF roundTo(const QPointF&, int step);

#endif //MISC_H