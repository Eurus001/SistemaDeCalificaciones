#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <ctype.h>

// Struct de alumno
struct Alumno
{
    char horaMod[21];
    char* nombre;
    int edad;
    float** promedios;
};

// Struct de fecha y hora
struct tm tiempoactual;

// Array de struct alumnos
struct Alumno* alumnos;

//Archivo principal
FILE* archivo;

//Declaraci�n de funciones
void MenuInicial();
void ModificarMetadatos();
void CargaDeDatos();
void ProcesarEstudiante(char* linea, const int i);
int MenuPrincipal();
void Promedios();
void IngresoEstudiantes();
char* FechayHora();
float** IngresoCalificaciones();
void AsignacionMemoriaPromedios(const int i, float** calificacion);
void VerDatosEstudiantes();
void EscribirDatosAlumnos();
void ReemplazoDatos();
void ResetArchivo();

//variable global (cambiar despues)
int n_alumnosTotal = 0, n_materiasTotal = 0, n_progresosTotal = 0;

//main
int main()
{
    //Consola en espa�ol
    setlocale(LC_ALL, "es_ES");

    //Variables
    int flag = 1;

    //Apertura de archivo
    errno_t resultado = fopen_s(&archivo, "Datos.csv", "r");
    if (resultado == 0 && archivo != NULL)
    {
        CargaDeDatos();
    }
    else
    {
        // Crear el archivo en modo escritura y lectura
        resultado = fopen_s(&archivo, "Datos.csv", "w+");
        MenuInicial();
    }


    //Bucle de Menu
    while (flag) {
        flag = MenuPrincipal();
    }

    //Cerrar archivo
    int close = fclose(archivo);
        if (close == 0) {
            // Cierre exitoso
            printf("Archivo cerrado");
        }
        else {
            // Error al cerrar el archivo
            printf("Error al cerrar el archivo");
        }

    free(alumnos);
    return 0;
}

void MenuInicial() {
    printf("Ingrese el numero de materias que hay: ");
    scanf_s("%d", &n_materiasTotal);
    printf("%d\n",n_materiasTotal);
    printf("Ingrese el numero de progresos que hay: ");
    scanf_s("%d", &n_progresosTotal);
    printf("%d\n",n_progresosTotal);
    ModificarMetadatos();
    fprintf_s(archivo, "\n");
}

//Modifica los metadatos de la base de datos (primera linea)
// 0 es materias, 1 es progresos y 2 es numero de alumnos
void ModificarMetadatos()
{
    fseek(archivo, 0, SEEK_SET);
    fprintf_s(archivo, "%d;%d;%d\n", n_materiasTotal, n_progresosTotal, n_alumnosTotal);
}

void CargaDeDatos() {
    char linea[200];

    if (!archivo) {
        printf("Error al abrir el archivo.\n");
        return;
    }

    if (fgets(linea, sizeof(linea), archivo) == NULL) {
        printf("Error al leer la línea de datos iniciales.\n");
        return;
    }

    if (sscanf_s(linea, "%d;%d;%d", &n_materiasTotal, &n_progresosTotal, &n_alumnosTotal) != 3) {
        printf("Error al leer los valores iniciales.\n");
        return;
    }

    alumnos = (struct Alumno*)malloc(n_alumnosTotal * sizeof(struct Alumno));
    if (alumnos == NULL) {
        printf("Error al asignar memoria.\n");
        return;
    }

    for (int i = 0; i < n_alumnosTotal; i++) {
        if (fgets(linea, sizeof(linea), archivo) != NULL) {
            // Eliminar el carácter de nueva línea al final de la línea leída
            linea[strcspn(linea, "\n")] = '\0';
            ProcesarEstudiante(linea, i);
        }
        else {
            printf("Error al leer la línea del estudiante %d.\n", i + 1);
        }
    }
}

void ProcesarEstudiante(const char* linea, const int i) {
    char nombre[50], hora[20]; // Búfer para almacenar el nombre

    // Leer los datos del estudiante de la línea
    if (sscanf_s(linea, "%19[^;];%49[^;];%d", hora, sizeof(hora), nombre, sizeof(nombre), &alumnos[i].edad) != 3) {
        printf("Error al leer los valores iniciales del estudiante.\n");
        return;
    }
    strcpy_s(alumnos[i].horaMod, sizeof(alumnos[i].horaMod), hora);
    alumnos[i].nombre = _strdup(nombre);

    // Inicializar el array de promedios
    alumnos[i].promedios = (float**)malloc(n_materiasTotal * sizeof(float*));
    if (alumnos[i].promedios == NULL) {
        printf("Error al asignar memoria para los promedios del estudiante.\n");
        return;
    }

    for (int j = 0; j < n_materiasTotal; j++) {
        alumnos[i].promedios[j] = (float*)malloc(n_progresosTotal * sizeof(float));
        if (alumnos[i].promedios[j] == NULL) {
            printf("Error al asignar memoria para los promedios de la materia %d del estudiante.\n", j + 1);

            // Liberar memoria asignada previamente
            for (int k = 0; k < j; k++) {
                free(alumnos[i].promedios[k]);
            }
            free(alumnos[i].promedios);

            return;
        }
    }
    // Ignorar los primeros tres tokens
    int tokensIgnorados = 0;
    const char* token = strchr(linea, ';'); // Buscar el primer punto y coma
    while (tokensIgnorados < 2 && token != NULL) {
        token++;
        token = strchr(token, ';');
        tokensIgnorados++;
    }

    // Leer los promedios de la línea
    for (int j = 0; j < n_materiasTotal; j++) {
        for (int k = 0; k < n_progresosTotal; k++) {
            if (token != NULL) {
                token++; // Avanzar al siguiente carácter después del punto y coma

                // Convertir directamente el token a float
                float promedio;
                if (sscanf_s(token, "%f", &promedio) != 1) {
                    printf("Error al leer los promedios de la materia %d del estudiante.\n", j + 1);
                    return;
                }
                alumnos[i].promedios[j][k] = promedio;
            }
            else {
                printf("Error al leer los promedios de la materia %d del estudiante.\n", j + 1);
                return;
            }

            token = strchr(token, ';'); // Buscar el siguiente punto y coma
        }
    }

}

int MenuPrincipal() {

    int menu, flag = 1;

    printf("Seleccione una opción\n");
    printf("1. Ingreso de datos de estudiantes\n2. Modificar datos de estudiantes\n3. Ver Datos de estudiantes\n4. Promedio de estudiante\n5. Salir\n");
    scanf_s("%d", &menu);


    switch (menu)
    {
        case 1:
            IngresoEstudiantes(&n_alumnosTotal);
            break;

        case 2:
            ReemplazoDatos();
            break;

        case 3:
            VerDatosEstudiantes();
            break;

        case 4:
            Promedios();
            break;

        case 5:
            flag = 0;
            break;

    default:
        break;
    }


    return flag;
}

void Promedios()
{
    int flagReporte = 0;

    float promedio = 0.0f;
    float promedioGen = 0.0f;
    float promedioGenA = 0.0f;
    float promedioProgA = 0.0f;

    float* promedioM = calloc(n_materiasTotal, sizeof(float));
    float* promedioP = calloc(n_progresosTotal, sizeof(float));


    system("cls");
    for (int i = 0; i < n_alumnosTotal; i++)
    {
        promedio = 0.0f;
        promedioGenA = 0.0f;
        printf("Promedios del alumno %d. %s\n", i + 1, alumnos[i].nombre);
        printf("Promedio del alumno en cada materia:\n");
        for (int j = 0; j < n_materiasTotal; j++)
        {
            promedio = 0.0f;
            for (int k = 0; k < n_progresosTotal; k++)
            {
                promedio = promedio + alumnos[i].promedios[j][k];
                promedioGen = promedioGen + alumnos[i].promedios[j][k];
                promedioGenA = promedioGenA + alumnos[i].promedios[j][k];
                promedioM[j] = promedioM[j] + alumnos[i].promedios[j][k];
                promedioP[k] = promedioP[k] + alumnos[i].promedios[j][k];
            }
            printf("Promedio de materia %d: %f\n", j + 1, promedio / n_progresosTotal);
        }
        promedioProgA = 0.0f;
        for (int j = 0; j < n_progresosTotal; j++)
        {
            promedioProgA = 0.0f;
            for (int k = 0; k < n_materiasTotal; k++)
            {
                promedioProgA = promedioProgA + alumnos[i].promedios[k][j];
            }
            printf("Promedio de progreso %d: %f\n", j + 1, promedioProgA / n_materiasTotal);
        }
        printf("Promedio general del alumno es: %f", promedioGenA / (n_materiasTotal*n_progresosTotal));
        printf("\n");
    }

    printf("Promedio general del curso %f\n", promedioGen / ((n_materiasTotal * n_progresosTotal) * n_alumnosTotal));

    for (int mat = 0; mat < n_materiasTotal; mat++)
    {
        printf("Promedio general de materia %d es: %f\n", mat + 1, (promedioM[mat] / (n_progresosTotal * n_alumnosTotal)));
    }
    for (int prog = 0; prog < n_progresosTotal; prog++)
    {
        printf("Promedio general de progreso %d es: %f\n", prog + 1, (promedioP[prog] / (n_materiasTotal * n_alumnosTotal)));
    }

    printf("Desea hacer un reporte? (1. Si - Otro numero. No)");

    scanf_s("%d", &flagReporte);

    if (flagReporte == 1)
    {
        FILE* reporte;
        errno_t resultado = fopen_s(&reporte, "Reporte.csv", "w+");
        fprintf(reporte, "Promedio general del curso %f\n", promedioGen / ((n_materiasTotal * n_progresosTotal) * n_alumnosTotal));
        for (int mat = 0; mat < n_materiasTotal; mat++)
        {
            fprintf(reporte, "Promedio general de materia %d es: %f\n", mat + 1, (promedioM[mat] / (n_progresosTotal * n_alumnosTotal)));
        }
        for (int prog = 0; prog < n_progresosTotal; prog++)
        {
            fprintf(reporte, "Promedio general de progreso %d es: %f\n", prog + 1, (promedioP[prog] / (n_materiasTotal * n_alumnosTotal)));
        }
        int error = fclose(reporte);
    } 

    free(promedioM);
    free(promedioP);
}


