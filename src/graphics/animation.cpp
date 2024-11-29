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
            break;
        }
        case AnimationType::AttackPlayer: {
            if (this->timer == 0) {
                this->keyFrames.push_back(20); // Windup
                this->keyFrames.push_back(30); // Attack
                this->keyFrames.push_back(45); // recoil
            }

            Card *attacker = this->cards->at(0);
            Rectangle *life = this->rects->at(0);

            if (this->timer < this->keyFrames[0]) {
                attacker->cardRect.y += 2;
            } else if (this->timer < this->keyFrames[1]) {
                int frames = this->keyFrames[1] - this->keyFrames[0];
                int yDifference = attacker->cardRect.y - life->y;
                int xDifference = attacker->cardRect.x - life->x;
                attacker->cardRect.y -= yDifference / frames;
                attacker->cardRect.x -= xDifference / frames;
            } else if (this->timer < this->keyFrames[3]) {
                attacker->cardRect.y += 5;
            } else {
                attacker->cardRect.x = attacker->pos_lock.x;
                attacker->cardRect.y = attacker->pos_lock.y;
                this->hasEnded = true;
            }
            break;
        }
        case AnimationType::Death: {
            if (this->timer == 0) {
                this->keyFrames.push_back(30);
                this->keyFrames.push_back(40);
                this->keyFrames.push_back(55);
                this->points.push_back(new Vector2{this->cards->at(0)->cardRect.x + this->cards->at(0)->cardRect.width / 2, this->cards->at(0)->cardRect.y + this->cards->at(0)->cardRect.height / 2});
            }

            Card* card = this->cards->at(0);

            if (this->timer < this->keyFrames[0]) {
                int x = GetRandomValue(-3, 3);
                int y = GetRandomValue(-3, 3);
                card->cardRect.x += x;
                card->cardRect.y += y;
            } else if (this->timer < this->keyFrames[1]) {
                card->cardRect.height += 2;
                card->cardRect.width += 2;
                card->cardRect.x -= 1;
                card->cardRect.y -= 1;
            } else if (this->timer < this->keyFrames[2]) {
                card->cardRect.height = 0;
            } else {
                this->hasEnded = true;
            }
            break;
        }
    }
    this->timer++;
}

void Animation::draw() {
    switch(this->type) {
        case AnimationType::Death: {
            // Explosion animation
            if (this->timer >= this->keyFrames[1] && this->timer < this->keyFrames[2]) {
                DrawCircleLinesV(*this->points.at(0), 20 + (this->timer - this->keyFrames[1]) * 2, RAYWHITE);
            }
            break;
        }
    }
}