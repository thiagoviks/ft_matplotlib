#include "ft_matplotlib2.h"
#include <ft_mini_numpy.h>


Canvas *create_canvas(int w, int h) {
    Canvas *c = ft_malloc(sizeof(Canvas));
    c->width = w;
    c->height = h;
    c->pixels = ft_malloc(3 * w * h);
    ft_memset(c->pixels, 255, 3 * w * h); // branco
    return c;
}

void np_free(ndarray *arr) {
    ft_free(arr->data);
    ft_free(arr);
}


void set_pixel(Canvas *c, int x, int y, Color col) {
    if (x < 0 || y < 0 || x >= c->width || y >= c->height) return;
    int idx = 3 * (y * c->width + x);
    c->pixels[idx] = col.r;
    c->pixels[idx + 1] = col.g;
    c->pixels[idx + 2] = col.b;
}

// desenha texto com fonte 8x8
void draw_char(Canvas *c, int x, int y, char ch, Color col) {
    if (ch < 32 || ch > 126) return;
    const unsigned char *bitmap = font8x8_basic[ch - 32];
    for (int row = 0; row < 8; row++) {
        for (int colb = 0; colb < 8; colb++) {
            if (bitmap[row] & (1 << colb)) {
                set_pixel(c, x + colb, y + row, col);
            }
        }
    }
}

void draw_text(Canvas *c, int x, int y, const char *text, Color col) {
    while (*text) {
        draw_char(c, x, y, *text, col);
        x += 8;
        text++;
    }
}


// desenha linha (Bresenham)
void draw_line(Canvas *c, int x0, int y0, int x1, int y1, Color col) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    while (1) {
        set_pixel(c, x0, y0, col);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void draw_rect(Canvas *c, int x, int y, int w, int h, Color col) {
    for (int j = 0; j < h; j++) {
        draw_line(c, x, y + j, x + w - 1, y + j, col);
    }
}

void draw_circle(Canvas *c, int cx, int cy, int r, Color col) {
    for (int y = -r; y <= r; y++) {
        for (int x = -r; x <= r; x++) {
            if (x*x + y*y <= r*r) {
                int px = cx + x;
                int py = cy + y;
                if (px >= 0 && px < c->width && py >= 0 && py < c->height) {
                    set_pixel(c, px, py, col);
                }
            }
        }
    }
}

// desenha eixos + grades + labels com decimais
void draw_axes(Canvas *c, double xmin, double xmax, double ymin, double ymax) {
    Color black = {0, 0, 0};
    Color gray = {220, 220, 220};

    // origem em pixel
    int x0 = (int)((0 - xmin) / (xmax - xmin) * (c->width - 1));
    int y0 = (int)((1 - (0 - ymin) / (ymax - ymin)) * (c->height - 1));

    // eixo X
    if (y0 >= 0 && y0 < c->height)
        draw_line(c, 0, y0, c->width - 1, y0, black);

    // eixo Y
    if (x0 >= 0 && x0 < c->width)
        draw_line(c, x0, 0, x0, c->height - 1, black);

    // ticks verticais (X)
    for (double i = ceil(xmin*2)/2; i <= xmax; i += 0.5) {
        int px = (int)((i - xmin) / (xmax - xmin) * (c->width - 1));
        if (px >= 0 && px < c->width) {
            draw_line(c, px, 0, px, c->height - 1, gray);

            char buf[16];
            ft_sprintf(buf, "%.1f", i);
            draw_text(c, px - 10, y0 + 8, buf, black);
        }
    }

    // ticks horizontais (Y)
    for (double j = ceil(ymin*2)/2; j <= ymax; j += 0.5) {
        int py = (int)((1 - (j - ymin) / (ymax - ymin)) * (c->height - 1));
        if (py >= 0 && py < c->height) {
            draw_line(c, 0, py, c->width - 1, py, gray);

            char buf[16];
            ft_sprintf(buf, "%.1f", j);
            draw_text(c, x0 + 6, py - 4, buf, black);
        }
    }
}


// título
void plt_title(Canvas *c, const char *text, Color col) {
    draw_text(c, c->width/2 - strlen(text)*4, 20, text, col);
}

// legenda
void plt_legend(Canvas *c, const char *text, int x, int y, Color col) {
    draw_text(c, x+20, y-5, text, col);
    draw_line(c, x, y, x+15, y, col);
}

// eixos
void plt_axes(Canvas *c, Color col) {
    draw_line(c, 50, c->height/2, c->width-50, c->height/2, col);
    draw_line(c, c->width/2, 50, c->width/2, c->height-50, col);
}

void plt_destroy(Canvas *c) {
    ft_free(c->pixels);
    ft_free(c);
}

// === Função auxiliar para ler double de ndarray 1D ===
static inline double ndarray_get1d(const ndarray *arr, int i) {
    char *base = (char *)arr->data;
    return *((double *)(base + i * arr->strides[0]));
}

// === Scatter Plot ===
void plt_scatter_ndarray(Canvas *c, ndarray *x, ndarray *y, Color col,
                         double xmin, double xmax, double ymin, double ymax) {
    int n = x->shape[0];  // assumindo x e y do mesmo tamanho

    for (int i = 0; i < n; i++) {
        double xv = ndarray_get1d(x, i);
        double yv = ndarray_get1d(y, i);

        int px = (int)((xv - xmin) / (xmax - xmin) * (c->width - 1));
        int py = (int)((1 - (yv - ymin) / (ymax - ymin)) * (c->height - 1));

        // desenha pequeno quadrado (3x3) representando ponto
        draw_rect(c, px - 1, py - 1, 3, 3, col);
    }
}

void plt_scatter_circle_ndarray(Canvas *c, ndarray *x, ndarray *y, Color col,
                         double xmin, double xmax, double ymin, double ymax,
                         int radius) {
    int n = x->shape[0];
    for (int i = 0; i < n; i++) {
        double xv = ndarray_get1d(x, i);
        double yv = ndarray_get1d(y, i);

        int px = (int)((xv - xmin) / (xmax - xmin) * (c->width - 1));
        int py = (int)((1 - (yv - ymin) / (ymax - ymin)) * (c->height - 1));

        draw_circle(c, px, py, radius, col);
    }
}


// === Bar Plot ===
void plt_bar_ndarray(Canvas *c, ndarray *x, ndarray *height, Color col,
                     double xmin, double xmax, double ymin, double ymax) {
    int n = x->shape[0];  // assumindo x e height do mesmo tamanho
    int bar_width = (int)(c->width / n) * 0.8; // largura relativa das barras

    for (int i = 0; i < n; i++) {
        double xv = ndarray_get1d(x, i);
        double hv = ndarray_get1d(height, i);

        int px = (int)((xv - xmin) / (xmax - xmin) * (c->width - 1));
        int py = (int)((1 - (hv - ymin) / (ymax - ymin)) * (c->height - 1));

        // barras desenhadas da base (eixo X) até a altura
        int base_y = (int)((1 - (0 - ymin) / (ymax - ymin)) * (c->height - 1));
        int h = base_y - py;  // altura em pixels

        if (h > 0)
        {
            draw_rect(c, px - bar_width / 2, py, bar_width, h, col);
        }
    }
}

void plt_bar_ndarray2(Canvas *c, ndarray *x, ndarray *y, Color col,
                     double xmin, double xmax, double ymin, double ymax,
                     int bar_width) {
    int n = x->shape[0];
    for (int i = 0; i < n; i++) {
        double xv = ndarray_get1d(x, i);
        double yv = ndarray_get1d(y, i);

        int px = (int)((xv - xmin) / (xmax - xmin) * (c->width - 1));
        int py = (int)((1 - (yv - ymin) / (ymax - ymin)) * (c->height - 1));
        int base = (int)((1 - (0 - ymin) / (ymax - ymin)) * (c->height - 1));

        for (int xx = px - bar_width/2; xx <= px + bar_width/2; xx++) {
            draw_line(c, xx, base, xx, py, col);
        }
    }
}


void plt_plot_ndarray(Canvas *c, ndarray *x, ndarray *y, Color col,
                      double xmin, double xmax, double ymin, double ymax) {
    int n = x->shape[0];
    if (n < 2) return;

    for (int i = 0; i < n - 1; i++) {
        double xv1 = ndarray_get1d(x, i);
        double yv1 = ndarray_get1d(y, i);
        double xv2 = ndarray_get1d(x, i + 1);
        double yv2 = ndarray_get1d(y, i + 1);

        // converte para pixels
        int px1 = (int)((xv1 - xmin) / (xmax - xmin) * (c->width - 1));
        int py1 = (int)((1 - (yv1 - ymin) / (ymax - ymin)) * (c->height - 1));

        int px2 = (int)((xv2 - xmin) / (xmax - xmin) * (c->width - 1));
        int py2 = (int)((1 - (yv2 - ymin) / (ymax - ymin)) * (c->height - 1));

        draw_line(c, px1, py1, px2, py2, col);
    }
}


void plt_savefig(Canvas *c, const char *filename) {
    stbi_write_png(filename, c->width, c->height, 3, c->pixels, c->width * 3);
}

AxisLimits plt_axis_auto(ndarray *x, ndarray *y) {
    AxisLimits lim;

    int n = x->shape[0];
    if (n <= 0) {
        lim.xmin = lim.xmax = 0;
        lim.ymin = lim.ymax = 0;
        return lim;
    }

    // inicializa com o primeiro valor
    lim.xmin = lim.xmax = ndarray_get1d(x, 0);
    lim.ymin = lim.ymax = ndarray_get1d(y, 0);

    for (int i = 1; i < n; i++) {
        double xv = ndarray_get1d(x, i);
        double yv = ndarray_get1d(y, i);

        if (xv < lim.xmin) lim.xmin = xv;
        if (xv > lim.xmax) lim.xmax = xv;

        if (yv < lim.ymin) lim.ymin = yv;
        if (yv > lim.ymax) lim.ymax = yv;
    }

    // adiciona margem de 5% (como matplotlib faz)
    double xmargin = (lim.xmax - lim.xmin) * 0.05;
    double ymargin = (lim.ymax - lim.ymin) * 0.05;

    if (xmargin == 0) xmargin = 1.0; // evita degenerate axis
    if (ymargin == 0) ymargin = 1.0;

    lim.xmin -= xmargin;
    lim.xmax += xmargin;
    lim.ymin -= ymargin;
    lim.ymax += ymargin;

    return lim;
}

AxisLimits plt_axis_auto_multi(ndarray **xs, ndarray **ys, int nplots) {
    AxisLimits lim;
    lim.xmin = lim.ymin =  1e308;   // grande positivo
    lim.xmax = lim.ymax = -1e308;   // grande negativo

    for (int p = 0; p < nplots; p++) {
        ndarray *x = xs[p];
        ndarray *y = ys[p];

        int n = x->shape[0];
        for (int i = 0; i < n; i++) {
            double xv = ndarray_get1d(x, i);
            double yv = ndarray_get1d(y, i);

            if (xv < lim.xmin) lim.xmin = xv;
            if (xv > lim.xmax) lim.xmax = xv;

            if (yv < lim.ymin) lim.ymin = yv;
            if (yv > lim.ymax) lim.ymax = yv;
        }
    }

    // margem de 5%
    double xmargin = (lim.xmax - lim.xmin) * 0.05;
    double ymargin = (lim.ymax - lim.ymin) * 0.05;

    if (xmargin == 0) xmargin = 1.0;
    if (ymargin == 0) ymargin = 1.0;

    lim.xmin -= xmargin;
    lim.xmax += xmargin;
    lim.ymin -= ymargin;
    lim.ymax += ymargin;

    return lim;
}



int main() {
    // exemplo simples
    double xdata[] = {0, 1, 2, 3, 4, 5};
    double ydata[] = {0, 1, 4, 9, 16, 25};

    double xdata1[] = {0, 1, 2*5, 3*5, 4*5, 5*5};
    double ydata1[] = {0, 1, 4*8, 9*8, 16*8, 25*8};

    int shape[1] = {6};
    int strides[1] = {sizeof(double)};

    ndarray x = {xdata, shape, 1, sizeof(double), strides};
    ndarray y = {ydata, shape, 1, sizeof(double), strides};

    ndarray x1 = {xdata1, shape, 1, sizeof(double), strides};
    ndarray y1 = {ydata1, shape, 1, sizeof(double), strides};

    Canvas *c = create_canvas(640, 480);
    Color red = {255, 0, 0};
    Color blue = {0, 0, 255};
    Color orange = {255, 172, 28};
    Color green = {81, 225, 90};
    Color black = {0, 0, 0};
    Color gray = {220, 220, 220};

    // AxisLimits lim = plt_axis_auto(&x, &y);

    // // linha
    //  //plt_plot_ndarray(c, &x, &y, red, 0, 5, 0, 25);
    // plt_plot_ndarray(c, &x, &y, orange, 
    //              lim.xmin, lim.xmax, lim.ymin, lim.ymax);


    // // scatter
    // plt_scatter_ndarray(c, &x, &y, blue, lim.xmin, lim.xmax, lim.ymin, lim.ymax);


    ndarray *xs[2] = {&x, &y};
    ndarray *ys[2] = {&x1, &y1};
    AxisLimits lim = plt_axis_auto_multi(xs, ys, 2);

    draw_axes(c, lim.xmin, lim.xmax, lim.ymin, lim.ymax);
    //plt_axes(c, black);
    plt_title(c, "grafico da funcao seno", green);
   //plt_legend(c, "auto scale", 60, 10, orange);
    plt_plot_ndarray(c, &x, &y, orange, lim.xmin,
        lim.xmax, lim.ymin, lim.ymax);
    plt_plot_ndarray(c, &x1, &y1, blue, lim.xmin,
        lim.xmax, lim.ymin, lim.ymax);
    
    plt_scatter_ndarray(c, &x, &y, blue, lim.xmin,
         lim.xmax, lim.ymin, lim.ymax);
    plt_scatter_circle_ndarray(c, &x1, &y1, orange, lim.xmin,
         lim.xmax, lim.ymin, lim.ymax,2);

    plt_bar_ndarray2(c, &x, &y, red, lim.xmin, lim.xmax, lim.ymin, lim.ymax, 8);
    plt_bar_ndarray2(c, &x1, &y1, green, lim.xmin, lim.xmax, lim.ymin, lim.ymax, 8);
    // salvar
    plt_savefig(c, "ft_matplotlib2_7.png");
    plt_destroy(c);

    return 0;
}
