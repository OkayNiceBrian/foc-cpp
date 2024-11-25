#include "zone.h"
#include "card.h"

void Zone::addCard(Card *card) {
    this->cards.emplace_back(card);
    int cardSize = 100;
    int gap = 10;
    int lastIndex = 0;
    int yOffset = 0;
    if (card->isPlayerCard) {
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

void Zone::setAttackersAndDefenders() {
    this->defenders.clear();
    this->attackers.clear();
    if (this->isPlayerZone) {
        for (Card *card : this->playerCards) {
            if (card->type == CardTypes::Sentient) this->defenders.push_back(card);
        }
        for (Card *card : opponentCards) {
            if (card->type == CardTypes::Sentient) this->attackers.push_back(card);
        }
    } else {
        for (Card *card : playerCards) {
            if (card->type == CardTypes::Sentient) this->attackers.push_back(card);
        }
        for (Card *card : opponentCards) {
            if (card->type == CardTypes::Sentient) this->defenders.push_back(card);
        }
    }
}

void Zone::setRect(int screenWidth, int screenHeight) {
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