#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#define TAM           40
#define LINHAS        20
#define COLUNAS       20
#define QTD_MOEDAS    15
#define QTD_PEDRAS    20
#define DANO_PEDRA    20
#define MAX_SCORES    50
#define ARQUIVO_MAPA  "mapa.dat"
#define ARQUIVO_SCORE "scores.txt"

typedef struct {
    int x;
    int y;
} Posicao;

typedef struct {
    Posicao pos;
    int coletada;
} Moeda;

typedef struct {
    Posicao pos;
} Pedra;

enum Estado {
    VIVO,
    GAME_OVER
};

typedef struct {
    char nome[50];
    Posicao pos;
    int pontos;
    int vida;
    enum Estado estado;
} Jogador;

typedef struct {
    char nome[50];
    int  pontos;
} ScoreEntry;

void mover(Jogador *j, int keycode) {
    if (keycode == ALLEGRO_KEY_UP    && j->pos.x > 0)           j->pos.x--;
    if (keycode == ALLEGRO_KEY_DOWN  && j->pos.x < LINHAS - 1)  j->pos.x++;
    if (keycode == ALLEGRO_KEY_LEFT  && j->pos.y > 0)           j->pos.y--;
    if (keycode == ALLEGRO_KEY_RIGHT && j->pos.y < COLUNAS - 1) j->pos.y++;
}

void verificar_coleta(Jogador *j, Moeda *moedas, int qtd) {
    for (int i = 0; i < qtd; i++) {
        if (!moedas[i].coletada &&
            moedas[i].pos.x == j->pos.x &&
            moedas[i].pos.y == j->pos.y) {
            moedas[i].coletada = 1;
            j->pontos += 10;
            printf("Moeda coletada! Pontos: %d\n", j->pontos);
        }
    }
}

void verificar_pedra(Jogador *j, Pedra *pedras, int qtd) {
    for (int i = 0; i < qtd; i++) {
        if (pedras[i].pos.x == j->pos.x && pedras[i].pos.y == j->pos.y) {
            j->vida -= DANO_PEDRA;
            printf("Bateu em pedra! -%d vida. Vida: %d\n", DANO_PEDRA, j->vida);
            if (j->vida <= 0) {
                j->vida = 0;
                j->estado = GAME_OVER;
            }
            return;
        }
    }
}

void salvar_mapa(const char *arquivo, Moeda *moedas, int qtd_m,
                 Pedra *pedras, int qtd_p) {
    FILE *f = fopen(arquivo, "wb");
    if (!f) { fprintf(stderr, "Erro ao salvar mapa.\n"); return; }
    fwrite(&qtd_m, sizeof(int), 1, f);
    fwrite(moedas, sizeof(Moeda), qtd_m, f);
    fwrite(&qtd_p, sizeof(int), 1, f);
    fwrite(pedras, sizeof(Pedra), qtd_p, f);
    fclose(f);
}

int carregar_mapa(const char *arquivo, Moeda **moedas, int *qtd_m,
                  Pedra **pedras, int *qtd_p) {
    FILE *f = fopen(arquivo, "rb");
    if (!f) return 0;
    fread(qtd_m, sizeof(int), 1, f);
    *moedas = malloc(*qtd_m * sizeof(Moeda));
    if (!*moedas) { fclose(f); return 0; }
    fread(*moedas, sizeof(Moeda), *qtd_m, f);
    fread(qtd_p, sizeof(int), 1, f);
    *pedras = malloc(*qtd_p * sizeof(Pedra));
    if (!*pedras) { free(*moedas); fclose(f); return 0; }
    fread(*pedras, sizeof(Pedra), *qtd_p, f);
    fclose(f);
    return 1;
}

void ordenar_scores(ScoreEntry *scores, int n) {
    for (int i = 1; i < n; i++) {
        ScoreEntry chave = scores[i];
        int j = i - 1;
        while (j >= 0 && scores[j].pontos < chave.pontos) {
            scores[j + 1] = scores[j];
            j--;
        }
        scores[j + 1] = chave;
    }
}

int buscar_score_linear(ScoreEntry *scores, int n, const char *nome) {
    for (int i = 0; i < n; i++) {
        if (strcmp(scores[i].nome, nome) == 0) return i;
    }
    return -1;
}

int buscar_score_binario(ScoreEntry *scores, int n, int pontos) {
    int inicio = 0, fim = n - 1;
    while (inicio <= fim) {
        int meio = (inicio + fim) / 2;
        if (scores[meio].pontos == pontos)      return meio;
        else if (scores[meio].pontos > pontos)  inicio = meio + 1;
        else                                    fim = meio - 1;
    }
    return -1;
}

int carregar_scores(const char *arquivo, ScoreEntry *scores) {
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

void salvar_scores(const char *arquivo, ScoreEntry *scores, int n) {
    FILE *f = fopen(arquivo, "w");
    if (!f) { fprintf(stderr, "Erro ao salvar scores.\n"); return; }
    for (int i = 0; i < n; i++) {
        fprintf(f, "%s %d\n", scores[i].nome, scores[i].pontos);
    }
    fclose(f);
}

void registrar_pontuacao(const char *nome, int pontos) {
    ScoreEntry scores[MAX_SCORES];
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
    if (pos != -1) printf("Sua posicao no ranking: %d\n", pos + 1);
}

void inicializar_mapa_padrao(Moeda *moedas, Pedra *pedras) {
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

    if (!al_init()) { fprintf(stderr, "Erro: al_init() falhou.\n"); return 1; }
    if (!al_init_primitives_addon()) { fprintf(stderr, "Erro: primitives falhou.\n"); return 1; }
    if (!al_install_keyboard()) { fprintf(stderr, "Erro: keyboard falhou.\n"); return 1; }

    ALLEGRO_DISPLAY *display = al_create_display(COLUNAS * TAM, LINHAS * TAM);
    if (!display) { fprintf(stderr, "Erro: display falhou.\n"); return 1; }

    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    if (!queue) { al_destroy_display(display); return 1; }

    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 30);
    if (!timer) { al_destroy_event_queue(queue); al_destroy_display(display); return 1; }

    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_timer_event_source(timer));

    Moeda *moedas = NULL;
    Pedra *pedras = NULL;
    int qtd_moedas = 0, qtd_pedras = 0;

    if (!carregar_mapa(ARQUIVO_MAPA, &moedas, &qtd_moedas, &pedras, &qtd_pedras)) {
        qtd_moedas = QTD_MOEDAS;
        qtd_pedras = QTD_PEDRAS;
        moedas = malloc(qtd_moedas * sizeof(Moeda));
        pedras = malloc(qtd_pedras * sizeof(Pedra));
        if (!moedas || !pedras) {
            al_destroy_timer(timer);
            al_destroy_event_queue(queue);
            al_destroy_display(display);
            return 1;
        }
        inicializar_mapa_padrao(moedas, pedras);
        salvar_mapa(ARQUIVO_MAPA, moedas, qtd_moedas, pedras, qtd_pedras);
        printf("Mapa criado e salvo em %s\n", ARQUIVO_MAPA);
    } else {
        printf("Mapa carregado de %s (%d moedas, %d pedras)\n",
               ARQUIVO_MAPA, qtd_moedas, qtd_pedras);
    }

    Jogador jogador;
    strcpy(jogador.nome, "Jogador");
    jogador.pos.x  = 0;
    jogador.pos.y  = 0;
    jogador.pontos = 0;
    jogador.vida   = 100;
    jogador.estado = VIVO;

    int redraw = 1;
    al_start_timer(timer);

    while (jogador.estado == VIVO) {

        ALLEGRO_EVENT ev;
        al_wait_for_event(queue, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            jogador.estado = GAME_OVER;

        if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                jogador.estado = GAME_OVER;
            } else {
                mover(&jogador, ev.keyboard.keycode);
                verificar_pedra(&jogador, pedras, qtd_pedras);
            }
        }

        if (ev.type == ALLEGRO_EVENT_TIMER) {
            if (jogador.estado == VIVO) {
                verificar_coleta(&jogador, moedas, qtd_moedas);
                if (jogador.pontos >= qtd_moedas * 10)
                    jogador.estado = GAME_OVER;
            }
            redraw = 1;
        }

        if (redraw && al_is_event_queue_empty(queue)) {

            redraw = 0;
            al_clear_to_color(al_map_rgb(20, 20, 20));

            for (int i = 0; i <= LINHAS; i++)
                al_draw_line(0, i * TAM, COLUNAS * TAM, i * TAM,
                             al_map_rgb(60, 60, 60), 1);
            for (int j = 0; j <= COLUNAS; j++)
                al_draw_line(j * TAM, 0, j * TAM, LINHAS * TAM,
                             al_map_rgb(60, 60, 60), 1);

            /* Pedras — cinza */
            for (int i = 0; i < qtd_pedras; i++) {
                al_draw_filled_rectangle(
                    pedras[i].pos.y * TAM + 2,
                    pedras[i].pos.x * TAM + 2,
                    pedras[i].pos.y * TAM + TAM - 2,
                    pedras[i].pos.x * TAM + TAM - 2,
                    al_map_rgb(130, 130, 130)
                );
            }

            /* Moedas — amarelo */
            for (int i = 0; i < qtd_moedas; i++) {
                if (!moedas[i].coletada) {
                    al_draw_filled_circle(
                        moedas[i].pos.y * TAM + TAM / 2,
                        moedas[i].pos.x * TAM + TAM / 2,
                        TAM / 2 - 6,
                        al_map_rgb(255, 215, 0)
                    );
                }
            }

            /* Jogador — verde */
            al_draw_filled_rectangle(
                jogador.pos.y * TAM + 3,
                jogador.pos.x * TAM + 3,
                jogador.pos.y * TAM + TAM - 3,
                jogador.pos.x * TAM + TAM - 3,
                al_map_rgb(0, 220, 80)
            );

            al_flip_display();
        }
    }

    printf("\nFim de jogo! Pontuacao: %d | Vida: %d\n",
           jogador.pontos, jogador.vida);

    registrar_pontuacao(jogador.nome, jogador.pontos);

    free(moedas);
    free(pedras);
    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_event_queue(queue);

    return 0;
}
