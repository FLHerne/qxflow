#ifndef EXTRA_ITEMS_H
#define EXTRA_ITEMS_H
#include <QGraphicsProxyWidget>
#include <QLinkedList>

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

//QGraphicsProxyWidget that raises all ancestors' z-values on hover.
//This is necessary to prevent popup menus being hidden under other items.
class ElevProxyWidget : public QGraphicsProxyWidget {
    Q_OBJECT
public:
    using QGraphicsProxyWidget::QGraphicsProxyWidget;
protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);
private:
    QLinkedList<QPair<QGraphicsItem*, qreal>> old_z;
};

#endif //EXTRA_ITEMS_H
