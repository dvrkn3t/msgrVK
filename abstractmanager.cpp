#include "abstractmanager.h"

QNetworkReply* Manager::makeHTTPRequest(QString url) const {
    auto request = QNetworkRequest(QUrl(url));
    return networkManager->get(request);
}

Manager::Manager(std::shared_ptr<QNetworkAccessManager> ptr) : networkManager(ptr) {
    QFile file(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/msgrVK/token.txt");
    file.open(QIODevice::ReadOnly);
    QString tokenCrypted = file.readLine();
    file.close();
    auto latinString = tokenCrypted.toLatin1();
    auto temp = latinString.data();
    for(int i = 0; i < latinString.size(); i++) _token += temp[i] ^ 1;
}
