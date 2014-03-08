#ifndef LINKLINE_H
#define LINKLINE_H

#include <QGraphicsLineItem>
#include <QLinkedList>

#include "linknode.h"

class LinkLineItem : public QGraphicsLineItem {
public:
    LinkLineItem(const QLineF& in_line, QGraphicsItem* parent = 0);
//Sets line rounded to grid.
    void setLine(const QLineF& in_line);
//Used by qgraphicsitem_cast<> to quickly determine type.
    enum { Type = UserType + 103 };
    virtual int type() const { return Type; }
//Creates child LinkNodes where other LinkNodes overlap.
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
private:
    void updateNodes();
    LinkNodeItem* p1_node = NULL, *p2_node = NULL;
//Spacing of grid, if parent is a ChartScene.
    int grid_size = 20;
//Whether line is horizontal (otherwise should be vertical).
    bool horizontal;
};

#endif //LINKLINE_H
