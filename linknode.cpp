#include <QGraphicsView>

#include "chartscene.h"
#include "extra_items.h"
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
    //TODO Rather clunky. Could be neater, probably.
    int left =   last_corner.x() < event_grid_pos.x() ? last_corner.x() : event_grid_pos.x();
    int right =  last_corner.x() > event_grid_pos.x() ? last_corner.x() : event_grid_pos.x();
    int top =    last_corner.y() < event_grid_pos.y() ? last_corner.y() : event_grid_pos.y();
    int bottom = last_corner.y() > event_grid_pos.y() ? last_corner.y() : event_grid_pos.y();
    int ux = x_first ? event_grid_pos.x() : last_corner.x();
    int uy = x_first ? last_corner.y() : event_grid_pos.y();

    x_line = new AliasingLineItem(left, uy, right, uy);
    scene()->addItem(x_line);
    y_line = new AliasingLineItem(ux, top, ux, bottom);
    scene()->addItem(y_line);

    QPen pen(Qt::black, 3);
    x_line->setPen(pen);
    y_line->setPen(pen);
    int item_step = grid_size ? grid_size : 20;
    for (int ix = left; ix <= right; ix += item_step) {
        new LinkNodeItem(ix, uy, Qt::transparent, Qt::blue, x_line);
    }
    for (int iy = top; iy <= bottom; iy += item_step) {
        new LinkNodeItem(ux, iy, Qt::transparent, Qt::blue, y_line);
    }
}

//Private
void LinkNodeItem::nextCursorLine() {
    line_segments.append(x_line);
    x_line = NULL;
    line_segments.append(y_line);
    y_line = NULL;
}
