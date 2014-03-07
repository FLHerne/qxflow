#ifndef CHARTSCENE_H
#define CHARTSCENE_H

#include <QDomDocument>
#include <QGraphicsScene>
#include <QGraphicsSceneWheelEvent>
#include <QLinkedList>

//Specialised QGraphicsScene, with background grid,
//...and BlockItem management functions.
class ChartScene : public QGraphicsScene {
    Q_OBJECT
public:
    using QGraphicsScene::QGraphicsScene;
    
    //Adds 'filename' to be searched for BlockItem types.
    //Displays a dialog if no filename provided.
    bool addBlockFile(QString filename = 0);
    
    //List of BlockItem types known by this scene.
    QStringList blockTypes() const;
    
    //Spacing between grid lines, in pixels.
    int gridSize() const { return grid_size; }
    void setGridSize(int new_size) { grid_size = new_size; }
    
public slots:
    //Adds a BlockItem of type 'block_type' to the scene at (0, 0).
    //Returns 1 on success or 0 otherwise.
    bool addBlockItem(const QString& block_type);
    
protected:
    //Paints a grid on the background, spacing from 'grid_size'.
    virtual void drawBackground(QPainter* painter, const QRectF& rect);
    
    //Sends wheel events to the mouse grabber if one exists.
    virtual void wheelEvent(QGraphicsSceneWheelEvent* event);
    
private:
    //Spacing of background grid.
    int grid_size = 20;
    
    //List of XML documents containing BlockItem types.
    QLinkedList<QDomDocument> block_documents;
};

#endif //CHARTSCENE_H
