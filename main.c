#include <stdio.h>
#include <string.h>
#include <unistd.h>   // Para fork, execvp, chdir
#include <sys/wait.h> // Para wait
#include <stdlib.h>   // Para exit

int main() {
    char buffer[1024];
    char *token;
    char *args[64];

    while (1) {
        int contador = 0;
        printf("santi-shell> ");

        // 1. LEER LA ENTRADA
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            size_t len = strlen(buffer);
            // Eliminar el salto de línea del final
            if (len > 0 && buffer[len - 1] == '\n') {
                buffer[len - 1] = '\0';
            }

            // Salida limpia
            if (strcmp(buffer, "salir") == 0) {
                printf("Saliendo del programa.\n");
                break;
            }
        } else {
            printf("\nError de lectura o fin de entrada.\n");
            break;
        }

        // 2. PARSEAR (TROCEAR)
        token = strtok(buffer, " ");
        while (token != NULL) {
            args[contador] = token;
            contador++;
            token = strtok(NULL, " ");
        }
        args[contador] = NULL; // Importante: terminar con NULL

        // Si el usuario dio Enter sin escribir nada, volvemos a empezar
        if (args[0] == NULL) {
            continue;
        }

        // Usamos strcmp, no ==. Si devuelve 0, son iguales.
        if (strcmp(args[0], "cd") == 0) {
            // Verificamos si hay un argumento (la ruta)
            if (args[1] == NULL) {
                fprintf(stderr, "Error: argumento requerido para \"cd\"\n");
            } else {
                // Intentamos cambiar de directorio
                if (chdir(args[1]) != 0) {
                    // Si falla (carpeta no existe, permisos...), imprime el error
                    perror("santi-shell: cd");
                }
            }
            // CRÍTICO: Saltamos el resto del bucle (fork/exec) y volvemos al prompt
            continue;
        }


        pid_t pid = fork();

        if (pid == -1) {
            // Error fatal al intentar clonar
            perror("Error en fork");
            exit(1);
        }

        if (pid == 0) {
            // --- PROCESO HIJO ---
            // Intenta mutar en el comando
            execvp(args[0], args);
            
            // Si llega aquí, execvp falló (comando no existe)
            perror("Error en exec");
            exit(1); // Matamos al hijo para que no siga ejecutando código del shell
        } else {
            // --- PROCESO PADRE ---
            // Espera a que el hijo termine
            wait(NULL);
        }
    }
    return 0;
}