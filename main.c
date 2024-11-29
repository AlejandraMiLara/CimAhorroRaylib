#include "raylib.h"

// SCREENS

typedef enum GameScreen {
    SCREEN_LOGO,
    SCREEN_MENU, 
    SCREEN_AHORROS_PRINCIPAL,
    SCREEN_AHORROS_VER,
    SCREEN_AHORROS_AHORRAR,
    SCREEN_AHORROS_RETIRAR,
    SCREEN_MAX,
} GameScreen;

GameScreen currentScreen = SCREEN_LOGO;
const int MAX_SCREENS_DESLIZABLES = SCREEN_AHORROS_PRINCIPAL + 1;

//VARIABLES GLOBALES
bool logoMostrado = false;

// ESTRUCTURAS

typedef struct Boton {
    Rectangle rec;
    Color color;
    const char *text;
    void (*accion)();
} Boton;

// FUNCIONES DE MANEJO DE PANTALLAS

void actualizarScreens();
void dibujarScreens();

// FUNCIONES DE MANEJO DE BOTONES

void dibujarBotones(Boton botones[], int cantidad);
void verificarBotones(Boton botones[], int cantidad);

// FUNCIONES DE MANEJO DE ACCIONES

void accionVerAhorros();
void accionAhorrar();
void accionRetirar();

int main(void)
{
    // CONFIGURACIONES INICIALES
    const int screenWidth = 800;
    const int screenHeight = 450;
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitAudioDevice();
    InitWindow(screenWidth, screenHeight, "CIMAHORRO");
    SetTargetFPS(60);

    // VARIABLES DE ENTORNO

    //Variables: SCREEN_AHORROS_PRINCIPAL
    Boton botones[3] = {
        {{ 0, 0, 200, 50 }, BLUE, "Mis Ahorros", accionVerAhorros},
        {{ 0, 0, 200, 50 }, BLUE, "Ahorrar", accionAhorrar},
        {{ 0, 0, 200, 50 }, BLUE, "Retirar", accionRetirar}
    };


    // BUCLE PRINCIPAL
    while (!WindowShouldClose()) {
        // LÓGICA
        actualizarScreens();
        
        // DIBUJO
        BeginDrawing();
        ClearBackground(RAYWHITE);

        dibujarScreens();
        
        if (currentScreen == SCREEN_AHORROS_PRINCIPAL) {
            verificarBotones(botones, 3);
            dibujarBotones(botones, 3);
        }
        
        EndDrawing();
    }

    CloseWindow();

    return 0;
}

// FUNCIONES DE MANEJO DE PANTALLAS

void actualizarScreens() {
    if(currentScreen == SCREEN_LOGO){
        logoMostrado = true;
    }

    if (IsKeyPressed(KEY_RIGHT)) {
        if (currentScreen < MAX_SCREENS_DESLIZABLES - 1) {
            ClearBackground(RAYWHITE);
            currentScreen = (currentScreen + 1) % MAX_SCREENS_DESLIZABLES;
        }

        if(currentScreen == SCREEN_LOGO){
            currentScreen = SCREEN_MENU;
        }
    }

    if (IsKeyPressed(KEY_LEFT)) {
        if (currentScreen > SCREEN_MENU) {
            ClearBackground(RAYWHITE);
            currentScreen = (currentScreen - 1 + MAX_SCREENS_DESLIZABLES) % MAX_SCREENS_DESLIZABLES;
        }

        if(currentScreen == SCREEN_LOGO){
            currentScreen = SCREEN_MENU;
        }
    }
}

void dibujarScreens() {
    switch (currentScreen) {
        case SCREEN_LOGO:
            DrawText("LOGO", 350, 200, 20, BLACK);
            break;
        case SCREEN_MENU:
            DrawText("MENU", 350, 200, 20, BLACK);
            break;
        case SCREEN_AHORROS_PRINCIPAL:
            DrawText("MIS AHORROS", 20, 20, 20, BLACK);
            break;
        case SCREEN_AHORROS_VER:
            DrawText("VER AHORROS", 350, 200, 20, BLACK);
            break;
        case SCREEN_AHORROS_AHORRAR:
            DrawText("AGREGAR AHORRO", 350, 200, 20, BLACK);
            break;
        case SCREEN_AHORROS_RETIRAR:
            DrawText("RETIRAR AHORRO", 350, 200, 20, BLACK);
            break;
        default:
            break;
    }
}

// FUNCIONES DE MANEJO DE BOTONES

void dibujarBotones(Boton botones[], int cantidad) {
    int padding = 20;
    int totalHeight = (cantidad * 50) + (padding * (cantidad - 1));
    int startY = (GetScreenHeight() - totalHeight) / 2;

    for (int i = 0; i < cantidad; i++) {
        botones[i].rec.y = startY + i * (50 + padding);
        botones[i].rec.x = (GetScreenWidth() - botones[i].rec.width) / 2;

        if (CheckCollisionPointRec(GetMousePosition(), botones[i].rec)) {
            botones[i].color = GRAY;
        } else {
            botones[i].color = BLUE;
        }

        DrawRectangleRec(botones[i].rec, botones[i].color);

        Vector2 textSize = MeasureTextEx(GetFontDefault(), botones[i].text, 20, 0);
        DrawText(botones[i].text, 
                 botones[i].rec.x + (botones[i].rec.width - textSize.x) / 2, 
                 botones[i].rec.y + (botones[i].rec.height - textSize.y) / 2, 
                 20, BLACK);
    }
}

void verificarBotones(Boton botones[], int cantidad) {
    for (int i = 0; i < cantidad; i++) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), botones[i].rec)) {
            botones[i].accion();
        }
    }
}

// FUNCIONES DE MANEJO DE ACCIONES

void accionVerAhorros() {
    currentScreen = SCREEN_AHORROS_VER;
}

void accionAhorrar() {
    currentScreen = SCREEN_AHORROS_AHORRAR;
}

void accionRetirar() {
    currentScreen = SCREEN_AHORROS_RETIRAR;
}