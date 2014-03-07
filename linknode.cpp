#include <QGraphicsView>

#include "chartscene.h"
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

//Public virtual
void LinkNodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    const QGraphicsItem* cur_item;
    bool now_highlighted = false;
    foreach (cur_item, collidingItems(Qt::IntersectsItemBoundingRect)) {
        if (qgraphicsitem_cast<const LinkNodeItem*>(cur_item)) {
            now_highlighted = true;
            break;
        }
    }
    if (highlighted != now_highlighted) {
        highlighted = now_highlighted;
        setBrush(highlighted ? active_color : normal_color);
        setPen(highlighted ? QPen(): normal_pen);
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
        //TODO: Do something with these pointers!
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
        if (ChartScene* chart_scene = dynamic_cast<ChartScene*>(value.value<QGraphicsScene*>())) {
            grid_size = chart_scene->gridSize();
        } else grid_size = 0;
    }
    return QGraphicsItem::itemChange(change, value);
}

//Private
void LinkNodeItem::drawCursorLine(const QPointF& to_point) {
    if (x_line) delete x_line;
    if (y_line) delete y_line;
    QPointF event_grid_pos = roundTo(to_point, grid_size);
    QPointF corner_pos;
    if (x_first) {
        corner_pos = QPointF(event_grid_pos.x(), last_corner.y());
        x_line = scene()->addLine(QLineF(last_corner, corner_pos));
        y_line = scene()->addLine(QLineF(corner_pos, event_grid_pos));
    } else {
        corner_pos = QPointF(last_corner.x(), event_grid_pos.y());
        x_line = scene()->addLine(QLineF(event_grid_pos, corner_pos));
        y_line = scene()->addLine(QLineF(corner_pos, last_corner));
    }
    int node_gap = grid_size ? grid_size : 20;
    for (int ix = x_line->boundingRect().left(); ix <= x_line->boundingRect().right(); ix += node_gap) {
        new LinkNodeItem(ix, x_line->boundingRect().top(), Qt::transparent, Qt::blue, x_line);
    }
    for (int iy = y_line->boundingRect().top(); iy <= y_line->boundingRect().bottom(); iy += node_gap) {
        new LinkNodeItem(y_line->boundingRect().left(), iy, Qt::transparent, Qt::blue, y_line);
    }
}

//Private
void LinkNodeItem::nextCursorLine() {
    line_segments.append(x_line);
    x_line = NULL;
    line_segments.append(y_line);
    y_line = NULL;
}
