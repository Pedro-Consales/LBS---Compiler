#include <stdio.h>
#include <string.h>

#include "gera_codigo.h"


//A função deve receber um arquivo previamente aberto em 'f'

void gera_codigo (FILE *f, unsigned char code[], funcp *entry)
{
    printf("Gerando código...\n\n");
    //percorre o arquivo
    unsigned char linha[255];
    int index_code = 0; // índice para preencher o vetor code_buf[] -> está alocado aqui para eu usar depois do while para garantir no debbug que o code está correto.

    while (fscanf(f, " %[^\n]", linha) == 1)
    {
        printf("\n------------ Linha lida: %s -------------\n\n", linha);
        unsigned char *linha_parsed = strtok(linha, " ");  // função que faz split da string em um vetor (achei na internet) -> Ele retorna a primeira palavra da linha e ao final faz outro comando que pula para a próxima palavra
    
        int code_pos = 0; // índice para preencher o vetor code[]

        while (linha_parsed != NULL) { // enquanto houver palavras na linha
            printf("Entrei no while com linha_parsed != NULL: --> linha_parsed = !%s!\n", linha_parsed);
            
            if(linha_parsed && strcmp(linha_parsed, "function") == 0){ // Pólogo para inciar uma função
                printf("Entrei no if de que Achei a palavra !function!\n");
                unsigned char code_buf[] = {
                    0x55,                   // push   %rbp
                    0x48, 0x89, 0xe5,       // mov    %rsp,%rbp
                    0x48, 0x83, 0xec, 0x20  // sub    $0x20,%rsp  
                };
                printf("gravei code_buf[], agora vou mandar para o code do parâmetro\n\n");
                for (int i = 0; i < sizeof(code_buf); i++) {
                    code[code_pos] = code_buf[i];
                    printf("code[%d] = 0x%02X\n", code_pos, code[code_pos]);
                    code_pos++;
                }
                index_code += sizeof(code_buf);
                printf("\nCode parou no index: %d\n\n", index_code);
                printf("!!!!!!!%s gravado com sucesso no vetor code[]\n!!!!!!!", linha_parsed);
            }
            else if (linha_parsed && strcmp(linha_parsed, "end") == 0) { // Epílogo para iniciar uma função
                printf("Entrei no if de que Achei a palavra !end!\n");
                unsigned char code_buf[] = {
                    0xc9,                   //leave
                    0xc3                   // ret
                };
                printf("gravei code_buf[], agora vou mandar para o code do parâmetro\n\n");
                for (int i = 0; i < sizeof(code_buf); i++) {
                    code[code_pos] = code_buf[i];
                    printf("code[%d] = 0x%02X\n", code_pos, code[code_pos]);
                    code_pos++;
                }
                index_code += sizeof(code_buf);
                printf("\nCode parou no index: %d\n\n", index_code);
                printf("!!!!!!!%s gravado com sucesso no vetor code[]\n!!!!!!!", linha_parsed);
            }
            else {
                printf("Palavra não reconhecida: %s\n", linha_parsed);
            }


            linha_parsed = strtok(NULL, " ");  // pula para a próxima palavra -> Isso tem que ser chamado no final do while depois do if, pois senão ele não anda na linha já que a primeira chamada retorna a primeira palavra e se tiver só uma palavra na linha, como é o caso do fuction, a próxima não existe então é null e sai do while, mas antes causa segfault se tentar comparar com a string "function"
        }
        
        printf("\n\n-------------------------------------------\n\n\n\n");

    }

    for(int i = 0; i < index_code; i++){
        printf("code[%d] = 0x%02X\n", i, code[i]);
    }
}



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