#ifndef JOGO_H
#define JOGO_H
#include <stdbool.h>
#define NUM_TRIBUTOS 24
#define MAX_NOME 50
#define ARQUIVO_SAVE "save.dat"

typedef enum {
    ITEM_NENHUM,
    ITEM_COMIDA,
    ITEM_AGUA,
    ITEM_MEDICINA,
    ITEM_FACA,
    ITEM_ESPADA,
    ITEM_ARCO
} TipoItem;

typedef struct {
    char nome[MAX_NOME];
    char distrito[10];
    int saude, fome, sede, energia, forca, agilidade, carisma;
    bool vivo;
} Tributo;

typedef struct {
    TipoItem tipo;
    int quantidade;
    int dano_extra;
} Item;

typedef struct {
    Tributo jogador;
    Tributo tributos_npcs[NUM_TRIBUTOS - 1];
    int diaAtual;
    int tributosVivos;
    Item inventario[10];
    char causa_morte[100];
} Jogo;

// Funções principais do jogo
void criar_novo_jogo(Jogo* jogo);
bool salvar_jogo(const Jogo* jogo);
bool carregar_jogo(Jogo* jogo);
void loop_principal(Jogo* jogo);
void processar_acao(Jogo* jogo, char decisao);
void acao_npc(Jogo* jogo);
void combate(Jogo* jogo, Tributo* inimigo);
int calcular_dano_total(Tributo* jogador, Jogo* jogo);
void evento_capital(Jogo* jogo);
void adicionar_item(Jogo* jogo, TipoItem tipo, int quantidade, int dano_extra);
void usar_item(Jogo* jogo);
const char* nome_item(TipoItem tipo);

#endif // JOGO_H
