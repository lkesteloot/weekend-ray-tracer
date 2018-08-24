#ifndef HITABLE_LIST_H
#define HITABLE_LIST_H

#include "Hitable.h"

class HitableList : public Hitable {
public:
    Hitable **mList;
    int mSize;

    HitableList() {
        // Nothing.
    }
    // Keeps pointer to list.
    HitableList(Hitable **list, int size)
        : mList(list), mSize(size) {

        // Nothing.
    }
    virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const {
        bool hit_anything = false;
        float closest = t_max;

        for (int i = 0; i < mSize; i++) {
            if (mList[i]->hit(r, t_min, closest, rec)) {
                hit_anything = true;
                closest = rec.t;
            }
        }

        return hit_anything;
    }
};


#endif // HITABLE_LIST_H
