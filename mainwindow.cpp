#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), vk(VKAPI::Instance()), _loadSpinnerLabel(std::make_unique<QLabel>()) {
    ui->setupUi(this);
    prepareWidgets();
    prepareLoadSpinner();
    showDialogs();
}

MainWindow::~MainWindow() {}

void MainWindow::prepareWidgets() {
    ui->leftWidget->setIconSize(QSize(50, 50));
    ui->rightWidget->setIconSize(QSize(20, 20));
    ui->messageLineEdit->setEnabled(false);
    //Show dialog on click
    connect(ui->leftWidget, &QListWidget::clicked, this, [this](auto index) {
        showDialogWithUser(vk.cachedDialogUser(index.row()));
        ui->messageLineEdit->setEnabled(true);
    });
    //----------------------------------------------------------------------------------------------------------------------
    //Scroll bar to right position
    connect(ui->rightWidget->verticalScrollBar(), &QScrollBar::rangeChanged, this, [this](auto min, auto max) {
        (void)min;
        ui->rightWidget->verticalScrollBar()->setValue(max - _scrollBarOldRange + ui->rightWidget->verticalScrollBar()->value());
        _scrollBarOldRange = max;
        _isMessagesLock = false;
    });
    //Update dialog at scroll up
    connect(ui->rightWidget->verticalScrollBar(), &QScrollBar::valueChanged, this, [this](auto value) {
        if (value < 200 && !_isMessagesLock && ui->rightWidget->verticalScrollBar()->value() != 0) {
            _isMessagesLock = true;
            requestForNewMessages(vk.cachedDialogUser(ui->leftWidget->currentRow()));
        }
    });
    //----------------------------------------------------------------------------------------------------------------------
    connect(ui->leftWidget->verticalScrollBar(), &QScrollBar::rangeChanged, this, [this](auto min, auto max) {
        (void)min;
        (void)max;
        _isDialogsLock = false;
    });
    connect(ui->leftWidget->verticalScrollBar(), &QScrollBar::valueChanged, this, [this](auto value) {
        if( value > ui->leftWidget->verticalScrollBar()->maximum() - 200 && !_isDialogsLock) {
            _isDialogsLock = true;
            showDialogs();
        }
    });
    //----------------------------------------------------------------------------------------------------------------------
    connect(ui->sendMessageButton, &QPushButton::pressed, this, [this]() {
        vk.sendMessage(vk.cachedDialogUser(ui->leftWidget->currentRow()), ui->messageLineEdit->text());
        ui->messageLineEdit->clear();
        showDialogs();
    });
}

void MainWindow::showDialogs() {
    showLoadSpinner(ui->leftWidget);
    //_leftListItems.clear();
    auto dialogs = vk.getDialogs();
    connect(dialogs.emitter(), &VKDataEmitter::finished, this, [this, dialogs]() mutable {
        hideLoadSpinner();
        auto data = dialogs.data();
        for(const auto& dialog : data) {
            auto messageOwner = dialog.second.out ? "Вы: " : "";
            auto item = std::make_unique<QListWidgetItem>(dialog.first.first_name + " " + dialog.first.last_name + "\n" + messageOwner + dialog.second.body, ui->leftWidget);
            if(!dialog.second.read_state) {
                auto font = item->font();
                font.setBold(true);
                item->setFont(font);
            }
            _leftListItems.push_back(std::move(item));
        }
        connect(dialogs.emitter(), &VKDataEmitter::photoFinished, this, [this, dialogs](auto position) {
            auto dialog = dialogs.data().at(position);
            QIcon icon;
            icon.addPixmap(dialog.first.photo, QIcon::Normal);
            icon.addPixmap(dialog.first.photo, QIcon::Selected);
            _leftListItems.at(position)->setIcon(icon);
        });
    });
}

void MainWindow::showDialogWithUser(const vktype::user& user) {
    ui->rightWidget->clear();
    showLoadSpinner(ui->rightWidget);
    vk.refreshMessageManager();
    requestForNewMessages(user);
}

void MainWindow::requestForNewMessages(const vktype::user& user) {
    auto dialog = vk.getDialog(user.uid);
    connect(dialog.emitter(), &VKDataEmitter::finished, this, [dialog, this, user]() {
        hideLoadSpinner();
        auto data = dialog.data();
        for(const auto& message : data) {
            //WARNING: Костыльно
            auto messageOwner = message.out ? vk.me().first_name : user.first_name;
            messageOwner += ": ";
            auto attachmentString = message.attachments ? " [attachment]" : "";
            auto item = new QListWidgetItem(messageOwner + message.body + attachmentString);
            QIcon icon;
            if(!message.out) {
                icon.addPixmap(user.photo, QIcon::Normal);
                icon.addPixmap(user.photo, QIcon::Selected);
                item->setIcon(icon);
            } else {
                icon.addPixmap(vk.me().photo, QIcon::Normal);
                icon.addPixmap(vk.me().photo, QIcon::Selected);
                item->setIcon(icon);
            }
            if(!message.read_state) {
                auto font = item->font();
                font.setBold(true);
                item->setFont(font);
            }
            ui->rightWidget->insertItem(0, item);
        }
    });
}

void MainWindow::prepareLoadSpinner() {
    _loadSpinnerMovie = std::make_unique<QMovie>(":/data/circle-loader.gif");
    _loadSpinnerMovie.get()->setScaledSize(QSize(20,20));
    _loadSpinnerLabel->setMovie(_loadSpinnerMovie.get());
    _loadSpinnerLabel->setAlignment(Qt::AlignCenter);
    _loadSpinnerMovie.get()->start();
}

void MainWindow::showLoadSpinner(QWidget *widget) {
    delete widget->layout();
    auto layout = new QBoxLayout(QBoxLayout::LeftToRight, 0);
    layout->addWidget(_loadSpinnerLabel.get());
    widget->setLayout(layout);
}

void MainWindow::hideLoadSpinner() {
    _loadSpinnerLabel->setParent(nullptr);
}
