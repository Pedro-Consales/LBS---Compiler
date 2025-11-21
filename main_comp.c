#include <stdio.h>
#include <stdlib.h>
#include "gera_codigo.h"


//Argumento só é passado aqui se a função usa um parâmetro p0
int main(int argc, char *argv[]) {
    FILE *fp;
    funcp funcLBS;
    unsigned char code[300];
    int res;
    int argumento = 5;   // número padrão para teste, pode mudar se quiser

    if (argc < 2) {
        printf("Uso: %s <arquivo LBS> [argumento]\n", argv[0]);
        return 1;
    }

    if (argc >= 3) {
        argumento = atoi(argv[2]);
    }

    fp = fopen(argv[1], "r");
    if (fp == NULL) {
        perror("Erro ao abrir arquivo");
        return 1;
    }

    /* Gera o código */
    gera_codigo(fp, code, &funcLBS);

    if (funcLBS == NULL) {
        printf("Erro na geracao\n");
        return 1;
    }

    fclose(fp);

    /* Chama a função gerada */
    res = (*funcLBS)(argumento);

    printf("Resultado: %d\n", res);

    return 0;
}
