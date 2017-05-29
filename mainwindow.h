#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollBar>
#include <QListWidgetItem>
#include <QLabel>
#include <QMovie>
#include <vkapi.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
    std::unique_ptr<Ui::MainWindow> ui;
    VKAPI& vk;

    std::unique_ptr<QLabel> _loadSpinnerLabel;
    std::unique_ptr<QMovie> _loadSpinnerMovie;
    std::vector<std::unique_ptr<QListWidgetItem>> _leftListItems;
    int _scrollBarOldRange = 0;
    bool _isMessagesLock = false;
    bool _isDialogsLock = false;

    void prepareWidgets();
    void showDialogs();
    void showDialogWithUser(const vktype::user& user);
    void requestForNewMessages(const vktype::user& user);

    void prepareLoadSpinner();
    void showLoadSpinner(QWidget* widget);
    void hideLoadSpinner();
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
};

#endif // MAINWINDOW_H
