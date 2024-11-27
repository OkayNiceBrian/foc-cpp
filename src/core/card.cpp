#include "card.h"
#include "zone.h"
#include <string>

using namespace std;

void Card::takeDamage(int damage, Zone *zone) {
    this->currentHealth -= damage;
    if (this->currentHealth <= 0) {
        if (this->isPlayerCard) {
            zone->playerCards.remove(this);
        } else {
            zone->opponentCards.remove(this);
        }
        zone->cards.remove(this);
    }
}


void Card::draw() {
    switch(this->state) {

        case CardStates::hand: {
            Rectangle cardRect = this->cardRect;

            DrawRectangleRec(cardRect, GRAY);
            DrawText(to_string(this->currentCost).c_str(), cardRect.x + 5, cardRect.y + 5, 14, WHITE);
            DrawText(this->name.c_str(), cardRect.x + 30, cardRect.y + 5, 14, WHITE);
            DrawText(this->desc.c_str(), cardRect.x + 5, cardRect.y + cardRect.height / 2 + 20, 14, WHITE);

            switch (this->type)
            {
            case CardTypes::Sentient:
            {
                string pt = to_string(this->currentPower) + "/" + to_string(this->currentHealth);
                DrawText(pt.c_str(), cardRect.x + cardRect.width - 44, cardRect.y + cardRect.height - 15, 14, WHITE);
                break;
            }
            default:
                break;
            }

            break;
        }
        case CardStates::zone: {
            if (this->isPlayerCard)
            {
                DrawRectangleRec(this->cardRect, Color{0, 121, 241, 180});
            }
            else
            {
                DrawRectangleRec(this->cardRect, Color{230, 41, 55, 180});
            }
            DrawText(this->name.c_str(), this->cardRect.x + 5, this->cardRect.y + 5, 12, WHITE);
            break;
        }
    }
}