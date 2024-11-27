#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
#include "../core/card.h"

enum AnimationType {
    Attack,
    Death
};

struct Animation {
    AnimationType type;
    std::vector<Card*> *cards;
    int timer = 0;
    std::vector<int> keyFrames;
    bool hasEnded = false;

    void update();
};

#endif