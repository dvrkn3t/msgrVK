#include "vkapi.h"
#include <QMessageBox>

VKAPI::VKAPI(QObject *parent) : QObject(parent), networkManager(std::make_shared<QNetworkAccessManager>(this)) {
    qDebug() << "[VK API] Initializing";
    QLoggingCategory::setFilterRules("qt.network.ssl.warning=false");
    Users.loadMe(_me);
}

VKAPI &VKAPI::Instance() {
    static VKAPI api;
    return api;
}

VKSmartData<std::pair<vktype::user, vktype::message>> VKAPI::getDialogs() {
    qDebug() << "[VK API] Dialogs request sent";
    VKSmartData<std::pair<vktype::user, vktype::message>> dialogsData;
    Messages.getDialogs();
    connect(&Messages, &MessageManager::dialogsReady, this, [this, dialogsData](){
        auto messages = Messages.getMessagesCache();
        std::vector<int> usersToLoad;
        for(const auto& msg : messages) usersToLoad.push_back(msg.uid);
        Users.get(usersToLoad);
        auto uidToNumberMap = std::make_shared<std::map<int, int>>();
        connect(&Users, &UserManager::usersQueryReady, this, [this, messages, dialogsData, uidToNumberMap]() mutable {
            int number = 0;
            for(const auto& msg : messages) {
                auto user = Users.findUser(msg.uid);
                _cachedDialogUsers.push_back(user);
                auto pair = std::make_pair(user, msg);
                dialogsData.push_back(pair);
                uidToNumberMap.get()->insert(std::make_pair(msg.uid, number++));
            }
            dialogsData.dataReady();
            disconnect(&Users, &UserManager::usersQueryReady, this, 0);
        });
        connect(&Users, &UserManager::userPhotoReady, this, [this, dialogsData, uidToNumberMap](auto uid) mutable {
            auto finder = uidToNumberMap.get()->find(uid);
            if(finder == uidToNumberMap.get()->end()) {
                //qDebug() << "Some error on photo load";
                disconnect(&Users, &UserManager::usersQueryReady, this, 0);
                return;
            }

            auto position = finder->second;
            auto user = Users.findUser(uid);
            dialogsData.changeFirst(user, position);
            _cachedDialogUsers.at(position) = user;
            dialogsData.photoReady(position);
            disconnect(&Users, &UserManager::usersQueryReady, this, 0);
        });
        disconnect(&Messages, &MessageManager::dialogsReady, this, 0);
    });
    return dialogsData;
}

VKSmartData<vktype::message> VKAPI::getDialog(int uid) {
    qDebug() << "[VK API] Dialog request send";
    VKSmartData<vktype::message> smartData;
    Messages.get(uid);
    connect(&Messages, &MessageManager::messagesReady, this, [this, smartData, uid]() mutable {
        auto msgs = Messages.getMessagesCache();
        for(const auto& msg : msgs) smartData.push_back(msg);
        smartData.dataReady();
        disconnect(&Messages, &MessageManager::messagesReady, this, 0);
    });
    return smartData;
}

void VKAPI::sendMessage(const vktype::user &user, QString text) {
    Messages.send(user.uid, text);
}

void VKAPI::refreshMessageManager() {
    Messages.resetLoadedPages();
}

vktype::user VKAPI::me() const {
    return _me;
}

vktype::user VKAPI::cachedDialogUser(int position) const {
    return _cachedDialogUsers.at(position);
}
