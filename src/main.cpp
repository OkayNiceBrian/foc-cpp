#include "raylib.h"
#include "raymath.h"
#include "core/card.h"
#include "core/deck.h"
#include "core/zone.h"
#include "helpers/collision.h"
#include <iostream>
#include <vector>
#include <string>
#include <format>
#include <algorithm>
#include <bits/stdc++.h>
using namespace std;

enum Phase
{
    Rotate,
    Play,
};

void releaseCard();
void nextTurn(bool *playerTurn, Phase *phase);
void shuffleDeck(vector<Card *> *deck);
void drawCard(vector<Card *> *hand, vector<Card *> *deck);
void repositionHand(vector<Card*> *hand);
void rotateCards(Zone *zones[], bool playerTurn);

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
    SetRandomSeed(time(NULL));
    bool playerTurn = (GetRandomValue(0, 1) == 1); // RANDOMLY CHOOSE STARTING PLAYER
    int turnNum = 1;
    int energy = turnNum * 2;
    Phase phase = Phase::Play;

    bool isHoveringCard = false;
    Card *hoveredCard;

    bool holdingCard = false;
    Card *heldCard;
    Vector2 heldCardOffset;

    InitWindow(screenWidth, screenHeight, "Forte of Cosmos");
    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    int frameCounter = 0;

    Image gameBackgroundImage = LoadImage("assets/background.png");
    Texture2D gameBackground = LoadTextureFromImage(gameBackgroundImage);
    UnloadImage(gameBackgroundImage);

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
        string name = string("Virgo") + to_string(i);
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
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            if (holdingCard)
            {
                if (playerTurn && phase == Phase::Play)
                {
                    if (heldCard->cost <= energy)
                    {
                        if (CheckCollisionPointRec(GetMousePosition(), zones[0]->rect))
                        {
                            auto it = find(hand.begin(), hand.end(), heldCard);
                            hand.erase(it);
                            switch (heldCard->type)
                            {
                            case CardTypes::Sentient:
                                zones[0]->addCard(heldCard);
                                break;
                            }
                            energy -= heldCard->cost;
                            repositionHand(&hand);
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
            rotateCards(zones, playerTurn);

            // ==== Draw ====
            if (playerTurn) {
                drawCard(&hand, &p1Deck);
            }

            // ==== Next Phase ====
            phase = Phase::Play;
        }
        break;
        // =============================== Play ============================================
        case Phase::Play:
        {
            if (playerTurn)
            {
                // Check if player's turn is over.
                int lowestCost = INT32_MAX;
                for (Card *card : hand)
                {
                    if (card->cost < lowestCost)
                    {
                        lowestCost = card->cost;
                    }
                }
                if (energy < lowestCost)
                {
                    nextTurn(&playerTurn, &phase);
                }
            } else if (!playerTurn)
            {
                // Switch turns if not player turn for testing
                nextTurn(&playerTurn, &phase);
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

            if (playerTurn)
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

            // Check if hovering over a card.
            if (!isHoveringCard)
            {
                for (Card *card : hand)
                {
                    if (CheckCollisionPointRec(GetMousePosition(), card->cardRect))
                    {
                        isHoveringCard = true;
                        hoveredCard = card;
                        card->cardRect.y = card->cardRect.y - (4 * card->cardRect.height / 5);
                    }
                }
            }
            else
            {
                if (!CheckCollisionPointRec(GetMousePosition(), hoveredCard->cardRect))
                {
                    isHoveringCard = false;
                    hoveredCard->cardRect.y = hoveredCard->pos_lock.y;
                    hoveredCard = 0;
                }
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
                DrawRectangleRec(card->cardRect, GRAY);
                DrawText(card->name.c_str(), card->cardRect.x + 5, card->cardRect.y + 5, 12, WHITE);
            }
        }

        // Draw Energy
        Color energyColor;
        if (playerTurn)
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

        DrawFPS(10, 10);

        // Draw Cards in hand
        for (Card *card : hand)
        {
            if (holdingCard && card == heldCard)
                continue;

            Rectangle cardRect = card->cardRect;

            DrawRectangleRec(cardRect, GRAY);
            DrawText(to_string(card->cost).c_str(), cardRect.x + 5, cardRect.y + 5, 14, WHITE);
            DrawText(card->name.c_str(), cardRect.x + 30, cardRect.y + 5, 14, WHITE);
            DrawText(card->desc.c_str(), cardRect.x + 5, cardRect.y + cardRect.height / 2 + 20, 14, WHITE);

            switch (card->type)
            {
            case CardTypes::Sentient:
            {
                string pt = to_string(card->power) + "/" + to_string(card->health);
                DrawText(pt.c_str(), cardRect.x + cardRect.width - 44, cardRect.y + cardRect.height - 15, 14, WHITE);
                break;
            }
            default:
                break;
            }
        }

        // Draw Mouse-Held Card
        if (holdingCard)
        {
            Card *card = heldCard;
            Rectangle cardRect = card->cardRect;

            DrawRectangleRec(cardRect, GRAY);
            DrawText(to_string(card->cost).c_str(), cardRect.x + 5, cardRect.y + 5, 14, WHITE);
            DrawText(card->name.c_str(), cardRect.x + 30, cardRect.y + 5, 14, WHITE);
            DrawText(card->desc.c_str(), cardRect.x + 5, cardRect.y + cardRect.height / 2 + 20, 14, WHITE);

            switch (card->type)
            {
            case CardTypes::Sentient:
            {
                string pt = to_string(card->power) + "/" + to_string(card->health);
                DrawText(pt.c_str(), cardRect.x + cardRect.width - 44, cardRect.y + cardRect.height - 15, 14, WHITE);
                break;
            }
            default:
                break;
            }
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

void nextTurn(bool *playerTurn, Phase *phase) {
    *playerTurn = !*playerTurn;
    *phase = Phase::Rotate;
}

void shuffleDeck(vector<Card *> *deck)
{
    random_shuffle(begin(*deck), end(*deck));
}

void drawCard(vector<Card *> *hand, vector<Card *> *deck)
{
    if (deck->size() > 0) {
        Card *card = deck->front();
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

void repositionHand(vector<Card*> *hand) {
    for (unsigned i = 0; i < hand->size(); i++) {
        Card *card = hand->at(i);
        card->cardRect.x = i * (card->cardRect.width + handCard_gap) + handCard_x;
        card->pos_lock.x = card->cardRect.x;
    }
}

void rotateCards(Zone *zones[], bool playerTurn) {
    Zone tempZone = {};
    for (int i = 3; i >= 0; --i)
    {
        Zone *zone = zones[i];
        vector<Card*> removedCards;
        for (Card *card : zone->cards)
        {
            if (card->playerCard == playerTurn) {
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
        for (Card *card : removedCards) {
            zone->cards.remove(card);
        }
    }
    for (Card *card : tempZone.cards)
    {
        zones[0]->addCard(card);
    }
    tempZone.cards.clear();
}