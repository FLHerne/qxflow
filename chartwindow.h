#ifndef CHARTWINDOW_H
#define CHARTWINDOW_H

#include <QComboBox>
#include <QGraphicsView>
#include <QMainWindow>
#include <QStringList>
#include <QToolBar>

class ChartWindow : public QMainWindow {
    Q_OBJECT
public:
    ChartWindow(QStringList in_block_files);
public slots:
    void newScene();
private:
    QToolBar* main_toolbar;
    QGraphicsView* chart_view{new QGraphicsView};
    QComboBox* block_combobox{new QComboBox};
    QStringList block_files;
};

#endif //CHARTWINDOW_H
