#ifndef ZONE_H
#define ZONE_H

#include "raylib.h"

// Clockwise starting at bottom-right
// Zone1 == bottom-right, Zone2 == bottom-left, Zone3 == top-left, Zone4 == top-right
enum Zones {
    Zone1 = 1,
    Zone2,
    Zone3,
    Zone4
};

struct Zone {
    static const int zoneMargin = 10;

    bool isPlayerZone;
    Zones zoneNum = Zones::Zone1;
    Rectangle rect = Rectangle(0, 0, 0, 0);
    Color color = Color(0, 0, 0);
    Color lock_color = Color(0, 0, 0);

    void setRect(int screenWidth, int screenHeight) {
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
};

#endif