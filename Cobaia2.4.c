#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>
#include "console_v1.5.4.h"
#include "conio_v3.2.4.h"
#include "graphics_v1.1.h"

#define M 10      /*define criado se baseando no tamanho da tabela hash, a qual possuirá 10 chaves, ou 10 espacos*/
#define MAIOR_INTEIRO (1 << 30) /*resultado de 2 elevado a 30*/
#define RAIZ 1                  /*valor que representa a raiz*/
#define HASHANTIGO << 1 % M       /* hash se consiste em, dado M = 10, e N de qualquer valor inteiro positivo
a função será   P(m,n): N * 2 mod 10
*/
#define HASH % M /*hash simples, o qual se baseia no resto da divisão por 0, seguindo a lógica do vetor de ponteiros, os primeiros indices alocados
o mais proximo da origem, sempre será os niveis mais proximos da raiz, ou seja, dado o indice 1 com seus hipoteticos 3 ponteiros para nos, teremos o 
seguinte:
indice[1]: pno |-|>|pno |-|>|pno |-|>|
a probabilidade do primeiro pno |-|>| estar no nivel 1, é exata, ele sempre estará, ja o segundo nó, sabe-se que pode estar tanto no nivel 1
quanto no nível 11, porém, impossível ele estar no nivel 21 se nao foi passado pelo nó 11.
*/
#define NIVEL_INICIAL 0
#define INICIOXEY 0
#define ALTURA 3
#define FIMX MaxDimensaoJanela().X
#define FIMY MaxDimensaoJanela().Y
#define PRIMEIROFILHO 0
#define CRIAR_NO malloc(sizeof(TIPONO))
#define TESTE printf("\nteste");

typedef struct QUADRADO
{
    COORD p1, p2, p3, p4;
    COORD centro;
    COORD centro_topo;
    COORD centro_base;
    int qtd_digit;
} QUADRADO;

typedef struct t_no_g
{
    QUADRADO caixinha;
    int nivel;
} TNG;

typedef struct t_no
{
    /* conteudo */
    bool visitado;
    int conteudo;
    int qtd_filhos;
    int nivel;
    int nivel_max;
    TNG etiqueta;
    struct t_no **filho; /*aponta para varios nos*/
} TIPONO;

typedef struct dados_pc
{
    COORD resolucao;
    COORD posicao;
    CORES cor;
    char *titulo;
} DADOS;
typedef struct pontos
{
    COORD p1;
    COORD p2;
    TIPONO *no;
} PONTOS;

typedef struct t_area
{
    PONTOS areasOcupadas;
    bool ocupado;
    TIPONO *no;
    struct t_area *prox;
} TAREA;

void Criarquadrado(TIPONO *);
void PlotadorDeVetices(TIPONO *);
void desenhaVertice(TIPONO *);
bool controleDePontos(COORD, COORD, COORD);
void buscaespaco(TAREA **, TIPONO *, TIPONO *);
void MSTdeCOORDENADAS(TIPONO *, TIPONO *, TAREA **);
bool validadorDeEspaco(TAREA **, TAREA);
TIPONO *PercorredorDeAltura(TIPONO *, int);
int contadigito(int);
DADOS PersonalizarConsoleFULL(char *);
void DespersonalizarConsoleFULL(DADOS);
void calcularAlturaArvore(TIPONO *);
void atribuirAltura(TIPONO *, int);
TIPONO *buscaNo(TIPONO *, int);
void insereFilho(TIPONO *, int);
void COORDENADASdoCDHU(TIPONO *);

int main(int argc, char **argv)
{
    int x, antx;

    DADOS console = PersonalizarConsoleFULL("teste");

    /*ativando o primeiro no*/
    TIPONO *pno = (TIPONO *)CRIAR_NO;
    TIPONO *raiz = pno;
    raiz->filho = (TIPONO **)CRIAR_NO;
    raiz->filho[0]->filho = (TIPONO **)CRIAR_NO;
    raiz->filho[0]->filho[0] = NULL;
    pno->qtd_filhos = 1;

    /*ativando prompt*/
    TAREA **prompt;
    ativaVetorPontArea(prompt, 10);
    prompt[0]->no = pno;

    scanf("%d", &antx);
    do
    {
        scanf("%d", &x);
        if (antx == 0)
        {
            antx = 1;
            pno->conteudo = x;
            pno->etiqueta.caixinha.qtd_digit = contadigito(x) + 2;
            calculaAreaUtilizada(pno);
        }
        else
        {
            if (x != -1)
            {
                pno = buscaNo(raiz, x); /*problema aqui*/
                scanf("%d", &x);        /*le a qtd de filhos pertencentes ao no*/
                insereFilho(pno, x);    /*problema aqui*/
            }
        }
    } while (x != -1);

    calcularAlturaArvore(raiz);
    MSTdeCOORDENADAS(raiz, raiz, prompt);
    /*COORDENADASdoCDHU(raiz);*/
    COORDENADASdoCDHU2000(prompt);
    DespersonalizarConsoleFULL(console);
    free(pno);
    free(prompt);
}
void ativaVetorPontArea(TAREA **tela, int qtdIndice)
{
    int i;
    tela = (TAREA **)malloc(sizeof(TAREA) * 10);
    for (i = 0; i < qtdIndice; i++)
    {
        tela[i] = (TAREA *)malloc(sizeof(TAREA));
    }
}
/*insere filho*/
void insereFilho(TIPONO *no, int qtdmax)
{
    int i = 0, x;
    int qtdmin = no->qtd_filhos;

    if (qtdmin == 0)
    {
        no->filho = (TIPONO **)malloc(sizeof(TIPONO) * qtdmax + 1); /*+ espaço para o no nulo*/
    }
    else
    {
        no->filho = (TIPONO **)realloc(no->filho, (no->qtd_filhos + qtdmax + 1) * (sizeof(TIPONO)));
    }
    /*no filho ira receber o parametro da seguinte condição:
        a qtd de filhos de nó, é 0?
            se sim,
                então aloque um novo endereço para os filhos de nó
            se não,
                então realoque o endereço para os filhos de nó
    */

    no->qtd_filhos += qtdmax;

    for (i = qtdmin; i < qtdmax; i++)
    {
        scanf("%d", &x);
        no->filho[i] = (TIPONO *)malloc(sizeof(TIPONO));
        if (x != -1)
        {
            no->filho[i]->conteudo = x;
            no->filho[i]->qtd_filhos = 0;
            no->filho[i]->visitado = false;
            no->filho[i]->etiqueta.caixinha.qtd_digit = contadigito(x) + 2;
            calculaAreaUtilizada(no->filho[i]);
            no->filho[i]->filho = NULL;
        }
        else
        {
            break;
        }
    }
    no->filho[i] = NULL;
}
void calculaAreaUtilizada(TIPONO *no)
{
    int tamanho = no->etiqueta.caixinha.qtd_digit;
    no->etiqueta.caixinha.p1.X = INICIOXEY;
    no->etiqueta.caixinha.p1.Y = INICIOXEY;
    no->etiqueta.caixinha.p2.X = tamanho;
    no->etiqueta.caixinha.p2.Y = INICIOXEY;
    no->etiqueta.caixinha.p3.X = INICIOXEY;
    no->etiqueta.caixinha.p3.Y = 3;
    no->etiqueta.caixinha.p4.X = tamanho;
    no->etiqueta.caixinha.p4.Y = 3;
}
TIPONO *buscaNo(TIPONO *raiz, int x)
{
    int i;
    if (raiz == NULL)
    {
        return NULL; /*Elemento não encontrado*/
    }
    if (raiz->conteudo == x)
    {
        return raiz; /*Encontrou o nó com o valor x*/
    }

    for (i = 0; i < raiz->qtd_filhos && raiz->filho[0] != NULL; i++)
    {

        TIPONO *resultado = buscaNo(raiz->filho[i], x);
        if (resultado != NULL)
        {
            return (TIPONO *)resultado; /*Encontrou o nó com o valor x em um dos filhos*/
        }
    }
    return NULL; /*Elemento não encontrado na subárvore atual*/
}

void atribuirAltura(TIPONO *raiz, int altura)
{
    int i;
    if (raiz == NULL)
    {
        return;
    }

    raiz->nivel = altura;

    for (i = 0; i < raiz->qtd_filhos; i++)
    {
        atribuirAltura(raiz->filho[i], altura + 1);
    }
}

void calcularAlturaArvore(TIPONO *raiz)
{
    atribuirAltura(raiz, 0);
}

void DespersonalizarConsoleFULL(DADOS antigo)
{
    /*titulo*/
    setTituloConsole(antigo.titulo);
    /*dimensionar*/
    setDimensaoJanela(antigo.resolucao.X, antigo.resolucao.Y);
    /*posicionar*/
    setPosicaoJanela(antigo.posicao.X, antigo.posicao.Y);
    /*ligar cursor*/
    setCursorStatus(LIGAR);
    /*barra de tarefas visivel*/
    setEstadoBarraTarefas(VISIVEL);
    textbackground(antigo.cor.CorFundo);
    textcolor(antigo.cor.CorTexto);
    clrscr();
}
DADOS PersonalizarConsoleFULL(char *Titulo)
{
    /*declarar variaveis*/
    DADOS tela;
    /*gravar cores antigas de fonte e fundo*/
    tela.cor = getCoresJanela();
    /*gravar resolução atual do console*/
    tela.resolucao = tamanhoJanelaConsole();
    /*gravar posição da janela*/
    tela.posicao = getPosicaoJanela();
    /*titulo antigo*/
    tela.titulo = getTituloConsole();
    /*titulo*/
    setTituloConsole(Titulo);
    /*desligar cursor*/
    setCursorStatus(DESLIGAR);
    /*barra de tarefas invisivel*/
    setEstadoBarraTarefas(INVISIVEL);
    /*posicionar*/
    setPosicaoJanela(0, 0);
    /*dimensionar*/
    setDimensaoJanela(MaxDimensaoJanela().X, MaxDimensaoJanela().Y);
    /*retornar a memoria*/
    return tela;
}

int contadigito(int n)
{
    return (n / 10 != 0) ? 1 + contadigito(n / 10) : 1;
}

TIPONO *PercorredorDeAltura(TIPONO *raiz, int nivel)
{
    int i;
    TIPONO **cobaia = raiz->filho;
    if (raiz == NULL)
    {
        return NULL;
    }
    for (i = 0; i < raiz->qtd_filhos; i++)
    {
        if (cobaia[i]->nivel == nivel)
        {
            if (!cobaia[i]->visitado)
            {
                cobaia[i]->visitado = true;
                return cobaia[i];
            }
        }
        PercorredorDeAltura(cobaia[i], nivel);
    }
}

bool validadorDeEspaco(TAREA **salvos, TAREA novo)
{
    int i = 0;
    TAREA *prox;
    while (salvos[i] != NULL)
    {
        prox = salvos[i];
        while (prox != NULL)
        {
            if (!controleDePontos(salvos[i]->areasOcupadas.p1, salvos[i]->areasOcupadas.p2, novo.areasOcupadas.p1) &&
                !controleDePontos(salvos[i]->areasOcupadas.p1, salvos[i]->areasOcupadas.p2, novo.areasOcupadas.p2))
            {
                return false; /*nao pode ser alocado pois colidiu com um dos salvos*/
            }
            prox = prox->prox;
        }
        i++;
    }
    return true; /*pode ser alocado*/
}

void buscaespaco(TAREA **tela, TIPONO *raiz, TIPONO *selecionado)
{
    int auxx, auxy;
    int i, j, k = 0;
    int particaoX = FIMX / selecionado->nivel_max;
    int particaoY = FIMY / selecionado->nivel_max;

    COORD limInf;
    COORD limSup;

    limSup.Y = particaoY * selecionado->nivel;
    limSup.X = FIMX / raiz->qtd_filhos;
    limInf.X = FIMX / raiz->qtd_filhos;
    limInf.Y = limSup.Y + particaoY - 1;
    /*

    LimSup-----------------
    |                     |
    |                     |
    |                     |
    |                     |
    LimInf----------------




    */

    TAREA nova;
    TAREA *proximo;

    /*qtd digitos + lados*/
    /*unsigned int largura = (unsigned int)(((selecionado->etiqueta.caixinha.p2.X - selecionado->etiqueta.caixinha.p1.X) << 1) >> 1); garantir a positividade*/
    unsigned int largura = selecionado->etiqueta.caixinha.p2.X;
    unsigned int altura = selecionado->etiqueta.caixinha.p3.Y;

    nova.areasOcupadas.no = selecionado;
    nova.areasOcupadas.p1.X = 0;
    nova.areasOcupadas.p1.Y = limInf.Y - 3;
    nova.areasOcupadas.p2.X = 0 + largura;
    nova.areasOcupadas.p2.Y = limInf.Y;
    while (!validadorDeEspaco(tela, nova))
    {
        if (nova.areasOcupadas.p2.X == limInf.X)
        {
            nova.areasOcupadas.p2.Y++;
            nova.areasOcupadas.p1.Y++;
            nova.areasOcupadas.p1.X = 0;
            nova.areasOcupadas.p2.X = 0 + largura;
        }
        nova.areasOcupadas.p1.X++;
        nova.areasOcupadas.p2.X++;
    }
    proximo = tela[selecionado->nivel HASH]; /*hash do resto de 11*/
    while (proximo->ocupado)
    {
        proximo = proximo->prox;
    }
    proximo->no = selecionado;
    selecionado->etiqueta.caixinha.p4.Y = proximo->areasOcupadas.p2.Y = nova.areasOcupadas.p2.Y;
    selecionado->etiqueta.caixinha.p1.X = proximo->areasOcupadas.p1.X = nova.areasOcupadas.p1.X;
    selecionado->etiqueta.caixinha.p4.X = proximo->areasOcupadas.p2.X = nova.areasOcupadas.p2.X;
    selecionado->etiqueta.caixinha.p1.Y = proximo->areasOcupadas.p1.Y = nova.areasOcupadas.p1.Y;
    proximo->ocupado = true;
    proximo->prox = NULL;

    selecionado->etiqueta.caixinha.centro.X = proximo->areasOcupadas.p2.X / 2;
    selecionado->etiqueta.caixinha.centro.Y = proximo->areasOcupadas.p2.Y / 2;

    selecionado->etiqueta.caixinha.centro_topo.X = proximo->areasOcupadas.p2.X / 2;
    selecionado->etiqueta.caixinha.centro_topo.Y = proximo->areasOcupadas.p1.Y;

    selecionado->etiqueta.caixinha.centro_base.X = proximo->areasOcupadas.p2.X / 2;
    selecionado->etiqueta.caixinha.centro_base.Y = proximo->areasOcupadas.p2.Y;
}
bool controleDePontos(COORD p1, COORD p2, COORD p3) /*p3 estara entre p1 e p2?*/
{
    int minX = p1.X < p2.X ? p1.X : p2.X;                             /*encontrar o menor X*/
    int maxX = p1.X > p2.X ? p1.X : p2.X;                             /*encontrar o maior X*/
    int minY = p1.Y < p2.Y ? p1.Y : p2.Y;                             /*encontrar o menor Y*/
    int maxY = p1.Y > p2.Y ? p1.Y : p2.Y;                             /*encontrar o maior Y*/
    if (p3.X >= minX && p3.X <= maxX && p3.Y >= minY && p3.Y <= maxY) /*Se o p3.x for maior ou igual ao menor ponto X e menor ou igual o maior ponto X (o mesmo feito com o Y)*/
    {
        return false;
    }

    return true;
}
void desenhaVertice(TIPONO *pai)
{
    int i = 0;
    int x1, y1;

    TIPONO *filho;
    int x2 = pai->etiqueta.caixinha.centro_base.X;
    int y2 = pai->etiqueta.caixinha.centro_base.Y;

    for (i = 0; i < pai->qtd_filhos; i++)
    {
        filho = pai->filho[i];
        x1 = filho->etiqueta.caixinha.centro_topo.X;
        y1 = filho->etiqueta.caixinha.centro_topo.Y;
        linha(x1, y1, x2, y2, RGB(255, 255, 255));
    }
}

void PlotadorDeVetices(TIPONO *raiz)
{
    int i;
    if (raiz != NULL)
    {
        desenhaVertice(raiz);
        if (raiz->qtd_filhos > 0 && raiz->filho != NULL)
        {
            for (i = 0; i < raiz->qtd_filhos; i++)
            {
                PlotadorDeVetices(raiz->filho[i]);
            }
        }
    }
}
void MSTdeCOORDENADAS(TIPONO *raiz, TIPONO *no, TAREA **tela)
{
    int i;
    if (raiz != NULL)
    {
        buscaespaco(tela, raiz, no);
        if (raiz->qtd_filhos > 0 && raiz->filho != NULL)
        {
            for (i = 0; i < raiz->qtd_filhos; i++)
            {
                MSTdeCOORDENADAS(raiz, no->filho[i], tela);
            }
        }
    }
}
void COORDENADASdoCDHU(TIPONO *raiz)
{
    int i;
    if (raiz != NULL)
    {
        Criarquadrado(raiz);
        PlotadorDeVetices(raiz);
        if (raiz->qtd_filhos > 0 && raiz->filho != NULL)
        {
            for (i = 0; i < raiz->qtd_filhos; i++)
            {
                COORDENADASdoCDHU(raiz);
            }
        }
    }
}
void COORDENADASdoCDHU2000(TAREA **vetor)
{
    int i = 0;
    TAREA *prox;
    while (vetor[i] != NULL)
    {
        prox = vetor[i];
        while (prox != NULL)
        {
            Criarquadrado(prox->no);
            prox = prox->prox;
        }
        i++;
    }
}
void Criarquadrado(TIPONO *raiz)
{
    QUADRADO telaSolicitada = raiz->etiqueta.caixinha;
    int i, j; /*i mexe no x e j meche no y*/
    int j2;   /*variável auxiliar*/
    int cor = WHITE;
    int x = telaSolicitada.p1.X + 1;
    int y = telaSolicitada.p1.Y + 1;

    textcolor(cor);
    textbackground(cor);
    j = telaSolicitada.p1.Y;
    j2 = telaSolicitada.p4.Y;
    for (i = telaSolicitada.p1.X; i <= telaSolicitada.p4.X; i++) /*imprime as linhas*/
    {
        gotoxy(i, j);
        printf(" ");
        gotoxy(i, j2);
        printf(" ");
    }
    j = telaSolicitada.p1.X;
    j2 = telaSolicitada.p4.X;
    for (i = telaSolicitada.p1.Y; i <= telaSolicitada.p4.Y; i++) /*imprime as colunas*/
    {
        gotoxy(j, i);
        printf(" ");
        gotoxy(j2, i);
        printf(" ");
    }
    textcolor(BLACK);
    textbackground(WHITE); /*volta as cores padrões*/
    gotoxy(x, y);
    printf("%d", raiz->conteudo);
    textcolor(LIGHTGRAY);
    textbackground(BLACK); /*volta as cores padrões*/
}