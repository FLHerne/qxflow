#ifndef BLOCKITEM_H
#define BLOCKITEM_H

#include <QDomElement>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QPen>
#include <QStyle>

#include "linknode.h"

class BlockItem : public QGraphicsItem {
public:
    BlockItem(QPointF in_pos, QGraphicsItem* parent = 0, QGraphicsScene* scene = 0);
    BlockItem(QPointF in_pos, QDomElement in_elem, QGraphicsItem* parent = 0, QGraphicsScene* scene = 0);
    virtual ~BlockItem() { delete widget_style; }
//Shape is union of descendants.
    virtual QPainterPath shape() const;
    virtual QRectF boundingRect() const { return bounding_rect; }
//Used by qgraphicsitem_cast<> to determine type quickly.
    enum { Type = UserType + 102 };
    virtual int type() const { return Type; }
//Draws colored outline if item is selected.
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget* = 0) {
        if (isSelected()) painter->strokePath(shape_path, selected_pen);
    }
protected:
//Align to the grid if parent is a ChartScene.
    void gridAlign();
//Re-calculate shape from those of descendants.
    void updateShape(const QGraphicsItem* in_item = 0) const;
//Handle movement and other events.
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value);
private:
//Get list of 'num_points' points from an XML element's attributes.
    //Returns empty vector in case of failure.
    QVector<QPointF> getXmlPoints(QDomElement elem, uint num_points);
//Add child items based on XML elements.
    void addXmlLinkNode(QDomElement elem);
    void addXmlPng(QDomElement elem);
    void addXmlSvg(QDomElement elem);
    void addXmlWidgetRow(QDomElement elem);
//Cache shape.
    mutable QPainterPath shape_path;
    mutable bool shape_outdated = false;
    mutable QRectF bounding_rect;
//Pen to draw outline when selected.
    QPen selected_pen;
//Style for widgets.
    QStyle* widget_style;
//List of child LinkNodes.
    QList<LinkNodeItem*> link_nodes;
//Position at start of move event series.
    QPointF prev_pos;
};

#endif //BLOCKITEM_H
