#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TOTAL_ACESSOS 20      // Total de acessos por execução
#define PAGINAS_VIRTUAIS 10   // Espaço de endereçamento
#define TOTAL_PAGINAS 10      // Número de páginas virtuais
#define TOTAL_QUADROS 4       // Memória física: número de quadros
#define __INT_MAX__ 99999     // Para simular valor "muito alto"

// Enum para escolher o algoritmo
typedef enum {
    FIFO,
    LRU
} AlgoritmoSubstituicao;

AlgoritmoSubstituicao algoritmo_usado = FIFO; // padrão: FIFO

// Estrutura para uma página virtual
typedef struct {
    int bitR;       // bit de referência
    int bitM;       // bit de modificação
    int timestamp;  // último tempo de acesso
    int quadro;     // posição na memória física
    int atual;      // está na memória? (1: sim, 0: não)
} Pagina;

// Estrutura de quadro da memória física
typedef struct {
    int pagina_virtual; // número da página carregada
    int ocupado;        // 1 se o quadro está em uso
} Quadro;

// Vetores principais
int acessos[TOTAL_ACESSOS];
Pagina tabela_paginas[TOTAL_PAGINAS];
Quadro memoria_fisica[TOTAL_QUADROS];

// Fila FIFO (circular)
int ponteiro_fifo = 0;

// Gera uma sequência de acessos com localidade
void gerar_acessos() {
    srand(time(NULL));
    for (int i = 0; i < TOTAL_ACESSOS; i++) {
        if (i > 0 && rand() % 100 < 70) {
            int deslocamento = (rand() % 3) - 1; // -1, 0, +1
            acessos[i] = acessos[i - 1] + deslocamento;
            if (acessos[i] < 0) acessos[i] = 0;
            if (acessos[i] >= PAGINAS_VIRTUAIS) acessos[i] = PAGINAS_VIRTUAIS - 1;
        } else {
            acessos[i] = rand() % PAGINAS_VIRTUAIS;
        }
    }
}

// Mostra a sequência de acessos
void imprimir_acessos() {
    printf("Sequencia de acessos a memoria:\n");
    for (int i = 0; i < TOTAL_ACESSOS; i++) {
        printf("%d ", acessos[i]);
    }
    printf("\n");
}

int menu() {
    int escolha;

    printf("\n===== SIMULADOR DE PAGINAÇAO =====\n");
    printf("1 - Escolher algoritmo de substituicao\n");
    printf("2 - Mostrar sequencia de acessos\n");
    printf("3 - Iniciar simulacao passo a passo\n");
    printf("4 - Executar simulacao completa (automatico)\n");
    printf("5 - Gerar nova sequencia de acessos\n");
    printf("6 - Sair\n");
    printf("Escolha uma opcao: ");
    scanf("%d", &escolha);
    return escolha;
}

// Inicializa as tabelas
void inicializar_simulador() {
    for (int i = 0; i < TOTAL_PAGINAS; i++) {
        tabela_paginas[i].atual = 0;
        tabela_paginas[i].quadro = -1;
        tabela_paginas[i].bitR = 0;
        tabela_paginas[i].bitM = 0;
        tabela_paginas[i].timestamp = 0;
    }

    for (int i = 0; i < TOTAL_QUADROS; i++) {
        memoria_fisica[i].ocupado = 0;
        memoria_fisica[i].pagina_virtual = -1;
    }
}

// Mostra a situação da memória a cada passo
void mostrar_estado_memoria() {
    printf("Memoria Fisica:\n");
    for (int i = 0; i < TOTAL_QUADROS; i++) {
        if (memoria_fisica[i].ocupado) {
            int pag = memoria_fisica[i].pagina_virtual;
            printf("Q%d: P%d [R=%d M=%d T=%d]\n",
                i,
                pag,
                tabela_paginas[pag].bitR,
                tabela_paginas[pag].bitM,
                tabela_paginas[pag].timestamp
            );
        } else {
            printf("Q%d: [vazio]\n", i);
        }
    }
    printf("\n");
}

// FIFO: retorna próximo quadro circularmente
int substituir_pagina_fifo() {
    int quadro_a_usar = ponteiro_fifo;
    ponteiro_fifo = (ponteiro_fifo + 1) % TOTAL_QUADROS;
    return quadro_a_usar;
}

// LRU: retorna quadro com timestamp mais antigo
int substituir_pagina_lru() {
    int quadro_mais_antigo = -1;
    int tempo_mais_antigo = __INT_MAX__;

    for (int i = 0; i < TOTAL_QUADROS; i++) {
        int pagina = memoria_fisica[i].pagina_virtual;
        int tempo = tabela_paginas[pagina].timestamp;

        if (tempo < tempo_mais_antigo) {
            tempo_mais_antigo = tempo;
            quadro_mais_antigo = i;
        }
    }

    return quadro_mais_antigo;
}

// Carrega nova página na memória (com ou sem substituição)
void carregar_pagina(int pagina, int tempo_atual) {
    // Verifica se há quadro livre
    for (int i = 0; i < TOTAL_QUADROS; i++) {
        if (!memoria_fisica[i].ocupado) {
            memoria_fisica[i].ocupado = 1;
            memoria_fisica[i].pagina_virtual = pagina;

            tabela_paginas[pagina].atual = 1;
            tabela_paginas[pagina].quadro = i;
            tabela_paginas[pagina].bitR = 1;
            tabela_paginas[pagina].bitM = 0;
            tabela_paginas[pagina].timestamp = tempo_atual;

            printf("Pagina %d carregada em quadro %d (sem substituicao)\n", pagina, i);
            return;
        }
    }

    // Substituição necessária
    int quadro_substituido;
    if (algoritmo_usado == FIFO) {
        quadro_substituido = substituir_pagina_fifo();
    } else {
        quadro_substituido = substituir_pagina_lru();
    }

    int pagina_antiga = memoria_fisica[quadro_substituido].pagina_virtual;

    // Remove página antiga
    tabela_paginas[pagina_antiga].atual = 0;
    tabela_paginas[pagina_antiga].quadro = -1;

    // Carrega nova
    memoria_fisica[quadro_substituido].pagina_virtual = pagina;
    tabela_paginas[pagina].atual = 1;
    tabela_paginas[pagina].quadro = quadro_substituido;
    tabela_paginas[pagina].bitR = 1;
    tabela_paginas[pagina].bitM = 0;
    tabela_paginas[pagina].timestamp = tempo_atual;

    printf("Pagina %d substituiu pagina %d no quadro %d\n", pagina, pagina_antiga, quadro_substituido);
}

// Processa o vetor de acessos simulando passo a passo
void acessar_pagina(int numero_pagina, int tempo_atual) {
    if (tabela_paginas[numero_pagina].atual) {
        // HIT
        tabela_paginas[numero_pagina].bitR = 1;
        tabela_paginas[numero_pagina].timestamp = tempo_atual;
        printf("Acesso a pagina %d -> HIT\n", numero_pagina);
    } else {
        // PAGE FAULT
        printf("Acesso a pagina %d -> PAGE FAULT\n", numero_pagina);
        carregar_pagina(numero_pagina, tempo_atual);
    }

    mostrar_estado_memoria();
}

// Executa a simulação completa
void executar_simulacao() {
    for (int tempo = 0; tempo < TOTAL_ACESSOS; tempo++) {
        acessar_pagina(acessos[tempo], tempo);
        getchar(); // Aguarda Enter para continuar
    }
}

// Função principal
int main() {
    int opcao;
    gerar_acessos();
    inicializar_simulador();

    do {
        opcao = menu();
        switch (opcao) {
            case 1:
                printf("\nEscolha o algoritmo:\n1 - FIFO\n2 - LRU\n> ");
                int escolha;
                scanf("%d", &escolha);
                algoritmo_usado = (escolha == 2) ? LRU : FIFO;
                printf("Algoritmo selecionado: %s\n", algoritmo_usado == LRU ? "LRU" : "FIFO");
                break;

            case 2:
                imprimir_acessos();
                break;

            case 3:
                printf("\n--- Simulacao Passo a Passo ---\n");
                executar_simulacao(); // já pausa com getchar()
                break;

            case 4:
                printf("\n--- Simulacao Automatica ---\n");
                for (int tempo = 0; tempo < TOTAL_ACESSOS; tempo++) {
                    acessar_pagina(acessos[tempo], tempo);
                    // sem pausa
                }
                break;

            case 5:
                gerar_acessos();
                inicializar_simulador();
                printf("Nova sequencia gerada e memoria reinicializada.\n");
                break;

            case 6:
                printf("Encerrando...\n");
                break;

            default:
                printf("Opcao invalida.\n");
        }

    } while (opcao != 6);

    return 0;
}