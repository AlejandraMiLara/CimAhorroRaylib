#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

//
// G E N E R A L I D A D E S
//

// ESTRUCTURAS

typedef struct Boton
{
    Rectangle rec;
    Color color;
    const char *text;
    void (*accion)();
} Boton;

typedef struct NodoAhorro
{
    int idUsuario;
    char nombreUsuario[50];
    int cantidad;
    char fecha[11];
    struct NodoAhorro *siguiente;
} NodoAhorro;

typedef struct NodoTanda
{
    int idTanda;
    char nombreTanda[50];
    int montoAportado;
    int totalParticipantes;
    int totalSemanal;
    char fechaInicio[11];
    char fechaFin[11];
    struct NodoTanda *siguiente;
} NodoTanda;

// Crear estructura prestamo

// ESTRUCTURA PARA ALMACENAR INFORMACION DE PRESTAMOS
typedef struct NodoPrestamo
{
    int idPrestamo;
    int idUsuario;
    char nombreUsuario[50];
    float cantidadSolicitada;
    float cantidadPendiente;
    int plazosMeses;
    float pagoMensual;
    char fechaSolicitud[11];
    char fechaproximopago[11];
    bool activo;
    bool incluirIntereses;
    struct NodoPrestamo *siguiente;
} NodoPrestamo;

// ESTRUCTURA PARA PAGOS REALIZADOS
typedef struct NodoPago
{
    int idPrestamo;
    float cantidad;
    char fecha[11];
    struct NodoPago *siguiente;
} NodoPago;

// SCREENS

typedef enum GameScreen
{
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
    SCREEN_PRESTAMOS_SOLICITAR,
    SCREEN_PRESTAMOS_VER,
} GameScreen;

GameScreen currentScreen = SCREEN_LOGO;

// VARIABLES GLOBALES
bool logoMostrado = false;
char inputBuffer[5] = "";     // Para capturar el ahorro del usuario
int inputPos = 0;             // Pos actual del buffer de entrada
bool registroExitoso = false; // Indica si la operacion fue exitosa
char mensaje[128] = "";       // Mensaje para el usuario
NodoAhorro *listaAhorros = NULL;
char usuario_global[50] = "Trunks";
int id_global = 1;
int logoTimer = 200;
bool clicReciente = false;
float tiempoEspera = 0.0f;
NodoTanda *listaTandas = NULL;
Boton regresarBoton;

// VARIABLES GLOBALES PARA PRESTAMOS
NodoPrestamo *listaPrestamos = NULL;
NodoPago *listaPagos = NULL;
bool prestamoActivo = false;
char mensajePrestamo[128] = "";
bool interes_activo = true; // Por defecto, el interés está activo
// Variables para la interfaz
static Rectangle inputCantidad = {100, 150, 300, 40};
static Rectangle inputPlazo = {100, 250, 300, 40};
static Rectangle inputTarjeta = {100, 350, 300, 40};
static Rectangle botonSolicitar = {100, 450, 200, 50};
// static Rectangle botonPagar = {400, 450, 200, 50};

static char bufferCantidad[10] = "";
static char bufferPlazo[3] = "";
static char bufferTarjeta[17] = "";
static int campoActivo = 0; // 0: ninguno, 1: cantidad, 2: plazo, 3: tarjeta

// Variables para las texturas
Texture2D LOGO;
Texture2D MENU;
Texture2D OPCS;
Texture2D OPC1;
Texture2D OPC2;
Texture2D OPC3;

// COLORES
Color verdeEsmeralda = {80, 200, 120, 255};
Color ColorBotones = {1, 182, 149, 255};
Color ColorBotonesOscuro = {1, 140, 115, 255};
//
// P R O T O T I P O S  D E  F U N C I O N E S
//

// FUNCIONES GENERALES
void obtenerFecha(char *buffer, size_t bufferSize);
void cargarTexturas();
void liberarTexturas();

// FUNCIONES DE MANEJO DE PANTALLAS

void actualizarScreens();
void dibujarScreens();

// FUNCIONES DE MANEJO DE BOTONES
void actualizarClicReciente();
void dibujarBotones(Boton botones[], int cantidad);
void verificarBotones(Boton botones[], int cantidad);
void dibujarBoton(Boton *boton);
void verificarBoton(Boton *boton);
void dibujarBotonAtras(Boton *boton);
void verificarBotonRegresar(Boton *boton);

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
void regresarAccion();

// FUNCIONES DE MANEJO DE ARCHIVOS
FILE *manejoArchivos(const char *nombreArchivo, const char *modo, bool *archivoCreado);
void cargarAhorrosDesdeArchivo(const char *nombreArchivo);
void cargarTandasDesdeArchivo(const char *nombreArchivo);
void cargarParticipantesDesdeArchivo(const char *nombreArchivo);
void agregarRegistroAhorro(int idUsuario, const char *nombreUsuario, int cantidad, const char *fecha, const char *nombreArchivo);
void agregarRegistroTanda(int idTanda, const char *nombreTanda, int montoAportado, int totalParticipantes, const char *fechaInicio, const char *fechaFin, const char *nombreArchivo);
bool agregarParticipanteATanda(int idTanda, const char *nombreParticipante, const char *nombreArchivo);
//

// FUNCIONES PARA APARTADO DE PRESTAMOS
void inicializarPrestamos(void);
void actualizarPantallaPrestamos(void);
void dibujarPantallaPrestamos(void);
void ProcesarSolicitudPrestamos(float cantidad, int plazo, const char *numeroTarjeta, bool interes_activo);
void realizarPagoPrestamo(int idPrestamo, float cantidad, const char *numeroTarjeta);
float calcularPagoMensual(float cantidad, int plazoMeses, bool interes_activo);
bool cargarPrestamos(void);
void guardarPrestamos(void);
void accionPrestamosSolicitar(void);
void accionPrestamosVer(void);
void registrarPagoEnArchivo(int idPrestamo, float cantidad, const char *fechaPago, const char *fechaProximoPago);
bool custom_strptime(const char *cadena, const char *formato, struct tm *fecha);
void obtenerFecha_prestamo(char *fecha, size_t tamano);

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
    cargarTandasDesdeArchivo("tandas.txt");
    cargarTexturas();
    inicializarPrestamos();
    // VARIABLES DE ENTORNO
    Boton regresar_menu = {{0, 0, 200, 50}, BLUE, "< MENU", accionMenu};

    // Variables: SCREEN_MENU
    Boton botones_menu[3] = {
        {{0, 0, 200, 50}, BLUE, "Ahorros", accionAhorrosPrincipal},
        {{0, 0, 200, 50}, BLUE, "Tandas", accionTandasPrincipal},
        {{0, 0, 200, 50}, BLUE, "Prestamos", accionPrestamosPrincipal}};

    // Variables: SCREEN_AHORROS_PRINCIPAL
    Boton botones[3] = {
        {{0, 0, 200, 50}, BLUE, "Mis Ahorros", accionVerAhorros},
        {{0, 0, 200, 50}, BLUE, "Ahorrar", accionAhorrar},
        {{0, 0, 200, 50}, BLUE, "Retirar", accionRetirar}};

    // Variables: SCREEN_TANDAS_PRINCIPAL
    Boton botonesTandas[3] = {
        {{0, 0, 200, 50}, verdeEsmeralda, "Crear Tanda", accionTandasCrear},
        {{0, 0, 200, 50}, verdeEsmeralda, "Unirme a Tanda", accionTandasUnir},
        {{0, 0, 200, 50}, verdeEsmeralda, "Mis Tandas", accionTandasVer}};

    // Variables: SCREEN_PRESTAMOS_PRINCIPAL
    Boton botonesPrestamos[2] = {
        {{0, 0, 228, 50}, BLUE, "Solicitar Prestamo", accionPrestamosSolicitar},
        {{0, 0, 228, 50}, BLUE, "Ver Mis Prestamos", accionPrestamosVer}};
    // Definir el boton de regresar
    Boton regresarBoton = {{0, 0, 200, 50}, BLUE, "< Regresar", regresarAccion};

    // BUCLE PRINCIPAL
    while (!WindowShouldClose())
    {
        // LoGICA
        actualizarScreens();
        actualizarClicReciente();

        if (currentScreen == SCREEN_PRESTAMOS_PRINCIPAL || currentScreen == SCREEN_PRESTAMOS_SOLICITAR || currentScreen == SCREEN_PRESTAMOS_VER)
        {
            actualizarPantallaPrestamos();
        }

        // DIBUJO
        BeginDrawing();
        ClearBackground(RAYWHITE);

        dibujarScreens();

        if (currentScreen == SCREEN_MENU)
        {
            verificarBotones(botones_menu, 3);
            dibujarBotones(botones_menu, 3);
        }

        if (currentScreen == SCREEN_AHORROS_PRINCIPAL)
        {
            verificarBotones(botones, 3);
            dibujarBotones(botones, 3);
        }
        if (currentScreen == SCREEN_TANDAS_PRINCIPAL)
        {
            verificarBotones(botonesTandas, 3);
            dibujarBotones(botonesTandas, 3);
        }
        if (currentScreen == SCREEN_AHORROS_PRINCIPAL || currentScreen == SCREEN_PRESTAMOS_PRINCIPAL || currentScreen == SCREEN_TANDAS_PRINCIPAL)
        {
            regresar_menu.rec.x = 20;
            regresar_menu.rec.y = 600;

            verificarBoton(&regresar_menu);
            dibujarBoton(&regresar_menu);
        }

        if (currentScreen == SCREEN_LOGO)
        {
            logoTimer--;
            if (logoTimer <= 0)
            {
                currentScreen = SCREEN_MENU;
            }
        }

        if (currentScreen == SCREEN_PRESTAMOS_PRINCIPAL)
        {
            verificarBotones(botonesPrestamos, 2);
            dibujarBotones(botonesPrestamos, 2);
        }

        // Verificar el boton de regresar cuando sea necesario
        if (currentScreen != SCREEN_MENU && currentScreen != SCREEN_LOGO && currentScreen != SCREEN_AHORROS_PRINCIPAL && currentScreen != SCREEN_TANDAS_PRINCIPAL && currentScreen != SCREEN_PRESTAMOS_PRINCIPAL)
        {
            regresarBoton.rec.x = 20;
            regresarBoton.rec.y = 600;
            verificarBotonRegresar(&regresarBoton);
            dibujarBotonAtras(&regresarBoton);
        }

        EndDrawing();
    }

    liberarTexturas();

    CloseWindow();

    return 0;
}

//
// F U N C I O N E S
//

// FUNCIONES GENERALES
void obtenerFecha(char *buffer, size_t bufferSize)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(buffer, bufferSize, "%02d/%02d/%04d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
}

void cargarTexturas()
{
    LOGO = LoadTexture("images/LOGO.png");
    MENU = LoadTexture("images/MENU.png");
    OPCS = LoadTexture("images/OPCS.png");
    OPC1 = LoadTexture("images/OPC1.png");
    OPC2 = LoadTexture("images/OPC2.png");
    OPC3 = LoadTexture("images/OPC3.png");
}

void liberarTexturas()
{
    UnloadTexture(LOGO);
    UnloadTexture(MENU);
    UnloadTexture(OPCS);
    UnloadTexture(OPC1);
    UnloadTexture(OPC2);
    UnloadTexture(OPC3);
}
// FUNCIONES DE MANEJO DE PANTALLAS

void actualizarScreens()
{
    if (currentScreen == SCREEN_LOGO)
    {
        logoMostrado = true;
    }
}

void dibujarScreens()
{

    switch (currentScreen)
    {
    case SCREEN_LOGO:
        DrawTexture(LOGO, 0, 0, WHITE);
        break;
    case SCREEN_MENU:
        DrawTexture(MENU, 0, 0, WHITE);
        DrawText("MENU", 150, 20, 20, BLACK);
        break;
    case SCREEN_AHORROS_PRINCIPAL:
        DrawTexture(OPCS, 0, 0, WHITE);
        DrawText("MIS AHORROS", 150, 20, 20, BLACK);
        break;
    case SCREEN_TANDAS_PRINCIPAL:
        DrawTexture(OPCS, 0, 0, WHITE);
        DrawText("MIS AHORROS", 150, 20, 20, BLACK);
        DrawText("MIS TANDAS", 150, 20, 20, BLACK);
        break;
    case SCREEN_PRESTAMOS_PRINCIPAL:
        DrawTexture(OPCS, 0, 0, WHITE);
        DrawText("PRESTAMOS", 150, 20, 20, BLACK);
        break;

    case SCREEN_PRESTAMOS_SOLICITAR:
        DrawTexture(OPC1, 0, 0, WHITE);
        DrawText("Solicitar Préstamo", 150, 20, 20, BLACK);

        // Dibujar recuadros de inputs
        DrawRectangleRec(inputCantidad, LIGHTGRAY);
        DrawRectangleRec(inputPlazo, LIGHTGRAY);
        DrawRectangleRec(inputTarjeta, LIGHTGRAY);

        DrawText("Cantidad del préstamo:", 100, 120, 20, BLACK);
        DrawText("Plazo (meses):", 100, 220, 20, BLACK);
        DrawText("Número de tarjeta:", 100, 320, 20, BLACK);

        DrawText(bufferCantidad, inputCantidad.x + 10, inputCantidad.y + 10, 20, BLACK);
        DrawText(bufferPlazo, inputPlazo.x + 10, inputPlazo.y + 10, 20, BLACK);
        DrawText(bufferTarjeta, inputTarjeta.x + 10, inputTarjeta.y + 10, 20, BLACK);

        Color colorBoton = prestamoActivo ? GRAY : BLUE;
        DrawRectangleRec(botonSolicitar, colorBoton);
        DrawText("Solicitar Préstamo", botonSolicitar.x + 10, botonSolicitar.y + 15, 20, WHITE);

        // Dibujar toggle para interés
        Rectangle toggleRect = {100, 400, 30, 30};
        DrawRectangleRec(toggleRect, interes_activo ? GREEN : RED);
        DrawText(interes_activo ? "Interés ON" : "Interés OFF", 140, 405, 20, BLACK);

        // Manejar clic en el toggle
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), toggleRect))
        {
            interes_activo = !interes_activo;
        }

        if (strlen(mensajePrestamo) > 0)
        {
            DrawText(mensajePrestamo, 100, 550, 20, RED);
        }
        break;

    case SCREEN_PRESTAMOS_VER:
        DrawTexture(OPC1, 0, 0, WHITE);
        DrawText("Mis Préstamos", 150, 20, 20, BLACK);

        if (prestamoActivo && listaPrestamos != NULL)
        {
            Rectangle labelInfoPrestamo = {100, 80, 400, 180};
            DrawRectangleRec(labelInfoPrestamo, LIGHTGRAY);

            char infoPrestamo[256];
            snprintf(infoPrestamo, sizeof(infoPrestamo),
                     "Préstamo activo:\nCantidad: $%.2f\nPendiente: $%.2f\nPago mensual: $%.2f\nPróximo pago: %s",
                     listaPrestamos->cantidadSolicitada,
                     listaPrestamos->cantidadPendiente,
                     listaPrestamos->pagoMensual,
                     listaPrestamos->fechaproximopago);
            DrawText(infoPrestamo, labelInfoPrestamo.x + 10, labelInfoPrestamo.y + 10, 20, BLACK);

            Rectangle botonPago = {100, 300, 200, 50};
            DrawRectangleRec(botonPago, BLUE);
            DrawText("Realizar Pago", botonPago.x + 10, botonPago.y + 15, 20, WHITE);

            if (CheckCollisionPointRec(GetMousePosition(), botonPago) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                realizarPagoPrestamo(listaPrestamos->idPrestamo, listaPrestamos->pagoMensual, "");
            }
        }
        else
        {
            Rectangle labelSinPrestamos = {100, 100, 400, 50};
            DrawRectangleRec(labelSinPrestamos, LIGHTGRAY);
            DrawText("No tienes préstamos activos", labelSinPrestamos.x + 10, labelSinPrestamos.y + 10, 20, BLACK);
        }
        break;
    case SCREEN_TANDAS_CREAR:
        DrawTexture(OPC1, 0, 0, WHITE);
        DrawText("Crear Tanda", 150, 20, 20, BLACK);
        // Definir las variables estáticas para los campos de entrada
        static char nombreTanda[50] = "";
        static char montoAportado[10] = "";
        static char totalParticipantes[5] = "";
        static char duracionSemanas[3] = "";
        static char mensajeTanda[128] = "";

        // Definir las posiciones y tamaños de los cuadros de texto y el boton
        Rectangle inputNombre = {100, 100, 300, 40};
        Rectangle inputMonto = {100, 175, 300, 40};
        Rectangle inputParticipantes = {100, 250, 300, 40};
        Rectangle inputDuracion = {100, 325, 300, 40};
        Rectangle botonCrear = {150, 400, 200, 50};

        // Nombres de los campos sobre las cajas de texto
        DrawText("Nombre de la Tanda", 100, 75, 20, BLACK);
        DrawText("Monto Aportado (Semanal)", 100, 150, 20, BLACK);
        DrawText("Total Participantes", 100, 225, 20, BLACK);
        DrawText("Duracion (Semanal)", 100, 300, 20, BLACK);

        // Logica de interaccion con los campos de texto
        static int posNombre = 0, posMonto = 0, posParticipantes = 0, posDuracion = 0;

        // Logica de seleccion de los campos para ingreso
        if (CheckCollisionPointRec(GetMousePosition(), inputNombre) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            posNombre = 1;
            posMonto = posParticipantes = posDuracion = 0;
        }
        else if (CheckCollisionPointRec(GetMousePosition(), inputMonto) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            posMonto = 1;
            posNombre = posParticipantes = posDuracion = 0;
        }
        else if (CheckCollisionPointRec(GetMousePosition(), inputParticipantes) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            posParticipantes = 1;
            posNombre = posMonto = posDuracion = 0;
        }
        else if (CheckCollisionPointRec(GetMousePosition(), inputDuracion) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            posDuracion = 1;
            posNombre = posMonto = posParticipantes = 0;
        }

        // Entrada para el nombre de la tanda
        if (posNombre)
        {
            if (IsKeyPressed(KEY_BACKSPACE) && strlen(nombreTanda) > 0)
            {
                nombreTanda[strlen(nombreTanda) - 1] = '\0';
            }
            else
            {
                char c = GetCharPressed();
                while (c > 0)
                {
                    if (strlen(nombreTanda) < 49)
                    {
                        nombreTanda[strlen(nombreTanda)] = c;
                    }
                    c = GetCharPressed();
                }
            }
        }

        // Entrada para el monto aportado (semanal)
        if (posMonto)
        {
            if (IsKeyPressed(KEY_BACKSPACE) && strlen(montoAportado) > 0)
            {
                montoAportado[strlen(montoAportado) - 1] = '\0';
            }
            else
            {
                char c = GetCharPressed();
                while (c > 0)
                {
                    if (c >= '0' && c <= '9' && strlen(montoAportado) < 9)
                    {
                        montoAportado[strlen(montoAportado)] = c;
                    }
                    c = GetCharPressed();
                }
            }
        }

        // Entrada para el número de participantes
        if (posParticipantes)
        {
            if (IsKeyPressed(KEY_BACKSPACE) && strlen(totalParticipantes) > 0)
            {
                totalParticipantes[strlen(totalParticipantes) - 1] = '\0';
            }
            else
            {
                char c = GetCharPressed();
                while (c > 0)
                {
                    if (c >= '0' && c <= '9' && strlen(totalParticipantes) < 4)
                    {
                        totalParticipantes[strlen(totalParticipantes)] = c;
                    }
                    c = GetCharPressed();
                }
            }
        }

        // Entrada para la duracion en semanas
        if (posDuracion)
        {
            if (IsKeyPressed(KEY_BACKSPACE) && strlen(duracionSemanas) > 0)
            {
                duracionSemanas[strlen(duracionSemanas) - 1] = '\0';
            }
            else
            {
                char c = GetCharPressed();
                while (c > 0)
                {
                    if (c >= '0' && c <= '9' && strlen(duracionSemanas) < 2)
                    {
                        duracionSemanas[strlen(duracionSemanas)] = c;
                    }
                    c = GetCharPressed();
                }
            }
        }

        // Dibuja los cuadros de texto (input boxes)
        DrawRectangleRec(inputNombre, GRAY);
        DrawRectangleRec(inputMonto, GRAY);
        DrawRectangleRec(inputParticipantes, GRAY);
        DrawRectangleRec(inputDuracion, GRAY);

        // Dibuja el texto dentro de los cuadros
        DrawText(nombreTanda, inputNombre.x + 10, inputNombre.y + 10, 20, BLACK);
        DrawText(montoAportado, inputMonto.x + 10, inputMonto.y + 10, 20, BLACK);
        DrawText(totalParticipantes, inputParticipantes.x + 10, inputParticipantes.y + 10, 20, BLACK);
        DrawText(duracionSemanas, inputDuracion.x + 10, inputDuracion.y + 10, 20, BLACK);

        // funcion para validar nombres duplicados
        bool existeTandaConNombre(const char *nombreTanda)
        {
            NodoTanda *actual = listaTandas;
            while (actual != NULL)
            {
                if (strcmp(actual->nombreTanda, nombreTanda) == 0)
                {
                    return true; // El nombre ya existe
                }
                actual = actual->siguiente;
            }
            return false; // No se encontro una tanda con ese nombre
        }

        // Funcion para verificar si el nombre contiene espacios
        bool contieneEspacios(const char *nombre)
        {
            while (*nombre)
            {
                if (*nombre == ' ')
                {
                    return true; // El nombre contiene espacios
                }
                nombre++;
            }
            return false; // El nombre no contiene espacios
        }

        // Boton Crear
        Color botonColor = BLUE;
        if (CheckCollisionPointRec(GetMousePosition(), botonCrear))
        {
            botonColor = GRAY;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                // Verificacion de campos vacíos o inválidos
                if (strlen(nombreTanda) > 0 && atoi(montoAportado) > 0 && atoi(totalParticipantes) > 0 && strlen(duracionSemanas) > 0)
                {
                    if (contieneEspacios(nombreTanda))
                    {
                        snprintf(mensajeTanda, sizeof(mensajeTanda), "El nombre no debe contener espacios.");
                    }
                    else if (existeTandaConNombre(nombreTanda))
                    {
                        snprintf(mensajeTanda, sizeof(mensajeTanda), "Ya existe una tanda con el nombre '%s'.", nombreTanda);
                    }
                    else
                    {
                        // Crear y configurar la nueva tanda
                        NodoTanda *nuevaTanda = (NodoTanda *)malloc(sizeof(NodoTanda));
                        nuevaTanda->idTanda = rand() % 10000;
                        strcpy(nuevaTanda->nombreTanda, nombreTanda);
                        nuevaTanda->montoAportado = atoi(montoAportado);
                        nuevaTanda->totalParticipantes = atoi(totalParticipantes);

                        // Obtener la fecha de inicio (fecha actual)
                        time_t t = time(NULL);
                        struct tm tmFecha = *localtime(&t);
                        strftime(nuevaTanda->fechaInicio, sizeof(nuevaTanda->fechaInicio), "%Y-%m-%d", &tmFecha);

                        int duracionEnDias = atoi(duracionSemanas) * 7; // Convertir semanas a días
                        tmFecha.tm_mday += duracionEnDias;
                        mktime(&tmFecha); // Normaliza la fecha
                        strftime(nuevaTanda->fechaFin, sizeof(nuevaTanda->fechaFin), "%Y-%m-%d", &tmFecha);

                        // Añadir a la lista de tandas
                        nuevaTanda->siguiente = listaTandas;
                        listaTandas = nuevaTanda;

                        // Guardar en archivo
                        bool creado;
                        FILE *archivo = manejoArchivos("tandas.txt", "a", &creado);
                        if (archivo != NULL)
                        {
                            agregarRegistroTanda(
                                nuevaTanda->idTanda,
                                nuevaTanda->nombreTanda,
                                nuevaTanda->montoAportado,
                                nuevaTanda->totalParticipantes,
                                nuevaTanda->fechaInicio,
                                nuevaTanda->fechaFin,
                                "tandas.txt");
                            fclose(archivo);

                            // Mensaje de exito
                            snprintf(mensajeTanda, sizeof(mensajeTanda), "La Tanda '%s' fue creada exitosamente!", nombreTanda);
                        }
                        else
                        {
                            snprintf(mensajeTanda, sizeof(mensajeTanda), "Error al abrir o crear el archivo.");
                        }
                    }
                }
                else
                {
                    // Mensaje de error si hay campos vacíos o inválidos
                    snprintf(mensajeTanda, sizeof(mensajeTanda), "Todos los campos son obligatorios.");
                }
            }
        }
        // Dibuja el boton Crear
        DrawRectangleRec(botonCrear, botonColor);
        DrawText("CREAR", botonCrear.x + 60, botonCrear.y + 15, 20, BLACK);

        // Mostrar mensaje de error o exito
        DrawText(mensajeTanda, 100, 465, 20, (strlen(mensajeTanda) > 0 && strstr(mensajeTanda, "exitosamente") != NULL) ? GREEN : RED);

        break;

    case SCREEN_TANDAS_UNIR:
        // Declarar variables de control de mensaje
        static bool mensajeMostrado = false; // Variable para controlar si el mensaje debe mostrarse
        static char mensaje[100];
        static int tandaSeleccionadaId = -1;
        static char participantesText[50] = "";

        DrawText("Unirse a Tanda", 150, 20, 20, BLACK);

        // Mostrar las tandas disponibles
        NodoTanda *actual = listaTandas;
        int yPos = 60; // Posicion de inicio para la primera tanda
        while (actual != NULL)
        {
            // Mostrar el nombre de la tanda
            DrawText(actual->nombreTanda, 100, yPos, 20, BLACK);

            // Verificar si la tanda está seleccionada
            if (tandaSeleccionadaId == actual->idTanda)
            {
                // Si está seleccionada, mostrar la informacion detallada de la tanda horizontalmente

                yPos += 30; // Desplazar hacia abajo para mostrar la informacion horizontalmente
                char montoAportadoText[50];
                snprintf(montoAportadoText, sizeof(montoAportadoText), "Monto Aportado: $%d", actual->montoAportado);
                DrawText(montoAportadoText, 100, yPos, 20, BLACK);

                char totalSemanalText[50];
                snprintf(totalSemanalText, sizeof(totalSemanalText), "Total Semanal: $%d", actual->totalSemanal);
                DrawText(totalSemanalText, 350, yPos, 20, BLACK);

                char fechasText[100];
                snprintf(fechasText, sizeof(fechasText), "Fechas: %s - %s", actual->fechaInicio, actual->fechaFin);
                DrawText(fechasText, 600, yPos, 20, BLACK);

                // Mostrar cantidad de participantes unidos vs total
                yPos += 30;
                FILE *archivo = fopen("participantes.txt", "r");
                int participantesUnidos = 0;
                if (archivo != NULL)
                {
                    char nombre[50];
                    int idTandaArchivo;
                    while (fscanf(archivo, "%d %49s\n", &idTandaArchivo, nombre) == 2)
                    {
                        if (idTandaArchivo == actual->idTanda)
                        {
                            participantesUnidos++;
                        }
                    }
                    fclose(archivo);
                }

                snprintf(participantesText, sizeof(participantesText), "%d/%d Participantes", participantesUnidos, actual->totalParticipantes);
                DrawText(participantesText, 100, yPos, 20, BLACK);
            }

            yPos += 80; // Espacio entre tandas
            actual = actual->siguiente;
        }

        // Poner un cuadro de texto para el nombre del participante
        static char nombreParticipante[50] = "";
        int inputNombreX = (GetScreenWidth() - 300) / 2;
        int inputNombreY = GetScreenHeight() - 140;
        Rectangle inputNombreP = {inputNombreX, inputNombreY, 300, 40};
        DrawText("Nombre del Participante", inputNombreX, inputNombreY - 20, 20, BLACK);
        DrawRectangleRec(inputNombreP, GRAY);
        DrawText(nombreParticipante, inputNombreP.x + 10, inputNombreP.y + 10, 20, BLACK);

        // Logica para capturar el nombre del participante
        if (IsKeyPressed(KEY_BACKSPACE) && strlen(nombreParticipante) > 0)
        {
            nombreParticipante[strlen(nombreParticipante) - 1] = '\0';
        }
        else
        {
            char c = GetCharPressed();
            while (c > 0)
            {
                if (strlen(nombreParticipante) < 49)
                {
                    nombreParticipante[strlen(nombreParticipante)] = c;
                }
                c = GetCharPressed();
            }
        }

        // Al hacer clic sobre una tanda, se selecciona
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            // Verificar cuál tanda ha sido seleccionada
            NodoTanda *tandaSeleccionada = listaTandas;
            yPos = 60; // Reajustar yPos al inicio de la lista de tandas
            while (tandaSeleccionada != NULL)
            {
                // Definir el área de la tanda para la seleccion
                Rectangle tandaRect = {100, yPos, 300, 40};
                if (CheckCollisionPointRec(GetMousePosition(), tandaRect))
                {
                    // Si la tanda ya está seleccionada, la deseleccionamos
                    if (tandaSeleccionadaId == tandaSeleccionada->idTanda)
                    {
                        tandaSeleccionadaId = -1; // Deseleccionar
                    }
                    else
                    {
                        tandaSeleccionadaId = tandaSeleccionada->idTanda; // Seleccionar
                    }
                    break;
                }
                yPos += 80; // Espacio entre tandas
                tandaSeleccionada = tandaSeleccionada->siguiente;
            }
        }

        // Boton para unirse a la tanda (en la parte inferior derecha)
        int botonX = GetScreenWidth() - 250;
        int botonY = GetScreenHeight() - 90;
        Rectangle botonUnirse = {botonX, botonY, 200, 40};
        DrawRectangleRec(botonUnirse, DARKGREEN);
        DrawText("Unirse a la Tanda", botonUnirse.x + 20, botonUnirse.y + 10, 20, WHITE);

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), botonUnirse))
        {
            // Validar si el nombre del participante está vacío
            if (strlen(nombreParticipante) == 0)
            {
                snprintf(mensaje, sizeof(mensaje), "¡Por favor ingresa tu nombre!");
                mensajeMostrado = true; // Mostrar mensaje de error
            }
            else
            {
                // Intentar unirse a la tanda seleccionada
                NodoTanda *tandaSeleccionada = listaTandas;
                while (tandaSeleccionada != NULL)
                {
                    if (tandaSeleccionada->idTanda == tandaSeleccionadaId)
                    {
                        if (agregarParticipanteATanda(tandaSeleccionada->idTanda, nombreParticipante, "participantes.txt"))
                        {
                            // Mensaje de exito
                            snprintf(mensaje, sizeof(mensaje), "¡Te has unido a la tanda: %s!", tandaSeleccionada->nombreTanda);
                            mensajeMostrado = true;
                        }
                        else
                        {
                            // Mensaje de error (ya estás unido a esta tanda)
                            snprintf(mensaje, sizeof(mensaje), "¡Ya estás unido a esta tanda!");
                            mensajeMostrado = true;
                        }
                        break;
                    }
                    tandaSeleccionada = tandaSeleccionada->siguiente;
                }
            }
        }

        // Mostrar mensaje si hay algún mensaje (debajo del cuadro de texto del nombre)
        if (mensajeMostrado)
        {
            // Mostrar el mensaje justo debajo del cuadro de texto del nombre
            DrawText(mensaje, inputNombreX, inputNombreY + 50, 20, RED);
        }

        break;

    case SCREEN_TANDAS_VER:
        DrawText("Mis Tandas", 150, 20, 20, BLACK);
        break;
    case SCREEN_AHORROS_VER:
        DrawTexture(OPC2, 0, 0, WHITE);
        DrawText("MIS AHORROS", 150, 20, 20, BLACK);

        int totalAhorros = 0;
        int yOffset = 60;
        int xDesglose = 100;

        if (listaAhorros == NULL)
        {
            DrawText("Ahorros: $ 0.00 MXN", xDesglose, yOffset, 20, RED);
        }
        else
        {
            NodoAhorro *actual = listaAhorros;
            while (actual != NULL)
            {
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
        DrawTexture(OPC2, 0, 0, WHITE);
        DrawText("MIS AHORROS", 150, 20, 20, BLACK);
        DrawText("AGREGAR AHORRO", 150, 100, 20, BLACK);

        Rectangle inputBox = {100, 200, 300, 50};
        DrawRectangleRec(inputBox, LIGHTGRAY);
        DrawRectangleLines(inputBox.x, inputBox.y, inputBox.width, inputBox.height, DARKGRAY);

        DrawText(inputBuffer, inputBox.x + 10, inputBox.y + 15, 20, DARKBLUE);

        DrawText("Ingrese una cantidad para transferir:", 100, 150, 20, BLACK);

        if (IsKeyPressed(KEY_BACKSPACE) && inputPos > 0)
        {
            inputBuffer[--inputPos] = '\0';
        }

        for (int key = KEY_ZERO; key <= KEY_NINE; key++)
        {
            if (IsKeyPressed(key) && inputPos < 4)
            {
                inputBuffer[inputPos++] = '0' + (key - KEY_ZERO);
                inputBuffer[inputPos] = '\0';
            }
        }

        for (int key = KEY_KP_0; key <= KEY_KP_9; key++)
        {
            if (IsKeyPressed(key) && inputPos < 4)
            {
                inputBuffer[inputPos++] = '0' + (key - KEY_KP_0);
                inputBuffer[inputPos] = '\0';
            }
        }

        Boton botonAhorrar = {{420, 200, 150, 50}, BLUE, "Ahorrar", NULL};
        if (CheckCollisionPointRec(GetMousePosition(), botonAhorrar.rec))
        {
            botonAhorrar.color = GRAY;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                int cantidad = atoi(inputBuffer);
                if (cantidad > 0 && cantidad < 9999)
                {
                    char fecha[11];
                    obtenerFecha(fecha, sizeof(fecha));

                    bool creado;
                    FILE *archivo = manejoArchivos("ahorros.txt", "a", &creado);
                    if (archivo != NULL)
                    {
                        agregarRegistroAhorro(id_global, usuario_global, cantidad, fecha, "ahorros.txt");
                        snprintf(mensaje, sizeof(mensaje), "Cantidad %d ingresada a tu ahorro!", cantidad);
                        registroExitoso = true;
                    }
                    else
                    {
                        snprintf(mensaje, sizeof(mensaje), "Error al abrir o crear el archivo.");
                    }
                }
                else
                {
                    snprintf(mensaje, sizeof(mensaje), "Cantidad invalida, debe ser entre 1 y 9999.");
                }

                inputBuffer[0] = '\0';
                inputPos = 0;
            }
        }
        DrawRectangleRec(botonAhorrar.rec, botonAhorrar.color);
        DrawText(botonAhorrar.text, botonAhorrar.rec.x + 10, botonAhorrar.rec.y + 15, 20, BLACK);

        if (registroExitoso)
        {
            DrawText(mensaje, 100, 300, 20, verdeEsmeralda);
        }
        else
        {
            DrawText(mensaje, 100, 300, 20, RED);
        }
        break;

    case SCREEN_AHORROS_RETIRAR:
        DrawTexture(OPC3, 0, 0, WHITE);
        DrawText("MIS AHORROS", 150, 20, 20, BLACK);
        DrawText("RETIRAR AHORRO", 150, 100, 20, BLACK);

        if (listaAhorros == NULL)
        {
            DrawText("No tienes ahorros disponibles para retirar.", 150, 200, 20, RED);
        }
        else
        {
            NodoAhorro *actual = listaAhorros;
            int ahorroRealizado = 0;
            float rendimientosGenerados = 0;

            while (actual != NULL)
            {
                ahorroRealizado += actual->cantidad;

                // Fecha manual
                int dia, mes, anio;
                if (sscanf(actual->fecha, "%d/%d/%d", &dia, &mes, &anio) == 3)
                {
                    struct tm fechaStruct = {0};
                    fechaStruct.tm_mday = dia;
                    fechaStruct.tm_mon = mes - 1;      // tm_mon va de 0 a 11
                    fechaStruct.tm_year = anio - 1900; // tm_year es desde 1900

                    time_t tFechaAhorro = mktime(&fechaStruct);
                    time_t tHoy = time(NULL);

                    int diasTranscurridos = (tHoy - tFechaAhorro) / (60 * 60 * 24);

                    // Rendimiento
                    float rendimientoBase = actual->cantidad * 0.01f;
                    float interesGanado = rendimientoBase * ((float)diasTranscurridos / 30.0f);
                    if (interesGanado < 0)
                        interesGanado = 0;

                    rendimientosGenerados += interesGanado;
                }
                else
                {
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
            if (CheckCollisionPointRec(GetMousePosition(), botonRetirar))
            {
                botonColor = GRAY;
            }

            DrawRectangleRec(botonRetirar, botonColor);
            Vector2 textSize = MeasureTextEx(GetFontDefault(), "RETIRAR", 20, 0);
            DrawText("RETIRAR",
                     botonRetirar.x + (botonRetirar.width - textSize.x) / 2,
                     botonRetirar.y + (botonRetirar.height - textSize.y) / 2,
                     20, BLACK);

            if (CheckCollisionPointRec(GetMousePosition(), botonRetirar) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                NodoAhorro *temp;
                while (listaAhorros != NULL)
                {
                    temp = listaAhorros;
                    listaAhorros = listaAhorros->siguiente;
                    free(temp);
                }

                FILE *archivo = fopen("ahorros.txt", "w");
                if (archivo != NULL)
                    fclose(archivo);

                DrawText("Todos los ahorros fueron retirados exitosamente.", 150, 500, 20, verdeEsmeralda);
            }
        }
        break;

    default:
        break;
    }
}

// FUNCIONES DE MANEJO DE BOTONES

void actualizarClicReciente()
{
    if (clicReciente)
    {
        tiempoEspera -= GetFrameTime();
        if (tiempoEspera <= 0.0f)
        {
            clicReciente = false;
        }
    }
}

void dibujarBotones(Boton botones[], int cantidad)
{
    int padding = 20;
    int totalHeight = (cantidad * 50) + (padding * (cantidad - 1));
    int startY = (GetScreenHeight() - totalHeight) / 2;

    for (int i = 0; i < cantidad; i++)
    {
        botones[i].rec.y = startY + i * (50 + padding);
        botones[i].rec.x = (GetScreenWidth() - botones[i].rec.width) / 2;

        if (CheckCollisionPointRec(GetMousePosition(), botones[i].rec))
        {
            botones[i].color = ColorBotonesOscuro;
        }
        else
        {
            botones[i].color = ColorBotones;
        }

        DrawRectangleRec(botones[i].rec, botones[i].color);

        Vector2 textSize = MeasureTextEx(GetFontDefault(), botones[i].text, 20, 0);
        DrawText(botones[i].text,
                 botones[i].rec.x + (botones[i].rec.width - textSize.x) / 2,
                 botones[i].rec.y + (botones[i].rec.height - textSize.y) / 2,
                 20, BLACK);
    }
}

void verificarBotones(Boton botones[], int cantidad)
{
    if (clicReciente)
        return;

    for (int i = 0; i < cantidad; i++)
    {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), botones[i].rec))
        {
            botones[i].accion();
            clicReciente = true;
            tiempoEspera = 0.2f;
            break;
        }
    }
}

void verificarBoton(Boton *boton)
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), boton->rec))
    {
        boton->accion();
    }
}

void dibujarBoton(Boton *boton)
{
    if (CheckCollisionPointRec(GetMousePosition(), boton->rec))
    {
        boton->color = GRAY;
    }
    else
    {
        boton->color = BLUE;
    }

    DrawRectangleRec(boton->rec, boton->color);

    Vector2 textSize = MeasureTextEx(GetFontDefault(), boton->text, 20, 0);
    DrawText(boton->text,
             boton->rec.x + (boton->rec.width - textSize.x) / 2,
             boton->rec.y + (boton->rec.height - textSize.y) / 2,
             20, BLACK);
}
void dibujarBotonAtras(Boton *boton)
{

    DrawRectangleRec(boton->rec, boton->color);

    // Mide el tamaño del texto
    Vector2 textSize = MeasureTextEx(GetFontDefault(), boton->text, 20, 0);

    // Calcula la posicion para centrar el texto dentro del boton
    float x = boton->rec.x + (boton->rec.width - textSize.x) / 2;
    float y = boton->rec.y + (boton->rec.height - textSize.y) / 2;

    // Dibuja el texto centrado
    DrawText(boton->text, x, y, 20, BLACK);
}

void verificarBotonRegresar(Boton *boton)
{
    if (CheckCollisionPointRec(GetMousePosition(), boton->rec) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {

        boton->accion();
    }
}

void regresarAccion()
{

    if (currentScreen == SCREEN_TANDAS_CREAR)
    {                                            // Aqui estas
        currentScreen = SCREEN_TANDAS_PRINCIPAL; // Aqui vas
    }

    else if (currentScreen == SCREEN_TANDAS_UNIR)
    {
        currentScreen = SCREEN_TANDAS_PRINCIPAL;
    }

    else if (currentScreen == SCREEN_TANDAS_VER)
    {
        currentScreen = SCREEN_TANDAS_PRINCIPAL;
    }

    else if (currentScreen == SCREEN_AHORROS_VER)
    {
        currentScreen = SCREEN_AHORROS_PRINCIPAL;
    }

    else if (currentScreen == SCREEN_AHORROS_RETIRAR)
    {
        currentScreen = SCREEN_AHORROS_PRINCIPAL;
    }

    else if (currentScreen == SCREEN_AHORROS_AHORRAR)
    {
        currentScreen = SCREEN_AHORROS_PRINCIPAL;
    }
    else if (currentScreen == SCREEN_PRESTAMOS_SOLICITAR)
    {
        currentScreen = SCREEN_PRESTAMOS_PRINCIPAL;
    }
    else if (currentScreen == SCREEN_PRESTAMOS_VER)
    {
        currentScreen = SCREEN_PRESTAMOS_PRINCIPAL;
    }
}

// FUNCIONES DE MANEJO DE ACCIONES EN BOTONES

void accionVerAhorros()
{
    currentScreen = SCREEN_AHORROS_VER;
}

void accionAhorrar()
{
    currentScreen = SCREEN_AHORROS_AHORRAR;
}

void accionRetirar()
{
    currentScreen = SCREEN_AHORROS_RETIRAR;
}

void accionAhorrosPrincipal()
{
    currentScreen = SCREEN_AHORROS_PRINCIPAL;
}

void accionTandasPrincipal()
{
    currentScreen = SCREEN_TANDAS_PRINCIPAL;
}

void accionTandasCrear()
{
    currentScreen = SCREEN_TANDAS_CREAR;
}

void accionTandasUnir()
{
    currentScreen = SCREEN_TANDAS_UNIR;
}

void accionTandasVer()
{
    currentScreen = SCREEN_TANDAS_VER;
}

void accionMenu()
{
    clicReciente = false;
    currentScreen = SCREEN_MENU;
}

void accionPrestamosPrincipal()
{
    currentScreen = SCREEN_PRESTAMOS_PRINCIPAL;
}

// FUNCIONES DE ACCION PARA PRESTAMOS
void accionPrestamosSolicitar(void)
{
    currentScreen = SCREEN_PRESTAMOS_SOLICITAR;
}

void accionPrestamosVer(void)
{
    currentScreen = SCREEN_PRESTAMOS_VER;
}

// FUNCIONES DE MANEJO DE ARCHIVOS

FILE *manejoArchivos(const char *nombreArchivo, const char *modo, bool *archivoCreado)
{
    FILE *archivo = fopen(nombreArchivo, modo);

    if (archivo == NULL)
    {
        archivo = fopen(nombreArchivo, "a+");
        if (archivo != NULL && archivoCreado != NULL)
        {
            *archivoCreado = true;
        }
    }
    else
    {
        if (archivoCreado != NULL)
        {
            *archivoCreado = false;
        }
    }

    return archivo;
}

void cargarTandasDesdeArchivo(const char *nombreArchivo)
{
    bool archivoCreado;
    FILE *archivo = manejoArchivos(nombreArchivo, "r", &archivoCreado);

    // Si no se pudo abrir el archivo, terminamos
    if (archivo == NULL)
    {
        return;
    }

    while (!feof(archivo))
    {
        NodoTanda *nuevoNodo = (NodoTanda *)malloc(sizeof(NodoTanda));

        if (fscanf(archivo, "%d %49s %d %d %d %s %s\n",
                   &nuevoNodo->idTanda,
                   nuevoNodo->nombreTanda,
                   &nuevoNodo->montoAportado,
                   &nuevoNodo->totalParticipantes,
                   &nuevoNodo->totalSemanal,
                   nuevoNodo->fechaInicio,
                   nuevoNodo->fechaFin) == 7)
        {
            // Agregar a la lista de tandas
            nuevoNodo->siguiente = listaTandas;
            listaTandas = nuevoNodo;
        }
        else
        {
            // Si el formato no es correcto, liberamos el nodo creado
            free(nuevoNodo);
        }
    }

    fclose(archivo);
}

void cargarAhorrosDesdeArchivo(const char *nombreArchivo)
{
    bool archivoCreado;
    FILE *archivo = manejoArchivos(nombreArchivo, "r", &archivoCreado);

    if (archivo == NULL)
    {
        return;
    }

    while (!feof(archivo))
    {
        NodoAhorro *nuevoNodo = (NodoAhorro *)malloc(sizeof(NodoAhorro));
        if (fscanf(archivo, "%d %s %d %s\n",
                   &nuevoNodo->idUsuario,
                   nuevoNodo->nombreUsuario,
                   &nuevoNodo->cantidad,
                   nuevoNodo->fecha) == 4)
        {
            nuevoNodo->siguiente = listaAhorros;
            listaAhorros = nuevoNodo;
        }
        else
        {
            free(nuevoNodo);
        }
    }

    fclose(archivo);
}

void agregarRegistroAhorro(int idUsuario, const char *nombreUsuario, int cantidad, const char *fecha, const char *nombreArchivo)
{
    NodoAhorro *nuevoNodo = (NodoAhorro *)malloc(sizeof(NodoAhorro));
    nuevoNodo->idUsuario = idUsuario;
    snprintf(nuevoNodo->nombreUsuario, sizeof(nuevoNodo->nombreUsuario), "%s", nombreUsuario);
    nuevoNodo->cantidad = cantidad;
    snprintf(nuevoNodo->fecha, sizeof(nuevoNodo->fecha), "%s", fecha);
    nuevoNodo->siguiente = listaAhorros;

    listaAhorros = nuevoNodo;

    bool archivoCreado;
    FILE *archivo = manejoArchivos(nombreArchivo, "a", &archivoCreado);
    if (archivo != NULL)
    {
        fprintf(archivo, "%d %s %d %s\n", idUsuario, nombreUsuario, cantidad, fecha);
        fclose(archivo);
    }
}

void agregarRegistroTanda(int idTanda, const char *nombreTanda, int montoAportado, int totalParticipantes, const char *fechaInicio, const char *fechaFin, const char *nombreArchivo)
{

    int totalSemanal = montoAportado * totalParticipantes;
    // Crear el nuevo nodo
    NodoTanda *nuevoNodo = (NodoTanda *)malloc(sizeof(NodoTanda));
    nuevoNodo->idTanda = idTanda;
    snprintf(nuevoNodo->nombreTanda, sizeof(nuevoNodo->nombreTanda), "%s", nombreTanda);
    nuevoNodo->montoAportado = montoAportado;
    nuevoNodo->totalParticipantes = totalParticipantes;
    nuevoNodo->totalSemanal = totalSemanal;
    snprintf(nuevoNodo->fechaInicio, sizeof(nuevoNodo->fechaInicio), "%s", fechaInicio);
    snprintf(nuevoNodo->fechaFin, sizeof(nuevoNodo->fechaFin), "%s", fechaFin);
    nuevoNodo->siguiente = listaTandas;

    // Insertar el nuevo nodo al principio de la lista
    listaTandas = nuevoNodo;

    bool archivoCreado;
    FILE *archivo = manejoArchivos(nombreArchivo, "a", &archivoCreado);
    if (archivo != NULL)
    {

        fprintf(archivo, "%d %s %d %d %d %s %s\n", idTanda, nombreTanda, montoAportado,
                totalParticipantes, totalSemanal, fechaInicio, fechaFin);
        fclose(archivo);
    }
}

bool agregarParticipanteATanda(int idTanda, const char *nombreParticipante, const char *nombreArchivo)
{
    FILE *archivo = manejoArchivos(nombreArchivo, "r", NULL);
    if (archivo == NULL)
    {
        return false; // Error al abrir el archivo
    }

    // Verificar si el participante ya está registrado en la tanda
    char linea[256];
    while (fgets(linea, sizeof(linea), archivo))
    {
        int idTandaArchivo;
        char nombreParticipanteArchivo[50];
        sscanf(linea, "%d %49s", &idTandaArchivo, nombreParticipanteArchivo);

        if (idTandaArchivo == idTanda && strcmp(nombreParticipante, nombreParticipanteArchivo) == 0)
        {
            fclose(archivo);
            return false; // El participante ya está unido a esta tanda
        }
    }
    fclose(archivo);

    // Si no está unido, agregar al archivo
    archivo = manejoArchivos(nombreArchivo, "a", NULL);
    if (archivo != NULL)
    {
        fprintf(archivo, "%d %s\n", idTanda, nombreParticipante);
        fclose(archivo);
        return true;
    }

    return false; // Error al agregar al archivo
}

// FUNCIONES MANEJO DE PRESTAMOS
void inicializarPrestamos(void)
{
    cargarPrestamos();
}

void actualizarPantallaPrestamos(void)
{
    Vector2 mousePoint = GetMousePosition();

    // Manejo de entrada de texto
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        if (CheckCollisionPointRec(mousePoint, inputCantidad))
            campoActivo = 1;
        else if (CheckCollisionPointRec(mousePoint, inputPlazo))
            campoActivo = 2;
        else if (CheckCollisionPointRec(mousePoint, inputTarjeta))
            campoActivo = 3;
        else
            campoActivo = 0;
    }

    // Procesar entrada de texto según el campo activo
    if (campoActivo > 0)
    {
        int key = GetCharPressed();
        char *buffer;
        int maxLen;

        switch (campoActivo)
        {
        case 1:
            buffer = bufferCantidad;
            maxLen = 9;
            break;
        case 2:
            buffer = bufferPlazo;
            maxLen = 2;
            break;
        case 3:
            buffer = bufferTarjeta;
            maxLen = 16;
            break;
        default:
            return;
        }

        while (key > 0)
        {
            if ((key >= '0' && key <= '9') && strlen(buffer) < maxLen)
            {
                int len = strlen(buffer);
                buffer[len] = (char)key;
                buffer[len + 1] = '\0';
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && strlen(buffer) > 0)
        {
            buffer[strlen(buffer) - 1] = '\0';
        }
    }

    // Procesar boton de solicitud
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
        CheckCollisionPointRec(mousePoint, botonSolicitar) &&
        !prestamoActivo)
    {
        float cantidad = atof(bufferCantidad);
        int plazo = atoi(bufferPlazo);

        if (cantidad > 0 && plazo > 0 && strlen(bufferTarjeta) == 16)
        {
            ProcesarSolicitudPrestamos(cantidad, plazo, bufferTarjeta, interes_activo);
            // Limpiar buffers
            memset(bufferCantidad, 0, sizeof(bufferCantidad));
            memset(bufferPlazo, 0, sizeof(bufferPlazo));
            memset(bufferTarjeta, 0, sizeof(bufferTarjeta));
        }
        else
        {
            strcpy(mensajePrestamo, "Por favor complete todos los campos correctamente");
        }
    }
}

void dibujarPantallaPrestamos(void)
{
    // Dibujar campos de entrada
    DrawRectangleRec(inputCantidad, LIGHTGRAY);
    DrawRectangleRec(inputPlazo, LIGHTGRAY);
    DrawRectangleRec(inputTarjeta, LIGHTGRAY);

    DrawText("Cantidad del prestamo:", 100, 120, 20, BLACK);
    DrawText("Plazo (meses):", 100, 220, 20, BLACK);
    DrawText("Número de tarjeta:", 100, 320, 20, BLACK);

    DrawText(bufferCantidad, inputCantidad.x + 10, inputCantidad.y + 10, 20, BLACK);
    DrawText(bufferPlazo, inputPlazo.x + 10, inputPlazo.y + 10, 20, BLACK);
    DrawText(bufferTarjeta, inputTarjeta.x + 10, inputTarjeta.y + 10, 20, BLACK);

    // Dibujar botones
    Color colorBoton = prestamoActivo ? GRAY : BLUE;
    DrawRectangleRec(botonSolicitar, colorBoton);
    DrawText("Solicitar Prestamo", botonSolicitar.x + 10, botonSolicitar.y + 15, 20, WHITE);

    // Dibujar toggle para interés
    Rectangle toggleRect = {100, 400, 30, 30};
    DrawRectangleRec(toggleRect, interes_activo ? GREEN : RED);
    DrawText(interes_activo ? "Interés ON" : "Interés OFF", 140, 405, 20, BLACK);

    // Manejar clic en el toggle
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), toggleRect))
    {
        interes_activo = !interes_activo;
    }

    // Mostrar prestamo activo si existe
    if (prestamoActivo && listaPrestamos != NULL)
    {
        char infoPrestamo[256];
        snprintf(infoPrestamo, sizeof(infoPrestamo),
                 "Prestamo activo:\nCantidad: $%.2f\nPendiente: $%.2f\nPago mensual: $%.2f\nProximo pago: %s",
                 listaPrestamos->cantidadSolicitada,
                 listaPrestamos->cantidadPendiente,
                 listaPrestamos->pagoMensual,
                 listaPrestamos->fechaproximopago);
        DrawText(infoPrestamo, 500, 150, 20, BLACK);
    }

    // Mostrar mensaje de estado
    if (strlen(mensajePrestamo) > 0)
    {
        DrawText(mensajePrestamo, 100, 550, 20, RED);
    }
}

void ProcesarSolicitudPrestamos(float cantidad, int plazo, const char *numeroTarjeta, bool interes_activo)
{
    if (prestamoActivo)
    {
        strcpy(mensajePrestamo, "Ya tiene un préstamo activo. Debe pagarlo completamente antes de solicitar uno nuevo.");
        return;
    }

    NodoPrestamo *nuevoPrestamo = (NodoPrestamo *)malloc(sizeof(NodoPrestamo));
    if (nuevoPrestamo == NULL)
    {
        strcpy(mensajePrestamo, "Error al procesar la solicitud");
        return;
    }

    // Configurar el nuevo préstamo
    nuevoPrestamo->idPrestamo = rand() % 10000;
    nuevoPrestamo->idUsuario = 1;                    // Usuario global
    strcpy(nuevoPrestamo->nombreUsuario, "Usuario"); // Nombre global
    nuevoPrestamo->cantidadSolicitada = cantidad;
    nuevoPrestamo->cantidadPendiente = cantidad;
    nuevoPrestamo->plazosMeses = plazo;
    nuevoPrestamo->pagoMensual = calcularPagoMensual(cantidad, plazo, interes_activo);
    nuevoPrestamo->activo = true;

    // Obtener fecha actual
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    strftime(nuevoPrestamo->fechaSolicitud, sizeof(nuevoPrestamo->fechaSolicitud),
             "%d/%m/%Y", &tm);

    // Calcular fecha del próximo pago (un mes después)
    tm.tm_mon += 1;
    mktime(&tm); // Normalizar la fecha
    strftime(nuevoPrestamo->fechaproximopago, sizeof(nuevoPrestamo->fechaproximopago),
             "%d/%m/%Y", &tm);

    // Agregar a la lista y actualizar estado
    nuevoPrestamo->siguiente = listaPrestamos;
    listaPrestamos = nuevoPrestamo;
    prestamoActivo = true;

    guardarPrestamos();
    strcpy(mensajePrestamo, "Préstamo aprobado exitosamente");
}

void realizarPagoPrestamo(int idPrestamo, float cantidad, const char *numeroTarjeta)
{
    NodoPrestamo *prestamo = listaPrestamos;
    while (prestamo != NULL)
    {
        if (prestamo->idPrestamo == idPrestamo)
        {
            float cantidadAPagar = fmin(cantidad, prestamo->cantidadPendiente);
            prestamo->cantidadPendiente -= cantidadAPagar;

            // Obtener la fecha actual para el registro del pago
            char fechaActual[11];
            obtenerFecha(fechaActual, sizeof(fechaActual));

            // Registrar el pago con la fecha actual
            registrarPagoEnArchivo(idPrestamo, cantidadAPagar, fechaActual, prestamo->fechaproximopago);

            // Actualizar fecha de próximo pago solo si aún hay pagos pendientes
            if (prestamo->cantidadPendiente > 0)
            {
                struct tm fechaProximoPago;
                memset(&fechaProximoPago, 0, sizeof(fechaProximoPago));

                if (!custom_strptime(prestamo->fechaproximopago, "%d/%m/%Y", &fechaProximoPago))
                {
                    printf("Error al analizar la fecha.\n");
                    return;
                }

                fechaProximoPago.tm_mon += 1;
                if (fechaProximoPago.tm_mon > 11)
                {
                    fechaProximoPago.tm_mon = 0;
                    fechaProximoPago.tm_year += 1;
                }

                strftime(prestamo->fechaproximopago, sizeof(prestamo->fechaproximopago), "%d/%m/%Y", &fechaProximoPago);
            }
            else
            {
                // Si ya no hay pagos pendientes, marcar como inactivo y no actualizar la fecha
                prestamo->activo = false;
                prestamoActivo = false; // actualizar estado global
                strcpy(prestamo->fechaproximopago, "Pagado");
            }

            guardarPrestamos();
            printf("Pago de $%.2f realizado con éxito. Pendiente: $%.2f\n", cantidadAPagar, prestamo->cantidadPendiente);
            return;
        }
        prestamo = prestamo->siguiente;
    }

    printf("Error: Préstamo no encontrado.\n");
}

void registrarPagoEnArchivo(int idPrestamo, float cantidad, const char *fechaPago, const char *fechaProximoPago)
{
    static int ultimoIdPrestamo = -1;
    static int numeroPago = 1;

    if (idPrestamo != ultimoIdPrestamo)
    {
        numeroPago = 1;
        ultimoIdPrestamo = idPrestamo;
    }

    FILE *archivo = fopen("historial_pagos_prestamos.txt", "a");
    if (archivo != NULL)
    {
        fprintf(archivo, "Préstamo ID: %d - Pago %d: $%.2f - Fecha de pago: %s - Próximo pago: %s\n",
                idPrestamo, numeroPago, cantidad, fechaPago, fechaProximoPago);

        fclose(archivo);
        numeroPago++;
    }
    else
    {
        printf("Error: No se pudo abrir el archivo de historial de pagos.\n");
    }
}

void obtenerFecha_prestamo(char *fecha, size_t tamano)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    strftime(fecha, tamano, "%d/%m/%Y", &tm);
}

float calcularPagoMensual(float cantidad, int plazo, bool interes_activo)
{
    // Lógica para calcular el pago mensual (simplificada para este ejemplo)
    float interes = interes_activo ? 0.05 : 0.0; // 5% de interés si está activo
    return (cantidad * (1 + interes)) / plazo;
}

bool cargarPrestamos(void)
{
    FILE *archivo = fopen("prestamos.txt", "r");
    if (archivo == NULL)
    {
        printf("Advertencia: No se encontró el archivo de préstamos. Se usará una lista vacía.\n");
        return false;
    }

    while (!feof(archivo))
    {
        NodoPrestamo *nuevo = (NodoPrestamo *)malloc(sizeof(NodoPrestamo));
        if (nuevo == NULL)
        {
            printf("Error: No se pudo asignar memoria para un préstamo.\n");
            fclose(archivo);
            return false;
        }

        int activoTemp; // Variable temporal para almacenar el valor de 'activo'
        if (fscanf(archivo, "%d %d %s %f %f %d %f %s %s %d\n",
                   &nuevo->idPrestamo,
                   &nuevo->idUsuario,
                   nuevo->nombreUsuario,
                   &nuevo->cantidadSolicitada,
                   &nuevo->cantidadPendiente,
                   &nuevo->plazosMeses,
                   &nuevo->pagoMensual,
                   nuevo->fechaSolicitud,
                   nuevo->fechaproximopago,
                   &activoTemp) == 10)
        {
            nuevo->activo = activoTemp;
            nuevo->siguiente = listaPrestamos;
            listaPrestamos = nuevo;

            if (nuevo->activo)
                prestamoActivo = true;
        }
        else
        {
            free(nuevo); // Liberar memoria en caso de error de lectura
            printf("Advertencia: Datos corruptos encontrados en el archivo de préstamos.\n");
        }
    }

    fclose(archivo);
    return true;
}

void guardarPrestamos(void)
{
    FILE *archivo = fopen("prestamos.txt", "w");
    if (archivo == NULL)
    {
        printf("Error: No se pudo abrir el archivo para guardar préstamos.\n");
        return;
    }

    NodoPrestamo *actual = listaPrestamos;
    while (actual != NULL)
    {
        if (fprintf(archivo, "%d %d %s %f %f %d %f %s %s %d\n",
                    actual->idPrestamo,
                    actual->idUsuario,
                    actual->nombreUsuario,
                    actual->cantidadSolicitada,
                    actual->cantidadPendiente,
                    actual->plazosMeses,
                    actual->pagoMensual,
                    actual->fechaSolicitud,
                    actual->fechaproximopago,
                    actual->activo) < 0)
        {
            printf("Error: No se pudo escribir en el archivo de préstamos.\n");
            fclose(archivo);
            return;
        }
        actual = actual->siguiente;
    }

    fclose(archivo);
}

bool custom_strptime(const char *cadena, const char *formato, struct tm *fecha)
{
    // Asumiendo que el formato es "dd/mm/yyyy"
    if (sscanf(cadena, "%d/%d/%d", &fecha->tm_mday, &fecha->tm_mon, &fecha->tm_year) == 3)
    {
        fecha->tm_mon -= 1;     // Ajustar el mes (0-11)
        fecha->tm_year -= 1900; // Ajustar el año (a partir de 1900)
        return true;            // Éxito
    }
    return false; // Error
}

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>


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
    int idTanda;           
    char nombreTanda[50];   
    int montoAportado;
    int totalParticipantes;
    int totalSemanal;
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
    SCREEN_PAGOS_VER,
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
int logoTimer = 200;
bool clicReciente = false;
float tiempoEspera = 0.0f;
NodoTanda *listaTandas = NULL;
Boton regresarBoton;

// Variables para las texturas
Texture2D LOGO;
Texture2D MENU;
Texture2D OPCS;
Texture2D OPC1;
Texture2D OPC2;
Texture2D OPC3;

// COLORES
Color verdeEsmeralda = {80, 200, 120, 255};
Color ColorBotones = {1,182,149,255};
Color ColorBotonesOscuro = {1, 140, 115, 255}; 
//
// P R O T O T I P O S  D E  F U N C I O N E S  
//

// FUNCIONES GENERALES
void obtenerFecha(char *buffer, size_t bufferSize);
void cargarTexturas();
void liberarTexturas();

// FUNCIONES DE MANEJO DE PANTALLAS

void actualizarScreens();
void dibujarScreens();

// FUNCIONES DE MANEJO DE BOTONES
void actualizarClicReciente();
void dibujarBotones(Boton botones[], int cantidad);
void verificarBotones(Boton botones[], int cantidad);
void dibujarBoton(Boton *boton);
void verificarBoton(Boton *boton);
void dibujarBotonAtras(Boton *boton);
void verificarBotonRegresar(Boton *boton);


// FUNCIONES DE MANEJO DE ACCIONES

void accionVerAhorros();
void accionAhorrar();
void accionRetirar();
void accionAhorrosPrincipal();
void accionTandasPrincipal();
void accionTandasCrear();
void accionTandasUnir();
void accionTandasVer();
void accionPagosVer();
void accionPrestamosPrincipal();
void accionMenu();
void regresarAccion();

// FUNCIONES DE MANEJO DE ARCHIVOS
FILE* manejoArchivos(const char *nombreArchivo, const char *modo, bool *archivoCreado);
void cargarAhorrosDesdeArchivo(const char *nombreArchivo);
void cargarTandasDesdeArchivo(const char *nombreArchivo);
void cargarParticipantesDesdeArchivo(const char *nombreArchivo);
void agregarRegistroAhorro(int idUsuario, const char *nombreUsuario, int cantidad, const char *fecha, const char *nombreArchivo);
void agregarRegistroTanda(int idTanda, const char *nombreTanda, int montoAportado, int totalParticipantes, const char *fechaInicio, const char *fechaFin, const char *nombreArchivo);
bool agregarParticipanteATanda(int idTanda, const char *nombreParticipante, const char *nombreArchivo);
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
    cargarTandasDesdeArchivo("tandas.txt");
    cargarTexturas();
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
    Boton botonesTandas[4] = {
            {{ 0, 0, 200, 50 }, verdeEsmeralda, "Crear Tanda", accionTandasCrear},
            {{ 0, 0, 200, 50 }, verdeEsmeralda, "Unirme a Tanda", accionTandasUnir},
            {{ 0, 0, 200, 50 }, verdeEsmeralda, "Mis Tandas", accionTandasVer},
            {{ 0, 0, 200, 50 }, verdeEsmeralda, "Historial de pagos", accionPagosVer}
            };

    // Definir el botón de regresar
    Boton regresarBoton = {{0, 0, 200, 50}, BLUE, "< Regresar", regresarAccion};

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
            verificarBotones(botonesTandas, 4);
            dibujarBotones(botonesTandas, 4);
        }
        if (currentScreen == SCREEN_AHORROS_PRINCIPAL || currentScreen == SCREEN_PRESTAMOS_PRINCIPAL || currentScreen == SCREEN_TANDAS_PRINCIPAL ) {
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

        // Verificar el botón de regresar cuando sea necesario
        if (currentScreen != SCREEN_MENU && currentScreen != SCREEN_LOGO && currentScreen != SCREEN_AHORROS_PRINCIPAL && currentScreen != SCREEN_TANDAS_PRINCIPAL &&currentScreen != SCREEN_PRESTAMOS_PRINCIPAL) { 
            regresarBoton.rec.x = 20;
            regresarBoton.rec.y = 600;
            verificarBotonRegresar(&regresarBoton);  
            dibujarBotonAtras(&regresarBoton);      
        }
        
        EndDrawing();
    }

    liberarTexturas();

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

void cargarTexturas() {
    LOGO = LoadTexture("images/LOGO.png");
    MENU = LoadTexture("images/MENU.png");
    OPCS = LoadTexture("images/OPCS.png");
    OPC1 = LoadTexture("images/OPC1.png");
    OPC2 = LoadTexture("images/OPC2.png");
    OPC3 = LoadTexture("images/OPC3.png");
}

void liberarTexturas() {
    UnloadTexture(LOGO);
    UnloadTexture(MENU);
    UnloadTexture(OPCS);
    UnloadTexture(OPC1);
    UnloadTexture(OPC2);
    UnloadTexture(OPC3);
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
            DrawTexture(LOGO, 0, 0, WHITE);
            break;
        case SCREEN_MENU:
            DrawTexture(MENU, 0, 0, WHITE);
            break;
        case SCREEN_AHORROS_PRINCIPAL:
            DrawTexture(OPCS, 0, 0, WHITE);
            DrawText("MIS AHORROS", 150, 20, 20, BLACK);
            break;
        case SCREEN_TANDAS_PRINCIPAL:
            DrawTexture(OPCS, 0, 0, WHITE);
            break;
        case SCREEN_PRESTAMOS_PRINCIPAL:
            DrawTexture(OPCS, 0, 0, WHITE);
            DrawText("MIS PRESTAMOS", 150, 20, 20, BLACK);
            break;
        case SCREEN_TANDAS_CREAR:
            DrawTexture(OPC1, 0, 0, WHITE);
            DrawText("Crear Tanda", 150, 20, 20, BLACK);
            // Definir las variables estáticas para los campos de entrada
            static char nombreTanda[50] = "";
            static char montoAportado[10] = "";
            static char totalParticipantes[5] = "";
            static char duracionSemanas[3] = "";  
            static char mensajeTanda[128] = "";
            
            // Definir las posiciones y tamaños de los cuadros de texto y el botón
            Rectangle inputNombre = {100, 100, 300, 40};
            Rectangle inputMonto = {100, 175, 300, 40};
            Rectangle inputParticipantes = {100, 250, 300, 40};
            Rectangle inputDuracion = {100, 325, 300, 40}; 
            Rectangle botonCrear = {150, 400, 200, 50};

            // Nombres de los campos sobre las cajas de texto
            DrawText("Nombre de la Tanda", 100, 75, 20, BLACK);
            DrawText("Monto Aportado (Semanal)", 100, 150, 20, BLACK);
            DrawText("Total Participantes", 100, 225, 20, BLACK);
            DrawText("Duración (Semanal)", 100, 300, 20, BLACK);

            // Lógica de interacción con los campos de texto
            static int posNombre = 0, posMonto = 0, posParticipantes = 0, posDuracion = 0;

            // Lógica de selección de los campos para ingreso
            if (CheckCollisionPointRec(GetMousePosition(), inputNombre) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                posNombre = 1;
                posMonto = posParticipantes = posDuracion = 0;
            } else if (CheckCollisionPointRec(GetMousePosition(), inputMonto) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                posMonto = 1;
                posNombre = posParticipantes = posDuracion = 0;
            } else if (CheckCollisionPointRec(GetMousePosition(), inputParticipantes) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                posParticipantes = 1;
                posNombre = posMonto = posDuracion = 0;
            } else if (CheckCollisionPointRec(GetMousePosition(), inputDuracion) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                posDuracion = 1;
                posNombre = posMonto = posParticipantes = 0;
            }

            // Entrada para el nombre de la tanda
            if (posNombre) {
                if (IsKeyPressed(KEY_BACKSPACE) && strlen(nombreTanda) > 0) {
                    nombreTanda[strlen(nombreTanda) - 1] = '\0';
                } else {
                    char c = GetCharPressed();
                    while (c > 0) {
                        if (strlen(nombreTanda) < 49) {
                            nombreTanda[strlen(nombreTanda)] = c;
                        }
                        c = GetCharPressed();
                    }
                }
            }

            // Entrada para el monto aportado (semanal)
            if (posMonto) {
                if (IsKeyPressed(KEY_BACKSPACE) && strlen(montoAportado) > 0) {
                    montoAportado[strlen(montoAportado) - 1] = '\0';
                } else {
                    char c = GetCharPressed();
                    while (c > 0) {
                        if (c >= '0' && c <= '9' && strlen(montoAportado) < 9) {
                            montoAportado[strlen(montoAportado)] = c;
                        }
                        c = GetCharPressed();
                    }
                }
            }

            // Entrada para el número de participantes
            if (posParticipantes) {
                if (IsKeyPressed(KEY_BACKSPACE) && strlen(totalParticipantes) > 0) {
                    totalParticipantes[strlen(totalParticipantes) - 1] = '\0';
                } else {
                    char c = GetCharPressed();
                    while (c > 0) {
                        if (c >= '0' && c <= '9' && strlen(totalParticipantes) < 4) {
                            totalParticipantes[strlen(totalParticipantes)] = c;
                        }
                        c = GetCharPressed();
                    }
                }
            }

            // Entrada para la duración en semanas
            if (posDuracion) {
                if (IsKeyPressed(KEY_BACKSPACE) && strlen(duracionSemanas) > 0) {
                    duracionSemanas[strlen(duracionSemanas) - 1] = '\0';
                } else {
                    char c = GetCharPressed();
                    while (c > 0) {
                        if (c >= '0' && c <= '9' && strlen(duracionSemanas) < 2) {
                            duracionSemanas[strlen(duracionSemanas)] = c;
                        }
                        c = GetCharPressed();
                    }
                }
            }

            // Dibuja los cuadros de texto (input boxes)
            DrawRectangleRec(inputNombre, GRAY);
            DrawRectangleRec(inputMonto, GRAY);
            DrawRectangleRec(inputParticipantes, GRAY);
            DrawRectangleRec(inputDuracion, GRAY);

            // Dibuja el texto dentro de los cuadros
            DrawText(nombreTanda, inputNombre.x + 10, inputNombre.y + 10, 20, BLACK);
            DrawText(montoAportado, inputMonto.x + 10, inputMonto.y + 10, 20, BLACK);
            DrawText(totalParticipantes, inputParticipantes.x + 10, inputParticipantes.y + 10, 20, BLACK);
            DrawText(duracionSemanas, inputDuracion.x + 10, inputDuracion.y + 10, 20, BLACK);
           
            // función para validar nombres duplicados
            bool existeTandaConNombre(const char *nombreTanda) {
                NodoTanda *actual = listaTandas;
                while (actual != NULL) {
                    if (strcmp(actual->nombreTanda, nombreTanda) == 0) {
                        return true; // El nombre ya existe
                    }
                    actual = actual->siguiente;
                }
                return false; // No se encontró una tanda con ese nombre
            }

            // Función para verificar si el nombre contiene espacios
            bool contieneEspacios(const char *nombre) {
                while (*nombre) {
                    if (*nombre == ' ') {
                        return true; // El nombre contiene espacios
                    }
                    nombre++;
                }
                return false; // El nombre no contiene espacios
            }

            // Botón Crear
            Color botonColor = BLUE;
            if (CheckCollisionPointRec(GetMousePosition(), botonCrear)) {
                botonColor = GRAY;
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    // Verificación de campos vacíos o inválidos
                    if (strlen(nombreTanda) > 0 && atoi(montoAportado) > 0 && atoi(totalParticipantes) > 0 && strlen(duracionSemanas) > 0) {
                        if (contieneEspacios(nombreTanda)) {
                            snprintf(mensajeTanda, sizeof(mensajeTanda), "El nombre no debe contener espacios.");
                        }
                        else if (existeTandaConNombre(nombreTanda)) {
                            snprintf(mensajeTanda, sizeof(mensajeTanda), "Ya existe una tanda con el nombre '%s'.", nombreTanda);
                        } 
                        else {
                            // Crear y configurar la nueva tanda
                            NodoTanda *nuevaTanda = (NodoTanda *)malloc(sizeof(NodoTanda));
                            nuevaTanda->idTanda = rand() % 10000; 
                            strcpy(nuevaTanda->nombreTanda, nombreTanda);
                            nuevaTanda->montoAportado = atoi(montoAportado);
                            nuevaTanda->totalParticipantes = atoi(totalParticipantes);

                            // Obtener la fecha de inicio (fecha actual)
                            time_t t = time(NULL);
                            struct tm tmFecha = *localtime(&t);
                            strftime(nuevaTanda->fechaInicio, sizeof(nuevaTanda->fechaInicio), "%Y-%m-%d", &tmFecha);

                            
                            int duracionEnDias = atoi(duracionSemanas) * 7; // Convertir semanas a días
                            tmFecha.tm_mday += duracionEnDias;
                            mktime(&tmFecha); // Normaliza la fecha
                            strftime(nuevaTanda->fechaFin, sizeof(nuevaTanda->fechaFin), "%Y-%m-%d", &tmFecha);

                            // Añadir a la lista de tandas
                            nuevaTanda->siguiente = listaTandas;
                            listaTandas = nuevaTanda;

                            // Guardar en archivo
                            bool creado;
                            FILE *archivo = manejoArchivos("tandas.txt", "a", &creado);
                            if (archivo != NULL) {
                                agregarRegistroTanda(
                                    nuevaTanda->idTanda,
                                    nuevaTanda->nombreTanda,
                                    nuevaTanda->montoAportado,
                                    nuevaTanda->totalParticipantes,
                                    nuevaTanda->fechaInicio,
                                    nuevaTanda->fechaFin,
                                    "tandas.txt"
                                );
                                fclose(archivo);

                                // Mensaje de éxito
                                snprintf(mensajeTanda, sizeof(mensajeTanda), "La Tanda '%s' fue creada exitosamente!", nombreTanda);
                            } else {
                                snprintf(mensajeTanda, sizeof(mensajeTanda), "Error al abrir o crear el archivo.");
                            }
                        }
                    } else {
                        // Mensaje de error si hay campos vacíos o inválidos
                        snprintf(mensajeTanda, sizeof(mensajeTanda), "Todos los campos son obligatorios.");
                    }
                }
            }  
            // Dibuja el botón Crear
            DrawRectangleRec(botonCrear, botonColor);
            DrawText("CREAR", botonCrear.x + 60, botonCrear.y + 15, 20, BLACK);

            // Mostrar mensaje de error o éxito
            DrawText(mensajeTanda, 100, 465, 20, (strlen(mensajeTanda) > 0 && strstr(mensajeTanda, "exitosamente") != NULL) ? GREEN : RED);

            break;

        case SCREEN_TANDAS_UNIR:
             DrawTexture(OPC1, 0, 0, WHITE);
            // Declarar variables de control de mensaje
            static bool mensajeMostrado = false;  // Variable para controlar si el mensaje debe mostrarse
            static char mensaje[100];  
            static int tandaSeleccionadaId = -1; 
            static char participantesText[50] = ""; 

            DrawText("Unirse a Tanda", 150, 20, 20, BLACK);

            // Mostrar las tandas disponibles
            NodoTanda *actual = listaTandas;
            int yPos = 60;  // Posición de inicio para la primera tanda
            while (actual != NULL) {
                // Mostrar el nombre de la tanda
                DrawText(actual->nombreTanda, 100, yPos, 20, BLACK);

                // Verificar si la tanda está seleccionada
                if (tandaSeleccionadaId == actual->idTanda) {
                    // Si está seleccionada, mostrar la información detallada de la tanda horizontalmente

                    yPos += 30; // Desplazar hacia abajo para mostrar la información horizontalmente
                    char montoAportadoText[50];
                    snprintf(montoAportadoText, sizeof(montoAportadoText), "Monto Aportado: $%d", actual->montoAportado);
                    DrawText(montoAportadoText, 100, yPos, 20, BLACK);

                    char totalSemanalText[50];
                    snprintf(totalSemanalText, sizeof(totalSemanalText), "Total Semanal: $%d", actual->totalSemanal);
                    DrawText(totalSemanalText, 350, yPos, 20, BLACK);

                    char fechasText[100];
                    snprintf(fechasText, sizeof(fechasText), "Fechas: %s - %s", actual->fechaInicio, actual->fechaFin);
                    DrawText(fechasText, 600, yPos, 20, BLACK);

                    // Mostrar cantidad de participantes unidos vs total
                    yPos += 30;
                    FILE *archivo = fopen("participantes.txt", "r");
                    int participantesUnidos = 0;
                    if (archivo != NULL) {
                        char nombre[50];
                        int idTandaArchivo;
                        while (fscanf(archivo, "%d %49s\n", &idTandaArchivo, nombre) == 2) {
                            if (idTandaArchivo == actual->idTanda) {
                                participantesUnidos++;
                            }
                        }
                        fclose(archivo);
                    }
                    
                    snprintf(participantesText, sizeof(participantesText), "%d/%d Participantes", participantesUnidos, actual->totalParticipantes);
                    DrawText(participantesText, 100, yPos, 20, BLACK);
                }

                yPos += 80;  // Espacio entre tandas
                actual = actual->siguiente;
            }

            // Poner un cuadro de texto para el nombre del participante 
            static char nombreParticipante[50] = ""; 
            int inputNombreX = (GetScreenWidth() - 300) / 2;  
            int inputNombreY = GetScreenHeight() - 140;  
            Rectangle inputNombreP = {inputNombreX, inputNombreY, 300, 40};  
            DrawText("Nombre del Participante", inputNombreX, inputNombreY - 20, 20, BLACK);  
            DrawRectangleRec(inputNombreP, GRAY);  
            DrawText(nombreParticipante, inputNombreP.x + 10, inputNombreP.y + 10, 20, BLACK);  

            // Lógica para capturar el nombre del participante
            if (IsKeyPressed(KEY_BACKSPACE) && strlen(nombreParticipante) > 0) {
                nombreParticipante[strlen(nombreParticipante) - 1] = '\0';
            } else {
                char c = GetCharPressed();
                while (c > 0) {
                    if (strlen(nombreParticipante) < 49) {
                        nombreParticipante[strlen(nombreParticipante)] = c;
                    }
                    c = GetCharPressed();
                }
            }

            // Al hacer clic sobre una tanda, se selecciona
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                // Verificar cuál tanda ha sido seleccionada
                NodoTanda *tandaSeleccionada = listaTandas;
                yPos = 60;  // Reajustar yPos al inicio de la lista de tandas
                while (tandaSeleccionada != NULL) {
                    // Definir el área de la tanda para la selección
                    Rectangle tandaRect = {100, yPos, 300, 40};
                    if (CheckCollisionPointRec(GetMousePosition(), tandaRect)) {
                        // Si la tanda ya está seleccionada, la deseleccionamos
                        if (tandaSeleccionadaId == tandaSeleccionada->idTanda) {
                            tandaSeleccionadaId = -1;  // Deseleccionar
                        } else {
                            tandaSeleccionadaId = tandaSeleccionada->idTanda;  // Seleccionar
                        }
                        break;
                    }
                    yPos += 80;  // Espacio entre tandas
                    tandaSeleccionada = tandaSeleccionada->siguiente;
                }
            }

            // Botón para unirse a la tanda (en la parte inferior derecha)
            int botonX = GetScreenWidth() - 250;  
            int botonY = GetScreenHeight() - 90;
            Rectangle botonUnirse = {botonX, botonY, 200, 40};
            DrawRectangleRec(botonUnirse, DARKGREEN);
            DrawText("Unirse a la Tanda", botonUnirse.x + 20, botonUnirse.y + 10, 20, WHITE);

        
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), botonUnirse)) {
                // Validar si el nombre del participante está vacío
                if (strlen(nombreParticipante) == 0) {
                    snprintf(mensaje, sizeof(mensaje), "¡Por favor ingresa tu nombre!");
                    mensajeMostrado = true;  // Mostrar mensaje de error
                } else {
                    // Intentar unirse a la tanda seleccionada
                    NodoTanda *tandaSeleccionada = listaTandas;
                    while (tandaSeleccionada != NULL) {
                        if (tandaSeleccionada->idTanda == tandaSeleccionadaId) {
                            if (agregarParticipanteATanda(tandaSeleccionada->idTanda, nombreParticipante, "participantes.txt")) {
                                // Mensaje de éxito
                                snprintf(mensaje, sizeof(mensaje), "¡Te has unido a la tanda: %s!", tandaSeleccionada->nombreTanda);
                                mensajeMostrado = true;
                            } else {
                                // Mensaje de error (ya estás unido a esta tanda)
                                snprintf(mensaje, sizeof(mensaje), "¡Ya estás unido a esta tanda!");
                                mensajeMostrado = true;
                            }
                            break;
                        }
                        tandaSeleccionada = tandaSeleccionada->siguiente;
                    }
                }
            }

            // Mostrar mensaje si hay algún mensaje (debajo del cuadro de texto del nombre)
            if (mensajeMostrado) {
                // Mostrar el mensaje justo debajo del cuadro de texto del nombre
                DrawText(mensaje, inputNombreX, inputNombreY + 50, 20, RED);
            }

            break;
        case SCREEN_TANDAS_VER:
             DrawTexture(OPC2, 0, 0, WHITE);
            // Declarar variables
            static char nombreUsuario[50] = "";
            static char mensajeError[128] = "";
            static bool nombreIngresado = false;
            static int tandaSeleccionadaIdd = -1;
            static char fechaActual[20] = "";

            DrawText("Mis Tandas", 150, 20, 20, BLACK);

            // Obtener la fecha actual usando tu función
            obtenerFecha(fechaActual, sizeof(fechaActual));

            // Si no se ha ingresado el nombre del usuario
            if (!nombreIngresado) {
                DrawText("Ingrese su nombre:", 100, 100, 20, BLACK);

                // Cuadro de texto para el nombre
                Rectangle inputNombre = {100, 140, 300, 40};
                DrawRectangleRec(inputNombre, GRAY);
                DrawText(nombreUsuario, inputNombre.x + 10, inputNombre.y + 10, 20, BLACK);

                // Entrada de texto
                if (IsKeyPressed(KEY_BACKSPACE) && strlen(nombreUsuario) > 0) {
                    nombreUsuario[strlen(nombreUsuario) - 1] = '\0';
                } else {
                    char c = GetCharPressed();
                    while (c > 0) {
                        if (strlen(nombreUsuario) < sizeof(nombreUsuario) - 1) {
                            nombreUsuario[strlen(nombreUsuario)] = c;
                        }
                        c = GetCharPressed();
                    }
                }

                // Botón para confirmar el nombre
                Rectangle botonConfirmar = {450, 140, 100, 40};
                DrawRectangleRec(botonConfirmar, DARKGREEN);
                DrawText("OK", botonConfirmar.x + 30, botonConfirmar.y + 10, 20, WHITE);

                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), botonConfirmar)) {
                    if (strlen(nombreUsuario) == 0) {
                        snprintf(mensajeError, sizeof(mensajeError), "El nombre no puede estar vacío.");
                    } else {
                        nombreIngresado = true;
                    }
                }

                // Mostrar mensaje de error si lo hay
                if (strlen(mensajeError) > 0) {
                    DrawText(mensajeError, 100, 200, 20, RED);
                }
            } else {
                // Mostrar tandas a las que el usuario está unido
                DrawText("Tandas a las que estás unido:", 100, 100, 20, BLACK);

                NodoTanda *actual = listaTandas;
                int yPos = 140;
                while (actual != NULL) {
                    // Verificar si el usuario pertenece a la tanda
                    FILE *archivo = fopen("participantes.txt", "r");
                    bool pertenece = false;
                    if (archivo != NULL) {
                        char participante[50];
                        int idTandaArchivo;
                        while (fscanf(archivo, "%d %49s\n", &idTandaArchivo, participante) == 2) {
                            if (idTandaArchivo == actual->idTanda && strcmp(participante, nombreUsuario) == 0) {
                                pertenece = true;
                                break;
                            }
                        }
                        fclose(archivo);
                    }

                    if (pertenece) {
                        // Dibujar nombre de la tanda
                        DrawText(actual->nombreTanda, 100, yPos, 20, BLACK);

                        // Botón para seleccionar tanda
                        Rectangle tandaRect = {400, yPos, 200, 40};
                        DrawRectangleRec(tandaRect, LIGHTGRAY);
                        DrawText("Pagar", tandaRect.x + 50, tandaRect.y + 10, 20, BLACK);

                        // Selección de tanda
                        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), tandaRect)) {
                            tandaSeleccionadaIdd = actual->idTanda;
                        }

                        yPos += 60;  // Espacio entre tandas
                    }
                    actual = actual->siguiente;
                }

                // Si se selecciona una tanda, mostrar detalles y opción para pagar
                if (tandaSeleccionadaIdd != -1) {
                    actual = listaTandas;
                    while (actual != NULL) {
                        if (actual->idTanda == tandaSeleccionadaIdd) {
                            // Mostrar detalles de la tanda
                            char infoTanda[128];
                            snprintf(infoTanda, sizeof(infoTanda), "Tanda: %s, Monto: $%d", actual->nombreTanda, actual->montoAportado);
                            DrawText(infoTanda, 100, yPos, 20, BLACK);

                            Rectangle botonPagar = {400, yPos, 200, 40};
                            DrawRectangleRec(botonPagar, DARKGREEN);
                            DrawText("Realizar Pago", botonPagar.x + 30, botonPagar.y + 10, 20, WHITE);

                            // Validar y realizar pago
                            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), botonPagar)) {
                                // Verificar si ya realizó un pago esta semana
                                FILE *archivoPagos = fopen("Pagos_Tandas.txt", "r");
                                bool pagoPermitido = true;
                                if (archivoPagos != NULL) {
                                    char usuarioPago[50], fechaPago[20];
                                    int idTandaPago, montoPago;
                                    while (fscanf(archivoPagos, "%49s %d %d %19s\n", usuarioPago, &idTandaPago, &montoPago, fechaPago) == 4) {
                                        if (strcmp(usuarioPago, nombreUsuario) == 0 && idTandaPago == tandaSeleccionadaIdd) {
                                            // Analizar la fecha de pago (usando sscanf en lugar de strptime)
                                            struct tm fechaPagoTm = {0};
                                            if (sscanf(fechaPago, "%d/%d/%d", &fechaPagoTm.tm_mday, &fechaPagoTm.tm_mon, &fechaPagoTm.tm_year) == 3) {
                                                fechaPagoTm.tm_year -= 1900;  // Ajuste para tm_year
                                                fechaPagoTm.tm_mon -= 1;      // Ajuste para tm_mon (0-11)

                                                time_t tiempoPago = mktime(&fechaPagoTm);
                                                time_t t = time(NULL);
                                                if (difftime(t, tiempoPago) < 7 * 24 * 3600) {
                                                    pagoPermitido = false;
                                                    snprintf(mensajeError, sizeof(mensajeError), "Solo puede pagar una vez por semana.");
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                    fclose(archivoPagos);
                                }

                                if (pagoPermitido) {
                                    // Registrar el pago
                                    archivoPagos = fopen("Pagos_Tandas.txt", "a");
                                    if (archivoPagos != NULL) {
                                        fprintf(archivoPagos, "%s %d %d %s\n", nombreUsuario, actual->idTanda, actual->montoAportado, fechaActual);
                                        fclose(archivoPagos);
                                        snprintf(mensajeError, sizeof(mensajeError), "Pago realizado con éxito.");
                                    } else {
                                        snprintf(mensajeError, sizeof(mensajeError), "Error al registrar el pago.");
                                    }
                                }
                                tandaSeleccionadaIdd = -1;
                            }

                            yPos += 60;
                            break;
                        }
                        actual = actual->siguiente;
                    }
                }

                // Mostrar mensaje de éxito o error
                if (strlen(mensajeError) > 0) {
                    DrawText(mensajeError, 100, yPos + 20, 20, RED);
                }
            }
            break;
        case SCREEN_PAGOS_VER:
            DrawTexture(OPC3, 0, 0, WHITE);
            // Declarar variables
            static char mensajeErrorPagos[128] = "";
            static bool nombreIngresadoPagos = false;
            static char nombreUsuarioPagos[50] = "";
            static int pagoSeleccionadoId = -1;  // Para almacenar el pago seleccionado

            DrawText("Mis Pagos", 150, 20, 20, BLACK);

            // Si no se ha ingresado el nombre del usuario
            if (!nombreIngresadoPagos) {
                DrawText("Ingrese su nombre:", 100, 100, 20, BLACK);

                // Cuadro de texto para el nombre
                Rectangle inputNombre = {100, 140, 300, 40};
                DrawRectangleRec(inputNombre, GRAY);
                DrawText(nombreUsuarioPagos, inputNombre.x + 10, inputNombre.y + 10, 20, BLACK);

                // Entrada de texto
                if (IsKeyPressed(KEY_BACKSPACE) && strlen(nombreUsuarioPagos) > 0) {
                    nombreUsuarioPagos[strlen(nombreUsuarioPagos) - 1] = '\0';
                } else {
                    char c = GetCharPressed();
                    while (c > 0) {
                        if (strlen(nombreUsuarioPagos) < sizeof(nombreUsuarioPagos) - 1) {
                            nombreUsuarioPagos[strlen(nombreUsuarioPagos)] = c;
                        }
                        c = GetCharPressed();
                    }
                }

                // Botón para confirmar el nombre
                Rectangle botonConfirmar = {450, 140, 100, 40};
                DrawRectangleRec(botonConfirmar, DARKGREEN);
                DrawText("OK", botonConfirmar.x + 30, botonConfirmar.y + 10, 20, WHITE);

                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), botonConfirmar)) {
                    if (strlen(nombreUsuarioPagos) == 0) {
                        snprintf(mensajeErrorPagos, sizeof(mensajeErrorPagos), "El nombre no puede estar vacío.");
                    } else {
                        nombreIngresadoPagos = true;
                    }
                }

                // Mostrar mensaje de error si lo hay
                if (strlen(mensajeErrorPagos) > 0) {
                    DrawText(mensajeErrorPagos, 100, 200, 20, RED);
                }
            } else {
                // Mostrar pagos realizados por el usuario
                DrawText("Pagos realizados:", 100, 100, 20, BLACK);

                FILE *archivoPagos = fopen("Pagos_Tandas.txt", "r");
                if (archivoPagos != NULL) {
                    char usuarioPago[50], fechaPago[20];
                    int idTandaPago, montoPago;
                    int yPos = 140;
                    bool pagosExistentes = false;

                    while (fscanf(archivoPagos, "%49s %d %d %19s\n", usuarioPago, &idTandaPago, &montoPago, fechaPago) == 4) {
                        if (strcmp(usuarioPago, nombreUsuarioPagos) == 0) {
                            pagosExistentes = true;
                            char textoPago[100];
                            snprintf(textoPago, sizeof(textoPago), "Tanda ID: %d | Monto: $%d | Fecha: %s", idTandaPago, montoPago, fechaPago);
                            DrawText(textoPago, 100, yPos, 20, BLACK);

                            // Botón para seleccionar un pago
                            Rectangle pagoRect = {400, yPos, 200, 40};
                            DrawRectangleRec(pagoRect, LIGHTGRAY);
                            DrawText("Ver detalles", pagoRect.x + 50, pagoRect.y + 10, 20, BLACK);

                            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), pagoRect)) {
                                pagoSeleccionadoId = idTandaPago; // Guardar el ID del pago seleccionado
                            }

                            yPos += 60;  // Espacio entre pagos
                        }
                    }

                    fclose(archivoPagos);

                    if (!pagosExistentes) {
                        DrawText("No se encontraron pagos para este usuario.", 100, 200, 20, RED);
                    }

                    // Si se ha seleccionado un pago, mostrar detalles adicionales
                    if (pagoSeleccionadoId != -1) {
                        // Leer nuevamente el archivo de pagos para mostrar los detalles del pago seleccionado
                        archivoPagos = fopen("Pagos_Tandas.txt", "r");
                        if (archivoPagos != NULL) {
                            while (fscanf(archivoPagos, "%49s %d %d %19s\n", usuarioPago, &idTandaPago, &montoPago, fechaPago) == 4) {
                                if (idTandaPago == pagoSeleccionadoId) {
                                    char detallesPago[200];
                                    snprintf(detallesPago, sizeof(detallesPago), "Detalles del Pago:\nID Tanda: %d\nMonto: $%d\nFecha: %s", idTandaPago, montoPago, fechaPago);
                                    DrawText(detallesPago, 100, yPos, 20, BLACK);
                                    break;
                                }
                            }
                            fclose(archivoPagos);
                        }
                    }
                } else {
                    DrawText("Error al cargar los pagos.", 100, 200, 20, RED);
                }
            }
            break;

        case SCREEN_AHORROS_VER:
            DrawTexture(OPC2, 0, 0, WHITE);
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
            DrawTexture(OPC2, 0, 0, WHITE);
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
            DrawTexture(OPC3, 0, 0, WHITE);
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
            botones[i].color = ColorBotonesOscuro;
        } else {
            botones[i].color = ColorBotones;
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
void dibujarBotonAtras(Boton *boton) {
    
    DrawRectangleRec(boton->rec, boton->color);
    
    // Mide el tamaño del texto
    Vector2 textSize = MeasureTextEx(GetFontDefault(), boton->text, 20, 0);
    
    // Calcula la posición para centrar el texto dentro del botón
    float x = boton->rec.x + (boton->rec.width - textSize.x) / 2;
    float y = boton->rec.y + (boton->rec.height - textSize.y) / 2;
    
    // Dibuja el texto centrado
    DrawText(boton->text, x, y, 20, BLACK);
}

void verificarBotonRegresar(Boton *boton) {
    if (CheckCollisionPointRec(GetMousePosition(), boton->rec) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
       
        boton->accion();
    }
}

void regresarAccion() {
    
    if (currentScreen == SCREEN_TANDAS_CREAR) { //Aqui estas
        currentScreen = SCREEN_TANDAS_PRINCIPAL; //Aqui vas
    } 
    
    else if (currentScreen == SCREEN_TANDAS_UNIR) {
        currentScreen = SCREEN_TANDAS_PRINCIPAL;  
    }

    else if (currentScreen == SCREEN_TANDAS_VER) {
        currentScreen = SCREEN_TANDAS_PRINCIPAL;  
    }
    else if (currentScreen == SCREEN_PAGOS_VER) {
        currentScreen = SCREEN_TANDAS_PRINCIPAL;  
    }

    else if (currentScreen == SCREEN_AHORROS_VER) {
        currentScreen = SCREEN_AHORROS_PRINCIPAL;  
    }

    else if (currentScreen == SCREEN_AHORROS_RETIRAR) {
        currentScreen = SCREEN_AHORROS_PRINCIPAL;  
    }

    else if (currentScreen == SCREEN_AHORROS_AHORRAR) {
        currentScreen = SCREEN_AHORROS_PRINCIPAL;  
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

void accionPagosVer(){
    currentScreen = SCREEN_PAGOS_VER;
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
        archivo = fopen(nombreArchivo, "a+");
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

void cargarTandasDesdeArchivo(const char *nombreArchivo) {
    bool archivoCreado;
    FILE *archivo = manejoArchivos(nombreArchivo, "r", &archivoCreado);

    // Si no se pudo abrir el archivo, terminamos
    if (archivo == NULL) {
        return;
    }

    while (!feof(archivo)) {
        NodoTanda *nuevoNodo = (NodoTanda *)malloc(sizeof(NodoTanda));
      
            if (fscanf(archivo, "%d %49s %d %d %d %s %s\n", 
            &nuevoNodo->idTanda, 
            nuevoNodo->nombreTanda, 
            &nuevoNodo->montoAportado, 
            &nuevoNodo->totalParticipantes,
            &nuevoNodo->totalSemanal, 
            nuevoNodo->fechaInicio, 
            nuevoNodo->fechaFin) == 7) {
            // Agregar a la lista de tandas
            nuevoNodo->siguiente = listaTandas;
            listaTandas = nuevoNodo;
        } else {
            // Si el formato no es correcto, liberamos el nodo creado
            free(nuevoNodo);
        }
    }

    fclose(archivo);
}


void cargarAhorrosDesdeArchivo(const char *nombreArchivo) {
    bool archivoCreado;
    FILE *archivo = manejoArchivos(nombreArchivo, "r", &archivoCreado);

    if (archivo == NULL) {
        return;
    }

    while (!feof(archivo)) {
        NodoAhorro *nuevoNodo = (NodoAhorro *)malloc(sizeof(NodoAhorro));
        if (fscanf(archivo, "%d %s %d %s\n",
                    &nuevoNodo->idUsuario, 
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

void agregarRegistroTanda(int idTanda, const char *nombreTanda, int montoAportado, int totalParticipantes, const char *fechaInicio, const char *fechaFin, const char *nombreArchivo) {
  
    int totalSemanal = montoAportado * totalParticipantes;
    // Crear el nuevo nodo
    NodoTanda *nuevoNodo = (NodoTanda *)malloc(sizeof(NodoTanda));
    nuevoNodo->idTanda = idTanda;
    snprintf(nuevoNodo->nombreTanda, sizeof(nuevoNodo->nombreTanda), "%s", nombreTanda);
    nuevoNodo->montoAportado = montoAportado;
    nuevoNodo->totalParticipantes = totalParticipantes;
    nuevoNodo->totalSemanal = totalSemanal;
    snprintf(nuevoNodo->fechaInicio, sizeof(nuevoNodo->fechaInicio), "%s", fechaInicio);
    snprintf(nuevoNodo->fechaFin, sizeof(nuevoNodo->fechaFin), "%s", fechaFin);
    nuevoNodo->siguiente = listaTandas;

    // Insertar el nuevo nodo al principio de la lista
    listaTandas = nuevoNodo;

    
    bool archivoCreado;
    FILE *archivo = manejoArchivos(nombreArchivo, "a", &archivoCreado);
    if (archivo != NULL) {
       
        fprintf(archivo, "%d %s %d %d %d %s %s\n", idTanda, nombreTanda, montoAportado, 
                totalParticipantes, totalSemanal, fechaInicio, fechaFin);
        fclose(archivo);
    }
}

bool agregarParticipanteATanda(int idTanda, const char *nombreParticipante, const char *nombreArchivo) {
    FILE *archivo = manejoArchivos(nombreArchivo, "r", NULL); 
    if (archivo == NULL) {
        return false; // Error al abrir el archivo
    }

    // Verificar si el participante ya está registrado en la tanda
    char linea[256];
    while (fgets(linea, sizeof(linea), archivo)) {
        int idTandaArchivo;
        char nombreParticipanteArchivo[50];
        sscanf(linea, "%d %49s", &idTandaArchivo, nombreParticipanteArchivo);

        if (idTandaArchivo == idTanda && strcmp(nombreParticipante, nombreParticipanteArchivo) == 0) {
            fclose(archivo);
            return false; // El participante ya está unido a esta tanda
        }
    }
    fclose(archivo);

    // Si no está unido, agregar al archivo
    archivo = manejoArchivos(nombreArchivo, "a", NULL); 
    if (archivo != NULL) {
        fprintf(archivo, "%d %s\n", idTanda, nombreParticipante);
        fclose(archivo);
        return true; 
    }

    return false; // Error al agregar al archivo
}