#include "usermanager.h"
#include <QBitmap>
#include <QPainter>

auto makeRoundedPhoto(QPixmap photo) {
    QBitmap map(100,100);
    map.fill(Qt::color0);
    QPainter painter(&map);
    painter.setBrush(Qt::color1);
    painter.drawRoundedRect(0, 0, 100, 100, 50, 50);
    photo.setMask(map);
    return photo;
}

vktype::user UserManager::userFromJson(const QJsonObject &current) const {
    int uid = current["uid"].toInt();
    QString first_name = current["first_name"].toString();
    QString last_name = current["last_name"].toString();
    QString photo_100 = current["photo_100"].toString();
    return {uid, first_name, last_name, photo_100};
}

void UserManager::loadImage(vktype::user &user) {
    auto imageReply = makeHTTPRequest(user.photo_100);
    connect(imageReply, &QNetworkReply::finished, this, [this, imageReply, user]() mutable {
        imageReply->deleteLater();
        auto data = imageReply->readAll();
        QPixmap photo;
        photo.loadFromData(data);
        user.photo = makeRoundedPhoto(photo);
        disconnect(imageReply, &QNetworkReply::finished, this, 0);
    });
}

void UserManager::addToCache(const vktype::user &user) {
    if(_usersCache.find(user.uid) == _usersCache.end()) _usersCache.insert(std::make_pair(user.uid, user));
}

void UserManager::setPhoto(int uid, QPixmap photo) {
    auto user = _usersCache.find(uid);
    if(user != _usersCache.end()) {
        user->second.photo = photo;
        emit userPhotoReady(uid);
    }
}

inline void UserManager::get(int uid) {
    return get({uid});
}

vktype::user UserManager::findUser(int uid) {
    auto user = _usersCache.find(uid);
    if(user == _usersCache.end()) return vktype::user({0, "None", "None"});
    return user->second;
}

UserManager::UserManager(std::shared_ptr<QNetworkAccessManager> ptr) : Manager(ptr) {}

void UserManager::get(std::vector<int> uids) {
    auto str = QString("https://api.vk.com/method/users.get?fields=photo_100&uids=");
    for(const auto& uid : uids) str += QString::number(uid) + ',';
    auto reply = makeHTTPRequest(str);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() mutable {
        reply->deleteLater();
        auto response = reply->readAll();
        if(response.contains("error")) {
            disconnect(reply, &QNetworkReply::finished, this, 0);
            return;
        }
        auto responseArray = QJsonDocument::fromJson(response).object()["response"].toArray();
        for(const auto& value : responseArray) {
            auto user = userFromJson(value.toObject());
            addToCache(user);
            auto photoReply = makeHTTPRequest(user.photo_100);
            connect(photoReply, &QNetworkReply::finished, this, [this, photoReply, user]() mutable {
                photoReply->deleteLater();
                auto data = photoReply->readAll();
                QPixmap photo;
                photo.loadFromData(data);
                setPhoto(user.uid, makeRoundedPhoto(photo));
                disconnect(photoReply, &QNetworkReply::finished, this, 0);
            });
        }
        emit usersQueryReady();
        disconnect(reply, &QNetworkReply::finished, this, 0);
    });
}

void UserManager::loadMe(vktype::user& _me) const {
    auto reply = makeHTTPRequest("https://api.vk.com/method/users.get?fields=photo_100&access_token=" + _token);
    connect(reply, &QNetworkReply::finished, this, [this, reply, &_me]() {
        reply->deleteLater();
        auto response = reply->readAll();
        if(response.contains("error")) {
            disconnect(reply, &QNetworkReply::finished, this, 0);
            return;
        }
        auto responseArray = QJsonDocument::fromJson(response).object()["response"].toArray();
        auto user = userFromJson(responseArray[0].toObject());
        _me = user;
        auto imageReply = makeHTTPRequest(user.photo_100);
        connect(imageReply, &QNetworkReply::finished, this, [this, imageReply, &_me]() {
            auto data = imageReply->readAll();
            QPixmap photo;
            photo.loadFromData(data);
            _me.photo = makeRoundedPhoto(photo);
        });
    });
}

