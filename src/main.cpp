#include "raylib.h"
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
    //--------------------------------------------------------------------------------------
    int counter = 0;
    const int card_width = 150;
    const int card_height = 250;
    const int card_gap = 30;
    vector<string> hand;
    for (int i = 0; i < 6; i++) {
        hand.push_back(string("card ") + to_string(i + 1));
    }

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        counter++;
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);
        DrawFPS(10, 10);

        for (int i = 0; i < static_cast<int>(hand.size()); i++) {
            int x = i * (card_width + card_gap) + card_gap + counter;
            int y = 400 - counter;
            DrawRectangle(x, y, card_width, card_height, GRAY);
            DrawText(hand[i].c_str(), x + 20, y + 20, 20, WHITE);
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
