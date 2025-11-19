#include<stdio.h>

#include "gera_codigo.h"

int main(void){

    FILE *f = fopen("entrada.txt", "rt");
    if(f == NULL){
        printf("Erro ao abrir o arquivo\n");
        return 1;
    }





    unsigned char code[1024];
    funcp entry;

    gera_codigo(f, code, &entry);

    return 0;
}