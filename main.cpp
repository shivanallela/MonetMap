#include <QApplication>
#include <QFile>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("MoneyMap");
    app.setApplicationDisplayName("MoneyMap – Personal Finance Tracker");
    app.setOrganizationName("MoneyMap");

    // Load global stylesheet from resources
    QFile styleFile(":/style.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text))
        app.setStyleSheet(QString::fromUtf8(styleFile.readAll()));

    MainWindow window;
    window.show();
    return app.exec();
}
