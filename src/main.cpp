#include "raylib.h"
#include "raymath.h"
#include "graphics/animation.h"
#include "core/card.h"
#include "core/deck.h"
#include "core/zone.h"
#include "helpers/collision.h"
#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <format>
#include <algorithm>
#include <bits/stdc++.h>

#include "graphics/animation.cpp"
#include "core/card.cpp"
#include "core/zone.cpp"

using namespace std;

enum GameState {
    Free,
    Animating
};

enum Phase
{
    Rotate,
    Play,
};

void releaseCard();
void endTurn(bool *isPlayerTurn, Phase *phase);
void shuffleDeck(vector<Card *> *deck);
void drawCard(vector<Card *> *hand, vector<Card *> *deck);
void repositionHand(vector<Card *> *hand);
void rotateCards(Zone *zones[], bool isPlayerTurn);
void stopAttacking(bool *isCardAttacking, Card *attackingCard, Zone *attackingZone);

// Set globals
const int screenWidth = 1280;
const int screenHeight = 720;
const int handCard_width = 150;
const int handCard_height = 230;
const int handCard_gap = 10;
const int handCard_x = screenWidth / 2 - (handCard_width + handCard_gap) * 3;
const int handCard_y = screenHeight - handCard_height / 5;

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    GameState gameState = GameState::Free;
    SetRandomSeed(time(NULL));
    bool isPlayerTurn = (GetRandomValue(0, 1) == 1); // RANDOMLY CHOOSE STARTING PLAYER
    int turnNum = 1;
    int energy = turnNum * 2;
    Phase phase = Phase::Play;

    vector<Animation> animations;
    queue<Animation> animationQueue;
    vector<Animation> idleAnimations;

    bool isHoveringHandCard = false;
    Card *hoveredHandCard;

    bool isHoveringPlayedCard = false;
    Card *hoveredPlayedCard;

    bool holdingCard = false;
    Card *heldCard;
    Vector2 heldCardOffset;

    bool isCardAttacking = false;
    Card *attackingCard;
    Vector2 attackingCardOrigin;
    Zone *attackingZone;

    InitWindow(screenWidth, screenHeight, "Forte of Cosmos");
    // ToggleFullscreen();
    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    int frameCounter = 0;

    Image gameBackgroundImage = LoadImage("assets/background.png");
    Texture2D gameBackground = LoadTextureFromImage(gameBackgroundImage);
    UnloadImage(gameBackgroundImage);

    // Player Health Info
    int playerHealth = 100;
    int opponentHealth = 100;
    float healthSize = screenWidth / 10;
    float pHealthY = screenHeight * 3 / 4 - healthSize / 2;
    float oHealthY = screenHeight / 4 - healthSize / 2;
    Rectangle playerHealthRect = {0, pHealthY, healthSize, healthSize};
    Rectangle opponentHealthRect = {0, oHealthY, healthSize, healthSize};

    // Setup Zones
    Zone *zones[4] = {
        new Zone{true, Zones::Zone1},
        new Zone{true, Zones::Zone2},
        new Zone{false, Zones::Zone3},
        new Zone{false, Zones::Zone4}};

    for (Zone *zone : zones)
    {
        zone->setRect(screenWidth, screenHeight);
    }

    Zone *playerZones[2] = {
        zones[0],
        zones[1]};

    Zone *opponentZones[2] = {
        zones[2],
        zones[3]};

    // Setup Player1's deck info
    DeckInfo p1DeckInfo = {};
    p1DeckInfo.name = "Straight Fiyah";
    for (unsigned i = 0; i < sizeof(p1DeckInfo.cards) / sizeof(p1DeckInfo.cards[0]); i++)
    {
        string name = string("Virgo");
        Card *card = new Card(name, "I'm a what?", 2, CardTypes::Sentient, 20, 20, true, CardStates::hand);
        p1DeckInfo.cards[i] = card;
    }

    // Setup Deck and shuffle
    vector<Card *> p1Deck;
    copy(begin(p1DeckInfo.cards), end(p1DeckInfo.cards), back_inserter(p1Deck));
    shuffleDeck(&p1Deck);

    // Setup Player1's hand
    vector<Card *> hand; // Consider using std::list instead of vector because of frequent insertions/deletions
    int startingHandSize = 5;
    for (int i = 0; i < startingHandSize; i++)
    {
        Card *card = p1Deck.front();
        p1Deck.erase(p1Deck.begin());
        hand.push_back(card);
        card->cardRect.width = handCard_width;
        card->cardRect.height = handCard_height;
        card->cardRect.x = i * (card->cardRect.width + handCard_gap) + handCard_x;
        card->cardRect.y = handCard_y;
        card->pos_lock.x = card->cardRect.x;
        card->pos_lock.y = card->cardRect.y;
    }

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Events
        //----------------------------------------------------------------------------------
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            if (!holdingCard)
            {
                for (Card *card : hand)
                {
                    if (CheckCollisionPointRec(GetMousePosition(), card->cardRect))
                    {
                        heldCard = card;
                        holdingCard = true;
                        Vector2 cardPos = Vector2(card->cardRect.x, card->cardRect.y);
                        heldCardOffset = {Vector2Subtract(cardPos, GetMousePosition())};
                    }
                }
            }

            // Handle attacking logic
            if (isPlayerTurn && phase == Phase::Play) {
                if (!isCardAttacking) {
                    for (Zone *zone : opponentZones) {
                        for (Card *card : zone->attackers) {
                            if (CheckCollisionPointRec(GetMousePosition(), card->cardRect)) {
                                isCardAttacking = true;
                                attackingCard = card;
                                attackingCardOrigin = {card->cardRect.x + card->cardRect.width / 2, card->cardRect.y + card->cardRect.height / 2};
                                attackingZone = zone;
                            }
                        }
                    }
                } else {
                    if (attackingZone->defenders.size() <= 0) {
                        if (CheckCollisionPointRec(GetMousePosition(), opponentHealthRect)) {
                            opponentHealth -= attackingCard->currentPower;
                            attackingZone->attackers.remove(attackingCard);
                            stopAttacking(&isCardAttacking, attackingCard, attackingZone);
                        }
                    } else {
                        Card *toRemove;
                        for (Card *defender : attackingZone->defenders) {
                            if (CheckCollisionPointRec(GetMousePosition(), defender->cardRect)) {
                                toRemove = defender;
                                defender->takeDamage(attackingCard->currentPower, attackingZone);
                                attackingCard->takeDamage(defender->currentPower, attackingZone);
                                stopAttacking(&isCardAttacking, attackingCard, attackingZone);
                            }
                        }
                        attackingZone->defenders.remove(toRemove);
                    }
                }
            }
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        {
            if (holdingCard)
            {
                holdingCard = false;
                heldCard->cardRect.x = heldCard->pos_lock.x;
                heldCard->cardRect.y = heldCard->pos_lock.y;
                heldCard = 0;
            }

            if (isCardAttacking) {
                stopAttacking(&isCardAttacking, attackingCard, attackingZone);
            }
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            if (holdingCard)
            {
                if (isPlayerTurn && phase == Phase::Play && gameState == GameState::Free)
                {
                    if (heldCard->currentCost <= energy)
                    {
                        for (Zone *zone : playerZones)
                        {
                            if (CheckCollisionPointRec(GetMousePosition(), zone->rect))
                            {
                                auto it = find(hand.begin(), hand.end(), heldCard);
                                hand.erase(it);
                                switch (heldCard->type)
                                {
                                case CardTypes::Sentient:
                                {
                                    zone->addCard(heldCard);
                                    break;
                                }
                                case CardTypes::Nova:
                                {
                                    zone->addCard(heldCard);
                                    break;
                                }
                                }
                                energy -= heldCard->currentCost;
                                repositionHand(&hand);
                            }
                        }
                    }
                }

                holdingCard = false;
                heldCard->cardRect.x = heldCard->pos_lock.x;
                heldCard->cardRect.y = heldCard->pos_lock.y;
                heldCard = 0;
            }
        }

        // ====================================================================
        // ============================== UPDATE ==============================
        // ====================================================================
        switch (phase)
        {
        // =============================== Rotate ==========================================
        case Phase::Rotate:
        {
            // Increment the turn and set the energy
            turnNum++;
            energy = turnNum + (turnNum % 2);

            // ==== Rotate ====
            rotateCards(zones, isPlayerTurn);

            // ==== Draw ====
            if (isPlayerTurn)
            {
                drawCard(&hand, &p1Deck);

                for (Zone *zone : opponentZones) {
                    zone->setAttackersAndDefenders();
                }
            }

            // ==== Next Phase ====
            phase = Phase::Play;
        }
        break;
        // =============================== Play ============================================
        case Phase::Play:
        {
            if (isPlayerTurn)
            {
                // Check if player's turn is over.
                int lowestCost = INT32_MAX;
                for (Card *card : hand)
                {
                    if (card->currentCost < lowestCost)
                    {
                        lowestCost = card->currentCost;
                    }
                }
                if (energy < lowestCost)
                {
                    endTurn(&isPlayerTurn, &phase);
                }
            }
            else if (!isPlayerTurn)
            {
                // If Opponent turn, randomly play cards to a random zone and end turn
                int cardCount = GetRandomValue(1, 2);
                int oppZone = GetRandomValue(0, 1);
                for (int i = 0; i < cardCount; i++)
                {
                    opponentZones[oppZone]->addCard(new Card("Virgo", "I'm a what?", 2, CardTypes::Sentient, 20, 20, false, CardStates::hand));
                }
                endTurn(&isPlayerTurn, &phase);
            }
        }
        break;
        }

        // =============================== All Phases ======================================
        if (holdingCard)
        {
            Vector2 mousePos = GetMousePosition();
            Vector2 cardPos = Vector2Add(mousePos, heldCardOffset);
            heldCard->cardRect.x = cardPos.x;
            heldCard->cardRect.y = cardPos.y;

            if (isPlayerTurn)
            {
                for (Zone *zone : playerZones)
                {
                    if (CheckCollisionPointRec(GetMousePosition(), zone->rect))
                    {
                        zone->color = Color(0, 0, 255);
                    }
                    else
                    {
                        zone->color = zone->lock_color;
                    }
                }
            }
        }

        if (!holdingCard)
        {
            for (Zone *zone : playerZones)
            {
                zone->color = zone->lock_color;
            }

            // Check if hovering over a hand card.
            if (!isHoveringHandCard)
            {
                for (Card *card : hand)
                {
                    if (CheckCollisionPointRec(GetMousePosition(), card->cardRect))
                    {
                        isHoveringHandCard = true;
                        hoveredHandCard = card;
                        card->cardRect.y = card->cardRect.y - (4 * card->cardRect.height / 5);
                    }
                }
            }
            else
            {
                if (!CheckCollisionPointRec(GetMousePosition(), hoveredHandCard->cardRect))
                {
                    isHoveringHandCard = false;
                    hoveredHandCard->cardRect.y = hoveredHandCard->pos_lock.y;
                    hoveredHandCard = 0;
                }
            }
        }

        // Check if hovering over a played card
        if (!isHoveringPlayedCard && !isHoveringHandCard)
        {
            for (Zone *zone : zones)
            {
                for (Card *card : zone->cards)
                {
                    if (CheckCollisionPointRec(GetMousePosition(), card->cardRect))
                    {
                        isHoveringPlayedCard = true;
                        hoveredPlayedCard = card;
                    }
                }
            }
        }
        else if (isHoveringPlayedCard)
        {
            if (!CheckCollisionPointRec(GetMousePosition(), hoveredPlayedCard->cardRect))
            {
                isHoveringPlayedCard = false;
                hoveredPlayedCard = 0;
            }
        }
        // =============== END STATE UPDATE ================

        frameCounter++;
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);
        DrawTexture(gameBackground, 0, 0, WHITE);

        // Draw Zones
        for (Zone *zone : zones)
        {
            DrawRectangleRec(zone->rect, zone->color);
            for (Card *card : zone->cards)
            {
                card->draw();
            }
        }

        // Draw Energy
        Color energyColor;
        if (isPlayerTurn)
        {
            energyColor = Color(40, 100, 150);
        }
        else
        {
            energyColor = Color(150, 100, 40);
        }
        DrawCircle(screenWidth / 2, screenHeight / 2, screenWidth / 40, ColorAlpha(energyColor, .8));
        int tw = MeasureText(to_string(energy).c_str(), 36);
        DrawText(to_string(energy).c_str(), screenWidth / 2 - tw / 2, screenHeight / 2 - 5 * tw / 6, 36, RAYWHITE);

        // Draw Health
        Color pHealthColor = Color { 100, 0, 250, 100 };
        Color oHealthColor = Color { 250, 0, 100, 100 };
        DrawRectangleRec(opponentHealthRect, oHealthColor);
        DrawText(to_string(opponentHealth).c_str(), 0, oHealthY, 36, WHITE);
        DrawRectangleRec(playerHealthRect, pHealthColor);
        DrawText(to_string(playerHealth).c_str(), 0, pHealthY, 36, WHITE);

        DrawFPS(10, 10);

        // Draw Cards in hand
        for (Card *card : hand)
        {
            if (holdingCard && card == heldCard)
                continue;

            card->draw();
        }

        // Draw Mouse-Held Card
        if (holdingCard)
        {
            Card *card = heldCard;
            card->draw();
        }
        else
        {
            // Draw hovered + played card
            if (isHoveringPlayedCard)
            {
                Vector2 mousePos = GetMousePosition();
                int xOffset = 10;
                int yOffset = 10;
                if (mousePos.x > screenWidth / 2)
                {
                    xOffset = -handCard_width;
                }
                if (mousePos.y > screenHeight / 2)
                {
                    yOffset = -handCard_height;
                }
                auto x = mousePos.x + xOffset;
                auto y = mousePos.y + yOffset;
                if (hoveredPlayedCard->isPlayerCard)
                {
                    DrawRectangle(x, y, handCard_width, handCard_height, BLUE);
                }
                else
                {
                    DrawRectangle(x, y, handCard_width, handCard_height, RED);
                }
                DrawText(to_string(hoveredPlayedCard->currentCost).c_str(), x + 5, y + 5, 14, WHITE);
                DrawText(hoveredPlayedCard->name.c_str(), x + 30, y + 5, 14, WHITE);
                DrawText(hoveredPlayedCard->desc.c_str(), x + 5, y + handCard_height / 2 + 20, 14, WHITE);
                switch (hoveredPlayedCard->type)
                {
                case CardTypes::Sentient:
                {
                    string pt = to_string(hoveredPlayedCard->currentPower) + "/" + to_string(hoveredPlayedCard->currentHealth);
                    DrawText(pt.c_str(), x + handCard_width - 44, y + handCard_height - 15, 14, WHITE);
                    break;
                }
                default:
                    break;
                }
            }
        }

        if (isCardAttacking) {
            DrawLineBezier(GetMousePosition(), attackingCardOrigin, 1.0, WHITE);
        }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(gameBackground);
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

void endTurn(bool *isPlayerTurn, Phase *phase)
{
    *isPlayerTurn = !*isPlayerTurn;
    *phase = Phase::Rotate;
}

void shuffleDeck(vector<Card *> *deck)
{
    random_shuffle(begin(*deck), end(*deck));
}

void drawCard(vector<Card *> *hand, vector<Card *> *deck)
{
    if (deck->size() > 0)
    {
        Card *card = deck->front();
        card->state = CardStates::hand;
        deck->erase(begin(*deck));
        hand->push_back(card);
        card->cardRect.width = handCard_width;
        card->cardRect.height = handCard_height;
        card->cardRect.x = (hand->size() - 1) * (card->cardRect.width + handCard_gap) + handCard_x;
        card->cardRect.y = handCard_y;
        card->pos_lock.x = card->cardRect.x;
        card->pos_lock.y = card->cardRect.y;
    }
}

void repositionHand(vector<Card *> *hand)
{
    for (unsigned i = 0; i < hand->size(); i++)
    {
        Card *card = hand->at(i);
        card->cardRect.x = i * (card->cardRect.width + handCard_gap) + handCard_x;
        card->pos_lock.x = card->cardRect.x;
    }
}

void rotateCards(Zone *zones[], bool isPlayerTurn)
{
    Zone tempZone = {};
    for (int i = 3; i >= 0; --i)
    {
        Zone *zone = zones[i];
        vector<Card *> removedCards;
        for (Card *card : zone->cards)
        {
            if (card->isPlayerCard == isPlayerTurn)
            {
                if (zone->zoneNum == Zones::Zone4)
                {
                    tempZone.addCard(card);
                }
                else
                {
                    zones[i + 1]->addCard(card);
                }
                removedCards.push_back(card);
            }
        }
        for (Card *card : removedCards)
        {
            zone->cards.remove(card);
            if (isPlayerTurn)
            {
                zone->playerCards.remove(card);
            }
            else
            {
                zone->opponentCards.remove(card);
            }
        }
    }
    for (Card *card : tempZone.cards)
    {
        zones[0]->addCard(card);
    }
    tempZone.cards.clear();
}

void stopAttacking(bool *isCardAttacking, Card *attackingCard, Zone* attackingZone) {
    *isCardAttacking = false;
    attackingCard = 0;
    attackingZone = 0;
}