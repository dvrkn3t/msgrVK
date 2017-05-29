#include "messagemanager.h"

MessageManager::MessageManager(std::shared_ptr<QNetworkAccessManager> ptr) : Manager(ptr) {}

vktype::message MessageManager::messageFromJson(const QJsonObject &current) {
    QString body = current["body"].toString();
    int uid = current["uid"].toInt();
    bool out = static_cast<bool>(current["out"].toInt());
    int date = current["date"].toInt();
    bool read_state = static_cast<bool>(current["read_state"].toInt());
    bool attachments = !current["attachment"].toObject().isEmpty();
    return {uid, body, out, date, attachments, read_state};
}

void MessageManager::getDialogs() {
    _messagesCache.clear();
    qDebug() << "-> [Message Manager] Dialogs request";
    auto offset = _loadedDialogPages * offsetSize;
    auto reply = makeHTTPRequest("https://api.vk.com/method/messages.getDialogs?count=40&access_token=" + _token + "&offset=" + QString::number(offset));
    connect(reply, &QNetworkReply::finished, this, [this, reply]() mutable {
        reply->deleteLater();
        auto responseArray = QJsonDocument::fromJson(reply->readAll()).object()["response"].toArray();
        responseArray.removeFirst();
        for(const auto& value : responseArray) {
            auto messageObject = messageFromJson(value.toObject());
            _messagesCache.push_back(messageObject);
        }
        std::sort(_messagesCache.begin(), _messagesCache.end(), [](const auto& a, const auto& b){return a.date > b.date;});
        emit dialogsReady();
        qDebug() << "<- [Message Manager] Got dialogs";
        _loadedDialogPages++;
        disconnect(reply, &QNetworkReply::finished, this, 0);
    });
}

void MessageManager::get(int uid) {
    _messagesCache.clear();
    qDebug() << "-> [Message Manager] Dialog request";
    auto offset = _loadedPagesInDialog * offsetSize;
    auto reply = makeHTTPRequest("https://api.vk.com/method/messages.getHistory?count=" + QString::number(count) + "&uid=" + QString::number(uid) + "&access_token=" + _token + "&offset=" + QString::number(offset));
    connect(reply, &QNetworkReply::finished, this, [this, reply]() mutable {
        reply->deleteLater();
        auto responseArray = QJsonDocument::fromJson(reply->readAll()).object()["response"].toArray();
        responseArray.removeFirst();
        for(const auto& value : responseArray) {
            QJsonObject current = value.toObject();
            QString body = current["body"].toString();
            int uid = current["uid"].toInt();
            bool out = static_cast<bool>(current["out"].toInt());
            int date = current["date"].toInt();
            bool read_state = static_cast<bool>(current["read_state"].toInt());
            bool attachments = !current["attachment"].toObject().isEmpty();
            if(attachments) {
                auto attachment = current["attachment"].toObject();
                if(attachment["type"] == "photo") {
                    auto photo = attachment["photo"].toObject();
                    auto photoLink = photo["src_big"].toString();
                    auto imageReply = makeHTTPRequest(photoLink);
                    connect(imageReply, &QNetworkReply::finished, this, [this, imageReply](){
                        imageReply->deleteLater();
                        auto data = imageReply->readAll();
                        QPixmap photo;
                        photo.loadFromData(data);
                        //Load photo;

                        //------------------------------
                        disconnect(imageReply, &QNetworkReply::finished, this, 0);
                    });
                }
            }
            _messagesCache.push_back( {uid, body, out, date, attachments, read_state} );
        }
        std::sort(_messagesCache.begin(), _messagesCache.end(), [](const auto& a, const auto& b){return a.date > b.date;});
        emit messagesReady();
        qDebug() << "<- [Message Manager] Got dialog";
        _loadedPagesInDialog++;
        disconnect(reply, &QNetworkReply::finished, this, 0);
    });
}

void MessageManager::send(int uid, QString text) const {
     makeHTTPRequest("https://api.vk.com/method/messages.send?user_id=" + QString::number(uid) + "&access_token=" + _token + "&message=" + text);
}

std::vector<vktype::message> MessageManager::getMessagesCache() const {
    return _messagesCache;
}

void MessageManager::resetLoadedPages() {
    _loadedPagesInDialog = 0;
}




