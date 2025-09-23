#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>


// Definições
#define NUM_TRIBUTOS 24
#define MAX_NOME 50
#define ARQUIVO_SAVE "save.dat"


// --- Enumerações e Structs ---


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
    int saude;
    int fome;
    int sede;
    int energia;
    int forca;
    int agilidade;
    int carisma;
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
} Jogo;


// --- Protótipos das Funções ---


void exibir_tela_titulo();
void exibir_menu_principal();
void criar_novo_jogo(Jogo* jogo);
bool salvar_jogo(const Jogo* jogo);
bool carregar_jogo(Jogo* jogo);
void exibir_status(const Jogo* jogo);
void loop_principal(Jogo* jogo);
void processar_acao(Jogo* jogo, int acao);
void acao_npc(Jogo* jogo);
void combate(Jogo* jogo, Tributo* inimigo);
int calcular_dano_total(Tributo* jogador, Jogo* jogo);
void evento_capital(Jogo* jogo);
void exibir_tela_fim_jogo(const Jogo* jogo);
const char* nome_item(TipoItem tipo);
void adicionar_item(Jogo* jogo, TipoItem tipo, int quantidade, int dano_extra);
void usar_item(Jogo* jogo);




// --- Funções de UI e Lógica do Jogo ---


void exibir_tela_titulo() {
    system("cls || clear");
    printf("=================================================================\n");
    printf("     _   _           _             ___                         \n");
    printf("    | | | |         | |           |__ \\                        \n");
    printf("    | |_| | __ _  _ | |__   ___      ) | __ _  ___   _ __   ___ \n");
    printf("    |  _  |/ _` ||_`| '_ \\ / _ \\    / / / _` |/ _ \\ | '_ \\ / _ \\ \n");
    printf("    | | | | (_| |  | | |_) |  __/   / /_| (_| | (_) || | | |  __/ \n");
    printf("    \\_| |_/\\__,_|  |_|_.__/ \\___|   \\____/\\__, |\\___/ |_| |_|\\___| \n");
    printf("                                           __/ |                 \n");
    printf("                                          |___/                  \n");
    printf("=================================================================\n");
    printf("\n                 (Pressione Enter para continuar)\n");
    getchar();
    getchar();
}


void exibir_menu_principal() {
    system("cls || clear");
    printf("--- Simulador de Hunger Games ---\n");
    printf("1. Novo Jogo\n");
    printf("2. Continuar Jogo\n");
    printf("3. Sair\n");
    printf("-----------------------------------\n");
}


void criar_novo_jogo(Jogo* jogo) {
    char nome[MAX_NOME];
    char distrito[10];


    system("cls || clear");
    printf("--- Novo Jogo ---\n");
    printf("Qual o seu nome? ");
    scanf("%s", nome);
    printf("De qual distrito você é? ");
    scanf("%s", distrito);


    // Cria o personagem do jogador
    Tributo jogador_base;
    strcpy(jogador_base.nome, nome);
    strcpy(jogador_base.distrito, distrito);
    jogador_base.saude = 100;
    jogador_base.fome = 0;
    jogador_base.sede = 0;
    jogador_base.energia = 100;
    jogador_base.forca = rand() % 10 + 1;
    jogador_base.agilidade = rand() % 10 + 1;
    jogador_base.carisma = rand() % 10 + 1;
    jogador_base.vivo = true;
    jogo->jogador = jogador_base;
   
    // Cria os 23 tributos NPCs
    for (int i = 0; i < NUM_TRIBUTOS - 1; i++) {
        char nome_npc[MAX_NOME];
        char distrito_npc[10];
        sprintf(nome_npc, "Tributo %d", i + 1);
        sprintf(distrito_npc, "D%d", (i % 12) + 1);


        Tributo npc_base;
        strcpy(npc_base.nome, nome_npc);
        strcpy(npc_base.distrito, distrito_npc);
        npc_base.saude = 100;
        npc_base.fome = 0;
        npc_base.sede = 0;
        npc_base.energia = 100;
        npc_base.forca = rand() % 10 + 1;
        npc_base.agilidade = rand() % 10 + 1;
        npc_base.carisma = rand() % 10 + 1;
        npc_base.vivo = true;
        jogo->tributos_npcs[i] = npc_base;
    }


    jogo->diaAtual = 1;
    jogo->tributosVivos = NUM_TRIBUTOS;
    for (int i = 0; i < 10; i++) {
        jogo->inventario[i].tipo = ITEM_NENHUM;
        jogo->inventario[i].quantidade = 0;
    }
    printf("\nBem-vindo aos Jogos Vorazes, %s do Distrito %s!\n", jogo->jogador.nome, jogo->jogador.distrito);
    salvar_jogo(jogo);
}


bool salvar_jogo(const Jogo* jogo) {
    FILE* arquivo = fopen(ARQUIVO_SAVE, "wb");
    if (arquivo == NULL) {
        return false;
    }
    fwrite(jogo, sizeof(Jogo), 1, arquivo);
    fclose(arquivo);
    return true;
}


bool carregar_jogo(Jogo* jogo) {
    FILE* arquivo = fopen(ARQUIVO_SAVE, "rb");
    if (arquivo == NULL) {
        printf("Nenhum jogo salvo encontrado. Iniciando novo jogo...\n");
        return false;
    }
    fread(jogo, sizeof(Jogo), 1, arquivo);
    fclose(arquivo);
    printf("Jogo carregado com sucesso! Dia atual: %d\n", jogo->diaAtual);
    return true;
}


void exibir_status(const Jogo* jogo) {
    system("cls || clear");
    printf("==========================================\n");
    printf(" Dia: %d | Tributos Vivos: %d de %d\n", jogo->diaAtual, jogo->tributosVivos, NUM_TRIBUTOS);
    printf("------------------------------------------\n");
    printf(" Tributo: %s (Distrito %s)\n", jogo->jogador.nome, jogo->jogador.distrito);
    printf(" Saúde:   [%-20s] %d\n", "####################" + (20 - jogo->jogador.saude/5), jogo->jogador.saude);
    printf(" Fome:    [%-20s] %d\n", "####################" + (20 - jogo->jogador.fome/5), jogo->jogador.fome);
    printf(" Sede:    [%-20s] %d\n", "####################" + (20 - jogo->jogador.sede/5), jogo->jogador.sede);
    printf(" Energia: [%-20s] %d\n", "####################" + (20 - jogo->jogador.energia/5), jogo->jogador.energia);
    printf("------------------------------------------\n");
}


void processar_acao(Jogo* jogo, int acao) {
    if (jogo->jogador.energia < 10 && acao != 3) {
        printf("Você está exausto demais para essa ação. Descanse!\n");
        return;
    }


    switch (acao) {
        case 1: // Explorar
            printf("\nVocê gasta energia explorando a área...\n");
            jogo->jogador.energia -= 15;
            if (rand() % 100 < 40) { // 40% de chance de encontrar algo
                int tipo_encontrado = rand() % 10;
                if (tipo_encontrado < 4) { // 40%
                    int item = rand() % 3;
                    adicionar_item(jogo, item == 0 ? ITEM_COMIDA : (item == 1 ? ITEM_AGUA : ITEM_MEDICINA), 1, 0);
                } else if (tipo_encontrado < 8) { // 40%
                    adicionar_item(jogo, ITEM_FACA, 1, 5);
                } else if (tipo_encontrado < 9) { // 10%
                    adicionar_item(jogo, ITEM_ESPADA, 1, 15);
                } else { // 10%
                    adicionar_item(jogo, ITEM_ARCO, 1, 20);
                }
            } else {
                printf("Você não encontra nada de útil.\n");
            }
            if (rand() % 100 < 30) { // 30% de chance de encontro
                int tributo_encontrado_index;
                do {
                    tributo_encontrado_index = rand() % (NUM_TRIBUTOS - 1);
                } while (!jogo->tributos_npcs[tributo_encontrado_index].vivo);
                Tributo* inimigo = &jogo->tributos_npcs[tributo_encontrado_index];
                printf("\nVocê se depara com %s do Distrito %s!\n", inimigo->nome, inimigo->distrito);
                int escolha_combate;
                printf("1. Atacar\n2. Tentar aliança\n3. Fugir\nSua escolha: ");
                scanf("%d", &escolha_combate);
                if (escolha_combate == 1) {
                    combate(jogo, inimigo);
                } else if (escolha_combate == 2) {
                    if (rand() % 100 < jogo->jogador.carisma * 5) {
                        printf("%s aceita a aliança. Vocês se separam em paz.\n", inimigo->nome);
                    } else {
                        printf("%s não confia em você. A tensão aumenta e vocês se separam.\n", inimigo->nome);
                    }
                } else {
                    if (rand() % 100 < jogo->jogador.agilidade * 7) {
                        printf("Você conseguiu fugir com sucesso!\n");
                    } else {
                        printf("Você falha em fugir e sofre um ataque de %s!\n", inimigo->nome);
                        jogo->jogador.saude -= 10;
                    }
                }
            }
            break;


        case 2: // Caçar
            printf("\nVocê gasta energia tentando caçar...\n");
            jogo->jogador.energia -= 20;
            if (rand() % 100 < jogo->jogador.agilidade * 5) {
                adicionar_item(jogo, ITEM_COMIDA, 2, 0);
            } else {
                printf("Você não conseguiu caçar nada.\n");
            }
            break;


        case 3: // Descansar
            printf("\nVocê descansa e recupera suas energias.\n");
            jogo->jogador.energia += 40;
            if (jogo->jogador.energia > 100) jogo->jogador.energia = 100;
            break;


        case 4: // Usar item
            usar_item(jogo);
            break;


        default:
            printf("Ação inválida. Tente novamente.\n");
            break;
    }
}


void acao_npc(Jogo* jogo) {
    for (int i = 0; i < NUM_TRIBUTOS - 1; i++) {
        Tributo* npc = &jogo->tributos_npcs[i];
        if (npc->vivo) {
            int acao = rand() % 3;
            if (acao == 0) { // Ataque
                int alvo_index;
                do {
                    alvo_index = rand() % (NUM_TRIBUTOS - 1);
                } while (!jogo->tributos_npcs[alvo_index].vivo || alvo_index == i);


                Tributo* alvo = &jogo->tributos_npcs[alvo_index];
                int dano = rand() % 10 + npc->forca;
                alvo->saude -= dano;


                printf("- %s ataca %s, causando %d de dano.\n", npc->nome, alvo->nome, dano);
               
                if (alvo->saude <= 0) {
                    alvo->vivo = false;
                    jogo->tributosVivos--;
                    printf(">>> %s foi eliminado por %s!\n", alvo->nome, npc->nome);
                }
            } else if (acao == 1) { // Procura
                printf("- %s está procurando por suprimentos.\n", npc->nome);
                npc->fome -= 10;
                npc->sede -= 10;
            } else { // Descansa
                printf("- %s está descansando.\n", npc->nome);
                npc->energia += 20;
            }


            npc->fome += 5;
            npc->sede += 8;
            if (npc->fome >= 100 || npc->sede >= 100) {
                npc->vivo = false;
                jogo->tributosVivos--;
                printf(">>> %s morreu de fome e sede.\n", npc->nome);
            }
        }
    }
}


void combate(Jogo* jogo, Tributo* inimigo) {
    while (jogo->jogador.vivo && inimigo->vivo) {
        printf("\n--- COMBATE! ---\n");
        printf("Sua vida: %d | Vida de %s: %d\n", jogo->jogador.saude, inimigo->nome, inimigo->saude);


        int dano_jogador = calcular_dano_total(&jogo->jogador, jogo);
        printf("Você ataca %s, causando %d de dano!\n", inimigo->nome, dano_jogador);
        inimigo->saude -= dano_jogador;
        if (inimigo->saude <= 0) {
            inimigo->vivo = false;
            jogo->tributosVivos--;
            printf("Você derrotou %s!\n", inimigo->nome);
            printf("Você saqueia o corpo de %s, encontrando comida e um kit médico.\n", inimigo->nome);
            adicionar_item(jogo, ITEM_COMIDA, 2, 0);
            adicionar_item(jogo, ITEM_MEDICINA, 1, 0);
            break;
        }


        int dano_inimigo = rand() % 5 + inimigo->forca;
        printf("%s ataca você, causando %d de dano!\n", inimigo->nome, dano_inimigo);
        jogo->jogador.saude -= dano_inimigo;
        if (jogo->jogador.saude <= 0) {
            jogo->jogador.vivo = false;
            printf("%s te derrotou. Você foi eliminado.\n", inimigo->nome);
            break;
        }
    }
}


int calcular_dano_total(Tributo* jogador, Jogo* jogo) {
    int dano_base = rand() % 5 + jogador->forca;
    int dano_arma = 0;
    for (int i = 0; i < 10; i++) {
        if (jogo->inventario[i].tipo == ITEM_FACA || jogo->inventario[i].tipo == ITEM_ESPADA || jogo->inventario[i].tipo == ITEM_ARCO) {
            dano_arma = jogo->inventario[i].dano_extra;
            break;
        }
    }
    return dano_base + dano_arma;
}


void evento_capital(Jogo* jogo) {
    if (rand() % 100 < 30) {
        int tipo_evento = rand() % 3;
        if (tipo_evento == 0) { // Evento Positivo
            printf("\n--- ANUNCIO DA CAPITAL ---\n");
            printf("Um paraquedas desce na arena com suprimentos!\n");
            adicionar_item(jogo, ITEM_MEDICINA, 1, 0);
            adicionar_item(jogo, ITEM_AGUA, 2, 0);
        } else if (tipo_evento == 1) { // Evento Negativo
            printf("\n--- ALERTA DA CAPITAL ---\n");
            printf("Uma neblina tóxica se espalha! Sua saude é afetada.\n");
            jogo->jogador.saude -= 15;
            if (jogo->jogador.saude <= 0) jogo->jogador.vivo = false;
        } else { // Evento Neutro
            printf("\n--- ACONTECIMENTO NA ARENA ---\n");
            printf("Uma chuva torrencial atinge a arena. Sua sede diminui.\n");
            jogo->jogador.sede -= 20;
            if (jogo->jogador.sede < 0) jogo->jogador.sede = 0;
        }
    }
}


void exibir_tela_fim_jogo(const Jogo* jogo) {
    system("cls || clear");
    if (jogo->jogador.vivo) {
        printf("\n==========================================\n");
        printf("||          VITORIA GLORIOSA!           ||\n");
        printf("==========================================\n");
        printf("  Parabéns, %s do Distrito %s!\n", jogo->jogador.nome, jogo->jogador.distrito);
        printf("  Você é o único sobrevivente dos 24 tributos.\n");
        printf("  Seu nome será eternizado em Panem!\n");
        printf("==========================================\n");
    } else {
        printf("\n==========================================\n");
        printf("||            FIM DE JOGO               ||\n");
        printf("==========================================\n");
        printf("  Você foi eliminado no Dia %d.\n", jogo->diaAtual);
        printf("  Seu esforço foi em vão, mas sua memória viverá.\n");
        printf("==========================================\n");
    }
    printf("\n--- Sua Jornada ---\n");
    printf("  Dias Sobrevividos: %d\n", jogo->diaAtual);
    printf("  Tributos Eliminados: %d\n", NUM_TRIBUTOS - jogo->tributosVivos);
    printf("  Saúde Final: %d\n", jogo->jogador.saude);
    printf("  Itens no Inventário: \n");
    for (int i = 0; i < 10; i++) {
        if (jogo->inventario[i].tipo != ITEM_NENHUM) {
            printf("    - %s (%d)\n", nome_item(jogo->inventario[i].tipo), jogo->inventario[i].quantidade);
        }
    }
    printf("\n\n(Pressione Enter para voltar ao Menu Principal)\n");
    getchar();
    getchar();
}


const char* nome_item(TipoItem tipo) {
    switch(tipo) {
        case ITEM_COMIDA: return "comida";
        case ITEM_AGUA: return "água";
        case ITEM_MEDICINA: return "kit médico";
        case ITEM_FACA: return "faca";
        case ITEM_ESPADA: return "espada";
        case ITEM_ARCO: return "arco";
        default: return "desconhecido";
    }
}


void adicionar_item(Jogo* jogo, TipoItem tipo, int quantidade, int dano_extra) {
    for (int i = 0; i < 10; i++) {
        if (jogo->inventario[i].tipo == ITEM_NENHUM || jogo->inventario[i].tipo == tipo) {
            jogo->inventario[i].tipo = tipo;
            jogo->inventario[i].quantidade += quantidade;
            jogo->inventario[i].dano_extra = dano_extra;
            printf("\nVocê encontrou %d %s!\n", quantidade, nome_item(tipo));
            return;
        }
    }
    printf("\nSeu inventário está cheio. Você não pode carregar mais itens.\n");
}


void usar_item(Jogo* jogo) {
    printf("\n--- Seu Inventário ---\n");
    for (int i = 0; i < 10; i++) {
        if (jogo->inventario[i].tipo != ITEM_NENHUM) {
            printf("%d. %s (%d)\n", i + 1, nome_item(jogo->inventario[i].tipo), jogo->inventario[i].quantidade);
        }
    }
    printf("Escolha o número do item que deseja usar (0 para cancelar): ");
    int escolha;
    scanf("%d", &escolha);
    escolha--;


    if (escolha >= 0 && escolha < 10 && jogo->inventario[escolha].tipo != ITEM_NENHUM) {
        Item* item = &jogo->inventario[escolha];
        switch(item->tipo) {
            case ITEM_COMIDA:
                jogo->jogador.fome -= 50;
                printf("Você comeu e a fome diminuiu.\n");
                break;
            case ITEM_AGUA:
                jogo->jogador.sede -= 50;
                printf("Você bebeu e a sede diminuiu.\n");
                break;
            case ITEM_MEDICINA:
                jogo->jogador.saude += 30;
                printf("Você usou o kit médico e recuperou vida.\n");
                break;
            default:
                printf("Você não pode usar este item agora.\n");
        }
        item->quantidade--;
        if (item->quantidade <= 0) {
            item->tipo = ITEM_NENHUM;
        }
    } else {
        printf("Escolha inválida.\n");
    }
}




// --- Loop Principal e Função Main ---


void loop_principal(Jogo* jogo) {
    int escolha;
    while (jogo->jogador.vivo && jogo->tributosVivos > 1) {
        exibir_status(jogo);
        evento_capital(jogo);


        // Atualização de atributos diários
        jogo->jogador.fome += 5;
        jogo->jogador.sede += 8;
        if (jogo->jogador.fome >= 100) jogo->jogador.saude -= 5;
        if (jogo->jogador.sede >= 100) jogo->jogador.saude -= 10;
       
        printf("\nO que você fará hoje?\n");
        printf("1. Explorar (caçar, encontrar itens e inimigos)\n");
        printf("2. Caçar (foca em obter comida)\n");
        printf("3. Descansar (recupera energia)\n");
        printf("4. Usar Item (do inventário)\n");
        printf("Sua escolha: ");
        scanf("%d", &escolha);


        processar_acao(jogo, escolha);
       
        // Ações dos NPCs
        acao_npc(jogo);


        // Condição de derrota por falta de atributos
        if (jogo->jogador.saude <= 0) jogo->jogador.vivo = false;


        jogo->diaAtual++;
        salvar_jogo(jogo);
    }
    exibir_tela_fim_jogo(jogo);
}


int main() {
    srand(time(NULL));
    int escolha;
    Jogo jogo;


    exibir_tela_titulo();
   
    while (true) {
        exibir_menu_principal();
        printf("Escolha uma opção: ");
        scanf("%d", &escolha);


        switch (escolha) {
            case 1:
                criar_novo_jogo(&jogo);
                loop_principal(&jogo);
                break;
            case 2:
                if (carregar_jogo(&jogo)) {
                    loop_principal(&jogo);
                }
                break;
            case 3:
                printf("Obrigado por jogar!\n");
                return 0;
            default:
                printf("Opção inválida. Tente novamente.\n");
        }
    }


    return 0;
}
