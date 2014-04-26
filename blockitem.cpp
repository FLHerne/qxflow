#include <QComboBox>
#include <QDebug>
#include <QGraphicsLinearLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>

#include "blockitem.h"
#include "extra_items.h"

//Public constructor
BlockItem::BlockItem(QPointF in_pos, QGraphicsItem* parent, QGraphicsScene* scene):
    QGraphicsItem(parent, scene) {
    setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsScenePositionChanges);
    setPos(in_pos);
    gridAlign();
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
        new_line_item = NULL;
        new_shape_item = NULL;
        tag_name = cur_elem.tagName();
        if (tag_name == "pen") {
            if (cur_elem.hasAttribute("color"))
                selected_pen.setColor(QColor(cur_elem.attribute("color")));
            if (cur_elem.hasAttribute("width"))
                selected_pen.setWidth(cur_elem.attribute("width").toFloat());
        } else if (tag_name == "linknode") {
            QVector<QPointF> corners = getXmlPoints(cur_elem, 1);
            if (corners.size()) addLinkNode(corners[0].x(), corners[0].y());
            else qDebug() << "Invalid linknode";
        } else if (tag_name == "line") {
            QVector<QPointF> corners = getXmlPoints(cur_elem, 2);
            if (corners.size()) {
                new_line_item = new QGraphicsLineItem(QLineF(corners[0], corners[1]), this);
            }
            else qDebug() << "Invalid line";
        } else if (tag_name == "rect") {
            QVector<QPointF> corners = getXmlPoints(cur_elem, 2);
            if (corners.size()) new_shape_item = new QGraphicsRectItem(QRectF(corners[0], corners[1]), this);
            else qDebug() << "Invalid rect";
        } else if (tag_name == "rhombus") {
            QVector<QPointF> corners = getXmlPoints(cur_elem, 2);
            if (corners.size()) new_shape_item = new MRhombusItem(QRectF(corners[0], corners[1]), this);
            else qDebug() << "Invalid rhombus";
        } else if (tag_name == "pllgram") {
            QVector<QPointF> corners = getXmlPoints(cur_elem, 2);
            float slew = cur_elem.attribute("skew").toFloat();
            if (corners.size()) new_shape_item = new MPllgramItem(QRectF(corners[0], corners[1]), slew, this);
            else qDebug() << "Invalid pllgram";
        } else if (tag_name == "roundrect") {
            QVector<QPointF> corners = getXmlPoints(cur_elem, 2);
            float radius = cur_elem.attribute("radius").toFloat();
            if (corners.size()) new_shape_item = new MRoundRectItem(QRectF(corners[0], corners[1]), radius, this);
            else qDebug() << "Invalid roundrect";
        } else if (tag_name == "poly") {
            QVector<QPointF> corners = getXmlPoints(cur_elem, cur_elem.attributes().length());
            if (corners.size()) new_shape_item = new QGraphicsPolygonItem(corners, this);
            else qDebug() << "Invalid poly";
        } else if (tag_name == "text") {
            addXmlText(cur_elem);
        } else if (tag_name == "widgetrow") {
            addXmlWidgetRow(cur_elem);
        } else {
            qDebug() << "Not a thing!";
        }
        QPen pen;
        QBrush brush;
        sub_elem = cur_elem.firstChildElement("pen");
        if (!sub_elem.isNull()) {
            if (sub_elem.hasAttribute("color"))
                pen.setColor(QColor(sub_elem.attribute("color")));
            if (sub_elem.hasAttribute("width"))
                pen.setWidthF(sub_elem.attribute("width").toFloat());
            if (new_line_item) new_line_item->setPen(pen);
            if (new_shape_item) new_shape_item->setPen(pen);
        }
        sub_elem = cur_elem.firstChildElement("brush");
        if (!sub_elem.isNull()) {
            if (sub_elem.hasAttribute("color")) {
                brush.setColor(QColor(sub_elem.attribute("color")));
                brush.setStyle(Qt::SolidPattern);
            }
            if (new_shape_item) new_shape_item->setBrush(brush);
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

//Protected
void BlockItem::addLinkNode(int in_x, int in_y) {
    link_nodes.append(new LinkNodeItem(in_x, in_y, this));
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
void BlockItem::addXmlText(QDomElement elem) {
    QVector<QPointF> corners = getXmlPoints(elem, 1);
    if (corners.size()) {
        QStringList font_styles;
        QColor font_color("black");
        int font_size = 12;
        QDomElement sub_elem = elem.firstChildElement("font");
        if (!sub_elem.isNull()) {
            font_styles = sub_elem.attribute("style").split(',');
            if (sub_elem.hasAttribute("color"))
                font_color = QColor(sub_elem.attribute("color"));
            if (sub_elem.hasAttribute("size"))
                font_size = sub_elem.attribute("size").toInt();
        }
        QFont text_font;
        text_font.setPixelSize(font_size);
        if (font_styles.contains("bold")) text_font.setBold(true);
        if (font_styles.contains("italic")) text_font.setItalic(true);
        if (font_styles.contains("underline")) text_font.setUnderline(true);
        QGraphicsItem* new_item;
        if (elem.attribute("editable") == "true") {
            QGraphicsTextItem* text_item = new QGraphicsTextItem(elem.attribute("text"), this);
            text_item->setTextInteractionFlags(Qt::TextEditorInteraction);
            text_item->setFont(text_font);
            text_item->setDefaultTextColor(font_color);
            new_item = text_item;
        } else {
            QGraphicsSimpleTextItem* stext_item = new QGraphicsSimpleTextItem(elem.attribute("text"), this);
            stext_item->setFont(text_font);
            stext_item->setBrush(font_color);
            new_item = stext_item;
        }
        if (elem.attribute("centered") == "true") {
            new_item->setPos(corners[0] - new_item->boundingRect().center());
        } else new_item->setPos(corners[0]);
    } else qDebug() << "Invalid text";
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
        QDomElement sub_elem = elem.firstChildElement();
        while (!sub_elem.isNull()) {
            if (sub_elem.tagName() == "palette") {
                base_widget->setPalette(QPalette(QColor(sub_elem.attribute("button"))));
            } else if (sub_elem.tagName() == "label") {
                box_layout->addWidget(new QLabel(sub_elem.attribute("text")));
            } else if (sub_elem.tagName() == "lineedit") {
                box_layout->addWidget(new QLineEdit(sub_elem.attribute("text")));
            } else if (sub_elem.tagName() == "combobox") {
                QComboBox* new_combobox = new QComboBox;
                box_layout->addWidget(new_combobox);
                if (sub_elem.attribute("editable") == "true") {
                    new_combobox->setEditable(true);
                    new_combobox->setInsertPolicy(QComboBox::NoInsert);
                    box_layout->setStretchFactor(new_combobox, 100);
                }
                new_combobox->addItems(sub_elem.attribute("items").split(','));
            }
            sub_elem = sub_elem.nextSiblingElement();
        }
        box_layout->addStretch();
        base_proxy->setWidget(base_widget);
        base_proxy->setPos(corners[0]);
        base_proxy->setMaximumSize(corners[1].x() - corners[0].x(), corners[1].y() - corners[0].y());
        base_proxy->setMinimumWidth(corners[1].x() - corners[0].x());
    } else qDebug() << "Invalid widgetrow";
}

#include "blockitem.moc"
