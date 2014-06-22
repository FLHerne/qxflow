#include <QComboBox>
#include <QDebug>
#include <QFileInfo>
#include <QGraphicsLinearLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSvgRenderer>
#include <QWindowsStyle>

#include "blockitem.h"
#include "extra_items.h"

//Public constructor
BlockItem::BlockItem(QPointF in_pos, QGraphicsItem* parent, QGraphicsScene* scene):
    QGraphicsItem(parent, scene) {
    setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsScenePositionChanges);
    setPos(in_pos);
    gridAlign();
    widget_style = new QWindowsStyle;
}

//Public constructor
BlockItem::BlockItem(QPointF in_pos, QDomElement in_elem, QGraphicsItem* parent, QGraphicsScene* scene):
    BlockItem(in_pos, parent, scene) {
    QDomElement cur_elem = in_elem.firstChildElement();
    QDomElement sub_elem;
    QString tag_name;
    QGraphicsLineItem* new_line_item;
    QAbstractGraphicsShapeItem* new_shape_item;
    while (!cur_elem.isNull()) {
        tag_name = cur_elem.tagName();
        if (tag_name == "pen") {
            selected_pen.setColor(QColor(cur_elem.attribute("color", "black")));
            selected_pen.setWidth(cur_elem.attribute("width", "3").toFloat());
        } else if (tag_name == "png") {
            addXmlPng(cur_elem);
        } else if (tag_name == "svg") {
            addXmlSvg(cur_elem);
        } else if (tag_name == "linknode") {
            addXmlLinkNode(cur_elem);
        } else if (tag_name == "widgetrow") {
            addXmlWidgetRow(cur_elem);
        } else {
            qDebug() << "Unknown unit type" << tag_name;
        }
        cur_elem = cur_elem.nextSiblingElement();
    }
}

//Public virtual
QPainterPath BlockItem::shape() const {
    if (shape_outdated) {
        updateShape();
    }
    return shape_path;
}

//Protected
void BlockItem::gridAlign() {
    if (link_nodes.size()) {
        setPos(pos() + link_nodes[0]->gridSnapOffset());
    }
}

//Protected
void BlockItem::updateShape(const QGraphicsItem* in_item) const {
    QPainterPath return_path;
    if (!in_item) {
        in_item = this;
        shape_path = QPainterPath();
    }
    foreach (const QGraphicsItem * cur_item, in_item->childItems()) {
        if (cur_item->type() != UserType + 101) {
            if (qgraphicsitem_cast<const QGraphicsObject*>(cur_item)) {
                //QGraphicsObject doesn't offset its shape().
                //This may be intended, but is inconvenient.
                shape_path += cur_item->shape().translated(cur_item->pos());
            } else {
                shape_path += cur_item->shape();
                updateShape(cur_item);
            }
        }
    }
    bounding_rect = shape_path.boundingRect();
    shape_outdated = false;
}

//Protected virtual
void BlockItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    QGraphicsItem::mousePressEvent(event);
    setZValue(1);
    prev_pos = pos();
}

//Protected virtual
void BlockItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    QGraphicsItem::mouseReleaseEvent(event);
    gridAlign();
    setZValue(0);
    QGraphicsItem* cur_item;
    QList<const QGraphicsItem*> colliding_non_nodes;
    foreach (cur_item, collidingItems()) {
        if (cur_item->type() == UserType + 101 || this->isAncestorOf(cur_item)) {
            continue;
        } else {
            colliding_non_nodes.append(cur_item);
        }
    }
    if (colliding_non_nodes.size()) {
        setPos(prev_pos);
    }
}

//Protected virtual
QVariant BlockItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value) {
    switch (change) {
    case ItemChildAddedChange: //Fall through
    case ItemChildRemovedChange:
        shape_outdated = true;
        break;
    case ItemSceneHasChanged:
        gridAlign();
        break;
    case ItemScenePositionHasChanged:
        foreach(LinkNodeItem* cur_node, link_nodes) {
            cur_node->update();
        }
    }
    return QGraphicsItem::itemChange(change, value);
}

//Private
QVector<QPointF> BlockItem::getXmlPoints(QDomElement elem, uint num_points) {
    QVector<QPointF> points(num_points);
    QString num_text;
    bool valid = true;
    QStringList coords;
    while (num_points) {
        num_text.setNum(num_points);
        --num_points;           //Do this here, because vector indexed from 0 but points from 1.
        coords = elem.attribute("p" + num_text).split(',');
        if (coords.size() != 2) valid = false;
        else {
            points[num_points].setX(coords[0].toFloat(&valid));
            points[num_points].setY(coords[1].toFloat(&valid));
        }
    }
    if (!valid) points.clear(); //Return an empty vector if points invalid.
    return points;
}

//Private
void BlockItem::addXmlLinkNode(QDomElement elem) {
    QVector<QPointF> corners = getXmlPoints(elem, 1);
    if (corners.size()) {
        link_nodes.append(new LinkNodeItem(corners[0].toPoint(), this));
    }
    else qDebug() << "Invalid linknode unit";
}

//Private
void BlockItem::addXmlPng(QDomElement elem) {
    QString filename = elem.attribute("file");
    QVector<QPointF> corners = getXmlPoints(elem, 1);
    if (filename.isEmpty() || !corners.size()) {
        qDebug() << "Invalid png unit";
        return;
    }
    QFileInfo fileinfo(filename);
    if (fileinfo.isReadable()) {
        QPixmap pm(filename);
        if (!pm.isNull()) {
            QGraphicsItem* new_item = new QGraphicsPixmapItem(pm, this);
            new_item->setPos(corners[0]);
        } else qDebug() << "Failed to create pixmap from" << filename;
    } else qDebug() << "File" << filename << "not readable";
}

//Private
void BlockItem::addXmlSvg(QDomElement elem) {
    QString filename = elem.attribute("file");
    QVector<QPointF> corners = getXmlPoints(elem, 1);
    if (filename.isEmpty() || !corners.size()) {
        qDebug() << "Invalid svg unit";
        return;
    }
    QFileInfo fileinfo(filename);
    if (fileinfo.isReadable()) {
        QSvgRenderer rdr(filename);
        QPixmap pm(rdr.defaultSize());
        pm.fill(Qt::transparent);
        QPainter pntr(&pm);
        pntr.setRenderHint(QPainter::Antialiasing);
        rdr.render(&pntr);
        QGraphicsItem* new_item = new QGraphicsPixmapItem(pm, this);
        new_item->setPos(corners[0]);
    } else qDebug() << "File" << filename << "not readable";
}

//Private
void BlockItem::addXmlWidgetRow(QDomElement elem) {
    QVector<QPointF> corners = getXmlPoints(elem, 2);
    if (corners.size()) {
        QHBoxLayout* box_layout = new QHBoxLayout;
        box_layout->setSpacing(0);
        box_layout->setContentsMargins(1,1,1,1);
        QWidget* base_widget = new QWidget;
        base_widget->setLayout(box_layout);
        QGraphicsProxyWidget* base_proxy = new QGraphicsProxyWidget(this);
        base_proxy->setZValue(0.5);
        box_layout->addStretch();
        QWidget* new_widget = nullptr;
        QDomElement sub_elem = elem.firstChildElement();
        while (!sub_elem.isNull()) {
            new_widget = nullptr;
            if (sub_elem.tagName() == "palette") {
                base_widget->setPalette(QPalette(QColor(sub_elem.attribute("button"))));
            } else if (sub_elem.tagName() == "label") {
                box_layout->addWidget(new_widget = new QLabel(sub_elem.attribute("text")));
            } else if (sub_elem.tagName() == "lineedit") {
                box_layout->addWidget(new_widget = new QLineEdit(sub_elem.attribute("text")));
            } else if (sub_elem.tagName() == "combobox") {
                QComboBox* new_combobox = new QComboBox;
                new_combobox->setStyle(widget_style);
                box_layout->addWidget(new_combobox);
                if (sub_elem.attribute("editable") == "true") {
                    new_combobox->setEditable(true);
                    new_combobox->setInsertPolicy(QComboBox::NoInsert);
                    box_layout->setStretchFactor(new_combobox, 100);
                }
                new_combobox->addItems(sub_elem.attribute("items").split(','));
                new_widget = new_combobox;
            }
            if (new_widget) new_widget->setStyle(widget_style);
            sub_elem = sub_elem.nextSiblingElement();
        }
        box_layout->addStretch();
        base_proxy->setWidget(base_widget);
        base_proxy->setPos(corners[0]);
        base_proxy->setMaximumSize(corners[1].x() - corners[0].x(), corners[1].y() - corners[0].y());
        base_proxy->setMinimumWidth(corners[1].x() - corners[0].x());
    } else qDebug() << "Invalid widgetrow unit";
}

#include "blockitem.moc"
