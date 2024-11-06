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
using namespace std;

void releaseCard();

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 720;

    SetRandomSeed(time(NULL));
    bool playerTurn = (GetRandomValue(0, 1) == 1);
    int turnNum = 1;
    int energy = turnNum * 2;

    bool isHoveringCard = false;
    Card *hoveredCard;

    bool holdingCard = false;
    Card *heldCard;
    Vector2 heldCardOffset;

    const int handCard_width = 150;
    const int handCard_height = 230;
    const int handCard_gap = 10;
    const int handCard_x = screenWidth / 2 - (handCard_width + handCard_gap) * 3;
    const int handCard_y = screenHeight - handCard_height / 5;

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

    // Setup Player1's deck
    DeckInfo p1Deck = {};
    p1Deck.name = "Straight Fiyah";
    for (unsigned i = 0; i < sizeof(p1Deck.cards) / sizeof(p1Deck.cards[0]); i++)
    {
        string name = string("Virgo") + to_string(i);
        Card *card = new Card(name, "I'm a what?", 2, CardTypes::Sentient, 20, 20, CardStates::hand);
        p1Deck.cards[i] = card;
    }

    // Setup Player1's hand
    vector<int> hand;
    for (int i = 0; i < 6; i++)
    {
        hand.push_back(i);
        Card *card = p1Deck.cards[i];
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
                for (int cardId : hand)
                {
                    Card *card = p1Deck.cards[cardId];
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

        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            if (holdingCard) {
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
                if (playerTurn) {
                    if (CheckCollisionRecs(heldCard->cardRect, zones[0]->rect)) {

                    }
                }

                holdingCard = false;
                heldCard->cardRect.x = heldCard->pos_lock.x;
                heldCard->cardRect.y = heldCard->pos_lock.y;
                heldCard = 0;
            }
        }

        // Update
        //----------------------------------------------------------------------------------
        if (holdingCard)
        {
            Vector2 mousePos = GetMousePosition();
            Vector2 cardPos = Vector2Add(mousePos, heldCardOffset);
            heldCard->cardRect.x = cardPos.x;
            heldCard->cardRect.y = cardPos.y;

            if (playerTurn) {
                for (Zone *zone : playerZones) {
                    if (CheckCollisionRecs(heldCard->cardRect, zone->rect)) {
                        zone->color = Color(0, 0, 255);
                    } else {
                        zone->color = zone->lock_color;
                    }
                }
            }
        }

        if (!holdingCard)
        {
            // Check if hovering over a card.
            if (!isHoveringCard)
            {
                for (int cardId : hand)
                {
                    Card *card = p1Deck.cards[cardId];
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
        }

        // Draw Energy
        Color energyColor;
        if (playerTurn) {
            energyColor = Color(40, 100, 150);
        } else {
            energyColor = Color(150, 100, 40);
        }
        DrawCircle(screenWidth/2, screenHeight/2, screenWidth/40, ColorAlpha(energyColor, .8));
        int tw = MeasureText(to_string(energy).c_str(), 36);
        DrawText(to_string(energy).c_str(), screenWidth/2 - tw/2, screenHeight/2 - 5*tw/6, 36, RAYWHITE);

        DrawFPS(10, 10);

        // Draw Cards in hand
        for (int i : hand)
        {
            Card *card = p1Deck.cards[i];
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