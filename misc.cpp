#include "misc.h"

//Global
int roundTo(float in_val, int step) {
    return std::roundf(in_val / step) * step;
}
