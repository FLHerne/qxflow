#ifndef SCENE_CORE_H
#define SCENE_CORE_H

#include <QtGui>
#include <QtXml>

int roundTo(float in_val, int step);

class LinkNodeItem : public QGraphicsEllipseItem {
public:
    LinkNodeItem(int in_x, int in_y, QGraphicsItem* parent);
    QPoint gridSnapOffset() const;
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    enum { Type = UserType + 101 };
    virtual int type() const { return Type; }
private:
    static constexpr float radius = 4.5;
};

class BlockItem : public QGraphicsItem {
public:
    BlockItem(QPointF in_pos, QGraphicsItem* parent = 0, QGraphicsScene* scene = 0);
    BlockItem(QPointF in_pos, QDomElement in_elem, QGraphicsItem* parent = 0, QGraphicsScene* scene = 0);
    void addLinkNode(int in_x, int in_y);
    virtual QRectF boundingRect() const { return shape().boundingRect(); }
    virtual QPainterPath shape() const;
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget* = 0) {
//         painter->strokePath(shape_path, QPen(Qt::red, 4));
    }
    enum { Type = UserType + 102 };
    virtual int type() const { return Type; }
protected:
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    void gridAlign();
    void updateShape(const QGraphicsItem* in_item = 0) const;
private:
    QVector<QPointF> getXmlPoints(QDomElement elem, uint num_points);
    mutable QPainterPath shape_path;
    mutable bool shape_outdated = false;
    QList<LinkNodeItem*> link_nodes;
    QPointF prev_pos;
};

class ChartScene : public QGraphicsScene {
    Q_OBJECT
public:
    using QGraphicsScene::QGraphicsScene;
    bool addBlockFile(QString filename = 0);
    QStringList blockTypes() const;
public slots:
    bool addBlock(const QString& block_name);
protected:
    virtual void drawBackground(QPainter* painter, const QRectF& rect);
private:
    QLinkedList<QDomDocument> block_documents;
};

class ElevProxyWidget : public QGraphicsProxyWidget {
    Q_OBJECT
public:
    using QGraphicsProxyWidget::QGraphicsProxyWidget;
protected:
    virtual void hoverEnterEvent( QGraphicsSceneHoverEvent* event);
    virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent* event);
private:
    QLinkedList<QPair<QGraphicsItem*, qreal>> old_z;
};

#endif // SCENE_CORE_H
