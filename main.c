// Bibliotecas padrão
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <locale.h>

// Definicoes
#define NUM_TRIBUTOS 24
#define MAX_NOME 50
#define ARQUIVO_SAVE "save.dat"

// --- Enumeracoes e Structs ---


void esperar_tecla();


#include "jogo.h"
#include "ui.h"

    int main() {
        setlocale(LC_ALL, "Portuguese_Brazil");
        srand(time(NULL));
        char escolha;
        Jogo jogo;

        exibir_tela_titulo();
        while (1) {
            exibir_menu_principal();
            printf("Escolha uma opcao: ");
            do {
                escolha = getchar();
            } while (escolha != '1' && escolha != '2' && escolha != '3');
            limpar_buffer();
            switch (escolha) {
                case '1':
                    display_art("cornucopia");
                    criar_novo_jogo(&jogo);
                    loop_principal(&jogo);
                    break;
                case '2':
                    if (carregar_jogo(&jogo)) {
                        loop_principal(&jogo);
                    }
                    break;
                case '3':
                    printf("Obrigado por jogar!\n");
                    return 0;
                default:
                    printf("Opcao invalida. Tente novamente.\n");
            }
        }
        return 0;
    }