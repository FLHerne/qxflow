#ifndef EXTRA_ITEMS_H
#define EXTRA_ITEMS_H
#include <QGraphicsProxyWidget>
#include <QLinkedList>
#include <QPainter>

//A rounded-rectangle shape item with corner-radius 'radius'.
class MRoundRectItem : public QGraphicsPathItem {
public:
    MRoundRectItem(const QRectF& rect, qreal radius, QGraphicsItem* parent = 0, QGraphicsScene* scene = 0);
};

//A rhombus shape item.
class MRhombusItem : public QGraphicsPolygonItem {
public:
    MRhombusItem(const QRectF& rect, QGraphicsItem* parent = 0, QGraphicsScene* scene = 0);
};

//A parallelogram shape item, top corners 'skew' px right of bottom ones.
class MPllgramItem : public QGraphicsPolygonItem {
public:
    MPllgramItem(const QRectF& rect, qreal skew, QGraphicsItem* parent = 0, QGraphicsScene* scene = 0);
};

//Line item that overrides scene/view antialiasing settings.
class AliasingLineItem : public QGraphicsLineItem {
public:
    using QGraphicsLineItem::QGraphicsLineItem;
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0) {
        painter->setRenderHint(QPainter::Antialiasing, false);
        QGraphicsLineItem::paint(painter, option, widget);
    }
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
