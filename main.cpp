#include "xorApp.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    xorApp mainWindow;
    mainWindow.setFixedSize(400, 300);
    mainWindow.show();
    return app.exec();
}
