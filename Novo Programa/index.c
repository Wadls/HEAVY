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





const char *ARQUIVOS[]   = {NULL, ARQ_USUARIO, ARQ_EXERCICIO, ARQ_GM, ARQ_TREINO}; 
const char *CABECALHOS[] = {NULL, "cod;nome;idade;peso;altura;rotina", "cod;nome;grupo muscular;series",
                                   "cod;nome",
                                   "cod;nome;exercicios"};
const char *SEMANA[] = {"Segunda-Feira", "Terça-Feira","Quarta-Feira","Quinta-Feira","Sexta-Feira","Sabado","Domingo"};
const char *NOMES[] = {NULL, "Usuário","Exercicio","Grupo Muscular","Treino"};

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
    if (tipo < 1 || tipo > 4 || tamanho <= 0) return; //Se o objeto não fazer do meu conjunto inicial estipulado, nem procure 

    char linha[TAM_BUFFER];
    const char *nomeDoObjeto = NOMES[tipo]; //Para personalizar os prints, relacionei, o tipo com um array de nomes 
    // 1 = Aluno | 2 = Exercicio | 3 = GrupoMuscular | 4 = Treino 
    for (int i = 0; i < tamanho; i++) {
        int objEscolhido = 0, objExiste = 0;

        listar(tipo);
        printf("Escreva o código do(a) %s.\nSe não estiver registrado, digite 0: ", nomeDoObjeto);
        if (scanf("%d", &objEscolhido) != 1) objEscolhido = 0; //Se a atribuição der errado, (escrevi letra onde é numero), obj vira 0
        limpar_buffer();

        if (objEscolhido != 0) {
            FILE *arq = fopen(ARQUIVOS[tipo], "r");
            if (arq == NULL) {
                printf("Erro ao abrir o arquivo de %s.\n", nomeDoObjeto);
                return;
            }
            fgets(linha, sizeof(linha), arq);          /* pula cabeçalho */
            while (fgets(linha, sizeof(linha), arq) != NULL) {
                int cod;
                if (sscanf(linha, "%d", &cod) == 1 && cod == objEscolhido) {
                    objExiste = 1;
                    break;
                }
            }
            fclose(arq);                               /* sempre fechado aqui */
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

    char linha[TAM_BUFFER];

    /* cabeçalho */
    if (fgets(linha, sizeof(linha), arq) != NULL) {
        linha[strcspn(linha, "\r\n")] = '\0'; // Não Entendi
        for (char *p = linha; *p; p++) if (*p == ';') *p = '\t'; // Essas duas linhas
        printf("\n%s\n", linha);
        printf("---------------------------------------------------\n", linha);
    }

    /* registros */
    while (fgets(linha, sizeof(linha), arq) != NULL) {
        linha[strcspn(linha, "\r\n")] = '\0';
        if (linha[0] == '\0') continue;           /* ignora linha em branco */
        for (char *p = linha; *p; p++) if (*p == ';') *p = '\t';
        printf("%s\n", linha);
    }
    printf("\n");
    fclose(arq);
}


int main(void){
    garantir_arquivo();
    listar(1);
    listar(2);
    listar(3);
    listar(4);

}