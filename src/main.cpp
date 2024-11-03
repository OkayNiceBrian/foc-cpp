#include "raylib.h"
#include "core/card.h"
#include "core/deck.h"
#include <iostream>
#include <vector>
#include <string>
#include <format>
using namespace std;

int main()
{

    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "raylib");
    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    int frameCounter = 0;
    //--------------------------------------------------------------------------------------
    const int card_width = 150;
    const int card_height = 250;
    const int card_gap = 30;

    DeckInfo p1Deck = {};
    p1Deck.name = "Straight Fiyah";
    for (unsigned i = 0; i < sizeof(p1Deck.cards)/sizeof(p1Deck.cards[0]); i++) {
        Card card = {"Virgo", "I'm a what?", 2, CardTypes::Sentient, card.base_power, card.base_health};
        p1Deck.cards[i] = &card;
    }

    vector<int> hand;
    for (int i = 0; i < 6; i++) {
        hand.push_back(i);
    }

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        frameCounter++;
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);
        DrawFPS(10, 10);

        for (int i = 0; i < static_cast<int>(hand.size()); i++) {
            int x = i * (card_width + card_gap) + card_gap;
            int y = 400;
            DrawRectangle(x, y, card_width, card_height, GRAY);

            Card card = *p1Deck.cards[i];
            DrawText(to_string(p1Deck.cards[i]->cost).c_str(), x + 5, y + 5, 14, WHITE);
            DrawText(card.name.c_str(), x + 30, y + 5, 14, WHITE);
            DrawText(card.desc.c_str(), x + 5, y + card_height / 2 + 20, 14, WHITE);
            switch (card.type) {
            case CardTypes::Sentient: {
                string pt = to_string(card.power) + "/" + to_string(card.health);
                DrawText(pt.c_str(), x + card_width - 30, y + card_height - 15, 14, WHITE);
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
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
