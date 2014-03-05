#include <QApplication>
#include <QtXml>
#include "scene_core.h"
#include "chartscene.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    
    ChartScene* m_scene = new ChartScene(-600, -300, 1200, 600);
    QGraphicsView* m_view = new QGraphicsView(m_scene);
    m_view->setRenderHint(QPainter::Antialiasing);
    m_scene->setBackgroundBrush(Qt::lightGray);

    QStringList arguments = app.arguments();
    if (arguments.size() > 1) {
        for (int i = 1; i!= arguments.size(); ++i) {
            m_scene->addBlockFile(arguments[i]);
        }
    } else m_scene->addBlockFile();
    
    QToolBar* m_toolbar = new QToolBar;
    QComboBox* m_combobox = new QComboBox();
    m_combobox->addItems(m_scene->blockTypes());
    QObject::connect(m_combobox, SIGNAL(activated(const QString &)), m_scene, SLOT(addBlockItem(const QString&)));
    m_toolbar->addWidget(m_combobox);
    
    QLayout* m_root_layout = new QVBoxLayout;
    m_root_layout->setMargin(2);
    m_root_layout->addWidget(m_toolbar);
    m_root_layout->addWidget(m_view);
    QWidget* m_root = new QWidget;
    m_root->setLayout(m_root_layout);
    m_root->show();
    
    return app.exec();
}