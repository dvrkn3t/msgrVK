#ifndef VKAPI_H
#define VKAPI_H

#include <messagemanager.h>
#include <usermanager.h>

class VKAPI : public QObject {
    std::shared_ptr<QNetworkAccessManager> networkManager;
    MessageManager Messages {networkManager};
    UserManager Users {networkManager};
    std::vector<vktype::user> _cachedDialogUsers;
    vktype::user _me;

    explicit VKAPI(QObject *parent = 0);
    VKAPI(const VKAPI&) = delete;
    VKAPI& operator=(const VKAPI&) = delete;
public:
    static VKAPI& Instance();
    VKSmartData<std::pair<vktype::user, vktype::message>> getDialogs();
    VKSmartData<vktype::message> getDialog(int uid);
    void sendMessage(const vktype::user& user, QString text);
    void refreshMessageManager();
    vktype::user me() const;
    vktype::user cachedDialogUser(int position) const;
};

#endif // VKAPI_H
