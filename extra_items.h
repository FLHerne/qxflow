#ifndef EXTRA_ITEMS_H
#define EXTRA_ITEMS_H
#include <QtGui>

class MRoundRectItem : public QGraphicsPathItem {
public:
    MRoundRectItem(const QRectF& rect, qreal radius, QGraphicsItem* parent = 0, QGraphicsScene* scene = 0);
};

class MRhombusItem : public QGraphicsPolygonItem {
public:
    MRhombusItem(const QRectF& rect, QGraphicsItem* parent = 0, QGraphicsScene* scene = 0);
};

class MPllgramItem : public QGraphicsPolygonItem {
public:
    MPllgramItem(const QRectF& rect, qreal slew, QGraphicsItem* parent = 0, QGraphicsScene* scene = 0);
};

#endif //EXTRA_ITEMS_H
