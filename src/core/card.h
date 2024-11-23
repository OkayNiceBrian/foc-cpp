#ifndef CARD_H
#define CARD_H

#include <string>
#include "raylib.h"

enum CardTypes {
    Sentient,
    Nova
};

enum CardStates {
    hand,
    discard,
    zone1,
    zone2,
    zone3,
    zone4,
    none
};

struct Card {
    std::string name;
    std::string desc;
    int cost;
    CardTypes type;
    int power;
    int health;
    bool playerCard;

    CardStates state = CardStates::none;

    Rectangle cardRect = {0, 0, 150, 250};
    Vector2 pos_lock = {0, 0};
};

#endif