#include <stdio.h>
#include <string.h>

int main(){
    char buffer[1024];
    char *token;
    char *args[64];

    while(1){
        int contador = 0;
        printf("santi-shell> ");
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            size_t len = strlen(buffer);
            if (len > 0 && buffer[len - 1] == '\n') {
                buffer[len - 1] = '\0';
            }

            if (strcmp(buffer, "salir") == 0) {
                printf("Saliendo del programa.\n");
                break;
            }

            printf("Leido: \"%s\"\n", buffer);
            printf("Longitud: %zu caracteres\n", len - (len > 0 && buffer[len-1]=='\0'));

        } else {
            printf("\nError de lectura o fin de entrada.\n");
            break;
        }

        token = strtok(buffer, " ");

        while (token != NULL) {
            args[contador] = token;
            contador++;                
            token = strtok(NULL, " ");
        }

        args[contador] = NULL;

        printf("Argumentos encontrados:\n");
        for (int j = 0; args[j] != NULL; j++) {
            printf("  args[%d]: %s\n", j, args[j]);
        }
        printf("----------\n");
    }
}