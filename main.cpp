#include <mainwindow.h>
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication vkMessenger(argc, argv);
    MainWindow window;
    window.show();
    return vkMessenger.exec();
}
