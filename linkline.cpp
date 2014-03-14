#include <QDebug>
#include <QGraphicsScene>

#include "linkline.h"
#include "misc.h"

//Public constructor
LinkLineItem::LinkLineItem(const QLineF& in_line, QGraphicsItem* parent): QGraphicsLineItem(parent) {
    setLine(in_line);
    setPen(QPen(QColor::fromHsv(qrand() % 256, 255, 190), 3));
}

//Public (hides non-virtual base)
void LinkLineItem::setLine(const QLineF& in_line) {
    QLineF aligned(roundTo(in_line.p1(), grid_size), roundTo(in_line.p2(), grid_size));
    if (aligned.y1() == aligned.y2()) horizontal = true;
    else {
        horizontal = false;
        if (aligned.x1() != aligned.x2()) qDebug() << "Not square!";
    }
    if (aligned.p1() != line().p1()) {
        if (p1_node)
            p1_node->setCenterPos(aligned.p1());
        else
            p1_node = new LinkNodeItem(in_line.x1(), in_line.y1(), Qt::transparent, Qt::blue, this);
    }
    if (aligned.p2() != line().p2()) {
        if (p2_node)
            p2_node->setCenterPos(aligned.p2());
        else
            p2_node = new LinkNodeItem(in_line.x2(), in_line.y2(), Qt::transparent, Qt::blue, this);
    }
    QGraphicsLineItem::setLine(aligned);
}

//Public virtual
void LinkLineItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    updateNodes();
    painter->setRenderHint(QPainter::Antialiasing, false);
    QGraphicsLineItem::paint(painter, option, widget);
}

void LinkLineItem::updateNodes() {
    const QGraphicsItem* cur_item;
    const LinkNodeItem* cur_node;
    QPointF aligned, new_node_pos;
    foreach (cur_item, collidingItems()) {
        if (isAncestorOf(cur_item))
            continue;
        if (cur_node = qgraphicsitem_cast<const LinkNodeItem*>(cur_item)) {
            aligned = roundTo(cur_node->sceneCenterPos(), grid_size);
            new_node_pos = QPointF(horizontal ? aligned.x() : line().p1().x(), horizontal ? line().p1().y() : aligned.y());
            if (new_node_pos == line().p1() || new_node_pos == line().p2())
                continue;
            scene()->addItem(new LinkLineItem(QLineF(new_node_pos, line().p2())));
            setLine(QLineF(line().p1(),new_node_pos));
            break;
        }
    }
}
