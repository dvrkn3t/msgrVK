#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H

#include <abstractmanager.h>

class MessageManager : public Manager {
    Q_OBJECT
    const int count = 40;
    const int offsetSize = 40;
    int _loadedDialogPages = 0;
    int _loadedPagesInDialog = 0;

    std::vector<vktype::message> _messagesCache;
    vktype::message messageFromJson(const QJsonObject& current);
public:
    MessageManager(std::shared_ptr<QNetworkAccessManager> ptr);
    void getDialogs();
    void get(int uid);
    void send(int uid, QString text) const;
    std::vector<vktype::message> getMessagesCache() const;
    void resetLoadedPages();
signals:
    void messagesReady();
    void dialogsReady();
};

#endif // MESSAGEMANAGER_H
