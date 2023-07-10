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



char* FechayHora() {

    //Setear la fecha y hora para ponerlas en el archivo
    time_t t = time(NULL);
    memset(&tiempoactual, 0, sizeof(struct tm));
    errno_t fechaerror = localtime_s(&tiempoactual, &t);
    char* fechayhorai;

    fechayhorai = (char*)malloc(21 * sizeof(char));
    if (fechayhorai == NULL)
    {
        printf("Error al asignar memoria");
    }
    else
    {
        printf("Exito al asignar memoria");
        system("cls");
    }

    if (fechaerror == 0) {
        strftime(fechayhorai, 20, "%Y-%m-%dT%H:%M:%S", &tiempoactual);
    }
    else {
        printf("Error al obtener la fecha local.\n");
        free(fechayhorai);
        return NULL;
    }
    return fechayhorai;
}


float** IngresoCalificaciones()
{
    float** promediosmaterias = (float**)malloc(n_materiasTotal * sizeof(float*));
    if (promediosmaterias == NULL) {
        printf("Error al asignar memoria\n");
        return NULL;
    }

    for (int i = 0; i < n_materiasTotal; i++)
    {
        promediosmaterias[i] = (float*)malloc(n_progresosTotal * sizeof(float));
        if (promediosmaterias[i] == NULL)
        {
            printf("Error al asignar memoria\n");
            // Free previously allocated memory
            for (int j = 0; j < i; j++)
                free(promediosmaterias[j]);
            free(promediosmaterias);
            return NULL;
        }
        else
        {
            // Ingreso Calificaciones
            for (int k = 0; k < n_progresosTotal; k++)
            {
                printf("Ingrese la calificacion de la materia %d durante el progreso %d: \n", i + 1, k + 1);
                scanf_s("%f", &promediosmaterias[i][k]);
            }
        }
    }
    return promediosmaterias;
}


void AsignacionMemoriaPromedios(const int i, float** calificaciones)
{
    alumnos[i].promedios = calificaciones;

    for (int j = 0; j < n_materiasTotal; j++)
    {
        for (int k = 0; k < n_progresosTotal; k++)
        {
            fprintf(archivo ,"%f;", alumnos[i].promedios[j][k]);
        }
    }
    fprintf(archivo, "-1;\n");
}

void VerDatosEstudiantes()
{
    for (int i = 0; i < n_alumnosTotal; i++)
    {
        printf("Estudiante %d:\nNombre: %s\nEdad: %d\n", i + 1, alumnos[i].nombre, alumnos[i].edad);
        for (int j = 0; j < n_materiasTotal; j++)
        {
            printf("\tCalificacion materia %d\n", j + 1);
            for (int k = 0; k < n_progresosTotal; k++)
            {
                printf("Progreso %d: %f\t", k+1, alumnos[i].promedios[j][k]);
            }
            printf("\n");
        }
        printf("\n");
    }
    printf("\n");
}


void EscribirDatosAlumnos()
{
    for (int i = 0; i < n_alumnosTotal; i++)
    {
        fprintf(archivo,"%s;%s;%d;", alumnos[i].horaMod, alumnos[i].nombre, alumnos[i].edad);
        for (int j = 0; j < n_materiasTotal; j++)
        {
            for (int k = 0; k < n_progresosTotal; k++)
            {
                fprintf(archivo,"%f;", alumnos[i].promedios[j][k]);
            }
        }
        fprintf(archivo, "-1;\n");
    }
}

void ReemplazoDatos()
{
    char* hora = NULL;
    int i, edad;
    char nombre[30];

    printf("Ingrese el numero del alumno del que desea cambiar los datos (Del 1 al %d)\n", n_alumnosTotal);
    scanf_s("%d", &i);

    i--;

    while (i > n_alumnosTotal) {
        i = 0;
        printf("Ingrese el numero del alumno del que desea cambiar los datos (Del 1 al %d)\n", n_alumnosTotal);
        scanf_s("%d", &i);
        i--;
    }

    //Ingreso fecha y hora modificacion
    hora = FechayHora();
    if (hora == NULL) {
        printf("Error fecha\n");
    }
    else {
        strcpy_s(alumnos[i].horaMod, sizeof(alumnos[i].horaMod), hora);;
    }

    //Ingreso de nombre
    printf("Ingrese el nombre del estudiante: ");
    scanf_s("%s", nombre, 30);
    alumnos[i].nombre = _strdup(nombre);

    //Ingreso de edad
    printf("Ingrese la edad del estudiante: ");
    scanf_s("%d", &edad);
    alumnos[i].edad = edad;


    AsignacionMemoriaPromedios(i, IngresoCalificaciones());

    free(hora);
    ResetArchivo();
    ModificarMetadatos();
    EscribirDatosAlumnos();
}

void ResetArchivo()
{
    fclose(archivo);
    errno_t resultado = fopen_s(&archivo, "Datos.csv", "w+");
}

void IngresoEstudiantes() {
    //Cadena para fecha y hora
    char* hora = NULL;
    int n_alumnosnuevos, edad;
    char nombre[30];

    //Pedir ingreso de numeros de alumnos nuevos que quiere ingresar
    printf("Especifique el numero de estudiantes que desea ingresar\n");
    scanf_s("%d", &n_alumnosnuevos);

    n_alumnosTotal = n_alumnosTotal + n_alumnosnuevos;

    alumnos = (struct Alumno*)realloc(alumnos ,n_alumnosnuevos * sizeof(struct Alumno));
    if (alumnos == NULL)
    {
        printf("Error al redimensionar memoria\n");
    }
    else
    {
        printf("Exito al redimensionar memoria");
        //ModificarMetadato(n_alumnosTotal, 2);
        system("cls");
    }

    for (int i = (n_alumnosTotal - n_alumnosnuevos); i < n_alumnosTotal; i++)
    {
        //Fecha y Hora de ingreso entran al archivo y al struct
        hora = FechayHora();
        if (hora == NULL) {
            printf("Error fecha\n");
        }
        else {
            strcpy_s(alumnos[i].horaMod, sizeof(alumnos[i].horaMod), hora);;
        }

        //Ingreso de nombre
        printf("Ingrese el nombre del estudiante: ");
        scanf_s("%s", nombre, 30);
        alumnos[i].nombre = _strdup(nombre);

        //Ingreso de edad
        printf("Ingrese la edad del estudiante: ");
        scanf_s("%d", &edad);
        alumnos[i].edad = edad;
        

        AsignacionMemoriaPromedios(i, IngresoCalificaciones());

        free(hora);
    }

    ResetArchivo();
    ModificarMetadatos();
    EscribirDatosAlumnos();

}
