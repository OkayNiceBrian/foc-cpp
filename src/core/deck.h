#ifndef DECK_H
#define DECK_H

#include <string>
#include "card.h"

struct DeckInfo {
    std::string name;
    Card *cards[20];
    const int size = 20;
};

#endif