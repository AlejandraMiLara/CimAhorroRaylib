#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//
// G E N E R A L I D A D E S
//

// ESTRUCTURAS

typedef struct Boton {
    Rectangle rec;
    Color color;
    const char *text;
    void (*accion)();
} Boton;

typedef struct NodoAhorro {
    int idUsuario;
    char nombreUsuario[50];
    int cantidadAhorrada;
    char fecha[11];
    struct NodoAhorro *siguiente;
} NodoAhorro;


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
char inputBuffer[5] = ""; // Para capturar el ahorro del usuario
int inputPos = 0;         // Pos actual del buffer de entrada
bool registroExitoso = false; // Indica si la operacion fue exitosa
char mensaje[128] = "";   // Mensaje para el usuario
NodoAhorro *listaAhorros = NULL;

// COLORES
Color verdeEsmeralda = {80, 200, 120, 255};

//
// P R O T O T I P O S  D E  F U N C I O N E S  
//

// FUNCIONES GENERALES
void obtenerFecha(char *buffer, size_t bufferSize);

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

// FUNCIONES DE MANEJO DE ARCHIVOS
FILE* manejoArchivos(const char *nombreArchivo, const char *modo, bool *archivoCreado);
void cargarAhorrosDesdeArchivo(const char *nombreArchivo);
void agregarRegistroAhorro(int idUsuario, const char *nombreUsuario, int cantidad, const char *fecha, const char *nombreArchivo);

//
// M A I N
//

int main(void)
{
    // CONFIGURACIONES INICIALES
    const int screenWidth = 1020;
    const int screenHeight = 720;
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitAudioDevice();
    InitWindow(screenWidth, screenHeight, "CIMAHORRO");
    SetTargetFPS(60);
    cargarAhorrosDesdeArchivo("ahorros.txt");

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

//
// F U N C I O N E S  
//

// FUNCIONES GENERALES
void obtenerFecha(char *buffer, size_t bufferSize) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(buffer, bufferSize, "%02d/%02d/%04d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
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
            DrawText("LOGO", 150, 20, 20, BLACK);
            break;
        case SCREEN_MENU:
            DrawText("MENU", 150, 20, 20, BLACK);
            break;
        case SCREEN_AHORROS_PRINCIPAL:
            DrawText("MIS AHORROS", 150, 20, 20, BLACK);
            break;
        case SCREEN_AHORROS_VER:
            DrawText("MIS AHORROS", 150, 20, 20, BLACK);
            DrawText("VER AHORROS", 150, 100, 20, BLACK);
            break;
        case SCREEN_AHORROS_AHORRAR:
            DrawText("MIS AHORROS", 150, 20, 20, BLACK);
            DrawText("AGREGAR AHORRO", 150, 100, 20, BLACK);

            Rectangle inputBox = {100, 200, 300, 50};
            DrawRectangleRec(inputBox, LIGHTGRAY);
            DrawRectangleLines(inputBox.x, inputBox.y, inputBox.width, inputBox.height, DARKGRAY);

            DrawText(inputBuffer, inputBox.x + 10, inputBox.y + 15, 20, DARKBLUE);

            DrawText("Ingrese una cantidad para transferir:", 100, 150, 20, BLACK);

            if (IsKeyPressed(KEY_BACKSPACE) && inputPos > 0) {
                inputBuffer[--inputPos] = '\0';
            }

            for (int key = KEY_ZERO; key <= KEY_NINE; key++) {
                if (IsKeyPressed(key) && inputPos < 4) {
                    inputBuffer[inputPos++] = '0' + (key - KEY_ZERO);
                    inputBuffer[inputPos] = '\0';
                }
            }

            for (int key = KEY_KP_0; key <= KEY_KP_9; key++) {
                if (IsKeyPressed(key) && inputPos < 4) {
                    inputBuffer[inputPos++] = '0' + (key - KEY_KP_0);
                    inputBuffer[inputPos] = '\0';
                }
            }

            Boton botonAhorrar = {{420, 200, 150, 50}, BLUE, "Ahorrar", NULL};
            if (CheckCollisionPointRec(GetMousePosition(), botonAhorrar.rec)) {
                botonAhorrar.color = GRAY;
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    int cantidad = atoi(inputBuffer);
                    if (cantidad > 0 && cantidad < 9999) {
                        char fecha[11];
                        obtenerFecha(fecha, sizeof(fecha));

                        bool creado;
                        FILE *archivo = manejoArchivos("ahorros.txt", "a", &creado);
                        if (archivo != NULL) {
                            agregarRegistroAhorro(1, "Trunks", cantidad, fecha, "ahorros.txt");
                            snprintf(mensaje, sizeof(mensaje), "Cantidad %d ingresada a tu ahorro!", cantidad);
                            registroExitoso = true;
                        } else {
                            snprintf(mensaje, sizeof(mensaje), "Error al abrir o crear el archivo.");
                        }
                    } else {
                        snprintf(mensaje, sizeof(mensaje), "Cantidad invalida, debe ser entre 1 y 9999.");
                    }

                    inputBuffer[0] = '\0';
                    inputPos = 0;
                }
            }
            DrawRectangleRec(botonAhorrar.rec, botonAhorrar.color);
            DrawText(botonAhorrar.text, botonAhorrar.rec.x + 10, botonAhorrar.rec.y + 15, 20, BLACK);

            if (registroExitoso) {
                DrawText(mensaje, 100, 300, 20, verdeEsmeralda);
            } else {
                DrawText(mensaje, 100, 300, 20, RED);
            }
        break;

        case SCREEN_AHORROS_RETIRAR:
            DrawText("MIS AHORROS", 150, 20, 20, BLACK);
            DrawText("RETIRAR AHORRO", 150, 100, 20, BLACK);
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

// FUNCIONES DE MANEJO DE ACCIONES EN BOTONES

void accionVerAhorros() {
    currentScreen = SCREEN_AHORROS_VER;
}

void accionAhorrar() {
    currentScreen = SCREEN_AHORROS_AHORRAR;
}

void accionRetirar() {
    currentScreen = SCREEN_AHORROS_RETIRAR;
}

// FUNCIONES DE MANEJO DE ARCHIVOS

FILE* manejoArchivos(const char *nombreArchivo, const char *modo, bool *archivoCreado) {
    FILE *archivo = fopen(nombreArchivo, modo);

    if (archivo == NULL) {
        archivo = fopen(nombreArchivo, "w+");
        if (archivo != NULL && archivoCreado != NULL) {
            *archivoCreado = true;
        }
    } else {
        if (archivoCreado != NULL) {
            *archivoCreado = false;
        }
    }

    return archivo;
}

void cargarAhorrosDesdeArchivo(const char *nombreArchivo) {
    bool archivoCreado;
    FILE *archivo = manejoArchivos(nombreArchivo, "r", &archivoCreado);

    if (archivo == NULL) {
        return;
    }

    while (!feof(archivo)) {
        NodoAhorro *nuevoNodo = (NodoAhorro *)malloc(sizeof(NodoAhorro));
        if (fscanf(archivo, "%d %s %d %s\n", &nuevoNodo->idUsuario, 
                   nuevoNodo->nombreUsuario, 
                   &nuevoNodo->cantidadAhorrada, 
                   nuevoNodo->fecha) == 4) {
            nuevoNodo->siguiente = listaAhorros;
            listaAhorros = nuevoNodo;
        } else {
            free(nuevoNodo);
        }
    }

    fclose(archivo);
}

void agregarRegistroAhorro(int idUsuario, const char *nombreUsuario, int cantidad, const char *fecha, const char *nombreArchivo) {
    NodoAhorro *nuevoNodo = (NodoAhorro *)malloc(sizeof(NodoAhorro));
    nuevoNodo->idUsuario = idUsuario;
    snprintf(nuevoNodo->nombreUsuario, sizeof(nuevoNodo->nombreUsuario), "%s", nombreUsuario);
    nuevoNodo->cantidadAhorrada = cantidad;
    snprintf(nuevoNodo->fecha, sizeof(nuevoNodo->fecha), "%s", fecha);
    nuevoNodo->siguiente = listaAhorros;

    listaAhorros = nuevoNodo;

    bool archivoCreado;
    FILE *archivo = manejoArchivos(nombreArchivo, "a", &archivoCreado);
    if (archivo != NULL) {
        fprintf(archivo, "%d %s %d %s\n", idUsuario, nombreUsuario, cantidad, fecha);
        fclose(archivo);
    }
}