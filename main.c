#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

int main() {
    char buffer[1024];
    char *args[64];
    char *token;

    while (1) {
        printf("santi-shell> ");
        
        // --- 1. LEER ---
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) break;
        
        // Limpiar salto de linea
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') buffer[len - 1] = '\0';
        if (strcmp(buffer, "salir") == 0) break;

        // --- 2. PARSEAR ---
        int i = 0;
        token = strtok(buffer, " ");
        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;
        if (args[0] == NULL) continue;

        // --- NIVEL 2: COMANDO CD (Interno) ---
        if (strcmp(args[0], "cd") == 0) {
            if (args[1] == NULL) fprintf(stderr, "Error: falta argumento\n");
            else if (chdir(args[1]) != 0) perror("Error en cd");
            continue; // Saltamos al siguiente ciclo
        }

        // --- NIVEL 4: PIPES / TUBERÍAS (|) ---
        // Buscamos si hay un "|"
        int pipe_pos = -1;
        for (int j = 0; args[j] != NULL; j++) {
            if (strcmp(args[j], "|") == 0) {
                pipe_pos = j;
                break;
            }
        }

        if (pipe_pos != -1) {
            // Hay un Pipe. Dividimos en dos comandos: Izq | Der
            args[pipe_pos] = NULL;       // Cortamos el comando Izquierdo
            char **args2 = &args[pipe_pos + 1]; // El comando Derecho empieza después del |

            int fd[2]; 
            pipe(fd); // Creamos la tubería (fd[0] lectura, fd[1] escritura)

            // Hijo 1 (Comando Izquierda) -> Escribe en el Pipe
            if (fork() == 0) {
                dup2(fd[1], STDOUT_FILENO); // Salida -> Entrada del Pipe
                close(fd[0]); close(fd[1]); // Cerramos lo que no usamos
                execvp(args[0], args);
                perror("Error exec izq"); exit(1);
            }

            // Hijo 2 (Comando Derecha) -> Lee del Pipe
            if (fork() == 0) {
                dup2(fd[0], STDIN_FILENO); // Entrada -> Salida del Pipe
                close(fd[0]); close(fd[1]);
                execvp(args2[0], args2);
                perror("Error exec der"); exit(1);
            }

            // Padre: Cierra tubos y espera a los dos hijos
            close(fd[0]); close(fd[1]);
            wait(NULL); wait(NULL);
            
            continue; // Terminamos esta vuelta, no seguimos abajo
        }

        // --- NIVEL 1 y 3: EJECUCIÓN NORMAL Y REDIRECCIÓN (>) ---
        pid_t pid = fork();
        if (pid == 0) {
            // Buscamos Redirección '>'
            for (int j = 0; args[j] != NULL; j++) {
                if (strcmp(args[j], ">") == 0) {
                    char *filename = args[j+1];
                    int file = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    dup2(file, STDOUT_FILENO); // Salida -> Archivo
                    close(file);
                    args[j] = NULL; // Cortamos el comando antes del '>'
                    break;
                }
            }
            execvp(args[0], args);
            perror("Error comando no encontrado");
            exit(1);
        } else {
            wait(NULL);
        }
    }
    return 0;
}