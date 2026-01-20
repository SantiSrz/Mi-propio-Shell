# Mi-propio-Shell
https://github.com/SantiSrz/Mi-propio-Shell.git

1. Gestión de Memoria y Strings (El Parseo)
Lo que haces: Usas strtok para convertir una frase plana ("ls -l") en un array de punteros comprensible para el sistema char *args[].

Por qué: El sistema operativo no entiende frases, entiende argumentos separados.

2. La Clonación de Procesos (fork)
Lo que haces: Creas una copia exacta de tu programa en ejecución.

La Magia: Tienes un bloque if (pid == 0) donde vive el Hijo (el comando) y un else donde vive el Padre (el Shell que espera). Sin esto, tu shell moriría tras ejecutar el primer comando.

3. La Mutación (execvp)
Lo que haces: El Hijo usa execvp para dejar de ser "tu shell" y convertirse en "ls", "grep" o "firefox".

Detalle: Si execvp tiene éxito, el código del hijo desaparece de la memoria y es sustituido por el nuevo programa.

4. Manipulación del Entorno (cd - chdir)
Lo que haces: Detectas cd manualmente y usas chdir.

Por qué: Entendiste que un proceso hijo no puede cambiar el directorio de su padre. Tuviste que hacerlo "in-house" (built-in).

5. Cirugía de Descriptores de Archivo (dup2)
Esta es la parte más avanzada (Niveles 3 y 4):

Redirección (>): Operas al "paciente" (el proceso hijo) desconectando su salida estándar (pantalla) y conectándola a un archivo (open).

Pipes (|): Creas un canal de comunicación en el kernel (pipe). Conectas la salida del Hijo 1 a la entrada del Hijo 2. Así logras que ls | grep funcione.

gcc mishell.c -o mishell && ./mishell