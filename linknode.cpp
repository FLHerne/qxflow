#include <QGraphicsView>

#include "chartscene.h"
#include "linkline.h"
#include "linknode.h"
#include "misc.h"

//Public constructor
LinkNodeItem::LinkNodeItem(int in_x, int in_y, const QColor& normal, const QColor& active, QGraphicsItem* parent):
    QGraphicsEllipseItem(0, 0, radius * 2, radius * 2, parent) {
    normal_color = normal;
    active_color = active;
    if (normal_color == Qt::transparent) normal_pen = Qt::NoPen;
    setBrush(normal_color);
    setPen(normal_pen);
    setCenterPos(QPoint(in_x, in_y));
    setZValue(1);
    ChartScene* chart_scene;
    if (scene() && (chart_scene = dynamic_cast<ChartScene*>(scene()))) {
        grid_size = chart_scene->gridSize();
    } else grid_size = 0;
}

//Public
QPointF LinkNodeItem::gridSnapOffset() const {
    if (grid_size) return roundTo(sceneCenterPos(), grid_size) - sceneCenterPos();
    else return QPointF();
}

//Public
bool LinkNodeItem::updateConnections() const {
    const QGraphicsItem* cur_item;
    const LinkNodeItem* cur_node;
    bool now_highlighted = false;
    connected_nodes.clear();
    foreach (cur_item, collidingItems(Qt::IntersectsItemBoundingRect)) {
        if (cur_node = qgraphicsitem_cast<const LinkNodeItem*>(cur_item)) {
            now_highlighted = true;
            connected_nodes.append(cur_node);
        }
    }
    bool highlight_changed = (now_highlighted != node_highlighted);
    node_highlighted = now_highlighted;
    return highlight_changed;
}

//Public virtual
void LinkNodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    if (updateConnections()) {
        setBrush(node_highlighted ? active_color : normal_color);
        setPen(node_highlighted ? QPen(): normal_pen);
    }
    QGraphicsEllipseItem::paint(painter, option, widget);
}

//Protected virtual
void LinkNodeItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        event->accept();
        if (!drawing) {
            drawing = true;
            grabMouse();
            foreach(QGraphicsView* view, scene()->views()) {
                view->setMouseTracking(true);
            }
            last_corner = sceneCenterPos();
        } else {
            nextCursorLine();
            last_corner = roundTo(event->scenePos(), grid_size);
        }
    } else if (event->button() == Qt::RightButton) {
        event->accept();
        ungrabMouse();
        nextCursorLine();
        line_segments.clear();
        drawing = false;
    }
}

//Protected virtual
void LinkNodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    event->accept();
    drawCursorLine(event->scenePos());
}

//Protected virtual
void LinkNodeItem::wheelEvent(QGraphicsSceneWheelEvent* event) {
    event->accept();
    x_first = !x_first;
    drawCursorLine(event->scenePos());
}

//Protected virtual
QVariant LinkNodeItem::itemChange(GraphicsItemChange change, const QVariant& value) {
    if (change == ItemSceneChange) {
        if (ChartScene* chart_scene = qobject_cast<ChartScene*>(value.value<QGraphicsScene*>())) {
            grid_size = chart_scene->gridSize();
        } else grid_size = 0;
    }
    return QGraphicsItem::itemChange(change, value);
}

//Private
void LinkNodeItem::drawCursorLine(const QPointF& to_point) {
    if (line_1) {
        delete line_1;
        line_1 = NULL;
    }
    if (line_2) {
        delete line_2;
        line_2 = NULL;
    }
    QPointF event_grid_pos = roundTo(to_point, grid_size);
    QPointF corner_pos(x_first ? event_grid_pos.x() : last_corner.x(),
                       x_first ? last_corner.y() : event_grid_pos.y());
    if (last_corner != corner_pos) {
        line_1 = new QGraphicsLineItem(QLineF(last_corner, corner_pos));
        line_1->setPen(QPen(Qt::black, 2));
        scene()->addItem(line_1);
    }
    if (corner_pos != event_grid_pos) {
        line_2 = new QGraphicsLineItem(QLineF(corner_pos, event_grid_pos));
        line_2->setPen(QPen(Qt::black, 2));
        scene()->addItem(line_2);
    }
}

//Private
void LinkNodeItem::nextCursorLine() {
    if (line_1) {
        line_segments.append(new LinkLineItem(line_1->line()));
        scene()->addItem(line_segments.back());
        delete line_1;
        line_1 = NULL;
    }
    if (line_2) {
        line_segments.append(new LinkLineItem(line_2->line()));
        scene()->addItem(line_segments.back());
        delete line_2;
        line_2 = NULL;
    }
}
