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
    int cantidad;
    char fecha[11];
    struct NodoAhorro *siguiente;
} NodoAhorro;

typedef struct NodoTanda {
    int idUsuario;
    char nombreUsuario[50];
    int montoAportado;
    int totalParticipantes;
    char fechaInicio[11];
    char fechaFin[11];
    struct NodoTanda *siguiente;
} NodoTanda;


// SCREENS

typedef enum GameScreen {
    SCREEN_LOGO,
    SCREEN_MENU, 
    SCREEN_AHORROS_PRINCIPAL,
    SCREEN_TANDAS_PRINCIPAL,   
    SCREEN_TANDAS_CREAR,     
    SCREEN_TANDAS_UNIR,      
    SCREEN_TANDAS_VER,       
    SCREEN_PRESTAMOS_PRINCIPAL,
    SCREEN_AHORROS_VER,
    SCREEN_AHORROS_AHORRAR,
    SCREEN_AHORROS_RETIRAR,
} GameScreen;

GameScreen currentScreen = SCREEN_LOGO;

//VARIABLES GLOBALES
bool logoMostrado = false; 
char inputBuffer[5] = ""; // Para capturar el ahorro del usuario
int inputPos = 0;         // Pos actual del buffer de entrada
bool registroExitoso = false; // Indica si la operacion fue exitosa
char mensaje[128] = "";   // Mensaje para el usuario
NodoAhorro *listaAhorros = NULL;
char usuario_global[50] = "Trunks";
int id_global = 1;
int logoTimer = 120;
bool clicReciente = false;
float tiempoEspera = 0.0f;
NodoTanda *listaTandas = NULL;

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
void actualizarClicReciente();
void dibujarBotones(Boton botones[], int cantidad);
void verificarBotones(Boton botones[], int cantidad);
void dibujarBoton(Boton *boton);
void verificarBoton(Boton *boton);

// FUNCIONES DE MANEJO DE ACCIONES

void accionVerAhorros();
void accionAhorrar();
void accionRetirar();
void accionAhorrosPrincipal();
void accionTandasPrincipal();
void accionTandasCrear();
void accionTandasUnir();
void accionTandasVer();
void accionPrestamosPrincipal();
void accionMenu();

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
    Boton regresar_menu = {{ 0, 0, 200, 50 }, BLUE, "< MENU", accionMenu};

    //Variables: SCREEN_MENU
    Boton botones_menu[3] = {
        {{ 0, 0, 200, 50 }, BLUE, "Ahorros", accionAhorrosPrincipal},
        {{ 0, 0, 200, 50 }, BLUE, "Tandas", accionTandasPrincipal},
        {{ 0, 0, 200, 50 }, BLUE, "Prestamos", accionPrestamosPrincipal}
    };

    //Variables: SCREEN_AHORROS_PRINCIPAL
    Boton botones[3] = {
        {{ 0, 0, 200, 50 }, BLUE, "Mis Ahorros", accionVerAhorros},
        {{ 0, 0, 200, 50 }, BLUE, "Ahorrar", accionAhorrar},
        {{ 0, 0, 200, 50 }, BLUE, "Retirar", accionRetirar}
    };

    //Variables: SCREEN_TANDAS_PRINCIPAL
    Boton botonesTandas[3] = {
            {{ 0, 0, 200, 50 }, BLUE, "Crear Tanda", accionTandasCrear},
            {{ 0, 0, 200, 50 }, BLUE, "Unirme a Tanda", accionTandasUnir},
            {{ 0, 0, 200, 50 }, BLUE, "Mis Tandas", accionTandasVer}
            };

    // BUCLE PRINCIPAL
    while (!WindowShouldClose()) {
        // LÓGICA
        actualizarScreens();
        actualizarClicReciente();
        
        // DIBUJO
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        dibujarScreens();
        
        if (currentScreen == SCREEN_MENU) {
            verificarBotones(botones_menu, 3);
            dibujarBotones(botones_menu, 3);
        }

        if (currentScreen == SCREEN_AHORROS_PRINCIPAL) {
            verificarBotones(botones, 3);
            dibujarBotones(botones, 3);
        }
        if (currentScreen == SCREEN_TANDAS_PRINCIPAL) {
            verificarBotones(botonesTandas, 3);
            dibujarBotones(botonesTandas, 3);
        }
        if (currentScreen != SCREEN_MENU && currentScreen != SCREEN_LOGO) {
            regresar_menu.rec.x = 20;
            regresar_menu.rec.y = 600;

            verificarBoton(&regresar_menu);
            dibujarBoton(&regresar_menu);
        }

        if (currentScreen == SCREEN_LOGO) {
            logoTimer--;
            if (logoTimer <= 0) {
                currentScreen = SCREEN_MENU;
            }
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
        case SCREEN_TANDAS_PRINCIPAL:
            DrawText("MIS TANDAS", 150, 20, 20, BLACK);
            break;
        case SCREEN_PRESTAMOS_PRINCIPAL:
            DrawText("MIS PRESTAMOS", 150, 20, 20, BLACK);
            break;
        case SCREEN_TANDAS_CREAR:
            DrawText("Crear Tanda", 150, 20, 20, BLACK);
            break;
        case SCREEN_TANDAS_UNIR:
            DrawText("Unirse a Tanda", 150, 20, 20, BLACK);
            break;
        case SCREEN_TANDAS_VER:
            DrawText("Mis Tandas", 150, 20, 20, BLACK);
            break;
        case SCREEN_AHORROS_VER:
            DrawText("MIS AHORROS", 150, 20, 20, BLACK);

            int totalAhorros = 0;
            int yOffset = 60;
            int xDesglose = 100;

            if (listaAhorros == NULL) {
                DrawText("Ahorros: $ 0.00 MXN", xDesglose, yOffset, 20, RED);
            } else {
                NodoAhorro *actual = listaAhorros;
                while (actual != NULL) {
                    totalAhorros += actual->cantidad;

                    char desglose[128];
                    snprintf(desglose, sizeof(desglose), "Cantidad: $%d MXN, Fecha: %s",
                         actual->cantidad, actual->fecha);

                    DrawText(desglose, xDesglose, yOffset, 20, DARKBLUE);
                    yOffset += 30;
                    actual = actual->siguiente;
                }

                char total[128];
                snprintf(total, sizeof(total), "Total de Ahorros: $%d.00 MXN", totalAhorros);
                DrawText(total, xDesglose, yOffset + 30, 20, verdeEsmeralda);
            }
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
                            agregarRegistroAhorro(id_global,usuario_global, cantidad, fecha, "ahorros.txt");
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

            if (listaAhorros == NULL) {
                DrawText("No tienes ahorros disponibles para retirar.", 150, 200, 20, RED);
            } else {
                NodoAhorro *actual = listaAhorros;
                int ahorroRealizado = 0;
                float rendimientosGenerados = 0;

                while (actual != NULL) {
                    ahorroRealizado += actual->cantidad;

                    // Fecha manual
                    int dia, mes, anio;
                    if (sscanf(actual->fecha, "%d/%d/%d", &dia, &mes, &anio) == 3) {
                        struct tm fechaStruct = {0};
                        fechaStruct.tm_mday = dia;
                        fechaStruct.tm_mon = mes - 1; // tm_mon va de 0 a 11
                        fechaStruct.tm_year = anio - 1900; // tm_year es desde 1900
                        
                        time_t tFechaAhorro = mktime(&fechaStruct);
                        time_t tHoy = time(NULL);

                        int diasTranscurridos = (tHoy - tFechaAhorro) / (60 * 60 * 24);

                        // Rendimiento
                        float rendimientoBase = actual->cantidad * 0.01f;
                        float interesGanado = rendimientoBase * ((float)diasTranscurridos / 30.0f);
                        if (interesGanado < 0) interesGanado = 0;

                        rendimientosGenerados += interesGanado;
                    } else {
                        DrawText("Error al leer la fecha de un registro.", 150, 300, 20, RED);
                    }

                    actual = actual->siguiente;
                }

                float totalDisponible = ahorroRealizado + rendimientosGenerados;

                char info[256];
                snprintf(info, sizeof(info),
                        "Ahorro realizado: $%d.00 MXN\n\n\nRendimientos Generados: $%.2f MXN\n\n\nTotal disponible para retiro: $%.2f MXN\n\n\n",
                        ahorroRealizado, rendimientosGenerados, totalDisponible);
                DrawText(info, 150, 200, 20, DARKGREEN);

                Rectangle botonRetirar = {150, 400, 200, 50};

                Color botonColor = BLUE;
                if (CheckCollisionPointRec(GetMousePosition(), botonRetirar)) {
                    botonColor = GRAY;
                }
                
                DrawRectangleRec(botonRetirar, botonColor);
                Vector2 textSize = MeasureTextEx(GetFontDefault(), "RETIRAR", 20, 0);
                DrawText("RETIRAR", 
                botonRetirar.x + (botonRetirar.width - textSize.x) / 2, 
                botonRetirar.y + (botonRetirar.height - textSize.y) / 2, 
                20, BLACK);

                if (CheckCollisionPointRec(GetMousePosition(), botonRetirar) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    NodoAhorro *temp;
                    while (listaAhorros != NULL) {
                        temp = listaAhorros;
                        listaAhorros = listaAhorros->siguiente;
                        free(temp);
                    }

                    FILE *archivo = fopen("ahorros.txt", "w");
                    if (archivo != NULL) fclose(archivo);

                    DrawText("Todos los ahorros fueron retirados exitosamente.", 150, 500, 20, verdeEsmeralda);
                }
            }
            break;

        default:
            break;
    }
}

// FUNCIONES DE MANEJO DE BOTONES

void actualizarClicReciente() {
    if (clicReciente) {
        tiempoEspera -= GetFrameTime();
        if (tiempoEspera <= 0.0f) {
            clicReciente = false;
        }
    }
}

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
    if (clicReciente) return;

    for (int i = 0; i < cantidad; i++) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), botones[i].rec)) {
            botones[i].accion();
            clicReciente = true;
            tiempoEspera = 0.2f;
            break;
        }
    }
}

void verificarBoton(Boton *boton) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), boton->rec)) {
        boton->accion();
    }
}

void dibujarBoton(Boton *boton) {
    if (CheckCollisionPointRec(GetMousePosition(), boton->rec)) {
        boton->color = GRAY;
    } else {
        boton->color = BLUE;
    }

    DrawRectangleRec(boton->rec, boton->color);

    Vector2 textSize = MeasureTextEx(GetFontDefault(), boton->text, 20, 0);
    DrawText(boton->text, 
             boton->rec.x + (boton->rec.width - textSize.x) / 2, 
             boton->rec.y + (boton->rec.height - textSize.y) / 2, 
             20, BLACK);
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

void accionAhorrosPrincipal(){
    currentScreen = SCREEN_AHORROS_PRINCIPAL;
}

void accionTandasPrincipal(){
    currentScreen = SCREEN_TANDAS_PRINCIPAL;
}

void accionTandasCrear() {
    currentScreen = SCREEN_TANDAS_CREAR;
}

void accionTandasUnir() {
    currentScreen = SCREEN_TANDAS_UNIR;
}

void accionTandasVer() {
    currentScreen = SCREEN_TANDAS_VER;
}

void accionMenu(){
    clicReciente = false;
    currentScreen = SCREEN_MENU;
}

void accionPrestamosPrincipal(){
    currentScreen = SCREEN_PRESTAMOS_PRINCIPAL;
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
                   &nuevoNodo->cantidad, 
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
    nuevoNodo->cantidad = cantidad;
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
