#ifndef UI_H
#define UI_H
#include "jogo.h"

void limpar_buffer();
void esperar_tecla();
void exibir_tela_titulo();
void exibir_menu_principal();
void exibir_status(const Jogo* jogo);
void exibir_tela_fim_jogo(const Jogo* jogo);
void display_art(const char* art_name);

#endif // UI_H
