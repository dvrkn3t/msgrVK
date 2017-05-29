#ifndef VKTYPES_H
#define VKTYPES_H

#include <QPixmap>

namespace vktype {

struct user {
    int uid;
    QString first_name;
    QString last_name;
    QString photo_100;
    QPixmap photo;
};

struct message {
    int uid;
    QString body;
    bool out;
    int date;
    bool attachments;
    bool read_state;
    std::vector<QPixmap> attachedPhotos;
};

}

#endif // VKTYPES_H
