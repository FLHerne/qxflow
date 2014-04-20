#include <QApplication>
#include <QStringList>

#include "chartwindow.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    QStringList arguments = app.arguments().mid(1);
    arguments.removeDuplicates();
    ChartWindow m_window(arguments);
    m_window.show();
    return app.exec();
}
