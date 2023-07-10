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
