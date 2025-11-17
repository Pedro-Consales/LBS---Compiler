#include <stdio.h>
#include <string.h>

#include "gera_codigo.h"

//Fazer uma função monta exepressão para saber onde eu tenho que guardar a variável no RA -(20 - 4*n)

void setRetConstante(unsigned char code[], unsigned char valor_constante, int *code_pos, int *index_code)
{
    unsigned char code_buf[] = {
        0xb8, //Representa o movl para qunado temos o movl $constatne %eax
        (int)valor_constante
    };
    printf("gravei code_buf[], agora vou mandar para o code do parâmetro\n\n");
    for (int i = 0; i < sizeof(code_buf); i++) {
        code[*code_pos] = code_buf[i];
        printf("code[%d] = 0x%02x\n", code_pos, code[*code_pos]);
        code_pos++;
    }
    *index_code += sizeof(code_buf);
}
//A função deve receber um arquivo previamente aberto em 'f'

void gera_codigo (FILE *f, unsigned char code[], funcp *entry)
{
    printf("Gerando código...\n\n");
    //percorre o arquivo
    unsigned char c;
    int index_code = 0; // índice para preencher o vetor code_buf[] -> está alocado aqui para eu usar depois do while para garantir no debbug que o code está correto.
    int code_pos = 0; // índice para preencher o vetor code[]

    while (fscanf(f, " %c", &c) == 1)
    {

        printf("%c! ",c);

        switch (c)
        {
            case 'f': {
                printf("\n----------- Encontrei 'f' -----------\n");
                // viu 'f' -> leia até espaço/tab/CR/LF para pegar "unction"
                char resto_da_string[254];
                if (fscanf(f, "%254[^ \t\r\n]", resto_da_string) == 1) {
                    printf("\nLi alguma coisa depois de f: *%s*\n", resto_da_string);
    
                    if (strcmp(resto_da_string, "unction") == 0) {
                        printf("Entrei no if de que Achei a palavra !function!\n\n");
                        unsigned char code_buf[] = {
                            0x55,                   // push   %rbp
                            0x48, 0x89, 0xe5,       // mov    %rsp,%rbp
                            0x48, 0x83, 0xec, 0x20  // sub    $0x20,%rsp  0x20 = 32 bytes
                        };
                        printf("gravei code_buf[], agora vou mandar para o code do parâmetro\n\n");
                        for (int i = 0; i < sizeof(code_buf); i++) {
                            code[code_pos] = code_buf[i];
                            printf("code[%d] = 0x%02x\n", code_pos, code[code_pos]);
                            code_pos++;
                        }
                        printf("\nCode parou no index: %d\n\n", code_pos);
                    }
                } else {
                    printf("\nNao consegui ler o resto apos 'f'\n");
                }
                printf("\n-------------------------------------\n\n");
                break;
            }
            case 'e': {

                printf("\n----------- Encontrei 'e' -----------\n");
                char resto_da_string[254];
                if (fscanf(f, "%254[^ \t\r\n]", resto_da_string) == 1) {
                    printf("\nLi alguma coisa depois de e: *%s*\n", resto_da_string);
    
                    if (strcmp(resto_da_string, "nd") == 0) {
                        printf("O resto da palavra é et, logo achei o 'end'\n\n");
                        unsigned char code_buf[] = {
                            0xc9,                   //leave
                            0xc3                   // ret
                        };
                        printf("gravei code_buf[], agora vou mandar para o code do parâmetro\n\n");
                        for (int i = 0; i < sizeof(code_buf); i++) {
                            code[code_pos] = code_buf[i];
                            printf("code[%d] = 0x%02x\n", code_pos, code[code_pos]);
                            code_pos++;
                        }
                        printf("\nCode parou no index: %d\n\n", code_pos);
                    }
                } else {
                    printf("\nNao consegui ler o resto apos 'e'\n");
                }
                printf("\n-------------------------------------\n\n");
                break;
            }
            case 'r':{
                printf("\n----------- Encontrei 'r' -----------\n");
                char resto_da_string[254];
                if (fscanf(f, "%254[^ \t\r\n]", resto_da_string) == 1) {
                    printf("\nLi alguma coisa depois de r: *%s*\n", resto_da_string);
                    
                    if (strcmp(resto_da_string, "et") == 0) {
                        printf("O resto da palavra é et, logo achei o 'ret'\n\n");
                        //Agora o ret pode ser um mov constate %eax ou mov registrador %eax, com isso vou criar um handle desses casos
                        unsigned char caractere;
                        fscanf(f, " %c", &caractere);
                        switch (caractere)
                        {
                            // se for uma constante vai iniciar com $
                            case '$':{
                                int valor_constante;
                                if (fscanf(f, " %d", &valor_constante) == 1) { // lê até \n
                                    printf("valor_constante: %d\n", valor_constante);

                                    //setRetConstante(code, valor_constante, &code_pos, &index_code);

                                    unsigned char code_buf[] = {
                                        0xb8, //Representa o movl para qunado temos o movl $constatne %eax
                                        (unsigned char) valor_constante
                                    };
                                    printf("gravei code_buf[], agora vou mandar para o code do parâmetro\n\n");
                                    for (int i = 0; i < sizeof(code_buf); i++) {
                                        code[code_pos] = code_buf[i];
                                        printf("code[%d] = 0x%02x\n", code_pos, code[code_pos]);
                                        code_pos++;
                                    }
                                    printf("\nCode parou no index: %d\n\n", code_pos);
                                    } else {
                                        printf("Nao consegui ler a constante apos '$'\n");
                                    }
                                    break;
                            }
                            //Se for registrador faz outra coisa
                            default:
                                break;
                        }
                    }
                } else {
                    printf("\nNao consegui ler o resto apos 'r'\n");
                } 
                printf("\n-------------------------------------\n\n");
                break;
            }

            default:
                break;
        }

    }

    for(int i = 0; i < code_pos; i++){
        printf("code[%d] = 0x%02x\n", i, code[i]);
    }
}