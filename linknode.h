#ifndef LINKNODE_H
#define LINKNODE_H

#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include <QLinkedList>
#include <QPainter>
#include <QPen>

class LinkLineItem;

class LinkNodeItem : public QGraphicsEllipseItem {
    static constexpr float radius = 4.5;
public:
    LinkNodeItem(int in_x, int in_y, QGraphicsItem* parent):
        LinkNodeItem(in_x, in_y, Qt::yellow, Qt::blue, parent) {}
    LinkNodeItem(int in_x, int in_y, const QColor& normal, const QColor& active, QGraphicsItem* parent);
//Returns the offset from this item's center to
    //...the nearest corner of the scene grid if parent is a ChartScene.
    QPointF gridSnapOffset() const;
//Position of the item's center.
    QPointF centerPos() const { return pos() + QPointF(radius, radius); }
    void setCenterPos(const QPointF& pos) { setPos(pos - QPointF(radius, radius)); }
//Center of the item in scene coords. Will break if scaled.
    //TODO: Make it work when scaled!
    QPointF sceneCenterPos() const { return scenePos() + QPointF(radius, radius); }
//Used by qgraphicsitem_cast<> to quickly determine type.
    enum { Type = UserType + 101 };
    virtual int type() const { return Type; }
    QList<const LinkNodeItem*> connected() const { return connected_nodes; }
//Whether item is in contact with another node.
    bool highlighted() const { return prev_highlighted; }
//Paints the item - 'active_color' if overlapping another LinkNodeItem,
    //...or 'normal_color' otherwise.
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    virtual void wheelEvent(QGraphicsSceneWheelEvent* event);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value);
private:
//Spacing of grid, if parent is a ChartScene.
    int grid_size = 0;
//Highlight state from last paint event.
    bool prev_highlighted = false;
//For painting the item.
    QColor normal_color = Qt::yellow, active_color = Qt::blue;
    QPen normal_pen;
//List of overlapping nodes
    QList<const LinkNodeItem*> connected_nodes;
//Below is related to link-lines, and should go away.
    void drawCursorLine(const QPointF& to_point);
    void nextCursorLine();
    bool drawing = false;
    bool x_first = true;
    QPointF last_corner;
    LinkLineItem* x_line = NULL, *y_line = NULL;
    QLinkedList<LinkLineItem*> line_segments;
};

#endif //LINKNODE_H
