#ifndef VKDATA_H
#define VKDATA_H

#include <QObject>
#include <memory>

class VKDataEmitter : public QObject {
    Q_OBJECT
signals:
    void finished();
    void photoFinished(int);
};

template<class T>
class VKSmartData {
    std::shared_ptr<VKDataEmitter> _emitter;
    std::shared_ptr<std::vector<T>> _data;
public:
    VKSmartData();
    VKSmartData(const VKSmartData& another);
    void push_back(T elem);
    void dataReady() const;
    void photoReady(int pos) const;
    auto getPhotosToProcess();
    auto data() const;
    auto emitter() const;
    template<class TFirst>
    void changeFirst(TFirst elem, int index) {
        _data.get()->at(index).first = elem;
    }
};

template<class T>
VKSmartData<T>::VKSmartData() : _emitter(std::make_shared<VKDataEmitter>()), _data(std::make_shared<std::vector<T>>()) {}

template<class T>
VKSmartData<T>::VKSmartData(const VKSmartData& another) : _emitter(another._emitter), _data(another._data) {}

template<class T>
inline void VKSmartData<T>::push_back(T elem) {
    _data.get()->push_back(elem);
}

template<class T>
inline void VKSmartData<T>::dataReady() const {
    emit _emitter.get()->finished();
}

template<class T>
inline auto VKSmartData<T>::data() const {
    return *_data.get();
}

template<class T>
inline auto VKSmartData<T>::emitter() const {
    return _emitter.get();
}

template<class T>
inline void VKSmartData<T>::photoReady(int pos) const {
    emit _emitter.get()->photoFinished(pos);
}

#endif // VKDATA_H
