#include "raylib.h"

typedef enum GameScreen {
     MENU, 
     AHORROS,
     AHORRAR,
} GameScreen;

int main(void)
{
    //CONFIGURACIONES INICIALES
    const int screenWidth = 1920;
    const int screenHeight = 1080;
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitAudioDevice();
    InitWindow(screenWidth, screenHeight, "CIMAHORRO");
    GameScreen currentScreen = MENU;
    SetTargetFPS(60);

    //VARIABLES DE ENTORNO


    //BUCLE DE LÓGICA
    while(!WindowShouldClose())
    {
        switch (currentScreen)
        {
            case MENU:
                break;
            
            default:
                break;
        }
    }
 
    BeginDrawing();
    ClearBackground(RAYWHITE);

    //BUCLE DE DIBUJO
    while (!WindowShouldClose())
    {
        switch (currentScreen)
        {
        case MENU:
            break;
        
        default:
            break;
        }
        EndDrawing();
    }

    CloseWindow();
    
    return 0;
}