#ifndef ABSTRACTMANAGER_H
#define ABSTRACTMANAGER_H

#include <QObject>
#include <QtNetwork>
#include <QPixmap>
#include <memory>
#include <vector>
#include <map>
#include <vktypes.h>
#include <vkdata.hpp>

class Manager : public QObject {
    Q_OBJECT
protected:
    QString _token;
    QNetworkReply* makeHTTPRequest(QString url) const;
public:
    Manager(std::shared_ptr<QNetworkAccessManager> ptr);
    std::shared_ptr<QNetworkAccessManager> networkManager;

    /*void loadImage(QString link) {
        auto imageReply = makeHTTPRequest(link);
        connect(imageReply, &QNetworkReply::finished, this, [this, imageReply]() mutable {
            imageReply->deleteLater();
            auto data = imageReply->readAll();
            QPixmap photo;
            photo.loadFromData(data);
            disconnect(imageReply, &QNetworkReply::finished, this, 0);
        });
    }*/
};

#endif // ABSTRACTMANAGER_H
