#ifndef ZONE_H
#define ZONE_H

#include "raylib.h"
#include <list>

// Clockwise starting at bottom-right
// Zone1 == bottom-right, Zone2 == bottom-left, Zone3 == top-left, Zone4 == top-right
enum Zones {
    Zone1 = 1,
    Zone2,
    Zone3,
    Zone4
};

struct Zone {
    static const int zoneMargin = 10;
    static const int zonePadding = 50;

    bool isPlayerZone;
    Zones zoneNum = Zones::Zone1;
    Rectangle rect = Rectangle(0, 0, 0, 0);
    Color color = Color(0, 0, 0);
    Color lock_color = Color(0, 0, 0);
    std::list<Card*> cards = {};
    std::list<Card*> playerCards = {};
    std::list<Card*> opponentCards = {};
    std::list<Card*> attackers = {};
    std::list<Card*> defenders = {};

    void addCard(Card *card);
    void setAttackersAndDefenders();
    void setRect(int screenWidth, int screenHeight);
};

#endif