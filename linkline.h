#ifndef LINKLINE_H
#define LINKLINE_H

#include <QGraphicsLineItem>
#include <QLinkedList>

#include "linknode.h"

class LinkLineItem : public QGraphicsLineItem {
public:
    LinkLineItem(const QLineF& line, QGraphicsItem* parent = 0, QGraphicsScene* scene = 0);
//Sets line rounded to grid.
    void setLine(const QLineF& line);
//Creates child LinkNodes where other LinkNodes overlap.
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
private:
//List of child linknodes, to avoid re-adding existing ones.
    QLinkedList<LinkNodeItem*> nodes;
//Spacing of grid, if parent is a ChartScene.
    int grid_size = 20;
//Whether line is horizontal (otherwise should be vertical).
    bool horizontal;
};

#endif //LINKLINE_H
