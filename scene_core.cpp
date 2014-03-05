#include "scene_core.h"
#include "extra_items.h"

//BEGIN LinkNodeItem
//Public
LinkNodeItem::LinkNodeItem(int in_x, int in_y, QGraphicsItem* parent):
    QGraphicsEllipseItem(0, 0, radius * 2, radius * 2, parent) {
    setPos(in_x - radius, in_y - radius);
    setZValue(1);
    setBrush(Qt::yellow);
}

QPoint LinkNodeItem::gridSnapOffset() const {
    QPointF center = scenePos() + QPointF(radius, radius);
    return QPoint(roundTo(center.x(), 20) - center.x(), roundTo(center.y(), 20) - center.y());
}

void LinkNodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    setBrush(Qt::yellow);
    const QGraphicsItem* cur_item;
    foreach (cur_item, collidingItems(Qt::IntersectsItemBoundingRect)) {
        if (qgraphicsitem_cast<const LinkNodeItem*>(cur_item)) {
            setBrush(Qt::blue);
            break;
        }
    }
    QGraphicsEllipseItem::paint(painter, option, widget);
}
//END

//BEGIN BlockItem
//Public
BlockItem::BlockItem(QPointF in_pos, QGraphicsItem* parent, QGraphicsScene* scene):
    QGraphicsItem(parent, scene) {
    setFlags(ItemIsMovable | ItemIsSelectable | ItemHasNoContents);
    setPos(in_pos);
    gridAlign();
}

BlockItem::BlockItem(QPointF in_pos, QDomElement in_elem, QGraphicsItem* parent, QGraphicsScene* scene):
    BlockItem(in_pos, parent, scene) {
    QDomElement cur_elem = in_elem.firstChildElement();
    QDomElement sub_elem;
    QString tag_name;
    QGraphicsItem* new_item;
    QGraphicsLineItem* new_line_item;
    QAbstractGraphicsShapeItem* new_shape_item;
    while (!cur_elem.isNull()) {
        new_item = NULL;
        new_line_item = NULL;
        new_shape_item = NULL;
        tag_name = cur_elem.tagName();
        if (tag_name == "linknode") {
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
            float slew = cur_elem.attribute("slew").toFloat();
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
            QVector<QPointF> corners = getXmlPoints(cur_elem, 1);
            if (corners.size()) {
                QStringList font_styles;
                QColor font_color("black");
                int font_size = 12;
                sub_elem = cur_elem.firstChildElement("font");
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
                if (cur_elem.attribute("editable") == "true") {
                    QGraphicsTextItem* text_item = new QGraphicsTextItem(cur_elem.attribute("text"), this);
                    text_item->setTextInteractionFlags(Qt::TextEditorInteraction);
                    text_item->setFont(text_font);
                    text_item->setDefaultTextColor(font_color);
                    new_item = text_item;
                } else {
                    QGraphicsSimpleTextItem* stext_item = new QGraphicsSimpleTextItem(cur_elem.attribute("text"), this);
                    stext_item->setFont(text_font);
                    stext_item->setBrush(font_color);
                    new_item = stext_item;
                }
                if (cur_elem.attribute("centered") == "true") {
                    new_item->setPos(corners[0] - new_item->boundingRect().center());
                } else new_item->setPos(corners[0]);
            } else qDebug() << "Invalid text";
        } else if (tag_name == "widgetrow") {
            QVector<QPointF> corners = getXmlPoints(cur_elem, 2);
            if (corners.size()) {
                QGraphicsLinearLayout* row_layout = new QGraphicsLinearLayout;
                row_layout->setSpacing(0);
                row_layout->setContentsMargins(1,1,1,1);
                row_layout->addStretch();
                QGraphicsWidget* row_widget = new QGraphicsWidget(this);
                row_widget->setPos(corners[0]);
                row_widget->setMaximumSize(corners[1].x() - corners[0].x(), corners[1].y() - corners[0].y());
                row_widget->setMinimumWidth(corners[1].x() - corners[0].x());
                row_widget->setLayout(row_layout);
                row_widget->setZValue(0.5);
                QGraphicsProxyWidget* new_proxy;
                sub_elem = cur_elem.firstChildElement();
                while (!sub_elem.isNull()) {
                    if (sub_elem.tagName() == "palette") {
                        row_widget->setPalette(QPalette(QColor(sub_elem.attribute("button"))));
                    } else if (sub_elem.tagName() == "label") {
                        new_proxy = new QGraphicsProxyWidget;
                        new_proxy->setWidget(new QLabel(sub_elem.attribute("text")));
                        row_layout->addItem(new_proxy);
                    } else if (sub_elem.tagName() == "lineedit") {
                        new_proxy = new QGraphicsProxyWidget;
                        new_proxy->setWidget(new QLineEdit(sub_elem.attribute("text")));
                        row_layout->addItem(new_proxy);
                    } else if (sub_elem.tagName() == "combobox") {
                        QComboBox* new_combobox = new QComboBox;
                        new_combobox->addItems(sub_elem.attribute("items").split(','));
                        new_proxy = new ElevProxyWidget;
                        new_proxy->setWidget(new_combobox);
                        row_layout->addItem(new_proxy);
                    }
                    sub_elem = sub_elem.nextSiblingElement();
                }
                row_layout->addStretch();
            } else qDebug() << "Invalid widgetrow";
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

void BlockItem::addLinkNode(int in_x, int in_y) {
    link_nodes.append(new LinkNodeItem(in_x, in_y, this));
}

QPainterPath BlockItem::shape() const {
    if (shape_outdated) {
        updateShape();
    }
    return shape_path;
}

//Protected
QVariant BlockItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value) {
    switch (change) {
    case ItemChildAddedChange: //Fall through
    case ItemChildRemovedChange:
        shape_outdated = true;
        break;
    case ItemSceneHasChanged:
        gridAlign();
        break;
    }
    return QGraphicsItem::itemChange(change, value);
}

void BlockItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    QGraphicsItem::mousePressEvent(event);
    setZValue(1);
    prev_pos = pos();
}

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

void BlockItem::gridAlign() {
    if (link_nodes.size()) {
        setPos(pos() + link_nodes[0]->gridSnapOffset());
    }
}

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
//END

//BEGIN ChartScene
bool ChartScene::addBlockFile(QString filename) {
    if (!filename.length()) {
        filename = QFileDialog::getOpenFileName(NULL, "Open File", "", "XML Files (*.xml)");
    }
    QFile file(filename);
    QDomDocument doc;
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Not opened!";
        return false;
    }
    file.open(QIODevice::ReadOnly);
    QString error;
    if (!doc.setContent(&file, &error)) {
        qDebug() << "Not set!: " << error;
        file.close();
        return false;
    }
    block_documents.append(doc);
    file.close();
    return true;
}

QStringList ChartScene::blockTypes() const {
    QDomElement block_elem;
    QStringList block_types;
    foreach (QDomDocument doc, block_documents) {
        block_elem = doc.documentElement().firstChildElement("block");
        while (!block_elem.isNull()) {
            block_types.append(block_elem.attribute("name"));
            block_elem = block_elem.nextSiblingElement("block");
        }
    }
    return block_types;
}

bool ChartScene::addBlock(const QString& block_name) {
    QDomElement block_elem;
    foreach (QDomDocument doc, block_documents) {
        block_elem = doc.documentElement().firstChildElement("block");
        while (!block_elem.isNull()) {
            if (block_elem.attribute("name") == block_name) {
                addItem(new BlockItem(QPoint(0,0), block_elem));
                return true;
            }
            block_elem = block_elem.nextSiblingElement("block");
        }
    }
    return false;
}

void ChartScene::drawBackground(QPainter* painter, const QRectF& rect) {
    QGraphicsScene::drawBackground(painter, rect);
    for (int ix = rect.left() - (int) rect.left() % 20; ix <= rect.right(); ix += 20) {
        painter->drawLine(ix, rect.top(), ix, rect.bottom());
    }
    for (int iy = rect.top() - (int) rect.top() % 20; iy <= rect.bottom(); iy += 20) {
        painter->drawLine(rect.left(), iy, rect.right(), iy);
    }
}
//END ChartScene

//BEGIN ElevProxyWidget
void ElevProxyWidget::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    QGraphicsProxyWidget::hoverEnterEvent(event);
    QGraphicsItem* cur_item = this;
    while (cur_item) {
        old_z.append(QPair<QGraphicsItem*, qreal>(cur_item, cur_item->zValue()));
        cur_item->setZValue(10);
        cur_item = cur_item->parentItem();
    }
}

void ElevProxyWidget::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    QGraphicsProxyWidget::hoverLeaveEvent(event);
    QPair<QGraphicsItem*, qreal> cur_pair;
    foreach (cur_pair, old_z) {
        if (cur_pair.first) cur_pair.first->setZValue(cur_pair.second);
    }
}
//END ElevProxyWidget

//BEGIN Misc
int roundTo(float in_val, int step) {
    return std::roundf(in_val / step) * step;
}
//END Misc
#include "scene_core.moc"
