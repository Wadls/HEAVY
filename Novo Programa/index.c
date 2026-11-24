#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ARQ_USUARIO   "usuarios.csv"   /* arquivo usuario */
#define ARQ_EXERCICIO   "exercicios.csv"   /* arquivo exercicio */
#define ARQ_GM   "grupos musculares.csv"   /* arquivo grupo muscular */
#define ARQ_TREINO   "treinos.csv"   /* arquivo treinos */
#define ARQ_TEMP  "temp.csv"     /* arquivo auxiliar */
#define MAX_NOME  80
#define MAX_EXERCICIOS  12
#define COD_INICIAL  1000
#define TAM_BUFFER 512
#define MAX_CAMPOS 10


typedef struct {
    int cod;
    char nome[MAX_NOME];
    int idade;
    float peso;
    float altura;
    int rotinaDeTreino[7];
}Usuario;

typedef struct {
    int cod;
    char nome[MAX_NOME];
    int codGrupoMuscular;
    int series;
}Exercicio;

typedef struct {
    int cod;
    char nome[MAX_NOME];
}GrupoMuscular;

typedef struct {
    int cod;
    char nome[MAX_NOME];
    int codExercicios[MAX_EXERCICIOS];
}Treino;


//Protótipos das Funções:
void limpar_tela(void); 
void limpar_buffer(void);
void garantir_arquivo(void);
int prox_codigo(int tipo);
void excluirArquivo(int tipo);

//Manipulação de Objetos
void listar(int tipo);
void cadastrar(int tipo);
void escolherObjeto(int codigos[], int tipo, int tamanho);
void atualizar(int tipo);





const char *ARQUIVOS[]   = {NULL, ARQ_USUARIO, ARQ_EXERCICIO, ARQ_GM, ARQ_TREINO}; 
const char *CABECALHOS[] = {NULL, "cod;nome;idade;peso;altura;rotina", "cod;nome;grupo muscular;series",
                                   "cod;nome",
                                   "cod;nome;exercicios"};
const char *SEMANA[] = {"Segunda-Feira", "Terça-Feira","Quarta-Feira","Quinta-Feira","Sexta-Feira","Sabado","Domingo"};
const char *NOMES[] = {NULL, "Usuário","Exercicio","Grupo Muscular","Treino"};
const char *SEMANA_ABREV[] = {"seg", "ter", "qua", "qui", "sex", "sab", "dom"};


void limpar_tela(void) {
    printf("\033[2J\033[H");  /* limpa a tela e posiciona o cursor no topo */
}
void limpar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;  
}
void garantir_arquivo(void) {
    for (int i = 1; i <= 4; i++) {
        FILE *f = fopen(ARQUIVOS[i], "r");
        if (f) { fclose(f); continue; }
        if ((f = fopen(ARQUIVOS[i], "w"))) { fprintf(f, "%s\n", CABECALHOS[i]); fclose(f);}
    }
}
int prox_codigo(int tipo){//Essa função percorre o arquivo e retorna o próximo código correspondente de forma dinâmica

    char linha[TAM_BUFFER]; 
    if (tipo < 1 || tipo > 4) return 0;
    FILE *arq = fopen(ARQUIVOS[tipo], "r"); // 1 = Aluno | 2 = Exercicio | 3 = GrupoMuscular | 4 = Treino 
    int cod = 0;
    int maior = COD_INICIAL; //1000
    if (arq == NULL) return COD_INICIAL; //Retorna 1000 se não achou o arquivo
    
    fgets(linha, sizeof(linha), arq);//Pula o Cabeçalho
    char *token;
    while (fgets(linha, sizeof(linha), arq) != NULL) {
        token = strtok(linha, ";");
        if (token != NULL) cod = atoi(token);
        if (cod > maior) maior = cod;
    }
    fclose(arq);
    return maior + 1; //Descobrindo o Maior ele retorna o próximo número 
}
void excluirArquivo(int tipo){ // 1 = Aluno | 2 = Exercicio | 3 = GrupoMuscular | 4 = Treino 
    if (tipo < 1 || tipo > 4) return;
    remove(ARQUIVOS[tipo]);
    rename(ARQ_TEMP, ARQUIVOS[tipo]);
    //Após apagar o arquivo temporário, é preciso recria-lo, nessa etapa  
    garantir_arquivo();
}

void escolherObjeto(int codigos[], int tipo, int tamanho) {
    if (tipo < 1 || tipo > 4 || tamanho <= 0) return;

    char linha[TAM_BUFFER];
    const char *nomeDoObjeto = NOMES[tipo];
    int ehRotina = (tipo == 4 && tamanho == 7);   /* escolha de treino para a semana inteira */

    for (int i = 0; i < tamanho; i++) {
        int objEscolhido = 0, objExiste = 0;

        listar(tipo);

        if (ehRotina) {
            printf("\n%s: digite o código do treino.\n", SEMANA[i]);
            printf("0 = não treinar | número que não existe = cadastrar novo treino\n> ");
        } else {
            printf("Escreva o código do(a) %s.\nSe não estiver registrado, digite 0: ", nomeDoObjeto);
        }

        if (scanf("%d", &objEscolhido) != 1) {
            if (ehRotina) {                        /* entrada inválida: repete o mesmo dia */
                limpar_buffer();
                printf("Valor inválido, tente de novo.\n");
                i--;
                continue;
            }
            objEscolhido = 0;
        }
        limpar_buffer();

        /* rotina: 0 = dia de descanso */
        if (ehRotina && objEscolhido == 0) {
            codigos[i] = 0;
            printf("%s: sem treino.\n", SEMANA[i]);
            continue;
        }

        if (objEscolhido != 0) {
            FILE *arq = fopen(ARQUIVOS[tipo], "r");
            if (arq == NULL) {
                printf("Erro ao abrir o arquivo de %s.\n", nomeDoObjeto);
                return;
            }

            fgets(linha, sizeof(linha), arq);      /* pula cabeçalho */
            while (fgets(linha, sizeof(linha), arq) != NULL) {
                int cod;
                if (sscanf(linha, "%d", &cod) == 1 && cod == objEscolhido) {
                    objExiste = 1;
                    break;
                }
            }
            fclose(arq);
        }

        if (objExiste) {
            codigos[i] = objEscolhido;
            printf("%s adicionado com sucesso!\n", nomeDoObjeto);
        } else {
            printf("Cadastre o novo %s\n", nomeDoObjeto);
            cadastrar(tipo);
            codigos[i] = prox_codigo(tipo) - 1;
        }
    }
}
char *conversorCodObjeto(const int codigos[], int tipo, int tamanho) {
    static char nomes[TAM_BUFFER];
    nomes[0] = '\0';
    if (tipo < 1 || tipo > 4 || tamanho <= 0) return nomes;

    FILE *arq = fopen(ARQUIVOS[tipo], "r");
    if (arq == NULL) return nomes;

    char linha[TAM_BUFFER];

    for (int i = 0; i < tamanho; i++) {
        if (codigos[i] <= 0) continue;

        char nomeEncontrado[MAX_NOME] = "(nao encontrado)";

        rewind(arq);
        fgets(linha, sizeof(linha), arq);       /* pula cabeçalho */

        while (fgets(linha, sizeof(linha), arq) != NULL) {
            int cod;
            char nome[MAX_NOME];
            if (sscanf(linha, "%d;%79[^;\r\n]", &cod, nome) == 2 && cod == codigos[i]) {
                strcpy(nomeEncontrado, nome);
                break;
            }
        }

        if (nomes[0] != '\0')
            strncat(nomes, "; ", sizeof(nomes) - strlen(nomes) - 1);
        strncat(nomes, nomeEncontrado, sizeof(nomes) - strlen(nomes) - 1);
    }
    fclose(arq);
    return nomes;
}



void cadastrar(int tipo) {
   
    limpar_tela();
    FILE *arq = NULL;
    char cab[TAM_BUFFER]; //Atribuo o cabeçalho específico de cada objeto
    strcpy(cab, CABECALHOS[tipo]);
    char linha[TAM_BUFFER]; //Crio uma nova linha em branco para colocar dentro do meu arquivo
    char valor[MAX_NOME];   

    snprintf(linha, sizeof(linha), "%d", prox_codigo(tipo)); //Primeiro eu escrevo o codigo do objeto
    strtok(cab, ";");/* pula o campo "cod" */
    
    //A partir daqui, eu vou passar por cada campo do cabeçalho, e atribui-los a uma linha em branco
    char *campo;
    while ((campo = strtok(NULL, ";")) != NULL) { //
        if (strcmp(campo, "rotina") == 0){ //Se o campo se chamar rotina
            /* rotina: um código de treino para cada dia da semana */
            int rotina[7] = {0};
            escolherObjeto(rotina, 4, 7);//array rotina recebe os valores, tipo 4:Objeto treino, 7 quantidades de treinos que rotina recebe
            strncat(linha, ";", sizeof(linha) - strlen(linha) - 1);
            for (int i = 0; i < 7; i++) {
                char num[16];
                snprintf(num, sizeof(num), i == 0 ? "%d" : ",%d", rotina[i]);
                strncat(linha, num, sizeof(linha) - strlen(linha) - 1);
            }
            break;
        }
        if (strcmp(campo, "grupo muscular") == 0){ //Se o campo se chamar grupo muscular
            /* grupo muscular, 1 codigo só */
            int grupoMuscular[1] = {0}; // 1 = Aluno | 2 = Exercicio | 3 = Grupo Muscular | 4 = Treino 
            escolherObjeto(grupoMuscular, 3, 1);// tipo 3:Grupo Muscular
            strncat(linha, ";", sizeof(linha) - strlen(linha) - 1);
            char num[16];
            snprintf(num, sizeof(num), "%d", grupoMuscular[0]);//Printf dentro de uma string
            strncat(linha, num, sizeof(linha) - strlen(linha) - 1);
            continue; //Ainda falta registrar séries
        }
        if (strcmp(campo, "exercicios") == 0){ //Se o campo se chamar exercicios
            int qtd_ex = 0;
            printf("Quantos exercícios (1 a %d)? ", MAX_EXERCICIOS);
            if (scanf("%d", &qtd_ex) != 1){ limpar_buffer(); return; }
            if (qtd_ex > 12) {
                printf("O valor máximo é 12\n");
                qtd_ex = 12;
            }
            if (qtd_ex < 1) {
                printf("O valor mínimo é 1\n");
                qtd_ex = 1;
            }

            int exercicios[MAX_EXERCICIOS] = {0}; //Inicializo um array com codigos de exercicios
            escolherObjeto(exercicios, 2, qtd_ex);// tipo 2:Exercicio

            strncat(linha, ";", sizeof(linha) - strlen(linha) - 1);
            int qtd = 0;
            for (int i = 0; i < qtd_ex; i++) {
                if (exercicios[i] == 0) continue;
                char num[16];
                snprintf(num, sizeof(num), qtd == 0 ? "%d" : ",%d", exercicios[i]);
                strncat(linha, num, sizeof(linha) - strlen(linha) - 1);
                qtd++;
            }
            break;
        }
        
        printf("%s: ", campo);// printa o valor do campo específico do cabeçalho, se escrever um valor inválido interrompe o cadastro;
        if (fgets(valor, sizeof(valor), stdin) == NULL) return;
        valor[strcspn(valor, "\n")] = '\0';

        //Se o gênio resolver colocar ; ao atribuir dos valores
        if (strchr(valor, ';') != NULL) {
            printf("Não use ';' nos dados.\n");
            return;
        }
        strncat(linha, ";", sizeof(linha) - strlen(linha) - 1);
        strncat(linha, valor, sizeof(linha) - strlen(linha) - 1);

    }
    /* grava só no final, com o arquivo aberto o mínimo de tempo */
     switch (tipo){ // 1 = Aluno | 2 = Exercicio | 3 = GrupoMuscular | 4 = Treino 
        case 1:{ 
            arq = fopen(ARQ_USUARIO, "a");
            break;
        }
        case 2:{ 
            arq = fopen(ARQ_EXERCICIO, "a");
            break;
        }
        case 3:{ 
            arq = fopen(ARQ_GM, "a");
            break;
        }
        case 4:{ 
            arq = fopen(ARQ_TREINO, "a");
            break;
        }
        
        default:
            break;
        }
        if (arq == NULL) return;
        fprintf(arq, "%s\n", linha);
        fclose(arq);
        printf("Cadastrado com sucesso\n");   
}

void listar(int tipo) {
    if (tipo < 1 || tipo > 4) return;

    FILE *arq = fopen(ARQUIVOS[tipo], "r");
    if (arq == NULL) {
        printf("Erro ao abrir o arquivo de %s.\n", NOMES[tipo]);
        return;
    }

    char linha[TAM_BUFFER], cab[TAM_BUFFER];
    char *campos[MAX_CAMPOS];
    int nCampos = 0;

    /* cabeçalho: separa os nomes das colunas (a "rotina" não aparece, vai em linha própria) */
    if (fgets(cab, sizeof(cab), arq) != NULL) {
        cab[strcspn(cab, "\r\n")] = '\0';

        char *p = cab;
        while (nCampos < MAX_CAMPOS) {
            campos[nCampos++] = p;
            p = strchr(p, ';');
            if (p == NULL) break;
            *p++ = '\0';
        }

        printf("\n");
        int primeiro = 1;
        for (int i = 0; i < nCampos; i++) {
            if (strcmp(campos[i], "rotina") == 0) continue;
            printf("%s%s", primeiro ? "" : " | ", campos[i]);
            primeiro = 0;
        }
        printf("\n---------------------------------------------------\n");
    }

    /* registros */
    while (fgets(linha, sizeof(linha), arq) != NULL) {
        linha[strcspn(linha, "\r\n")] = '\0';
        if (linha[0] == '\0') continue;           /* ignora linha em branco */

        int dias[7] = {0}, nDias = 0, primeiro = 1, i = 0;
        char *p = linha;

        printf("\n");
        while (p != NULL && i < nCampos) {
            char *valor = p;                      /* separa o próximo campo */
            p = strchr(p, ';');
            if (p) *p++ = '\0';
            const char *campo = campos[i++];

            int ehRotina = strcmp(campo, "rotina") == 0;
            int ehGM     = strcmp(campo, "grupo muscular") == 0;
            int ehEx     = strcmp(campo, "exercicios") == 0;

            if (!ehRotina) {
                printf("%s", primeiro ? "" : " | ");
                primeiro = 0;
            }

            if (ehRotina || ehGM || ehEx) {
                /* "1001,0,1002" -> int[] */
                int cods[MAX_EXERCICIOS], n = 0;
                char *fim;
                const char *s = valor;
                while (*s && n < MAX_EXERCICIOS) {
                    cods[n] = (int)strtol(s, &fim, 10);
                    if (fim == s) break;
                    n++;
                    s = fim;
                    if (*s == ',') s++;
                    else break;
                }

                if (ehRotina) {                   /* guarda para imprimir abaixo */
                    nDias = n > 7 ? 7 : n;
                    for (int d = 0; d < nDias; d++) dias[d] = cods[d];
                }
                else {
                    printf("%s", conversorCodObjeto(cods, ehEx ? 2 : 3, n));
                }
            }
            else {
                printf("%s", valor);
            }
        }

        /* linha da rotina, logo abaixo do usuário */
        if (nDias > 0) {
            printf("\n   ");
            for (int d = 0; d < nDias; d++) {
                printf("%s%s: %s", d ? " | " : "", SEMANA_ABREV[d],
                       dias[d] > 0 ? conversorCodObjeto(&dias[d], 4, 1) : "descanso");
            }
        }
    }
    printf("\n");
    fclose(arq);
}

void atualizar(int tipo) {
    if (tipo < 1 || tipo > 4) return;

    FILE *arq = fopen(ARQUIVOS[tipo], "r");
    FILE *temp = fopen(ARQ_TEMP, "w");
    if (arq == NULL || temp == NULL) {
        printf("Erro ao abrir os arquivos.\n");
        if (arq) fclose(arq);
        if (temp) fclose(temp);
        return;
    }

    char linha[TAM_BUFFER], cab[TAM_BUFFER], novo[TAM_BUFFER];
    char *campos[MAX_CAMPOS], *valores[MAX_CAMPOS];
    int nCampos = 0, cod = 0, achou = 0, alterado = 0;

    listar(tipo);
    printf("Cod do(a) %s a atualizar: ", NOMES[tipo]);
    if (scanf("%d", &cod) != 1) cod = 0;
    limpar_buffer();

    /* cabeçalho: copia pro temp e separa os nomes das colunas */
    if (fgets(cab, sizeof(cab), arq) != NULL) {
        fputs(cab, temp);
        cab[strcspn(cab, "\r\n")] = '\0';
        char *p = cab;
        while (nCampos < MAX_CAMPOS) {
            campos[nCampos++] = p;
            p = strchr(p, ';');
            if (p == NULL) break;
            *p++ = '\0';
        }
    }

    while (fgets(linha, sizeof(linha), arq) != NULL) {
        linha[strcspn(linha, "\r\n")] = '\0';
        if (linha[0] == '\0') continue;                 /* ignora linha em branco */

        /* separa os valores da linha, um para cada campo do cabeçalho */
        char *p = linha;
        for (int i = 0; i < nCampos; i++) {
            valores[i] = p ? p : "";
            if (p) {
                p = strchr(p, ';');
                if (p) *p++ = '\0';
            }
        }

        if (atoi(valores[0]) == cod && !achou) {
            achou = 1;

            /* menu montado a partir do cabeçalho */
            limpar_tela();
            printf("Pretende atualizar o que?\n");
            printf("+----------------------+\n");
            printf(">>cod: %d (imutavel)\n", cod);
            for (int i = 1; i < nCampos; i++) {
                int ref = !strcmp(campos[i], "rotina") ? 4 :
                          !strcmp(campos[i], "grupo muscular") ? 3 :
                          !strcmp(campos[i], "exercicios") ? 2 : 0;
                printf("%d- %s: ", i, campos[i]);
                if (ref) {                              /* códigos -> nomes */
                    int cods[MAX_EXERCICIOS], n = 0;
                    char *fim;
                    const char *s = valores[i];
                    while (*s && n < MAX_EXERCICIOS) {
                        cods[n] = (int)strtol(s, &fim, 10);
                        if (fim == s) break;
                        n++;
                        s = fim;
                        if (*s == ',') s++;
                        else break;
                    }
                    printf("%s\n", conversorCodObjeto(cods, ref, n));
                } else {
                    printf("%s\n", valores[i]);
                }
            }
            printf("0- Nada (sair)\n");
            printf("+----------------------+\n");

            int escolha = 0;
            if (scanf("%d", &escolha) != 1) escolha = 0;
            limpar_buffer();

            if (escolha >= 1 && escolha < nCampos) {
                const char *campo = campos[escolha];
                int ok = 1;
                novo[0] = '\0';

                if (strcmp(campo, "rotina") == 0) {
                    int rot[7] = {0}, len = 0;
                    escolherObjeto(rot, 4, 7);
                    for (int j = 0; j < 7; j++)
                        len += snprintf(novo + len, sizeof(novo) - len, j ? ",%d" : "%d", rot[j]);
                }
                else if (strcmp(campo, "grupo muscular") == 0) {
                    int gm[1] = {0};
                    escolherObjeto(gm, 3, 1);
                    snprintf(novo, sizeof(novo), "%d", gm[0]);
                }
                else if (strcmp(campo, "exercicios") == 0) {
                    int qtd = 0, ex[MAX_EXERCICIOS] = {0}, len = 0, usados = 0;
                    printf("Quantos exercicios (1 a %d)? ", MAX_EXERCICIOS);
                    if (scanf("%d", &qtd) != 1) qtd = 1;
                    limpar_buffer();
                    if (qtd > MAX_EXERCICIOS) qtd = MAX_EXERCICIOS;
                    if (qtd < 1) qtd = 1;
                    escolherObjeto(ex, 2, qtd);
                    for (int j = 0; j < qtd; j++) {
                        if (ex[j] == 0) continue;
                        len += snprintf(novo + len, sizeof(novo) - len, usados ? ",%d" : "%d", ex[j]);
                        usados++;
                    }
                }
                else {                                  /* campo de texto/número */
                    printf("Novo valor de %s: ", campo);
                    if (fgets(novo, sizeof(novo), stdin) == NULL) ok = 0;
                    else novo[strcspn(novo, "\r\n")] = '\0';
                    if (ok && strchr(novo, ';') != NULL) {
                        printf("Não use ';' nos dados.\n");
                        ok = 0;
                    }
                }

                if (ok) {
                    valores[escolha] = novo;
                    alterado = 1;
                }
            } else {
                printf("Nada alterado.\n");
            }
        }

        /* grava a linha (alterada ou não) */
        for (int i = 0; i < nCampos; i++)
            fprintf(temp, "%s%s", i ? ";" : "", valores[i]);
        fprintf(temp, "\n");
    }
    fclose(arq);
    fclose(temp);

    if (alterado) {
        excluirArquivo(tipo);
        printf("%s atualizado(a) com sucesso!\n", NOMES[tipo]);
    } else {
        remove(ARQ_TEMP);
        if (!achou) printf("Cod %d nao encontrado.\n", cod);
    }
}

void excluir(int tipo) {
    if (tipo < 1 || tipo > 4) return;

    static const int PAI[] = {0, 0, 4, 2, 1};   /* arquivo que referencia o objeto */
    static const int COL[] = {0, 0, 2, 2, 5};   /* coluna onde o código aparece lá */

    char linha[TAM_BUFFER];
    int cod = 0, achou = 0, emUso = 0;

    listar(tipo);
    printf("Cod do(a) %s a excluir: ", NOMES[tipo]);
    if (scanf("%d", &cod) != 1) cod = 0;
    limpar_buffer();
    if (cod <= 0) {
        printf("Codigo invalido.\n");
        return;
    }

    /* bloqueia se algum registro de outro tipo ainda usa esse código */
    if (tipo > 1) {
        FILE *f = fopen(ARQUIVOS[PAI[tipo]], "r");
        if (f) {
            fgets(linha, sizeof(linha), f);                 /* pula cabeçalho */
            while (!emUso && fgets(linha, sizeof(linha), f)) {
                char *p = linha;
                for (int c = 0; c < COL[tipo] && p; c++) {  /* anda até a coluna */
                    p = strchr(p, ';');
                    if (p) p++;
                }
                while (p && *p && *p != ';' && *p != '\n' && *p != '\r') {
                    char *fim;
                    int v = (int)strtol(p, &fim, 10);       /* "1001,0,1002" */
                    if (fim == p) break;
                    if (v == cod) { emUso = 1; break; }
                    p = fim;
                    if (*p == ',') p++;
                    else break;
                }
            }
            fclose(f);
        }
    }
    if (emUso) {
        printf("Exclusao negada: %s esta em uso em %s.\n", NOMES[tipo], NOMES[PAI[tipo]]);
        return;
    }

    /* copia tudo para o temp, menos a linha do código */
    FILE *f = fopen(ARQUIVOS[tipo], "r");
    FILE *t = fopen(ARQ_TEMP, "w");
    if (f == NULL || t == NULL) {
        printf("Erro ao abrir os arquivos.\n");
        if (f) fclose(f);
        if (t) fclose(t);
        return;
    }

    fgets(linha, sizeof(linha), f);
    fputs(linha, t);                                        /* cabeçalho */
    while (fgets(linha, sizeof(linha), f)) {
        if (linha[0] == '\n' || linha[0] == '\r') continue; /* descarta linha em branco */
        if (atoi(linha) == cod) achou = 1;
        else fputs(linha, t);
    }
    fclose(f);
    fclose(t);

    if (achou) {
        excluirArquivo(tipo);
        printf("%s excluido(a) com sucesso!\n", NOMES[tipo]);
    } else {
        remove(ARQ_TEMP);
        printf("Cod %d nao encontrado.\n", cod);
    }
}

int main(void) {
    garantir_arquivo();

    int op = 0, sub = 0;
    do {
        limpar_tela();
        printf("==================================\n");
        printf("    > >   SISTEMA DE TREINO   < <  \n");
        printf("==================================\n");
        printf("| 1 - Usuarios                   |\n");
        printf("| 2 - Exercicios                 |\n");
        printf("| 3 - Grupos Musculares          |\n");
        printf("| 4 - Treinos                    |\n");
        printf("| 5 - Sair                       |\n");
        printf("==================================\n> ");

        if (scanf("%d", &op) != 1) op = 0;
        limpar_buffer();

        if (op >= 1 && op <= 4) {                /* op já é o tipo do objeto */
            do {
                limpar_tela();
                printf("==================================\n");
                printf("  > >   GESTAO DE %s   < <\n", NOMES[op]);
                printf("==================================\n");
                printf("| 1 - Cadastrar                  |\n");
                printf("| 2 - Listar                     |\n");
                printf("| 3 - Atualizar                  |\n");
                printf("| 4 - Excluir                    |\n");
                printf("| 5 - Voltar                     |\n");
                printf("==================================\n> ");

                if (scanf("%d", &sub) != 1) sub = 0;
                limpar_buffer();
                limpar_tela();

                switch (sub) {
                    case 1: cadastrar(op); break;
                    case 2: listar(op);    break;
                    case 3: atualizar(op); break;
                    case 4: excluir(op);   break;
                    case 5: break;
                    default: printf("Valor invalido, tente novamente.\n"); break;
                }

                if (sub != 5) {
                    printf("\nPressione Enter para continuar...");
                    getchar();
                }
            } while (sub != 5);
        }
        else if (op != 5) {
            printf("Valor invalido, tente novamente.\n");
            printf("\nPressione Enter para continuar...");
            getchar();
        }
    } while (op != 5);

    limpar_tela();
    printf("Ate mais!\n");
    return 0;
}