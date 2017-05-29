#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <abstractmanager.h>

class UserManager : public Manager {
    Q_OBJECT
    std::map<int, vktype::user> _usersCache;

    vktype::user userFromJson(const QJsonObject &current) const;
    void loadImage(vktype::user& user);
    void addToCache(const vktype::user& user);
    void setPhoto(int uid, QPixmap photo);
public:
    UserManager(std::shared_ptr<QNetworkAccessManager> ptr);
    inline void get(int uid);
    void get(std::vector<int> uids);
    void loadMe(vktype::user& _me) const;
    vktype::user findUser(int uid);
signals:
    void usersQueryReady();
    void userPhotoReady(int uid);
};

#endif // USERMANAGER_H
