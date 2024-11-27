#include "animation.h"

void Animation::update() {
    switch(this->type) {

        case AnimationType::Attack: {
            if (this->timer == 0) {
                this->keyFrames.push_back(20); // Windup
                this->keyFrames.push_back(30); // Attack
                this->keyFrames.push_back(32);
                this->keyFrames.push_back(45); // recoil
            }

            Card *attackingCard = this->cards->at(0);
            Card *defendingCard = this->cards->at(1);

            if (this->timer < this->keyFrames[0]) {
                attackingCard->cardRect.y += 2;
            } else if (this->timer < this->keyFrames[1]) {
                int frames = this->keyFrames[1] - this->keyFrames[0];
                int yDifference = attackingCard->cardRect.y - defendingCard->cardRect.y;
                int xDifference = attackingCard->cardRect.x - defendingCard->cardRect.x;
                attackingCard->cardRect.y -= yDifference / frames;
                attackingCard->cardRect.x -= xDifference / frames;
            } else if (this->timer < this->keyFrames[2]) {
                defendingCard->cardRect.y -= 20;
            } else if (this->timer < this->keyFrames[3]) {
                attackingCard->cardRect.y += 5;
            } else {
                attackingCard->cardRect.x = attackingCard->pos_lock.x;
                attackingCard->cardRect.y = attackingCard->pos_lock.y;
                defendingCard->cardRect.x = defendingCard->pos_lock.x;
                defendingCard->cardRect.y = defendingCard->pos_lock.y;
                this->hasEnded = true;
            }

            this->timer++;
            break;
        }
        case AnimationType::Death: {
            break;
        }
    }
}