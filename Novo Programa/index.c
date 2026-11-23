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

void escolherObjeto(int codigos[], int tipo, int tamanho){
    char linha[TAM_BUFFER];
    const char *nomeDoObjeto = NOMES[tipo]; 
    FILE *arq = fopen(ARQUIVOS[tipo], "r");
    //Definindo o nome do Objeto Para printf's

    if (arq == NULL) {
            printf("Erro ao abrir o arquivo de %s.\n", nomeDoObjeto);
            return;
    }

    for (int i = 0; i < tamanho; i++){
        int  objEscolhido = 0, objExiste = 0;
        listar(tipo); 
        char operator;
    
        printf("Escreva o Cod do %s: \nSe o %s nao foi registrado, digite 0\n", nomeDoObjeto, nomeDoObjeto);
        if (scanf("%d", &objEscolhido) != 1) objEscolhido = 0; //Se ele retornar um caracter inválido, obj vira zero
        limpar_buffer();
        char *token;

        fgets(linha, sizeof(linha), arq);//Pula o Cabeçalho
        while (fgets(linha, sizeof(linha), arq) != NULL) {
            token = strtok(linha, ";");//Aqui estou atribuindo o cod da linha
            if (token != NULL && objEscolhido == atoi(token)){ //Para depois compara-la com o objEscolhido
                objExiste = 1;
                codigos[i] = objEscolhido;
            };
        }
        
        if (objExiste){
            printf("%s Adicionado com sucesso!", nomeDoObjeto);
        }
        else{
            printf("Cadastre o Novo %s\n", nomeDoObjeto);
            cadastrar(tipo);
            objEscolhido = prox_codigo(tipo) - 1; //Por meio desse método, adciono o código do último autor registrado
            codigos[i] = objEscolhido;
        }
        rewind(arq);
        
    }
    fclose(arq);
}





void cadastrar(int tipo) {
   
    FILE *arq = NULL;

    switch (tipo){ //Tipo 01: Usuario | 02:Autor | 03:Livro | 04:Emprestimo
        case 1:{ 
            arq = fopen(ARQ_USUARIO, "a");   /* modo append */
            Usuario a;
            if (arq == NULL) {
                printf("Erro ao abrir o arquivo %s.\n", ARQ_USUARIO);
                return;
            }
            a.cod = prox_codigo(1);
            if (a.cod == 0){
                printf("Error, Tente de novo\n");
                fclose(arq);
                return;
            }
            
            //Atribuição do Nome, com limitador
            printf("Nome: ");
            fgets(a.nome, MAX_NOME, stdin);
            a.nome[strcspn(a.nome, "\n")] = '\0';

            printf("Idade: ");
            scanf("%d", &a.idade);
            printf("Peso: ");
            scanf("%f", &a.peso);
            printf("Altura: ");
            scanf("%f", &a.altura);
            for (int i = 0; i < 7; i++)
            {

                
            };
            
            
            fprintf(arq, "\n%d;%s;%d\n", a.cod, a.nome, a.status);

            fclose(arq);
            printf("Usuário cadastrado com sucesso!\n");
            break;
        }
        case 2:{
            arq = fopen(ARQ_AUTOR, "a");   /* modo append, adiciona um novo objeto no final do arquivo */
            Autor a;
            if (arq == NULL) {
                printf("Erro ao abrir o arquivo %s.\n", ARQ_AUTOR);
                return;
            }
            a.cod = prox_codigo(2); //Tipo 01: Usuario | 02:Autor | 03:Livro | 04 Emprestimo

            printf("Nome: ");
            fgets(a.nome, MAX_NOME, stdin); //Atribuição do Nome, com limitador
            a.nome[strcspn(a.nome, "\n")] = '\0';
            fprintf(arq, "\n%d;%s;\n", a.cod, a.nome);

            fclose(arq);
            printf("Autor cadastrado com sucesso!\n");
            break;}
        
        case 3:{
            arq = fopen(ARQ_LIVRO, "a");   /* modo append, adiciona um novo objeto no final do arquivo */
            Livro a;

            if (arq == NULL) {
                printf("Erro ao abrir o arquivo %s.\n", ARQ_LIVRO);
                return;
            }

            a.cod = prox_codigo(3); //Tipo 01: Usuario | 02:Autor | 03:Livro 
            printf("Titulo do Livro: ");
            fgets(a.titulo, MAX_NOME, stdin); //Atribuição do Nome, com limitador de 80 caracteres
            a.titulo[strcspn(a.titulo, "\n")] = '\0';

            printf("Defina o Estoque do livro %s:\n", a.titulo);
            scanf("%d", &a.estoque);
            limpar_buffer();
            
            //Aqui começa o registro
            
            a.codAutores[0] = a.codAutores[1] = a.codAutores[2] = 0;
            escolherAutor(a.codAutores);
            fprintf(arq, "\n%d;%s;%d;%d;%d;%d\n", a.cod, a.titulo,a.estoque,a.codAutores[0],a.codAutores[1],a.codAutores[2]);
            fclose(arq);
            printf("Livro cadastrado com sucesso!\n");
            break;
        }
        case 4:{
            Emprestimo a;
            char linha[TAM_BUFFER], nome[MAX_NOME], titulo[MAX_NOME];
            int codUser, statusUser, estoque, qtd = 0;

            a.codLivro[0] = a.codLivro[1] = a.codLivro[2] = 0;
            a.status = 0;

            /* 1) Aluno */
            listar(1);
            printf("Cod do Usuario: ");
            scanf("%d", &a.codAluno);
            limpar_buffer();

            if (!buscar_registro(ARQ_USUARIO, a.codAluno, linha, sizeof(linha)) ||
                sscanf(linha, "%d;%79[^;];%d", &codUser, nome, &statusUser) != 3) {
                printf("Usuario nao encontrado.\n");
                return;
            }
            if (statusUser != 0) {
                printf("Usuario nao esta ATIVO, emprestimo negado.\n");
                return;
            }

            /* 2) Livros (até 3) */
            for (int i = 0; i < 3; i++) {
                int codEscolhido, repetido = 0;

                listar(3);
                printf("Cod do Livro: ");
                scanf("%d", &codEscolhido);
                limpar_buffer();

                for (int j = 0; j < i; j++)
                    if (a.codLivro[j] == codEscolhido) repetido = 1;

                if (repetido) {
                    printf("Esse livro ja esta neste emprestimo.\n"); //Não permito emprestar dois livros iguais
                    i--;                                  
                    continue;
                }

                if (!buscar_registro(ARQ_LIVRO, codEscolhido, linha, sizeof(linha)) ||
                    sscanf(linha, "%d;%79[^;];%d", &codEscolhido, titulo, &estoque) != 3) {
                    printf("Livro nao encontrado.\n");
                    i--;
                    continue;
                }
                if (estoque <= 0) {
                    printf("Livro '%s' sem estoque.\n", titulo);
                    i--;
                    continue;
                }
                if (!alterar_estoque(codEscolhido, -1)) {
                    printf("Erro ao atualizar o estoque.\n");
                    break;
                }

                a.codLivro[qtd++] = codEscolhido;
                printf("Livro '%s' adicionado!\n", titulo);

                if (i < 2) {
                    char operator;
                    printf("Pretende emprestar mais algum livro?(S/N)\n");
                    scanf(" %c", &operator);
                    limpar_buffer();
                    if (operator == 'N' || operator == 'n') break;
                    limpar_tela();
                }
            }

            if (qtd == 0) {
                printf("Nenhum livro selecionado, emprestimo cancelado.\n");
                return;
            }

            /* 3) Grava */
            arq = fopen(ARQ_EMP, "a");
            if (arq == NULL) {
                printf("Erro ao abrir o arquivo %s.\n", ARQ_EMP);
                for (int i = 0; i < qtd; i++) alterar_estoque(a.codLivro[i], +1);  /* desfaz */
                return;
            }
            a.cod = prox_codigo(4); // 1 Aluno | 2 = Autor | 3 = Livro | 4 = Empréstimo
            fprintf(arq, "%d;%d;%d;%d;%d;%d\n", a.cod, a.codAluno,
                    a.codLivro[0], a.codLivro[1], a.codLivro[2], a.status);
            fclose(arq);
            printf("Emprestimo %d cadastrado com sucesso!\n", a.cod);
            break;
        }
        default:
            break;
    }
    
    
}

