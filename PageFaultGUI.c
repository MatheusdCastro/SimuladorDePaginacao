#include <gtk/gtk.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define TOTAL_ACESSOS 20
#define PAGINAS_VIRTUAIS 10
#define TOTAL_PAGINAS 10
#define TOTAL_QUADROS 4
#define __INT_MAX__ 99999

typedef enum {
    FIFO,
    LRU
} AlgoritmoSubstituicao;

AlgoritmoSubstituicao algoritmo_usado = FIFO;

typedef struct {
    int bitR;
    int bitM;
    int timestamp;
    int quadro;
    int atual;
} Pagina;

typedef struct {
    int pagina_virtual;
    int ocupado;
} Quadro;

int acessos[TOTAL_ACESSOS];
Pagina tabela_paginas[TOTAL_PAGINAS];
Quadro memoria_fisica[TOTAL_QUADROS];
int ponteiro_fifo = 0;
int tempo_atual = 0;

GtkWidget *label_info;
GtkWidget *button_next;
GtkWidget *combo_algoritmo;
GtkWidget *memory_grid;
GtkWidget *status_label;
GtkWidget *pages_list_box;

GtkWidget *frames[TOTAL_QUADROS];
GtkWidget *frame_labels[TOTAL_QUADROS];

int ultimo_page_fault = 0;
gboolean simulacao_iniciada = FALSE;

const char *css_data =
    "frame {"
    "  border-radius: 10px;"
    "  border: 2px solid #94BFBE;"
    "  background-color: #94BFBE;"
    "  color: #333333;"
    "}"
    "frame.page-fault {"
    "  background-color: #ACF7C1;"
    "  border-color: #ACF7C1;"
    "  color: #000000;"
    "}"
    "#janela-principal {"
    "  background-color: #3D6665;"
    "}";


void aplicar_css(GtkWidget *widget) {
    GtkCssProvider *cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(cssProvider, css_data, -1, NULL);
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(cssProvider);
}

void gerar_acessos() {
    srand(time(NULL));
    for (int i = 0; i < TOTAL_ACESSOS; i++) {
        if (i > 0 && rand() % 100 < 70) {
            int deslocamento = (rand() % 3) - 1;
            acessos[i] = acessos[i - 1] + deslocamento;
            if (acessos[i] < 0) acessos[i] = 0;
            if (acessos[i] >= PAGINAS_VIRTUAIS) acessos[i] = PAGINAS_VIRTUAIS - 1;
        } else {
            acessos[i] = rand() % PAGINAS_VIRTUAIS;
        }
    }
}

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

int substituir_pagina_fifo() {
    int quadro_a_usar = ponteiro_fifo;
    ponteiro_fifo = (ponteiro_fifo + 1) % TOTAL_QUADROS;
    return quadro_a_usar;
}

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

void carregar_pagina(int pagina, int tempo_atual) {
    for (int i = 0; i < TOTAL_QUADROS; i++) {
        if (!memoria_fisica[i].ocupado) {
            memoria_fisica[i].ocupado = 1;
            memoria_fisica[i].pagina_virtual = pagina;
            tabela_paginas[pagina].atual = 1;
            tabela_paginas[pagina].quadro = i;
            tabela_paginas[pagina].bitR = 1;
            tabela_paginas[pagina].timestamp = tempo_atual;
            ultimo_page_fault = 1;
            return;
        }
    }
    int quadro_substituido = (algoritmo_usado == FIFO) ? substituir_pagina_fifo() : substituir_pagina_lru();
    int pagina_antiga = memoria_fisica[quadro_substituido].pagina_virtual;
    tabela_paginas[pagina_antiga].atual = 0;
    tabela_paginas[pagina_antiga].quadro = -1;
    memoria_fisica[quadro_substituido].pagina_virtual = pagina;
    tabela_paginas[pagina].atual = 1;
    tabela_paginas[pagina].quadro = quadro_substituido;
    tabela_paginas[pagina].bitR = 1;
    tabela_paginas[pagina].timestamp = tempo_atual;
    ultimo_page_fault = 1;
}

void acessar_pagina(int numero_pagina, int tempo_atual) {
    if (tabela_paginas[numero_pagina].atual) {
        tabela_paginas[numero_pagina].bitR = 1;
        tabela_paginas[numero_pagina].timestamp = tempo_atual;
        ultimo_page_fault = 0;
    } else {
        carregar_pagina(numero_pagina, tempo_atual);
    }
}

void atualizar_lista_paginas() {
    GList *children, *iter;
    children = gtk_container_get_children(GTK_CONTAINER(pages_list_box));
    for (iter = children; iter != NULL; iter = g_list_next(iter)) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);
    for (int i = 0; i <= tempo_atual && i < TOTAL_ACESSOS; i++) {
        char temp[16];
        snprintf(temp, sizeof(temp), "Pagina %d", acessos[i]);
        GtkWidget *item = gtk_label_new(temp);
        gtk_box_pack_start(GTK_BOX(pages_list_box), item, FALSE, FALSE, 2);
        gtk_widget_show(item);
    }
}

void atualizar_interface() {
    gchar status[256];
    snprintf(status, sizeof(status), "Tempo %d/%d - Acesso a pagina: %d - %s", tempo_atual + 1, TOTAL_ACESSOS, acessos[tempo_atual], ultimo_page_fault ? "PAGE FAULT" : "HIT");
    gtk_label_set_text(GTK_LABEL(status_label), status);
    for (int i = 0; i < TOTAL_QUADROS; i++) {
        gchar info[128];
        if (memoria_fisica[i].ocupado) {
            int p = memoria_fisica[i].pagina_virtual;
            snprintf(info, sizeof(info), "Q%d\nP%d\nR=%d M=%d T=%d", i, p, tabela_paginas[p].bitR, tabela_paginas[p].bitM, tabela_paginas[p].timestamp);
        } else {
            snprintf(info, sizeof(info), "Q%d\n[vazio]", i);
        }
        gtk_label_set_text(GTK_LABEL(frame_labels[i]), info);
        GtkStyleContext *context = gtk_widget_get_style_context(frames[i]);
        if (ultimo_page_fault && memoria_fisica[i].pagina_virtual == acessos[tempo_atual]) {
            gtk_style_context_add_class(context, "page-fault");
        } else {
            gtk_style_context_remove_class(context, "page-fault");
        }
    }
    atualizar_lista_paginas();
}

void on_next_clicked(GtkButton *button, gpointer user_data) {
    if (!simulacao_iniciada) {
        const gchar *alg_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_algoritmo));
        algoritmo_usado = (g_strcmp0(alg_text, "LRU") == 0) ? LRU : FIFO;
        simulacao_iniciada = TRUE;
        gtk_widget_set_sensitive(combo_algoritmo, FALSE);
    }
    if (tempo_atual < TOTAL_ACESSOS) {
        acessar_pagina(acessos[tempo_atual], tempo_atual);
        atualizar_interface();
        tempo_atual++;
    } else {
        gtk_label_set_text(GTK_LABEL(label_info), "Simulacao finalizada.");
        gtk_widget_set_sensitive(button_next, FALSE);
    }
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    gerar_acessos();
    inicializar_simulador();

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Simulador de Paginas");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 500);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_container_add(GTK_CONTAINER(window), hbox);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(hbox), vbox, TRUE, TRUE, 5);

    combo_algoritmo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_algoritmo), "FIFO");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_algoritmo), "LRU");
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo_algoritmo), 0);
    gtk_box_pack_start(GTK_BOX(vbox), combo_algoritmo, FALSE, FALSE, 5);

    label_info = gtk_label_new("Clique em 'Proximo' para iniciar.");
    gtk_box_pack_start(GTK_BOX(vbox), label_info, FALSE, FALSE, 5);

    GtkWidget *align = gtk_alignment_new(0.5, 0.5, 0, 0);
    gtk_box_pack_start(GTK_BOX(vbox), align, TRUE, TRUE, 5);

    memory_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(memory_grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(memory_grid), 10);
    gtk_container_add(GTK_CONTAINER(align), memory_grid);

    for (int i = 0; i < TOTAL_QUADROS; i++) {
        frames[i] = gtk_frame_new(NULL);
        gtk_widget_set_size_request(frames[i], 100, 100);
        frame_labels[i] = gtk_label_new("Q[vazio]");
        gtk_container_add(GTK_CONTAINER(frames[i]), frame_labels[i]);
        gtk_grid_attach(GTK_GRID(memory_grid), frames[i], i % 2, i / 2, 1, 1);
        aplicar_css(frames[i]);
    }

    status_label = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(vbox), status_label, FALSE, FALSE, 5);

    button_next = gtk_button_new_with_label("Proximo Acesso");
    gtk_box_pack_start(GTK_BOX(vbox), button_next, FALSE, FALSE, 5);
    g_signal_connect(button_next, "clicked", G_CALLBACK(on_next_clicked), NULL);

    GtkWidget *vbox_right = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(hbox), vbox_right, FALSE, FALSE, 5);

    pages_list_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_box_pack_start(GTK_BOX(vbox_right), pages_list_box, TRUE, TRUE, 5);

    aplicar_css(window);
    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}
