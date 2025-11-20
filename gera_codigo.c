#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gera_codigo.h"

typedef struct nofunc_info NoFuncInfo;
struct nofunc_info {
    int endereco_fun;
    int num_fun;
    NoFuncInfo *prox_func;
};

struct func_info {
    NoFuncInfo *func;
};
typedef struct func_info HeadFuncs;

HeadFuncs* criaLista()
{
    HeadFuncs *lista = (HeadFuncs *)malloc(sizeof(HeadFuncs));
    if(lista == NULL)
    {
        printf("Erro ao alocar memoria para lista no na função cria Lista!!!");
        exit(1);
    }
    lista->func = NULL;
    return lista;
}

int offset_var(int n) {
    return -4 * (n + 1);   // v0=-4, v1=-8, ....
}

void insere(HeadFuncs *raiz, int endereco_fun, int *num_fun) {

    if (raiz->func == NULL) { //Não tem ngm, aloca o priemiro

        NoFuncInfo *novo_no = (NoFuncInfo *)malloc(sizeof(NoFuncInfo));
        if(novo_no == NULL)
        {
            printf("Erro ao alocar memoria para o novo no (if) na função insere!!!");
            exit(1);
        }
        novo_no->endereco_fun = endereco_fun;
        novo_no->num_fun = *num_fun;
        novo_no->prox_func = NULL;

        raiz->func = novo_no;
        (*num_fun)++;
    }
    else
    {
        NoFuncInfo *novo_no = (NoFuncInfo *)malloc(sizeof(NoFuncInfo)); 
        if(novo_no == NULL)
        {
            printf("Erro ao alocar memoria para o novo no (else) na função insere!!!");
            exit(1);
        }
        novo_no->endereco_fun = endereco_fun;
        novo_no->num_fun = *num_fun;
        novo_no->prox_func = NULL;

        NoFuncInfo *atual = raiz->func;
        while(atual->prox_func != NULL)
        {
            atual = atual->prox_func;
        }

        atual->prox_func = novo_no;
        (*num_fun)++;
    }
    return; 
}

void exibeLista(HeadFuncs *raiz) {
    NoFuncInfo *atual = raiz->func;
    while (atual != NULL) {
        printf("\nEndereço da função %d: %d (Indice do final do vetor código)\n", atual->num_fun, atual->endereco_fun);
        atual = atual->prox_func;
    }
    printf("\nTerminei de printar todas as funções guardadas\n\n\n\n");
    return;
}

//Fazer uma função monta exepressão para saber onde eu tenho que guardar a variável no RA -(20 - 4*n)

// void setRetConstante(unsigned char code[], unsigned char valor_constante, int *code_pos, int *index_code)
// {
//     unsigned char code_buf[] = {
//         0xb8, //Representa o movl para qunado temos o movl $constatne %eax
//         (int)valor_constante
//     };
//     printf("gravei code_buf[], agora vou mandar para o code do parâmetro\n\n");
//     for (int i = 0; i < sizeof(code_buf); i++) {
//         code[*code_pos] = code_buf[i];
//         printf("code[%d] = 0x%02x\n", code_pos, code[*code_pos]);
//         code_pos++;
//     }
//     *index_code += sizeof(code_buf);
// }

void grava_bytes(unsigned char code[], unsigned char *code_buf, int *code_pos, size_t tam_code_buf)
{
    int local_code_pos = *code_pos;
    printf("gravei code_buf[], agora vou mandar para o code do parâmetro\n\n");
    for (int i = 0; i < tam_code_buf; i++) {
        code[local_code_pos] = code_buf[i];
        printf("code[%d] = 0x%02x\n", local_code_pos, code[local_code_pos]);
        local_code_pos++;
    }
    *code_pos = local_code_pos;
    printf("\nCode parou no index: %d\n\n", local_code_pos);
}




//A função deve receber um arquivo previamente aberto em 'f'

void gera_codigo (FILE *f, unsigned char code[], funcp *entry)
{
    printf("Gerando código...\n\n");
    //percorre o arquivo
    char c;
    int code_pos = 0; // índice para preencher o vetor code[]
    int contador_func = 0;
    HeadFuncs* lista_raiz = criaLista();

    while (fscanf(f, " %c", &c) == 1)
    {

        printf("%c! ",c);

        switch (c)
        {
            case 'f': {
                printf("\n----------- Encontrei 'f' -----------\n"); //Funcion deve ter uma lista encadeada qu tem que ter o indice da priemira instrução de onde ta o código no vetro ou seja o como se fosse o endereço da "pilha" e o número da função -> o número da função é a ordem em que ela foi insirida. Priemira função numeo 0...
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
                        //
                        grava_bytes(code, code_buf, &code_pos,sizeof(code_buf));
                        insere(lista_raiz, code_pos, &contador_func);
                        printf("Contador func fora das fuções para ver se mudou: %d\n", contador_func);
                        // printf("gravei code_buf[], agora vou mandar para o code do parâmetro\n\n");
                        // for (int i = 0; i < sizeof(code_buf); i++) {
                        //     code[code_pos] = code_buf[i];
                        //     printf("code[%d] = 0x%02x\n", code_pos, code[code_pos]);
                        //     code_pos++;
                        // }
                        // printf("\nCode parou no index: %d\n\n", code_pos);
                        
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
                        printf("Entrei no if de que Achei a palavra !end!\n\n");
                        unsigned char code_buf[] = {
                            0xc9,                   //leave
                            0xc3                   // ret
                        };
                        grava_bytes(code, code_buf, &code_pos,sizeof(code_buf));
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
                        printf("Entrei no if de que Achei a palavra !ret!\n\n");
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
                                    grava_bytes(code, code_buf, &code_pos,sizeof(code_buf));

                                    } else {
                                        printf("Nao consegui ler a constante apos '$'\n");
                                    }
                                    break;
                            }
                            case 'v':{ //Pode retornar parâmetro? Se sim tem que fazer um caso onde é p o case
                                char var;
                                if (fscanf(f, " %c", &var) == 1) { // lê até \n
                                    printf(" ------ Restante da variavel : %c ---- \n", var);
                                    
                                    if (var < '0' || var > '4') {
                                        printf("Variavel nao suportada: v%c\n", var);
                                        break;
                                    }

                                    int var_buff = var - '0'; // Converte char para int (Não da para fazer type casting direto)
                                    int offset = offset_var(var_buff); // Função que retorna o offset da variável no RA 
                                    printf("Offset da variável v%c: %0x2\n", var, offset);
                                    signed char offset_buf = (signed char)offset;
                                    unsigned char code_buf[] = { // Já que só podem 5 variáveis e tem que necessáriamente guarduardar em RA eu só vou usar called-save registers
                                        0x8b, 0x45, offset_buf
                                    };
                                    grava_bytes(code, code_buf, &code_pos,sizeof(code_buf));

                                    // if (var == '0') { //ebx -> eax
                                    //     printf("Entrou no if de !v0!\n");
                                    //     unsigned char code_buf[] = { // Já que só podem 5 variáveis e tem que necessáriamente guarduardar em RA eu só vou usar called-save registers
                                    //         0x8b, 0x45, 0xfc //mov %ebx, %eax
                                    //     };
                                    //     grava_bytes(code, code_buf, &code_pos,sizeof(code_buf));
                                    // } 
                                    // else if (var == '1') {//r12d -> eax
                                    //     printf("Entrou no if de !v1!\n");
                                    //     unsigned char code_buf[] = { // Guardo direto no RA
                                    //         0x8b, 0x45, 0xf8 
                                    //     };
                                    //     grava_bytes(code, code_buf, &code_pos,sizeof(code_buf));
                                    // } 
                                    // else if (var == '2') {//r13d -> eax
                                    //     printf("Entrou no if de !v2!\n");
                                    //     unsigned char code_buf[] = { // Já que só podem 5 variáveis e tem que necessáriamente guarduardar em RA eu só vou usar called-save registers
                                    //         0x8b, 0x45, 0xf4
                                    //     };
                                    //     grava_bytes(code, code_buf, &code_pos,sizeof(code_buf));
                                    // } 
                                    // else if (var == '3') {//r14d -> eax
                                    //     printf("Entrou no if de !v3!\n");
                                    //     unsigned char code_buf[] = { // Já que só podem 5 variáveis e tem que necessáriamente guarduardar em RA eu só vou usar called-save registers
                                    //         0x8b, 0x45, 0xf0
                                    //     };
                                    //     grava_bytes(code, code_buf, &code_pos,sizeof(code_buf));
                                    // }
                                    // else if (var == '4') {//r15d -> eax
                                    //     printf("Entrou no if de !v4!\n");
                                    //     unsigned char code_buf[] = { // Já que só podem 5 variáveis e tem que necessáriamente guarduardar em RA eu só vou usar called-save registers
                                    //         0x8b, 0x45, 0xec
                                    //     };
                                    //     grava_bytes(code, code_buf, &code_pos,sizeof(code_buf));
                                    // }
                                    // else {
                                    //     printf("Variavel nao suportado no momento: %c\n", var);
                                    // }

                                } else {
                                    printf("Nao consegui ler o registrador apos 'v'\n");
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
            //case 'v'

            default:
                break;
        }

    }

    for(int i = 0; i < code_pos; i++){
        printf("code[%d] = 0x%02x\n", i, code[i]);
    }
    printf("\n ---- Local das funções guardadas na lista encadead ---- \n");
    exibeLista(lista_raiz);
}