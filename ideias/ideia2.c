#include <stdio.h>
#include <math.h>
#include "ft_matplotlib.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define WIDTH 800
#define HEIGHT 600


static unsigned char image[HEIGHT][WIDTH][3];

void put_pixel(int x, int y, Color c) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        image[HEIGHT - y - 1][x][0] = c.r;
        image[HEIGHT - y - 1][x][1] = c.g;
        image[HEIGHT - y - 1][x][2] = c.b;
    }
}

void draw_line(int x0, int y0, int x1, int y1, Color c) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    while (1) {
        put_pixel(x0, y0, c);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

// função para texto simples (desenha só retângulos p/ simular caracteres)
void draw_char(int x, int y, char c, Color col) {
    // Desenho extremamente tosco: cada char = quadrado 5x7
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 7; j++)
            if (((i + j + c) % 7) < 3) // só para parecer "ruído de letra"
                put_pixel(x+i, y+j, col);
}

void draw_text(int x, int y, const char *text, Color col) {
    int offset = 0;
    for (int i = 0; text[i] != '\0'; i++) {
        draw_char(x + offset, y, text[i], col);
        offset += 6; // espaçamento
    }
}

int main() {
    // fundo branco
    for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++)
            image[y][x][0] = image[y][x][1] = image[y][x][2] = 255;

    // parâmetros
    int margin = 60;
    double xmin = 0, xmax = 4 * M_PI;
    double ymin = -1.2, ymax = 1.2;

    Color black = {0, 0, 0};
    Color red   = {255, 0, 0};
    Color blue  = {0, 0, 255};

    // eixos
    int x0 = margin, y0 = margin;
    int x1 = WIDTH - margin, y1 = HEIGHT - margin;
    draw_line(x0, y0, x1, y0, black); // eixo X
    draw_line(x0, y0, x0, y1, black); // eixo Y

    // ticks no eixo X
    int nticks = 8;
    for (int i = 0; i <= nticks; i++) {
        double t = xmin + (xmax - xmin) * i / nticks;
        int px = x0 + (int)((t - xmin) / (xmax - xmin) * (x1 - x0));
        draw_line(px, y0 - 5, px, y0 + 5, black);
    }

    // ticks no eixo Y
    nticks = 6;
    for (int i = 0; i <= nticks; i++) {
        double t = ymin + (ymax - ymin) * i / nticks;
        int py = y0 + (int)((t - ymin) / (ymax - ymin) * (y1 - y0));
        draw_line(x0 - 5, py, x0 + 5, py, black);
    }

    // plot do seno
    int N = 1000;
    int prev_x = -1, prev_y = -1;
    for (int i = 0; i < N; i++) {
        double x = xmin + (xmax - xmin) * i / (N - 1);
        double y = sin(x);

        int px = x0 + (int)((x - xmin) / (xmax - xmin) * (x1 - x0));
        int py = y0 + (int)((y - ymin) / (ymax - ymin) * (y1 - y0));

        if (prev_x != -1) draw_line(prev_x, prev_y, px, py, red);

        prev_x = px;
        prev_y = py;
    }

    // título
    draw_text(WIDTH/2 - 50, HEIGHT - margin + 20, "Grafico do Seno", blue);

    // salva imagem
    stbi_write_png("seno_axes.png", WIDTH, HEIGHT, 3, image, WIDTH * 3);
    printf("Imagem salva: seno_axes.png\n");
    return 0;
}
