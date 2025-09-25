
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui.h"
#include "jogo.h"

void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void esperar_tecla() {
    printf("(Pressione Enter para continuar)");
    fflush(stdin);
    getchar();
}

void exibir_tela_titulo() {
    system("cls || clear");
    printf("=================================================================\n");
    printf("   __  __ _                      _ _   _                          \n");
    printf("  |  \\/  (_) ___ ___  _ __   ___| | | | |_ _ __ ___              \n");
    printf("  | |\/| | |/ __/ _ \\| '_ \\ / _ | | | | __| '_ ` _ \\           \n");
    printf("  | |  | | | (_| (_) | | | |  __/ | |_| |_| | | | | |            \n");
    printf("  |_|  |_|_|\\___\\___/|_| |_|\\___|_|\\__\\__|_| |_| |_|          \n");
    printf("=================================================================\n");
    printf("\n                 (Pressione qualquer tecla para continuar)\n");
    esperar_tecla();
}

void exibir_menu_principal() {
    system("cls || clear");
    printf("--- Simulador de Hunger Games ---\n");
    printf("1. Novo Jogo\n");
    printf("2. Continuar Jogo\n");
    printf("3. Sair\n");
    printf("-----------------------------------\n");
}

void exibir_status(const Jogo* jogo) {
    system("cls || clear");
    printf("==========================================\n");
    printf(" Dia: %d | Tributos Vivos: %d de %d\n", jogo->diaAtual, jogo->tributosVivos, NUM_TRIBUTOS);
    printf("------------------------------------------\n");
    printf(" Tributo: %s (Distrito %s)\n", jogo->jogador.nome, jogo->jogador.distrito);
    printf(" Saude:   [%-20s] %d\n", "####################" + (20 - jogo->jogador.saude/5), jogo->jogador.saude);
    printf(" Fome:    [%-20s] %d\n", "####################" + (20 - jogo->jogador.fome/5), jogo->jogador.fome);
    printf(" Sede:    [%-20s] %d\n", "####################" + (20 - jogo->jogador.sede/5), jogo->jogador.sede);
    printf(" Energia: [%-20s] %d\n", "####################" + (20 - jogo->jogador.energia/5), jogo->jogador.energia);
    printf("------------------------------------------\n");
}

void exibir_tela_fim_jogo(const Jogo* jogo) {
    system("cls || clear");
    if (jogo->jogador.vivo) {
        display_art("victory");
        printf("\n==========================================\n");
        printf("||          VITORIA GLORIOSA!           ||\n");
        printf("==========================================\n");
        printf("  Parabens, %s do Distrito %s!\n", jogo->jogador.nome, jogo->jogador.distrito);
        printf("  Voce e o unico sobrevivente dos 24 tributos.\n");
        printf("  Seu nome sera eternizado em Panem!\n");
        printf("==========================================\n");
    } else {
        display_art("defeat");
        printf("\n==========================================\n");
        printf("||            FIM DE JOGO               ||\n");
        printf("==========================================\n");
        printf("  Voce foi eliminado no Dia %d.\n", jogo->diaAtual);
        printf("  Causa da morte: %s\n", jogo->causa_morte[0] ? jogo->causa_morte : "Desconhecida");
        printf("  Seu esforco foi em vao, mas sua memoria vivera.\n");
        printf("==========================================\n");
    }
    printf("\n--- Sua Jornada ---\n");
    printf("  Dias Sobrevividos: %d\n", jogo->diaAtual);
    printf("  Tributos Eliminados: %d\n", NUM_TRIBUTOS - jogo->tributosVivos);
    printf("  Saude Final: %d\n", jogo->jogador.saude);
    printf("  Itens no Inventario: \n");
    for (int i = 0; i < 10; i++) {
        if (jogo->inventario[i].tipo != ITEM_NENHUM) {
            printf("    - %s (%d)\n", nome_item(jogo->inventario[i].tipo), jogo->inventario[i].quantidade);
        }
    }
    printf("\n\n(Pressione Enter para voltar ao Menu Principal)\n");
    getchar();
    limpar_buffer();
}

void display_art(const char* art_name) {
    printf("\n");
    if (strcmp(art_name, "cornucopia") == 0) {
        printf("        /\\                                    /\\        \n");
        printf("       /  \\_________________________________ /  \\       \n");
        printf("      /                                         \\      \n");
        printf("      \\  (  A Cornucopia esta aberta!  )           /      \n");
        printf("       \\ /_______________________________________\\ /       \n");
        printf("        \\/                                        \\/        \n");
    } else if (strcmp(art_name, "forest") == 0) {
        printf("       /\\             /\\             /\\       \n");
        printf("      /  \\           /  \\           /  \\      \n");
        printf("     /    \\         /    \\         /    \\     \n");
        printf("    /______\\       /______\\       /______\\    \n");
        printf("       ||             ||             ||        \n");
        printf("       ||             ||             ||        \n");
    } else if (strcmp(art_name, "tribute_encounter") == 0) {
        printf("          /\\                               /\\         \n");
        printf("        /  \\                             /  \\        \n");
        printf("       /    \\      ! CONFRONTO !       /    \\       \n");
        printf("      /______\\                         /______\\      \n");
    } else if (strcmp(art_name, "combat") == 0) {
        printf("        __--_              _--__        \n");
        printf("       /\\_-_/              \\_-_/\\       \n");
        printf("      / /                   \\ \\       \n");
        printf("     /  (      COMBATE      )  \\      \n");
        printf("    /   \\___________________/   \\     \n");
    } else if (strcmp(art_name, "victory") == 0) {
        printf("       ____  _                           _  _   _ \n");
        printf("      / ___|| |_ _   _ _ __ ___  _ __   | || \\ | |\n");
        printf("     | |  _ | __| | | | '__/ _ \\| '_ \\  | ||  \\| |\n");
        printf("     | |_| || |_| |_| | | |  __/| | | | | || |\\  |\n");
        printf("      \\____| \\__|\\__,_|_|  \\___|_| |_| |_||_| \\_|\n");
        printf("                                              \n");
    } else if (strcmp(art_name, "defeat") == 0) {
        printf("         _                       _   \n");
        printf("        | |__   ___   __ _  _ __| |_ \n");
        printf("        | '_ \\ / _ \\ / _` || '__| __|\n");
        printf("        | |_) |  __/| (_| || |  | |_ \n");
        printf("        |_.__/ \\___| \\__,_||_|   \\__|\n");
        printf("                                    \n");
    }
    printf("\n");
}
