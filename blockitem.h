#ifndef BLOCKITEM_H
#define BLOCKITEM_H

#include <QDomElement>
#include <QGraphicsItem>
#include <QPainter>
#include <QPen>

#include "linknode.h"

class BlockItem : public QGraphicsItem {
public:
    BlockItem(QPointF in_pos, QGraphicsItem* parent = 0, QGraphicsScene* scene = 0);
    BlockItem(QPointF in_pos, QDomElement in_elem, QGraphicsItem* parent = 0, QGraphicsScene* scene = 0);
    void addLinkNode(int in_x, int in_y);
    virtual QRectF boundingRect() const { return bounding_rect; }
    virtual QPainterPath shape() const;
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget* = 0) {
        if (isSelected()) painter->strokePath(shape_path, selected_pen);
    }
    enum { Type = UserType + 102 };
    virtual int type() const { return Type; }
protected:
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    void gridAlign();
    void updateShape(const QGraphicsItem* in_item = 0) const;
private:
    QPen selected_pen = QPen(Qt::black, 3);
    QVector<QPointF> getXmlPoints(QDomElement elem, uint num_points);
    void addXmlText(QDomElement elem);
    void addXmlWidgetRow(QDomElement elem);
    mutable QRectF bounding_rect;
    mutable QPainterPath shape_path;
    mutable bool shape_outdated = false;
    QList<LinkNodeItem*> link_nodes;
    QPointF prev_pos;
};

#endif //BLOCKITEM_H
