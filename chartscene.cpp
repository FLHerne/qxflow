#include <QDebug>
#include <QFileDialog>
#include "scene_core.h"
#include "chartscene.h"

//Public
bool ChartScene::addBlockFile(QString filename) {
    //If no filename provided, ask the user for one.
    if (!filename.length()) {
        filename = QFileDialog::getOpenFileName(NULL, "Open File", "", "XML Files (*.xml)");
    }
    QFile file(filename);
    QDomDocument doc;
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file " << filename;
        return false;
    }
    file.open(QIODevice::ReadOnly);
    QString err_msg;
    int err_line, err_column;
    if (!doc.setContent(&file, &err_msg, &err_line, &err_column)) {
        qWarning() << "Failed to load file filename:";
        qWarning() << err_msg << "at line" << err_line << "column" << err_column;
        file.close();
        return false;
    }
    block_documents.append(doc);
    file.close();
    return true;
}

//Public
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

//Public slot
bool ChartScene::addBlockItem(const QString& block_type) {
    QDomElement block_elem;
    foreach (QDomDocument doc, block_documents) {
        block_elem = doc.documentElement().firstChildElement("block");
        while (!block_elem.isNull()) {
            if (block_elem.attribute("name") == block_type) {
                addItem(new BlockItem(QPoint(0,0), block_elem));
                return true;
            }
            block_elem = block_elem.nextSiblingElement("block");
        }
    }
    //No block types with name 'block_type'.
    return false;
}

//Protected
void ChartScene::drawBackground(QPainter* painter, const QRectF& rect) {
    QGraphicsScene::drawBackground(painter, rect);
    for (int ix = rect.left() - (int) rect.left() % 20; ix <= rect.right(); ix += 20) {
        painter->drawLine(ix, rect.top(), ix, rect.bottom());
    }
    for (int iy = rect.top() - (int) rect.top() % 20; iy <= rect.bottom(); iy += 20) {
        painter->drawLine(rect.left(), iy, rect.right(), iy);
    }
}
