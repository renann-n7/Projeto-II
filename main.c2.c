#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM           20
#define QTD_MOEDAS    15
#define QTD_PEDRAS    20
#define DANO_PEDRA    20
#define MAX_SCORES    50
#define ARQUIVO_MAPA  "mapa.dat"
#define ARQUIVO_SCORE "scores.txt"
#define MAX_HISTORICO 200

struct Pilha {
    char movimentos[MAX_HISTORICO];
    int topo;
};

void pilha_inicializar(struct Pilha *p) {
    p->topo = -1;
}

void pilha_empurrar(struct Pilha *p, char mov) {
    if (p->topo < MAX_HISTORICO - 1)
        p->movimentos[++p->topo] = mov;
}

void pilha_exibir(struct Pilha *p) {
    printf("Historico de movimentos: ");
    for (int i = 0; i <= p->topo; i++)
        printf("%c ", p->movimentos[i]);
    printf("\n");
}

struct Posicao {
    int x;
    int y;
};

enum Estado {
    VIVO,
    GAME_OVER
};

struct Moeda {
    struct Posicao pos;
    int coletada;
};

struct Pedra {
    struct Posicao pos;
};

struct Jogador {
    char nome[50];
    struct Posicao pos;
    int pontos;
    int vida;
    enum Estado estado;
};

struct ScoreEntry {
    char nome[50];
    int  pontos;
};

void mover(struct Jogador *j, char d) {
    if (d == 'w' && j->pos.x > 0)        j->pos.x--;
    if (d == 's' && j->pos.x < TAM - 1)  j->pos.x++;
    if (d == 'a' && j->pos.y > 0)        j->pos.y--;
    if (d == 'd' && j->pos.y < TAM - 1)  j->pos.y++;
}

int tem_pedra(struct Pedra *pedras, int qtd, int x, int y) {
    for (int i = 0; i < qtd; i++) {
        if (pedras[i].pos.x == x && pedras[i].pos.y == y) return 1;
    }
    return -1;
}

void verificar_pedra(struct Jogador *j, struct Pedra *pedras, int qtd) {
    for (int i = 0; i < qtd; i++) {
        if (pedras[i].pos.x == j->pos.x && pedras[i].pos.y == j->pos.y) {
            j->vida -= DANO_PEDRA;
            printf("Voce bateu em uma pedra! -%d de vida. Vida restante: %d\n",
                   DANO_PEDRA, j->vida);
            if (j->vida <= 0) {
                j->vida = 0;
                j->estado = GAME_OVER;
                printf("Voce morreu!\n");
            }
            return;
        }
    }
}

void salvar_mapa(const char *arquivo, struct Moeda *moedas, int qtd_m,
                 struct Pedra *pedras, int qtd_p) {
    FILE *f = fopen(arquivo, "wb");
    if (!f) { fprintf(stderr, "Erro ao salvar mapa.\n"); return; }
    fwrite(&qtd_m, sizeof(int), 1, f);
    fwrite(moedas, sizeof(struct Moeda), qtd_m, f);
    fwrite(&qtd_p, sizeof(int), 1, f);
    fwrite(pedras, sizeof(struct Pedra), qtd_p, f);
    fclose(f);
}

int carregar_mapa(const char *arquivo, struct Moeda **moedas, int *qtd_m,
                  struct Pedra **pedras, int *qtd_p) {
    FILE *f = fopen(arquivo, "rb");
    if (!f) return 0;
    fread(qtd_m, sizeof(int), 1, f);
    *moedas = malloc(*qtd_m * sizeof(struct Moeda));
    if (!*moedas) { fclose(f); return 0; }
    fread(*moedas, sizeof(struct Moeda), *qtd_m, f);
    fread(qtd_p, sizeof(int), 1, f);
    *pedras = malloc(*qtd_p * sizeof(struct Pedra));
    if (!*pedras) { free(*moedas); fclose(f); return 0; }
    fread(*pedras, sizeof(struct Pedra), *qtd_p, f);
    fclose(f);
    return 1;
}

void ordenar_scores(struct ScoreEntry *scores, int n) {
    for (int i = 1; i < n; i++) {
        struct ScoreEntry chave = scores[i];
        int j = i - 1;
        while (j >= 0 && scores[j].pontos < chave.pontos) {
            scores[j + 1] = scores[j];
            j--;
        }
        scores[j + 1] = chave;
    }
}

int buscar_score_linear(struct ScoreEntry *scores, int n, const char *nome) {
    for (int i = 0; i < n; i++) {
        if (strcmp(scores[i].nome, nome) == 0) return i;
    }
    return -1;
}

int buscar_score_binario(struct ScoreEntry *scores, int n, int pontos) {
    int inicio = 0, fim = n - 1;
    while (inicio <= fim) {
        int meio = (inicio + fim) / 2;
        if (scores[meio].pontos == pontos)      return meio;
        else if (scores[meio].pontos > pontos)  inicio = meio + 1;
        else                                    fim = meio - 1;
    }
    return -1;
}

int carregar_scores(const char *arquivo, struct ScoreEntry *scores) {
    FILE *f = fopen(arquivo, "r");
    if (!f) return 0;
    int n = 0;
    while (n < MAX_SCORES &&
           fscanf(f, "%49s %d", scores[n].nome, &scores[n].pontos) == 2) {
        n++;
    }
    fclose(f);
    return n;
}

void salvar_scores(const char *arquivo, struct ScoreEntry *scores, int n) {
    FILE *f = fopen(arquivo, "w");
    if (!f) { fprintf(stderr, "Erro ao salvar scores.\n"); return; }
    for (int i = 0; i < n; i++) {
        fprintf(f, "%s %d\n", scores[i].nome, scores[i].pontos);
    }
    fclose(f);
}

void registrar_pontuacao(const char *nome, int pontos) {
    struct ScoreEntry scores[MAX_SCORES];
    int n = carregar_scores(ARQUIVO_SCORE, scores);

    int idx = buscar_score_linear(scores, n, nome);
    if (idx != -1) {
        if (pontos > scores[idx].pontos) scores[idx].pontos = pontos;
    } else {
        if (n < MAX_SCORES) {
            strcpy(scores[n].nome, nome);
            scores[n].pontos = pontos;
            n++;
        }
    }

    ordenar_scores(scores, n);
    salvar_scores(ARQUIVO_SCORE, scores, n);

    printf("\n=== RANKING (Top 5) ===\n");
    int exibir = n < 5 ? n : 5;
    for (int i = 0; i < exibir; i++) {
        printf("%d. %s — %d pts\n", i + 1, scores[i].nome, scores[i].pontos);
    }

    int pos = buscar_score_binario(scores, n, pontos);
    if (pos != -1) {
        printf("Sua posicao no ranking: %d\n", pos + 1);
    }
}

/* Posições fixas das 15 moedas e 20 pedras no mapa 20x20 */
void inicializar_mapa_padrao(struct Moeda *moedas, struct Pedra *pedras) {
    int pm[QTD_MOEDAS][2] = {
        {0,5},{0,15},{2,10},{4,1},{4,18},
        {6,6},{7,13},{8,3},{9,17},{10,8},
        {12,2},{13,15},{15,5},{17,11},{19,19}
    };
    for (int i = 0; i < QTD_MOEDAS; i++) {
        moedas[i].pos.x = pm[i][0];
        moedas[i].pos.y = pm[i][1];
        moedas[i].coletada = 0;
    }

    int pp[QTD_PEDRAS][2] = {
        {1,3},{1,12},{3,7},{3,16},{5,2},
        {5,9},{6,14},{7,4},{8,18},{9,1},
        {10,11},{11,6},{12,17},{13,3},{14,8},
        {14,13},{15,19},{16,5},{17,15},{18,9}
    };
    for (int i = 0; i < QTD_PEDRAS; i++) {
        pedras[i].pos.x = pp[i][0];
        pedras[i].pos.y = pp[i][1];
    }
}

int main() {
    struct Jogador jogador;
    struct Pilha historico;
    pilha_inicializar(&historico);
    struct Moeda  *moedas = NULL;
    struct Pedra  *pedras = NULL;
    int qtd_moedas = 0, qtd_pedras = 0;

    if (!carregar_mapa(ARQUIVO_MAPA, &moedas, &qtd_moedas, &pedras, &qtd_pedras)) {
        qtd_moedas = QTD_MOEDAS;
        qtd_pedras = QTD_PEDRAS;
        moedas = malloc(qtd_moedas * sizeof(struct Moeda));
        pedras = malloc(qtd_pedras * sizeof(struct Pedra));
        if (!moedas || !pedras) { printf("Erro de memoria\n"); return 1; }
        inicializar_mapa_padrao(moedas, pedras);
        salvar_mapa(ARQUIVO_MAPA, moedas, qtd_moedas, pedras, qtd_pedras);
        printf("Mapa criado e salvo em %s\n", ARQUIVO_MAPA);
    } else {
        printf("Mapa carregado de %s (%d moedas, %d pedras)\n",
               ARQUIVO_MAPA, qtd_moedas, qtd_pedras);
    }

    char nome_temp[50];
    printf("Digite seu nome: ");
    scanf(" %[^\n]", nome_temp);
    strcpy(jogador.nome, nome_temp);

    jogador.pos.x  = 0;
    jogador.pos.y  = 0;
    jogador.pontos = 0;
    jogador.vida   = 100;
    jogador.estado = VIVO;

    char comando;

    while (jogador.estado == VIVO) {

        printf("\nJogador: %s | Vida: %d | Pontos: %d\n",
               jogador.nome, jogador.vida, jogador.pontos);

        for (int i = 0; i < TAM; i++) {
            for (int j = 0; j < TAM; j++) {
                if (i == jogador.pos.x && j == jogador.pos.y) {
                    printf("P ");
                } else if (tem_pedra(pedras, qtd_pedras, i, j) == 1) {
                    printf("# ");
                } else {
                    int tem_moeda = 0;
                    for (int k = 0; k < qtd_moedas; k++) {
                        if (moedas[k].pos.x == i &&
                            moedas[k].pos.y == j &&
                            moedas[k].coletada == 0) {
                            printf("$ ");
                            tem_moeda = 1;
                            break;
                        }
                    }
                    if (!tem_moeda) printf(". ");
                }
            }
            printf("\n");
        }

        printf("Mover (w/a/s/d ou q): ");
        scanf(" %c", &comando);

        if (comando == 'q') break;

        mover(&jogador, comando);
        pilha_empurrar(&historico, comando);
        verificar_pedra(&jogador, pedras, qtd_pedras);

        if (jogador.estado == VIVO) {
            for (int i = 0; i < qtd_moedas; i++) {
                if (jogador.pos.x == moedas[i].pos.x &&
                    jogador.pos.y == moedas[i].pos.y &&
                    moedas[i].coletada == 0) {
                    moedas[i].coletada = 1;
                    jogador.pontos += 10;
                    printf("Moeda coletada! +10 pts\n");
                }
            }

            if (jogador.pontos >= qtd_moedas * 10) {
                jogador.estado = GAME_OVER;
            }
        }
    }

    printf("\nFim de jogo! Pontuacao final: %d | Vida restante: %d\n",
           jogador.pontos, jogador.vida);

    pilha_exibir(&historico);
    registrar_pontuacao(jogador.nome, jogador.pontos);

    free(moedas);
    free(pedras);
    return 0;
}
