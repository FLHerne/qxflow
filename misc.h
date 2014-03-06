#ifndef MISC_H
#define MISC_H
#include <QObject>

//Round 'in_val' up or down to the nearest multiple of 'step'.
int roundTo(float in_val, int step);

//Qt bug workaround: deleting QGraphicsItem immediately causes the scene to crash,
//...but they don't have a deleteLater() function.
//Taken from http://permalink.gmane.org/gmane.comp.lib.qt.general/24194
template <typename T>
class DelayedDelete : public QObject {
public:
    explicit DelayedDelete(T*& item) : m_item(item) {
        item = 0;
        deleteLater();
    }
    virtual ~DelayedDelete() {
        delete m_item;
    }
private:
    T* m_item;
};

#endif //MISC_H