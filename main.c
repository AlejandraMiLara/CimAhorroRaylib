#include "raylib.h"

typedef enum GameScreen {
    SCREEN_MENU, 
    SCREEN_AHORROS,
    SCREEN_MAX,
} GameScreen;

// Definir la variable global currentScreen
GameScreen currentScreen = SCREEN_MENU;

void actualizarScreens() {
    if (IsKeyPressed(KEY_RIGHT)) {
        currentScreen = (currentScreen + 1) % SCREEN_MAX;
    }

    if (IsKeyPressed(KEY_LEFT)) {
        currentScreen = (currentScreen - 1 + SCREEN_MAX) % SCREEN_MAX;
    }
}

void dibujarScreens() {
    ClearBackground(RAYWHITE);

    switch (currentScreen) {
        case SCREEN_MENU:
            DrawText("MENU", 350, 200, 20, BLACK);
            break;
        case SCREEN_AHORROS:
            DrawText("MIS AHORROS", 350, 200, 20, BLACK);
            break;
        default:
            break;
    }
}

int main(void)
{
    // CONFIGURACIONES INICIALES
    const int screenWidth = 1920;
    const int screenHeight = 1080;
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitAudioDevice();
    InitWindow(screenWidth, screenHeight, "CIMAHORRO");
    SetTargetFPS(60);

    // BUCLE PRINCIPAL
    while (!WindowShouldClose()) {
        // LÓGICA
        actualizarScreens();
        
        // DIBUJO
        BeginDrawing();
        dibujarScreens();
        EndDrawing();
    }
 
    CloseWindow();
    
    return 0;
}
