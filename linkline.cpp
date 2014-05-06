#include <QDebug>

#include "linkline.h"
#include "misc.h"

//Public constructor
LinkLineItem::LinkLineItem(const QLineF& line, QGraphicsItem* parent, QGraphicsScene* scene): QGraphicsLineItem(parent, scene) {
    setLine(line);
    setPen(QPen(Qt::black, 3));
    setFlag(ItemIsSelectable);
}

//Public (hides non-virtual base)
void LinkLineItem::setLine(const QLineF& line) {
    QLineF aligned(roundTo(line.p1(), grid_size), roundTo(line.p2(), grid_size));
    if (aligned.y1() == aligned.y2()) horizontal = true;
    else {
        horizontal = false;
        if (aligned.x1() != aligned.x2()) qDebug() << "Not square!";
    }
    foreach (LinkNodeItem* node, nodes) delete node;
    nodes.clear();
    QGraphicsLineItem::setLine(aligned);
}

//Public virtual
void LinkLineItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    painter->setRenderHint(QPainter::Antialiasing, false);
    QGraphicsLineItem::paint(painter, option, widget);
    const LinkNodeItem* cur_node;
    bool line_node;
    auto i = nodes.begin();
    while (i != nodes.end()) {
        if (!(*i)->highlighted()) {
            delete *i;
            i = nodes.erase(i);
        } else {
            line_node = true;
            (*i)->updateConnections();
            foreach (cur_node, (*i)->connections()) {
                if (!cur_node->parentItem() || !qgraphicsitem_cast<LinkLineItem*>(cur_node->parentItem())) {
                    line_node = false;
                    break;
                }
            }
            if (line_node) {
                delete *i;
                i = nodes.erase(i);
            } else ++i;
        }
    }
    QGraphicsItem* cur_item;
    QPointF aligned, new_node_pos;
    bool node_exists;
    foreach (cur_item, collidingItems(Qt::IntersectsItemBoundingRect)) {
        cur_node = qgraphicsitem_cast<LinkNodeItem*>(cur_item);
        if (cur_node) {
            if (qgraphicsitem_cast<LinkLineItem*>(cur_node->parentItem())) continue;
            aligned = roundTo(cur_node->sceneCenterPos(), grid_size);
            new_node_pos = QPointF(horizontal ? aligned.x() : line().p1().x(), horizontal ? line().p1().y() : aligned.y());
            node_exists = false;
            foreach (cur_node, nodes) {
                if (cur_node->centerPos() == new_node_pos) {
                    node_exists = true;
                    break;
                }
            }
            if (!node_exists) nodes.append(new LinkNodeItem(new_node_pos.x(), new_node_pos.y(), this));
        }
    }
}
