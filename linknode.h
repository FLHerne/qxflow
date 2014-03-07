#ifndef LINKNODE_H
#define LINKNODE_H

#include <QGraphicsEllipseItem>
#include <QLinkedList>

class LinkNodeItem : public QGraphicsEllipseItem {
public:
    LinkNodeItem(int in_x, int in_y, QGraphicsItem* parent):
        LinkNodeItem(in_x, in_y, Qt::yellow, Qt::blue, parent) {}
    LinkNodeItem(int in_x, int in_y, const QColor& normal, const QColor& active, QGraphicsItem* parent);
    QPointF gridSnapOffset() const;
    void setCenterPos(const QPointF& pos);
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    enum { Type = UserType + 101 };
    virtual int type() const { return Type; }
protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    virtual void wheelEvent(QGraphicsSceneWheelEvent* event);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value);
private:
    bool highlighted = false;
    //Center of the item in scene coords. Will break if scaled.
    //TODO: Make it work when scaled!
    QPointF sceneCenter() const { return scenePos() + QPointF(radius, radius); }
    void drawCursorLine(const QPointF& to_point);
    void nextCursorLine();
    static constexpr float radius = 4.5;
    bool x_first = true;
    int grid_size = 0;
    bool drawing = false;
    QGraphicsLineItem* x_line = NULL, *y_line = NULL;
    QLinkedList<QGraphicsLineItem*> line_segments;
    QPointF last_corner;
    QColor normal_color = Qt::yellow, active_color = Qt::blue;
    QPen normal_pen;
};

#endif //LINKNODE_H
