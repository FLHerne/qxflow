#include "extra_items.h"

MRoundRectItem::MRoundRectItem(const QRectF& rect, qreal radius, QGraphicsItem* parent, QGraphicsScene* scene):
    QGraphicsPathItem(parent, scene) {
    QPainterPath path;
    path.addRoundedRect(rect, radius, radius);
    setPath(path);
}

MRhombusItem::MRhombusItem(const QRectF& rect, QGraphicsItem* parent, QGraphicsScene* scene):
    QGraphicsPolygonItem(parent, scene) {
    QPolygonF poly(4);
    poly[0] = QPointF(rect.center().x(), rect.top());
    poly[1] = QPointF(rect.right(), rect.center().y());
    poly[2] = QPointF(rect.center().x(), rect.bottom());
    poly[3] = QPointF(rect.left(),  rect.center().y());
    setPolygon(poly);
}

MPllgramItem::MPllgramItem(const QRectF& rect, qreal slew, QGraphicsItem* parent, QGraphicsScene* scene):
    QGraphicsPolygonItem(parent, scene) {
    QPolygonF poly(4);
    if (slew >= 0) {
        poly[0] = rect.topLeft() + QPointF(slew, 0);
        poly[1] = rect.topRight();
        poly[2] = rect.bottomRight() - QPointF(slew, 0);
        poly[3] = rect.bottomLeft();
    } else {
        poly[0] = rect.topLeft();
        poly[1] = rect.topRight() - QPointF(slew, 0);
        poly[2] = rect.bottomRight();
        poly[3] = rect.bottomLeft() + QPointF(slew, 0);
    }
    setPolygon(poly);
}

//Protected virtual
void ElevProxyWidget::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    QGraphicsProxyWidget::hoverEnterEvent(event);
    QGraphicsItem* cur_item = this;
    while (cur_item) {
        old_z.append(QPair<QGraphicsItem*, qreal>(cur_item, cur_item->zValue()));
        cur_item->setZValue(10);
        cur_item = cur_item->parentItem();
    }
}

//Protected virtual
void ElevProxyWidget::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    QGraphicsProxyWidget::hoverLeaveEvent(event);
    QPair<QGraphicsItem*, qreal> cur_pair;
    foreach (cur_pair, old_z) {
        if (cur_pair.first) cur_pair.first->setZValue(cur_pair.second);
    }
}
