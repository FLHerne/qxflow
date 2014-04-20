#include <QPushButton>

#include "chartscene.h"
#include "chartwindow.h"

ChartWindow::ChartWindow(QStringList in_block_files):
    QMainWindow() {
    block_files = in_block_files;
    main_toolbar = addToolBar("Main Toolbar");
    main_toolbar->addWidget(block_combobox);
    QPushButton* reload_button{new QPushButton{"Reload"}};
    QObject::connect(reload_button, SIGNAL(pressed()), this, SLOT(newScene()));
    main_toolbar->addWidget(reload_button);
    chart_view->setRenderHint(QPainter::Antialiasing);
    setCentralWidget(chart_view);
    newScene();
}

void ChartWindow::newScene() {
    ChartScene* m_scene = new ChartScene(-600, -300, 1200, 600);
    m_scene->setBackgroundBrush(Qt::lightGray);
    if (block_files.size()) {
        for (int i = 0; i!= block_files.size(); ++i) {
            m_scene->addBlockFile(block_files[i]);
        }
    } else m_scene->addBlockFile();
    QGraphicsScene* old_scene = chart_view->scene();
    chart_view->setScene(m_scene);
    if (old_scene) old_scene->deleteLater();
    while (block_combobox->count()) {
        block_combobox->removeItem(0);
    }
    block_combobox->addItems(m_scene->blockTypes());
    QObject::connect(block_combobox, SIGNAL(activated(const QString &)), m_scene, SLOT(addBlockItem(const QString&)));
}
