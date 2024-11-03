#ifndef CARD_H
#define CARD_H

#include <string>

enum CardTypes {
    Sentient,
    Nova
};

struct Card {
    std::string name;
    std::string desc;
    int cost;
    CardTypes type;
    const int base_power = 20;
    const int base_health = 20;
    int power;
    int health;
};

#endif