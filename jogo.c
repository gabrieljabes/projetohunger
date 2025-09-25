#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "jogo.h"
#include "ui.h"

void pausa() {
	printf("\nPressione ENTER para continuar...");
	getchar();
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "jogo.h"
#include "ui.h"

// Funções de lógica do jogo

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

void criar_novo_jogo(Jogo* jogo) {
	char nome[MAX_NOME];
	char distrito[10];
	system("cls || clear");
	printf("--- Novo Jogo ---\n");
	printf("Qual o seu nome? ");
	scanf("%s", nome);
	limpar_buffer();
	printf("De qual distrito voce e? ");
	scanf("%s", distrito);
	limpar_buffer();

	Tributo jogador_base;
	strcpy(jogador_base.nome, nome);
	strcpy(jogador_base.distrito, distrito);
	jogador_base.saude = 100;
	jogador_base.fome = 0;   // Começa sem fome
	jogador_base.sede = 0;   // Começa sem sede
	jogador_base.energia = 100;
	jogador_base.forca = rand() % 10 + 1;
	jogador_base.agilidade = rand() % 10 + 1;
	jogador_base.carisma = rand() % 10 + 1;
	jogador_base.vivo = true;
	jogo->jogador = jogador_base;

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

void processar_acao(Jogo* jogo, char decisao) {
	if (jogo->jogador.energia < 10 && decisao != '3') {
		printf("Voce esta exausto demais para essa acao. Descanse!\n");
		do{
			decisao = getchar();
		}while(decisao != '3');
	}
	switch (decisao) {
		case '1':
			printf("\nVocê gasta energia explorando a área...\n");
			jogo->jogador.energia -= 15;
			display_art("forest");
			int achou_algo = 0;
			if (rand() % 100 < 40) {
				int tipo_encontrado = rand() % 10;
				if (tipo_encontrado < 4) {
					int item = rand() % 3;
					if (item == 0) {
						adicionar_item(jogo, ITEM_COMIDA, 1, 0);
						printf("Você encontrou comida durante a exploração!\n");
					} else if (item == 1) {
						adicionar_item(jogo, ITEM_AGUA, 1, 0);
						printf("Você encontrou água durante a exploração!\n");
					} else {
						adicionar_item(jogo, ITEM_MEDICINA, 1, 0);
						printf("Você encontrou um kit médico durante a exploração!\n");
					}
					achou_algo = 1;
				} else if (tipo_encontrado < 8) {
					adicionar_item(jogo, ITEM_FACA, 1, 5);
					printf("Você encontrou uma faca!\n");
					achou_algo = 1;
				} else if (tipo_encontrado < 9) {
					adicionar_item(jogo, ITEM_ESPADA, 1, 15);
					printf("Você encontrou uma espada!\n");
					achou_algo = 1;
				} else {
					adicionar_item(jogo, ITEM_ARCO, 1, 20);
					printf("Você encontrou um arco!\n");
					achou_algo = 1;
				}
			}
			if (!achou_algo) {
				printf("Você não encontrou nada de útil desta vez.\n");
			}
			pausa();
			if (rand() % 100 < 30) {
				int tributo_encontrado_index;
				do {
					tributo_encontrado_index = rand() % (NUM_TRIBUTOS - 1);
				} while (!jogo->tributos_npcs[tributo_encontrado_index].vivo);
				Tributo* inimigo = &jogo->tributos_npcs[tributo_encontrado_index];
				printf("\nVoce se depara com %s do Distrito %s!\n", inimigo->nome, inimigo->distrito);
				display_art("tribute_encounter");
				int escolha_combate;
				printf("1. Atacar\n2. Tentar alianca\n3. Fugir\nSua escolha: ");
				if(scanf("%d", &escolha_combate) != 1) {
					printf("Entrada invalida. Acao cancelada.\n");
					limpar_buffer();
					pausa();
					break;
				}
				limpar_buffer();
				if (escolha_combate == 1) {
					combate(jogo, inimigo);
				} else if (escolha_combate == 2) {
					if (rand() % 100 < jogo->jogador.carisma * 5) {
						printf("%s aceita a alianca. Voces se separam em paz.\n", inimigo->nome);
					} else {
						printf("%s nao confia em voce. A tensao aumenta e voces se separam.\n", inimigo->nome);
					}
				} else if (escolha_combate == 3) {
					if (rand() % 100 < jogo->jogador.agilidade * 7) {
						printf("Voce conseguiu fugir com sucesso!\n");
					} else {
						printf("Voce falha em fugir e sofre um ataque de %s!\n", inimigo->nome);
						jogo->jogador.saude -= 10;
					}
				} else {
					printf("Escolha invalida. Acao cancelada.\n");
				}
				pausa();
			}
			break;
		case '2':
			printf("\nVocê gasta energia tentando caçar...\n");
			jogo->jogador.energia -= 20;
			if (rand() % 100 < jogo->jogador.agilidade * 5) {
				adicionar_item(jogo, ITEM_COMIDA, 2, 0);
				printf("Você conseguiu caçar e obteve 2 unidades de comida!\n");
			} else {
				printf("Você não conseguiu caçar nada desta vez.\n");
			}
			pausa();
			break;
		case '3':
			printf("\nVoce descansa e recupera suas energias.\n");
			jogo->jogador.energia += 40;
			if (jogo->jogador.energia > 100) jogo->jogador.energia = 100;
			pausa();
			break;
		case '4':
			usar_item(jogo);
			pausa();
			break;
		default:
			printf("Acao invalida. Tente novamente.\n");
			break;
	}
}

void acao_npc(Jogo* jogo) {
	for (int i = 0; i < NUM_TRIBUTOS - 1; i++) {
		Tributo* npc = &jogo->tributos_npcs[i];
		if (npc->vivo) {
			int acao = rand() % 3;
			if (acao == 0) {
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
			} else if (acao == 1) {
				printf("- %s esta procurando por suprimentos.\n", npc->nome);
				npc->fome += 10;
				npc->sede += 10;
			} else {
				printf("- %s esta descansando.\n", npc->nome);
				npc->energia += 20;
			}
			if (npc->fome >= 100) {
				npc->vivo = false;
				jogo->tributosVivos--;
				printf(">>> %s morreu de fome.\n", npc->nome);
			}
			if (npc->sede >= 100) {
				npc->vivo = false;
				jogo->tributosVivos--;
				printf(">>> %s morreu de sede.\n", npc->nome);
			}
		}
	}
}

void combate(Jogo* jogo, Tributo* inimigo) {
	printf("\n");
	display_art("combat");
	while (jogo->jogador.vivo && inimigo->vivo) {
		printf("\n--- COMBATE! ---\n");
		printf("Sua vida: %d | Vida de %s: %d\n", jogo->jogador.saude, inimigo->nome, inimigo->saude);
		int dano_jogador = calcular_dano_total(&jogo->jogador, jogo);
		printf("Voce ataca %s, causando %d de dano!\n", inimigo->nome, dano_jogador);
		inimigo->saude -= dano_jogador;
		if (inimigo->saude <= 0) {
			inimigo->vivo = false;
			jogo->tributosVivos--;
			printf("Voce derrotou %s!\n", inimigo->nome);
			printf("Voce saqueia o corpo de %s, encontrando comida e um kit medico.\n", inimigo->nome);
			adicionar_item(jogo, ITEM_COMIDA, 2, 0);
			adicionar_item(jogo, ITEM_MEDICINA, 1, 0);
			break;
		}
		int dano_inimigo = rand() % 5 + inimigo->forca;
		printf("%s ataca voce, causando %d de dano!\n", inimigo->nome, dano_inimigo);
		jogo->jogador.saude -= dano_inimigo;
		if (jogo->jogador.saude <= 0) {
			jogo->jogador.vivo = false;
			printf("%s te derrotou. Voce foi eliminado.\n", inimigo->nome);
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
		if (tipo_evento == 0) {
			printf("\n--- ANUNCIO DA CAPITAL ---\n");
			printf("Um paraquedas desce na arena com suprimentos!\n");
			adicionar_item(jogo, ITEM_MEDICINA, 1, 0);
			adicionar_item(jogo, ITEM_AGUA, 2, 0);
		} else if (tipo_evento == 1) {
			printf("\n--- ALERTA DA CAPITAL ---\n");
			printf("Uma neblina toxica se espalha! Sua saude e afetada.\n");
			jogo->jogador.saude -= 15;
			if (jogo->jogador.saude <= 0) jogo->jogador.vivo = false;
		} else {
			printf("\n--- ACONTECIMENTO NA ARENA ---\n");
			printf("Uma chuva torrencial atinge a arena. Sua sede diminui.\n");
			jogo->jogador.sede -= 20;
			if (jogo->jogador.sede < 0) jogo->jogador.sede = 0;
		}
	}
}

void adicionar_item(Jogo* jogo, TipoItem tipo, int quantidade, int dano_extra) {
	for (int i = 0; i < 10; i++) {
		if (jogo->inventario[i].tipo == ITEM_NENHUM || jogo->inventario[i].tipo == tipo) {
			jogo->inventario[i].tipo = tipo;
			jogo->inventario[i].quantidade += quantidade;
			jogo->inventario[i].dano_extra = dano_extra;
			printf("\nVoce encontrou %d %s!\n", quantidade, nome_item(tipo));
			return;
		}
	}
	printf("\nSeu inventario esta cheio. Voce nao pode carregar mais itens.\n");
}

void usar_item(Jogo* jogo) {
	printf("\n--- Seu Inventario ---\n");
	for (int i = 0; i < 10; i++) {
		if (jogo->inventario[i].tipo != ITEM_NENHUM) {
			printf("%d. %s (%d)\n", i + 1, nome_item(jogo->inventario[i].tipo), jogo->inventario[i].quantidade);
		} else {
			printf("%d. [vazio]\n", i + 1);
		}
	}
	int escolha = -1;
	while (1) {
		printf("Escolha o numero do item que deseja usar (0 para cancelar): ");
		if(scanf("%d", &escolha) != 1) {
			printf("Entrada invalida.\n");
			limpar_buffer();
			continue;
		}
		limpar_buffer();
		if (escolha == 0) {
			printf("Acao cancelada.\n");
			return;
		}
		if (escolha < 1 || escolha > 10) {
			printf("Escolha invalida. Digite um numero de 1 a 10 ou 0 para cancelar.\n");
			continue;
		}
		int idx = escolha - 1;
		if (jogo->inventario[idx].tipo == ITEM_NENHUM) {
			printf("Esse slot esta vazio. Escolha outro.\n");
			continue;
		}
		Item* item = &jogo->inventario[idx];
		switch(item->tipo) {
			case ITEM_COMIDA:
				jogo->jogador.fome -= 50;
				if(jogo->jogador.fome < 0) jogo->jogador.fome = 0;
				printf("Voce comeu e a fome diminuiu.\n");
				break;
			case ITEM_AGUA:
				jogo->jogador.sede -= 50;
				if(jogo->jogador.sede < 0) jogo->jogador.sede = 0;
				printf("Voce bebeu e a sede diminuiu.\n");
				break;
			case ITEM_MEDICINA:
				jogo->jogador.saude += 30;
				if(jogo->jogador.saude > 100) jogo->jogador.saude = 100;
				printf("Voce usou o kit medico e recuperou vida.\n");
				break;
			default:
				printf("Voce nao pode usar este item agora.\n");
		}
		item->quantidade--;
		if (item->quantidade <= 0) {
			item->tipo = ITEM_NENHUM;
		}
		break;
	}
}

const char* nome_item(TipoItem tipo) {
	switch(tipo) {
		case ITEM_COMIDA: return "comida";
		case ITEM_AGUA: return "agua";
		case ITEM_MEDICINA: return "kit medico";
		case ITEM_FACA: return "faca";
		case ITEM_ESPADA: return "espada";
		case ITEM_ARCO: return "arco";
		default: return "desconhecido";
	}
}

void loop_principal(Jogo* jogo) {
	char decisao;
	while (jogo->jogador.vivo && jogo->tributosVivos > 1) {
		exibir_status(jogo);
		evento_capital(jogo);
		jogo->jogador.fome += 5;
		if(jogo->jogador.fome > 100) jogo->jogador.fome = 100;
		jogo->jogador.sede += 8;
		if(jogo->jogador.sede > 100) jogo->jogador.sede = 100;
		if (jogo->jogador.fome >= 100) {
			printf("\n(Atencao: Você está com fome extrema! Sua saúde está sendo afetada.)\n");
			jogo->jogador.saude -= 5;
			if (jogo->jogador.saude <= 0) {
				jogo->jogador.vivo = false;
				snprintf(jogo->causa_morte, sizeof(jogo->causa_morte), "Morreu de fome");
				continue;
			}
		}
		if (jogo->jogador.sede >= 100) {
			printf("\n(Atencao: Você está com sede extrema! Sua saúde está sendo afetada.)\n");
			jogo->jogador.saude -= 10;
			if (jogo->jogador.saude <= 0) {
				jogo->jogador.vivo = false;
				snprintf(jogo->causa_morte, sizeof(jogo->causa_morte), "Morreu de sede");
				continue;
			}
		}
		printf("\nO que voce fara hoje?\n");
		printf("1. Explorar (cacar, encontrar itens e inimigos)\n");
		printf("2. Cacar (foca em obter comida)\n");
		printf("3. Descansar (recupera energia)\n");
		printf("4. Usar Item (do inventario)\n");
		printf("Sua escolha: ");
		do{
			decisao = getchar();
		} while(decisao != '1' && decisao != '2' && decisao != '3' && decisao != '4');
		limpar_buffer();
		processar_acao(jogo, decisao);
		acao_npc(jogo);
		if (jogo->jogador.saude <= 0) {
			jogo->jogador.vivo = false;
		}
		jogo->diaAtual++;
		salvar_jogo(jogo);
	}
	exibir_tela_fim_jogo(jogo);
}
