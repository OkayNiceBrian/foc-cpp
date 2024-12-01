#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
#include "../core/card.h"

enum AnimationType {
    Attack,
    AttackPlayer,
    Death,
    Rotation
};

struct Animation {
    AnimationType type;
    std::vector<Card*> *cards = new std::vector<Card*>{};
    std::vector<Rectangle> *rects = new std::vector<Rectangle>{};
    std::vector<Vector2> *points = new std::vector<Vector2>{};
    int timer = 0;
    std::vector<int> keyFrames;
    bool hasEnded = false;

    void update();
    void draw();
};

#endif