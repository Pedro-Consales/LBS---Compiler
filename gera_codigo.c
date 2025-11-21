//Pedro Consales Margaronis - 2410238 - 3WB


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

static HeadFuncs* criaLista()
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



static void insere(HeadFuncs *raiz, int endereco_fun, int *num_fun) {
    
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

static void exibeLista(HeadFuncs *raiz) {
    NoFuncInfo *atual = raiz->func;
    while (atual != NULL) {
        printf("\nEndereço da função %d: %d (Indice do Início do vetor código)\n", atual->num_fun, atual->endereco_fun);
        atual = atual->prox_func;
    }
    printf("\nTerminei de printar todas as funções guardadas\n\n\n\n");
    return;
}

static NoFuncInfo* busca_func(HeadFuncs *lista, int num) {
    NoFuncInfo *at = lista->func;
    while (at != NULL) {
        if (at->num_fun == num) {
            return at;
        }
        at = at->prox_func;
    }
    return NULL;
}

static void grava_bytes(unsigned char code[], unsigned char *code_buf, int *code_pos, size_t tam_code_buf)
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

static void gera_call_para_func(HeadFuncs *lista, int func_num, unsigned char code[], int *code_pos) {
    
    NoFuncInfo *info = busca_func(lista, func_num);
    if (info == NULL) {
    printf("Erro: funcao %d nao encontrada na lista de funcoes\n", func_num);
    exit(1);
    }

    int alvo = info->endereco_fun;   // índice no code[] do início da função
    int origem = *code_pos + 5;      // endereço lógico da próxima instrucao
    int disp = alvo - origem;        // deslocamento relativo

    unsigned char buf[5];
    buf[0] = 0xe8;
    buf[1] = (unsigned char)(disp & 0xff);
    buf[2] = (unsigned char)((disp >> 8) & 0xff);
    buf[3] = (unsigned char)((disp >> 16) & 0xff);
    buf[4] = (unsigned char)((disp >> 24) & 0xff);

    grava_bytes(code, buf, code_pos, 5);
}


static int offset_var(int n) {
    return -4 * (n + 1);   // v0=-4, v1=-8, ....
}

static int le_char_mesma_linha(FILE *f, char *out) { //helper que ajuda a não consumir o \n da próxima linha do arquivo.
    int ch;
    for (;;) {
        ch = fgetc(f);
        if (ch == EOF) return 0;
        if (ch == '\n') return -1;  // acabou a linha
        if (ch == ' ' || ch == '\t' || ch == '\r') continue;
        *out = (char)ch;
        return 1;  // char válido
    }
}





//A função deve receber um arquivo previamente aberto em 'f'

void gera_codigo (FILE *f, unsigned char code[], funcp *entry)
{
    printf("Gerando código...\n\n");
    //percorre o arquivo
    char c;
    int code_pos = 0; // índice para preencher o vetor code[]
    int contador_func = 0;
    int ultimo_cmd_foi_ret = 0;
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
                        int inicio_func = code_pos;
                        grava_bytes(code, code_buf, &code_pos,sizeof(code_buf));
                        insere(lista_raiz, inicio_func, &contador_func);
                        printf("Contador func fora das fuções para ver se mudou: %d\n", contador_func);
                        ultimo_cmd_foi_ret = 0; 
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
                        if (!ultimo_cmd_foi_ret) {
                            unsigned char code_buf[] = {
                                0xc9,   // leave
                                0xc3    // ret
                            };
                            grava_bytes(code, code_buf, &code_pos,sizeof(code_buf));
                        }
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
    
                        char caractere;
                        int r = le_char_mesma_linha(f, &caractere);
                        if (r != 1) {
                            printf("Erro de sintaxe: faltou argumento para 'ret' na mesma linha\n");
                            exit(1);
                        }
    
                        switch (caractere) {
    
                            case '$': {
                                int valor_constante;
                                if (fscanf(f, " %d", &valor_constante) != 1) {
                                    printf("Erro de sintaxe: constante invalida apos '$' em 'ret'\n");
                                    exit(1);
                                }
                                printf("valor_constante: %d\n", valor_constante);
    
                                // mov $imm32, %eax   -> b8 imm32
                                unsigned char code_buf[5];
                                code_buf[0] = 0xb8;
                                code_buf[1] = (unsigned char)(valor_constante & 0xff);
                                code_buf[2] = (unsigned char)((valor_constante >> 8) & 0xff);
                                code_buf[3] = (unsigned char)((valor_constante >> 16) & 0xff);
                                code_buf[4] = (unsigned char)((valor_constante >> 24) & 0xff);
                                grava_bytes(code, code_buf, &code_pos, sizeof(code_buf));
                                break;                 
                            }
    
                            case 'v': { // retorno de variável
                                char var;
                                int rvar = le_char_mesma_linha(f, &var);
                                if (rvar != 1) {
                                    printf("Erro de sintaxe: faltou indice depois de 'v' em 'ret'\n");
                                    exit(1);
                                }
                                printf(" ------ Restante da variavel : %c ---- \n", var);
                                if (var < '0' || var > '4') {
                                    printf("Variavel nao suportada em 'ret': v%c\n", var);
                                    exit(1);
                                }
    
                                int var_buff = var - '0';
                                int offset = offset_var(var_buff);
                                printf("Offset da variável v%c: %02x\n", var, offset); 
                                signed char offset_buf = (signed char)offset;
    
                                // mov -off(%rbp), %eax   -> 8b 45 disp8
                                unsigned char code_buf[] = { 0x8b, 0x45, (unsigned char)offset_buf };
                                grava_bytes(code, code_buf, &code_pos, sizeof(code_buf));
                                break;
                            }
    
                            case 'p': {  // retorno do parâmetro p0
                                char idx;
                                int rp = le_char_mesma_linha(f, &idx);
                                if (rp != 1) {
                                    printf("Erro de sintaxe: faltou indice apos 'p' em 'ret'\n");
                                    exit(1);
                                }
                                if (idx != '0') {
                                    printf("Parametro nao suportado em 'ret': p%c (apenas p0)\n", idx);
                                    exit(1);
                                }
                            
                                // mov %edi, %eax  -> 89 f8
                                unsigned char code_buf[] = { 0x89, 0xf8 };
                                grava_bytes(code, code_buf, &code_pos, sizeof(code_buf));
                                break;
                            }
    
                            default:
                                printf("Erro de sintaxe: operando invalido '%c' em 'ret' (esperava $, v ou p)\n",
                                       caractere);
                                exit(1);
                        }
    
                        unsigned char leave_ret[] = { 0xc9, 0xc3 };
                        grava_bytes(code, leave_ret, &code_pos, sizeof(leave_ret));
                    }
                    else {
                        printf("Comando invalido apos 'r': %s (esperava 'et')\n", resto_da_string);
                        exit(1);
                    }
    
                } else {
                    printf("\nNao consegui ler o resto apos 'r'\n");
                    exit(1);
                } 
                ultimo_cmd_foi_ret = 1;
                printf("\n-------------------------------------\n\n");
                break;
            }
    

            case 'v': { // atribuicao a variavel
                printf("\n----------- Encontrei o primeiro 'v' -----------\n");

                ultimo_cmd_foi_ret = 0;
                char var, op, expr;
                int rvar = le_char_mesma_linha(f, &var);
                if (rvar != 1) {
                    printf("Erro de sintaxe: faltou indice depois de 'v' na atribuicao\n");
                    exit(1);
                }
                
                printf(" ------ Restante da variavel v : %c ----  \n", var);

                if (var < '0' || var > '4') { //Verifico se está no range permitido pela LBS
                    printf("Variavel nao suportada: v%c -> possíveis: v0, v1, v2, v3, v4\n", var);
                    break;
                }
                //Se chegou aqui, a variavel é valida
                int rop = le_char_mesma_linha(f, &op);
                if (rop != 1) {
                    printf("Erro de sintaxe: faltou '=' depois de v%c\n", var);
                    exit(1);
                }
    
                if (op != '=') {
                    printf("Erro ao ler o '=' apos a variavel v%c\n", var);
                    exit(1);
                }

    
                    printf(" ------ Operador encontrado : %c ---- \n", op);

                    int is_call = 0;
                    int rexpr = le_char_mesma_linha(f, &expr);
                    if (rexpr != 1) {
                        printf("Erro de sintaxe: faltou primeiro operando depois de '=' em v%c\n", var);
                        exit(1);
                    }
                    printf(" ------ Expressão encontrada : %c ---- \n", expr);
                    
                    switch (expr) { //Casos que a expressão pode ter

                        case 'c': {  // expressão é CALL
                            is_call = 1;
                    
                            // lê o "all"
                            char resto_call[25];
                            if (fscanf(f, "%24[^ \t\r\n]", resto_call) != 1) {
                                printf("Erro de sintaxe apos 'c' em call\n");
                                exit(1);
                            }
                            if (strcmp(resto_call, "all") != 0) {
                                printf("Comando invalido apos 'c': %s (esperava 'all')\n", resto_call);
                                exit(1);
                            }
                    
                            // lê o numero da funcao
                            int func_num;
                            if (fscanf(f, " %d", &func_num) != 1) {
                                printf("Erro: faltou numero da funcao em call\n");
                                exit(1);
                            }
                            printf("CALL para funcao %d\n", func_num);
                    
                            // lê o varpc (argumento)
                            char kind;
                            int rkind = le_char_mesma_linha(f, &kind);
                            if (rkind != 1) {
                                printf("Erro: faltou argumento varpc em call\n");
                                exit(1);
                            }
                    
                            switch (kind) {
                                case 'v': {
                                    char idx;
                                    int r = le_char_mesma_linha(f, &idx);
                                    if (r != 1) {
                                        printf("Erro: faltou indice depois de 'v' em argumento de call\n");
                                        exit(1);
                                    }
                                    if (idx < '0' || idx > '4') {
                                        printf("Variavel invalida em argumento de call: v%c\n", idx);
                                        exit(1);
                                    }
                                    int numv = idx - '0';
                                    int off = offset_var(numv);
                                    signed char offb = (signed char)off;
                                    // mov -off(%rbp), %edi  => 8b 7d disp8
                                    unsigned char buf[] = { 0x8b, 0x7d, (unsigned char)offb };
                                    grava_bytes(code, buf, &code_pos, sizeof(buf));
                                    break;
                                }
                    
                                case 'p': {
                                    char idx;
                                    int r = le_char_mesma_linha(f, &idx);
                                    if (r != 1 || idx != '0') {
                                        printf("Parametro invalido em argumento de call (esperava p0)\n");
                                        exit(1);
                                    }
                                    // p0 já está em %edi pela convenção -> não precisa gerar nada
                                    printf("Argumento de call: p0 (ja em %%edi)\n");
                                    break;
                                }
                    
                                case '$': {
                                    int val;
                                    if (fscanf(f, " %d", &val) != 1) {
                                        printf("Constante invalida em argumento de call\n");
                                        exit(1);
                                    }
                                    printf("Argumento de call: constante %d\n", val);
                                    // mov $imm32, %edi  => bf imm32
                                    unsigned char buf[5];
                                    buf[0] = 0xbf;
                                    buf[1] = (unsigned char)(val & 0xff);
                                    buf[2] = (unsigned char)((val >> 8) & 0xff);
                                    buf[3] = (unsigned char)((val >> 16) & 0xff);
                                    buf[4] = (unsigned char)((val >> 24) & 0xff);
                                    grava_bytes(code, buf, &code_pos, 5);
                                    break;
                                }
                    
                                default:
                                    printf("Argumento de call invalido: %c (esperava v | p | $)\n", kind);
                                    exit(1);
                            }
                    
                            // Agora emite o call para func_num
                            gera_call_para_func(lista_raiz, func_num, code, &code_pos);
                    
                            // Depois do call, resultado esta em %eax
                            break;
                        }
                        case 'v': { //é uma variável ou seja temos vX = vY ...
                            printf("Entrei no case de expressão ter uma variável\n");


                            char var_expr;
                            int rve = le_char_mesma_linha(f, &var_expr);
                            if (rve != 1) {
                                printf("Erro de sintaxe: faltou indice depois de 'v' no primeiro operando\n");
                                exit(1);
                            }

                            if (var_expr < '0' || var_expr > '4') { //Verifico se está no range permitido pela LBS
                                printf("Variavel nao suportada: v%c\n", var_expr);
                                break;
                            }

                            //Tercho que verifica onde a variável está no RA e grava o código que pega o valor dela
                            int var_expr_buff = var_expr - '0';
                            int offset_expr = offset_var(var_expr_buff);
                            printf("Offset da variável v%c: %02x\n", var_expr, offset_expr);
                            signed char offset_expr_buf = (signed char)offset_expr;
                            unsigned char code_buf[] = { 0x8b, 0x45, offset_expr_buf };
                            grava_bytes(code, code_buf, &code_pos,sizeof(code_buf));

                            break;
                        }
                        case 'p': { //Caso onde temos vX = p0 -> Só pode ser p0 pois no padrão LBS só existe esse parâmetro

                            printf("Entrei no case de expressão ter um parâmetro\n");

                            char d;
                            int rd = le_char_mesma_linha(f, &d);
                            if (rd != 1) {
                                printf("Erro de sintaxe: faltou indice depois de 'p' no primeiro operando\n");
                                exit(1);
                            }

                            printf(" ------ Restante do parâmetro na expressão p : %c ---- \n", d);
                            if (d != '0') {
                                printf("Parâmetro não suportado: p%c\n", d);
                                break;
                            }
                            unsigned char mov_eax_edi[] = { 0x89, 0xf8 };
                            grava_bytes(code, mov_eax_edi, &code_pos, sizeof(mov_eax_edi));
                            break;
                            
                        }
                        case '$': { // Caso onde temos vX = $constante
                            printf("Entrei no case de expressão ter uma constante\n");

                            int val;
                            if (fscanf(f, " %d", &val) != 1) {
                                printf("Constante invalida em varpc\n");
                                break;
                            }
                            //Trata a constante lida e grava o código que coloca ela em %eax
                            //ex: mov $100, %eax -> b8 64 00 00 00 ( os 00 00 00 são os bytes mais significativos do valor 100), ou seja não aparece uma instrução claramente para guardar %eax, mas sim o mov com o valor direto que vai ao registrador
                            unsigned char buf[5];
                            buf[0] = 0xb8;
                            buf[1] = (unsigned char)(val & 0xff);
                            buf[2] = (unsigned char)((val >> 8) & 0xff);
                            buf[3] = (unsigned char)((val >> 16) & 0xff);
                            buf[4] = (unsigned char)((val >> 24) & 0xff);
                            grava_bytes(code, buf, &code_pos, 5);
                            break;
                        }
                        default:
                            printf("Expressão não suportada: %c\n", expr);
                            exit(1);
                    }

                    //Tenho que verificar se vem algo depois. Um | * | + | - | para fazer a operação
                    if(!is_call){
                        
                        char operador;
                        int rop2 = le_char_mesma_linha(f, &operador);
                        if (rop2 != 1) {
                            printf("Erro de sintaxe: esperava operador (+,-,*) depois do primeiro operando em atribuicao de v%c\n", var);
                            exit(1);
                        }
                        if (operador != '+' && operador != '-' && operador != '*') {
                            printf("Erro de sintaxe: operador invalido '%c' na atribuicao de v%c (esperava +, - ou *)\n",
                                operador, var);
                            exit(1);
                        }
    
                        printf(" ------ Operador  na expressão : %c ---- \n", operador);   //Se chegou até aqui é por que tem um operador
    
                        //Devmos verificar se tem algo após o operador
                        char var2;
                        int r2 = le_char_mesma_linha(f, &var2);
                        if (r2 != 1) {
                            printf("Erro de sintaxe: faltou segundo operando depois de '%c' na atribuicao de v%c\n", operador, var);
                            exit(1);
                        }
    
                        switch (var2) {
    
                            case 'v': {   // segundo operando é variável local vY
                                char restante_var2;
                                int rrest = le_char_mesma_linha(f, &restante_var2);
                                if (rrest != 1) {
                                    printf("Erro: faltou indice da variavel depois de 'v' no segundo operando (vX = ... %c v?)\n", operador);
                                    exit(1);
                                }
    
                                if (restante_var2 < '0' || restante_var2 > '4') {
                                    printf("Variavel nao suportada no segundo operando: v%c\n", restante_var2);
                                    exit(1);
                                }
    
                                int offset_var2 = restante_var2 - '0';
                                int offset_var_buff2 = offset_var(offset_var2);
                                signed char off2_byte = (signed char)offset_var_buff2;
                                printf("Segundo operando: v%c, offset %02x\n", restante_var2, (unsigned char)off2_byte);
    
                                // Agora gera a operação: %eax (primeiro operando) op vY
                                switch (operador) {
                                    case '+': {
                                        // addl -off2(%rbp), %eax  → 03 45 disp8
                                        unsigned char add_buf[] = { 0x03, 0x45, (unsigned char)off2_byte };
                                        grava_bytes(code, add_buf, &code_pos, sizeof(add_buf));
                                        break;
                                    }
                                    case '-': {
                                        // subl -off2(%rbp), %eax  → 2b 45 disp8
                                        unsigned char sub_buf[] = { 0x2b, 0x45, (unsigned char)off2_byte };
                                        grava_bytes(code, sub_buf, &code_pos, sizeof(sub_buf));
                                        break;
                                    }
                                    case '*': {
                                        // imull -off2(%rbp), %eax → 0f af 45 disp8
                                        unsigned char imul_buf[] = { 0x0f, 0xaf, 0x45, (unsigned char)off2_byte };
                                        grava_bytes(code, imul_buf, &code_pos, sizeof(imul_buf));
                                        break;
                                    }
                                    default:
                                        printf("Bug interno: operador '%c' inesperado em expr2==v\n", operador);
                                        exit(1);
                                }
    
                                break;
                            }
    
                            case 'p': {
                                // segundo operando é p0
                                char idx2;
                                int rp2 = le_char_mesma_linha(f, &idx2);
                                if (rp2 != 1) {
                                    printf("Erro: faltou indice depois de 'p' no segundo operando\n");
                                    exit(1);
                                }
                                if (idx2 != '0') {
                                    printf("Parametro nao suportado no segundo operando: p%c (apenas p0)\n", idx2);
                                    exit(1);
                                }
                            
                                printf("Segundo operando: p0\n");
                            
                                switch (operador) {
                                    case '+': {
                                        // add %edi, %eax  -> 01 f8
                                        unsigned char buf[] = { 0x01, 0xf8 };
                                        grava_bytes(code, buf, &code_pos, sizeof(buf));
                                        break;
                                    }
                                    case '-': {
                                        // sub %edi, %eax  -> 29 f8
                                        unsigned char buf[] = { 0x29, 0xf8 };
                                        grava_bytes(code, buf, &code_pos, sizeof(buf));
                                        break;
                                    }
                                    case '*': {
                                        // imul %eax, %edi  -> 0f af c7
                                        // (eax = eax * edi)
                                        unsigned char buf[] = { 0x0f, 0xaf, 0xc7 };
                                        grava_bytes(code, buf, &code_pos, sizeof(buf));
                                        break;
                                    }
                                    default:
                                        printf("Error: operador '%c' inesperado em segundo operando p0\n", operador);
                                        exit(1);
                                }
                            
                                break;
                            }
    
                            case '$': {
                                int val;
                                if (fscanf(f, " %d", &val) != 1) {
                                    printf("Constante invalida no segundo operando\n");
                                    exit(1);
                                }
                                printf("Segundo operando: constante %d\n", val);
                            
                                switch (operador) {
                                    case '+': {
                                        // add $imm32, %eax  -> 05 imm32
                                        unsigned char buf[5];
                                        buf[0] = 0x05;
                                        buf[1] = (unsigned char)(val & 0xff);
                                        buf[2] = (unsigned char)((val >> 8) & 0xff);
                                        buf[3] = (unsigned char)((val >> 16) & 0xff);
                                        buf[4] = (unsigned char)((val >> 24) & 0xff);
                                        grava_bytes(code, buf, &code_pos, 5);
                                        break;
                                    }
                                    case '-': {
                                        // sub $imm32, %eax  -> 2d imm32
                                        unsigned char buf[5];
                                        buf[0] = 0x2d;
                                        buf[1] = (unsigned char)(val & 0xff);
                                        buf[2] = (unsigned char)((val >> 8) & 0xff);
                                        buf[3] = (unsigned char)((val >> 16) & 0xff);
                                        buf[4] = (unsigned char)((val >> 24) & 0xff);
                                        grava_bytes(code, buf, &code_pos, 5);
                                        break;
                                    }
                                    case '*': {
                                        // imul %eax, %eax, imm32  -> 69 c0 imm32
                                        unsigned char buf[6];
                                        buf[0] = 0x69;
                                        buf[1] = 0xc0;  // modrm: dest eax, fonte eax
                                        buf[2] = (unsigned char)(val & 0xff);
                                        buf[3] = (unsigned char)((val >> 8) & 0xff);
                                        buf[4] = (unsigned char)((val >> 16) & 0xff);
                                        buf[5] = (unsigned char)((val >> 24) & 0xff);
                                        grava_bytes(code, buf, &code_pos, 6);
                                        break;
                                    }
                                    default:
                                        printf("Error: operador '%c' inesperado em segundo operando constante\n", operador);
                                        exit(1);
                                }
                            
                                break;
                            }
                            
    
                            default:
                                printf("Erro de sintaxe: segundo operando '%c' invalido na atribuicao de v%c = %c %c\n",
                                    var2, var, operador, var2);
                                exit(1);
                        }
                    }
                    

                //Finalmente grava o código que salva o valor da primeira variável %eax no espaço de RA correta\o (vX (guardo esse aqui!) = alguam coisa) 
                int var_buff = var - '0';
                int offset = offset_var(var_buff);
                printf("Offset da variável v%c: %02x\n", var, offset); 
                signed char offset_buf = (signed char)offset;
                unsigned char code_buf[] = { 0x89, 0x45, offset_buf };
                grava_bytes(code, code_buf, &code_pos,sizeof(code_buf));
                break;
            }
            case 'z': {
                printf("\n----------- Encontrei 'z' -----------\n");
                ultimo_cmd_foi_ret = 0;
                char resto[254];
                if (fscanf(f, "%254[^ \t\r\n]", resto) != 1) {
                    printf("Nao consegui ler o resto apos 'z'\n");
                    exit(1);
                }
    
                if (strcmp(resto, "ret") != 0) {
                    printf("Comando inesperado apos 'z': %s (esperava 'ret')\n", resto);
                    exit(1);
                }
    
                printf("Achei a palavra !zret!\n\n");
    
                /* --------- Primeiro varpc (condição) --------- */
                char sym1;
                int r1 = le_char_mesma_linha(f, &sym1);
                if (r1 != 1) {
                    printf("Erro de sintaxe: faltou primeiro operando em zret\n");
                    exit(1);
                }
                printf("Primeiro varpc de zret: %c\n", sym1);
    
                // gerar primeiro varpc em %eax
                switch (sym1) {
                    case 'v': {
                        char idx;
                        int rv = le_char_mesma_linha(f, &idx);
                        if (rv != 1) {
                            printf("Erro de sintaxe: faltou indice depois de 'v' em zret\n");
                            exit(1);
                        }
                        if (idx < '0' || idx > '4') {
                            printf("Variavel nao suportada em zret: v%c\n", idx);
                            exit(1);
                        }
                        int num = idx - '0';
                        int off = offset_var(num);
                        signed char offb = (signed char)off;
                        printf("zret: primeiro operando v%c, offset %02x\n", idx, (unsigned char)offb);
                        unsigned char buf[] = { 0x8b, 0x45, (unsigned char)offb }; // mov -off(%rbp),%eax
                        grava_bytes(code, buf, &code_pos, sizeof(buf));
                        break;
                    }
                    case 'p': {
                        char idx;
                        int rp = le_char_mesma_linha(f, &idx);
                        if (rp != 1) {
                            printf("Erro de sintaxe: faltou indice depois de 'p' em zret\n");
                            exit(1);
                        }
                        if (idx != '0') {
                            printf("Parametro nao suportado em zret: p%c (apenas p0)\n", idx);
                            exit(1);
                        }
                        printf("zret: primeiro operando p0\n");
                        // mov %edi,%eax
                        unsigned char buf[] = { 0x89, 0xf8 };
                        grava_bytes(code, buf, &code_pos, sizeof(buf));
                        break;
                    }
                    case '$': {
                        int val;
                        if (fscanf(f, " %d", &val) != 1) {
                            printf("Constante invalida em zret (primeiro operando)\n");
                            exit(1);
                        }
                        printf("zret: primeiro operando constante %d\n", val);
                        unsigned char buf[5];
                        buf[0] = 0xb8;
                        buf[1] = (unsigned char)(val & 0xff);
                        buf[2] = (unsigned char)((val >> 8) & 0xff);
                        buf[3] = (unsigned char)((val >> 16) & 0xff);
                        buf[4] = (unsigned char)((val >> 24) & 0xff);
                        grava_bytes(code, buf, &code_pos, 5);
                        break;
                    }
                    default:
                        printf("Primeiro operando de zret invalido: %c (esperava v | p | $)\n", sym1);
                        exit(1);
                }
    
                /* --------- test %eax,%eax + jne --------- */
    
                // test %eax,%eax  (85 c0)
                unsigned char test_buf[] = { 0x85, 0xc0 };
                grava_bytes(code, test_buf, &code_pos, sizeof(test_buf));
    
                // jne Lcontinuar  (0f 85 disp32) com disp32 placeholder
                int pos_jne = code_pos;  // posição do 0f
                unsigned char jne_buf[] = { 0x0f, 0x85, 0x00, 0x00, 0x00, 0x00 };
                grava_bytes(code, jne_buf, &code_pos, sizeof(jne_buf));
    
                /* --------- Segundo varpc (valor de retorno) --------- */
    
                char sym2;
                int r2 = le_char_mesma_linha(f, &sym2);
                if (r2 != 1) {
                    printf("Erro de sintaxe: faltou segundo operando em zret\n");
                    exit(1);
                }
                printf("Segundo varpc de zret: %c\n", sym2);
    
                // gerar segundo varpc em %eax (valor de retorno se cond == 0)
                switch (sym2) {
                    case 'v': {
                        char idx;
                        int rv = le_char_mesma_linha(f, &idx);
                        if (rv != 1) {
                            printf("Erro de sintaxe: faltou indice depois de 'v' no segundo operando de zret\n");
                            exit(1);
                        }
                        if (idx < '0' || idx > '4') {
                            printf("Variavel nao suportada em zret (2o operando): v%c\n", idx);
                            exit(1);
                        }
                        int num = idx - '0';
                        int off = offset_var(num);
                        signed char offb = (signed char)off;
                        printf("zret: segundo operando v%c, offset %02x\n", idx, (unsigned char)offb);
                        unsigned char buf[] = { 0x8b, 0x45, (unsigned char)offb };
                        grava_bytes(code, buf, &code_pos, sizeof(buf));
                        break;
                    }
                    case 'p': {
                        char idx;
                        int rp = le_char_mesma_linha(f, &idx);
                        if (rp != 1) {
                            printf("Erro de sintaxe: faltou indice depois de 'p' no segundo operando de zret\n");
                            exit(1);
                        }
                        if (idx != '0') {
                            printf("Parametro nao suportado em zret (2o operando): p%c (apenas p0)\n", idx);
                            exit(1);
                        }
                        printf("zret: segundo operando p0\n");
                        unsigned char buf[] = { 0x89, 0xf8 }; // mov %edi,%eax
                        grava_bytes(code, buf, &code_pos, sizeof(buf));
                        break;
                    }
                    case '$': {
                        int val;
                        if (fscanf(f, " %d", &val) != 1) {
                            printf("Constante invalida em zret (segundo operando)\n");
                            exit(1);
                        }
                        printf("zret: segundo operando constante %d\n", val);
                        unsigned char buf[5];
                        buf[0] = 0xb8;
                        buf[1] = (unsigned char)(val & 0xff);
                        buf[2] = (unsigned char)((val >> 8) & 0xff);
                        buf[3] = (unsigned char)((val >> 16) & 0xff);
                        buf[4] = (unsigned char)((val >> 24) & 0xff);
                        grava_bytes(code, buf, &code_pos, 5);
                        break;
                    }
                    default:
                        printf("Segundo operando de zret invalido: %c (esperava v | p | $)\n", sym2);
                        exit(1);
                }
    
                /* --------- Epílogo condicional (leave; ret) --------- */
    
                unsigned char leave_ret[] = { 0xc9, 0xc3 };
                grava_bytes(code, leave_ret, &code_pos, sizeof(leave_ret));
    
                /* --------- Ajustar o deslocamento do JNE --------- */
    
                int end_block = code_pos;           // posição depois de leave;ret
                int origem = pos_jne + 6;           // endereço "depois" da instrução jne
                int disp = end_block - origem;      // salto até depois do bloco zret
    
                code[pos_jne + 2] = (unsigned char)(disp & 0xff);
                code[pos_jne + 3] = (unsigned char)((disp >> 8) & 0xff);
                code[pos_jne + 4] = (unsigned char)((disp >> 16) & 0xff);
                code[pos_jne + 5] = (unsigned char)((disp >> 24) & 0xff);
    
                printf("zret gerado: jump de %d bytes ate Lcontinuar\n", disp);
                printf("\n-------------------------------------\n\n");
                break;
            }    
            default:
                break;
        }

    }

    if (lista_raiz->func == NULL) {
        // Nenhuma function encontrada: por segurança
        *entry = NULL;
        return;
    }

    NoFuncInfo *at = lista_raiz->func;
    while (at->prox_func != NULL)
        at = at->prox_func;

    *entry = (funcp)(code + at->endereco_fun);



    for(int i = 0; i < code_pos; i++){
        printf("code[%d] = 0x%02x\n", i, code[i]);
    }
    printf("\n ---- Local das funções guardadas na lista encadead ---- \n");
    exibeLista(lista_raiz);
}

