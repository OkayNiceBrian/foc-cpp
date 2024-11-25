#include "card.h"
#include "zone.h"

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
