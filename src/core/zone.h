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

    void addCard(Card *card) {
        this->cards.emplace_back(card);
        int cardSize = 100;
        int gap = 10;
        int lastIndex = 0;
        int yOffset = 0;
        if (card->playerCard) {
            this->playerCards.emplace_back(card);
            lastIndex = this->playerCards.size() - 1;
            yOffset = cardSize * 1.5;
        } else {
            this->opponentCards.emplace_back(card);
            lastIndex = this->opponentCards.size() - 1;
        }

        card->pos_lock = Vector2(rect.x + lastIndex * (cardSize + gap) + zonePadding, rect.y + gap + zonePadding + yOffset);
        card->cardRect.width = card->cardRect.height = cardSize;
        card->cardRect.x = card->pos_lock.x;
        card->cardRect.y = card->pos_lock.y;
    }

    void setRect(int screenWidth, int screenHeight) {
        switch (this->zoneNum) {
            case Zones::Zone1:
                this->rect = Rectangle(screenWidth / 2 + zoneMargin / 2, screenHeight / 2 + zoneMargin / 2, screenWidth / 2.5 - zoneMargin, screenHeight / 2 - zoneMargin);
                this->color = this->lock_color = ColorAlpha(Color(100, 0, 250), .2);
                break;
            case Zones::Zone2:
                this->rect = Rectangle(screenWidth / 10 + zoneMargin / 2, screenHeight / 2 + zoneMargin / 2, screenWidth / 2.5 - zoneMargin, screenHeight / 2 - zoneMargin);
                this->color = this->lock_color = ColorAlpha(Color(100, 0, 250), .2);
                break;
            case Zones::Zone3:
                this->rect = Rectangle(screenWidth / 10 + zoneMargin / 2, zoneMargin / 2, screenWidth / 2.5 - zoneMargin, screenHeight / 2 - zoneMargin);
                this->color = this->lock_color = ColorAlpha(Color(250, 0, 100), .2);
                break;
            case Zones::Zone4:
                this->rect = Rectangle(screenWidth / 2 + zoneMargin / 2, zoneMargin / 2, screenWidth / 2.5 - zoneMargin, screenHeight / 2 - zoneMargin);
                this->color = this->lock_color = ColorAlpha(Color(250, 0, 100), .2);
                break;
        }
    }
};

#endif