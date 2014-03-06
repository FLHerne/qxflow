#ifndef SCENE_CORE_H
#define SCENE_CORE_H

#include <QtGui>
#include <QDomElement>

class LinkNodeItem : public QGraphicsEllipseItem {
public:
    LinkNodeItem(int in_x, int in_y, QGraphicsItem* parent);
    QPoint gridSnapOffset() const;
    void setCenterPos(const QPointF& pos);
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    enum { Type = UserType + 101 };
    virtual int type() const { return Type; }
protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
private:
    //Center of the item in scene coords. Will break if scaled.
    //TODO: Make it work when scaled!
    QPointF sceneCenter() const { return scenePos() + QPointF(radius, radius); }
    static constexpr float radius = 4.5;
    QLinkedList<QGraphicsLineItem*> line_segments;
    QPointF last_corner;
};

class BlockItem : public QGraphicsItem {
public:
    BlockItem(QPointF in_pos, QGraphicsItem* parent = 0, QGraphicsScene* scene = 0);
    BlockItem(QPointF in_pos, QDomElement in_elem, QGraphicsItem* parent = 0, QGraphicsScene* scene = 0);
    void addLinkNode(int in_x, int in_y);
    virtual QRectF boundingRect() const { return bounding_rect; }
    virtual QPainterPath shape() const;
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget* = 0) {
        //painter->strokePath(shape_path, QPen(Qt::red, 4));
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
    QVector<QPointF> getXmlPoints(QDomElement elem, uint num_points);
    void addXmlText(QDomElement elem);
    void addXmlWidgetRow(QDomElement elem);
    mutable QRectF bounding_rect;
    mutable QPainterPath shape_path;
    mutable bool shape_outdated = false;
    QList<LinkNodeItem*> link_nodes;
    QPointF prev_pos;
};

#endif //SCENE_CORE_H
