#include "../include/ft_matplotlib.h"
#include <ft_maki.h>
#include <ft_mini_numpy.h>

Canvas *create_canvas(int w, int h)
{
    Canvas *c = ft_malloc(sizeof(Canvas));
    c->width = w;
    c->height = h;
    c->pixels = ft_malloc(3 * w * h);
    ft_memset(c->pixels, 255, 3 * w * h); // white
    return (c);
}

Legend *legend_create(int max_items)
{
    Legend *lg = ft_malloc(sizeof(Legend));
    lg->items = ft_malloc(sizeof(LegendItem) * max_items);
    lg->nitems = 0;
    lg->max_items = max_items;
    return (lg);
}

void legend_add(Legend *lg, const char *label, Color col)
{
    if (lg->nitems >= lg->max_items) return;
    lg->items[lg->nitems].label = label;
    lg->items[lg->nitems].col = col;
    lg->nitems++;
}

void plt_draw_legend(Canvas *c, Legend *lg)
{
    if (!lg || lg->nitems == 0) return;

    int start_x = c->width - 150; // top right corner
    int start_y = 20;
    int line_height = 20;

    for (int i = 0; i < lg->nitems; i++)
    {
        int y = start_y + i * line_height;

        // color rectangle
        draw_rect(c, start_x, y, 15, 15, lg->items[i].col);

        // label
        draw_text(c, start_x + 20, y, lg->items[i].label,
             (Color){0,0,0});
    }
}

void set_pixel(Canvas *c, int x, int y, Color col)
{
    if (x < 0 || y < 0 || x >= c->width || y >= c->height) return;
    int idx = 3 * (y * c->width + x);
    c->pixels[idx] = col.r;
    c->pixels[idx + 1] = col.g;
    c->pixels[idx + 2] = col.b;
}

// draw text with font8x8_basic
void draw_char(Canvas *c, int x, int y, char ch, Color col)
{
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

void draw_text(Canvas *c, int x, int y, const char *text, Color col)
{
    while (*text) {
        draw_char(c, x, y, *text, col);
        x += 8;
        text++;
    }
}

// draw line (Bresenham)
void draw_line(Canvas *c, int x0, int y0, int x1, int y1, Color col)
{
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

void draw_rect(Canvas *c, int x, int y, int w, int h, Color col)
{
    for (int j = 0; j < h; j++) {
        draw_line(c, x, y + j, x + w - 1, y + j, col);
    }
}

void draw_circle(Canvas *c, int cx, int cy, int r, Color col)
{
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

//only internal use
static inline double matplotlib_ceil(double x) {
    long long i = (long long)x;
    if (x > 0 && x != (double)i) return ((double)(i + 1));
    return ((double)i);
}


// draws axes + grids + labels with decimals
void draw_axes(Canvas *c, double xmin, double xmax, double ymin, double ymax)
{
    Color black = {0, 0, 0};
    Color gray = {220, 220, 220};

    // pixel origin
    int x0 = (int)((0 - xmin) / (xmax - xmin) * (c->width - 1));
    int y0 = (int)((1 - (0 - ymin) / (ymax - ymin)) * (c->height - 1));

    // axis X
    if (y0 >= 0 && y0 < c->height)
        draw_line(c, 0, y0, c->width - 1, y0, black);

    // axis Y
    if (x0 >= 0 && x0 < c->width)
        draw_line(c, x0, 0, x0, c->height - 1, black);

    // vertical ticks (X)
    for (double i = matplotlib_ceil(xmin*2)/2; i <= xmax; i += 0.5)
    {
        int px = (int)((i - xmin) / (xmax - xmin) * (c->width - 1));
        if (px >= 0 && px < c->width) {
            draw_line(c, px, 0, px, c->height - 1, gray);

            char buf[16];
            sprintf(buf, "%.1f", i);
            draw_text(c, px - 10, y0 + 8, buf, black);
        }
    }

    // horizontal ticks (Y)
    for (double j = matplotlib_ceil(ymin*2)/2; j <= ymax; j += 0.5)
    {
        int py = (int)((1 - (j - ymin) / (ymax - ymin)) * (c->height - 1));
        if (py >= 0 && py < c->height) {
            draw_line(c, 0, py, c->width - 1, py, gray);

            char buf[16];
            sprintf(buf, "%.1f", j);
            draw_text(c, x0 + 6, py - 4, buf, black);
        }
    }
}

void plt_axes2(Canvas *c, AxisLimits lim, Color col, int n_ticks)
{
    // axis X
    int y0 = (int)((1 - (0 - lim.ymin)/(lim.ymax - lim.ymin)) * (c->height - 1));
    draw_line(c, 0, y0, c->width-1, y0, col);

    // axis Y
    int x0 = (int)((0 - lim.xmin)/(lim.xmax - lim.xmin) * (c->width - 1));
    draw_line(c, x0, 0, x0, c->height-1, col);

    // ticks X
    for (int i = 0; i <= n_ticks; i++) {
        double val = lim.xmin + i*(lim.xmax - lim.xmin)/n_ticks;
        int px = (int)((val - lim.xmin)/(lim.xmax - lim.xmin) * (c->width - 1));
        draw_line(c, px, y0-5, px, y0+5, col);
        char buf[32];
        sprintf(buf, "%.2g", val);
        draw_text(c, px, y0+8, buf, col);
    }

    // ticks Y
    for (int i = 0; i <= n_ticks; i++) {
        double val = lim.ymin + i*(lim.ymax - lim.ymin)/n_ticks;
        int py = (int)((1 - (val - lim.ymin)/(lim.ymax - lim.ymin)) * (c->height - 1));
        draw_line(c, x0-5, py, x0+5, py, col);
        char buf[32];
        sprintf(buf, "%.2g", val);
        draw_text(c, x0-30, py, buf, col); // shift to the left
    }
}


void plt_title(Canvas *c, const char *text, Color col)
{
    int len = ft_strlen(text);

    int text_width = 8 * len; //width in pixels
    int x = (c->width - text_width) / 2;       // horizontal center
    int y = 20;                 // distance from the top
    draw_text(c, x, y, text, col); // draw_text should center on x
}

void plt_xlabel(Canvas *c, const char *text, Color col)
{
    int len = ft_strlen(text);

    int text_width = 8 * len;
    int x = (c->width - text_width) / 2;       // horizontal center
    int y = c->height - 20;     // lower axis distance
    draw_text(c, x, y, text, col);
}

void draw_text_rot90(Canvas *c, int x, int y, const char *text, Color col)
{
    while (*text) {
        unsigned char ch = (unsigned char) *text;
        if (ch < 0 || ch > 127) ch = '?'; // fallback ASCII
        
        // runs through lines of the letter
        for (int row = 0; row < 8; row++)
        {
            uint8_t bits = font8x8_basic[ch][row];
            
            // traverses bits
            for (int col_bit = 0; col_bit < 8; col_bit++)
            {
                if (bits & (1 << col_bit)) {
                    int px = x + row;        // x rotated
                    int py = y - col_bit;    // y rotated
                    set_pixel(c, px, py, col);
                }
            }
        }
        x += 8;  // advance horizontal cursor to next letter
        text++;
    }
}

void draw_text_rot90_vertical(Canvas *c, int x, int y, const char *text, Color col)
{
    int cursor_y = y; // vertical starting point (center or top)
    
    while (*text) {
        unsigned char ch = (unsigned char) *text;
        if (ch < 0 || ch > 127) ch = '?'; // fallback ASCII
        
        // Draws letter rotated 90° (clockwise)
        for (int row = 0; row < 8; row++) {
            uint8_t bits = font8x8_basic[ch][row];
            for (int col_bit = 0; col_bit < 8; col_bit++) {
                if (bits & (1 << col_bit)) {
                    int px = x + row;         // letter width
                    int py = cursor_y - col_bit; // descending Y axis
                    set_pixel(c, px, py, col);
                }
            }
        }

        cursor_y += 8; // advance to the next letter down
        text++;
    }
}

/*
reverse_string ensures that the text is read from top to bottom.

text_height calculates the total height of the text on the y-axis (n_letters * 8).

y_start automatically centers the text vertically.

x keeps the text close to the axis.

Each letter is still rotated 90°, ensuring the matplotlib y-label effect.

*/

void plt_ylabel(Canvas *c, const char *text, Color col)
{
    // copy to buffer to be able to invert
    char buf[256];
    snprintf(buf, sizeof(buf), "%s", text);
    ft_reverse(buf, ft_strlen(text));

    int text_height = 8 * ft_strlen(buf);

    // centers vertically approximate
    int y = (c->height - text_height) / 2 + 8; 
    int x = 20;                  // left axis distance
    draw_text_rot90_vertical(c, x, y, buf, col);
}

void plt_legend(Canvas *c, const char *text, int x, int y, Color col)
{
    draw_text(c, x+20, y-5, text, col);
    draw_line(c, x, y, x+15, y, col);
}

// axis
void plt_axes(Canvas *c, Color col)
{
    draw_line(c, 50, c->height/2, c->width-50, c->height/2, col);
    draw_line(c, c->width/2, 50, c->width/2, c->height-50, col);
}

void plt_destroy(Canvas *c)
{
    ft_free(c->pixels);
    ft_free(c);
}

// Helper function to read double from 1D ndarray
static inline double ndarray_get1d(const ndarray *arr, int i)
{
    char *base = (char *)arr->data;
    return (*((double *)(base + i * arr->strides[0])));
}

// Scatter Plot
void plt_scatter_ndarray(Canvas *c, ndarray *x, ndarray *y, Color col, double xmin, double xmax, double ymin, double ymax)
{
    int n = x->shape[0];  // assuming x and y are the same size

    for (int i = 0; i < n; i++) {
        double xv = ndarray_get1d(x, i);
        double yv = ndarray_get1d(y, i);

        int px = (int)((xv - xmin) / (xmax - xmin) * (c->width - 1));
        int py = (int)((1 - (yv - ymin) / (ymax - ymin)) * (c->height - 1));

        // draws a small square (3x3) representing a point
        draw_rect(c, px - 1, py - 1, 3, 3, col);
    }
}

void plt_scatter_circle_ndarray(Canvas *c, ndarray *x, ndarray *y, Color col, double xmin, double xmax, double ymin, double ymax, int radius)
{
    int n = x->shape[0];
    for (int i = 0; i < n; i++) {
        double xv = ndarray_get1d(x, i);
        double yv = ndarray_get1d(y, i);

        int px = (int)((xv - xmin) / (xmax - xmin) * (c->width - 1));
        int py = (int)((1 - (yv - ymin) / (ymax - ymin)) * (c->height - 1));

        draw_circle(c, px, py, radius, col);
    }
}

// Bar Plot
void plt_bar_ndarray(Canvas *c, ndarray *x, ndarray *height, Color col, double xmin, double xmax, double ymin, double ymax)
{
    int n = x->shape[0];  // assuming x and height are the same size
    int bar_width = (int)(c->width / n) * 0.8; // relative width of the bars

    for (int i = 0; i < n; i++) {
        double xv = ndarray_get1d(x, i);
        double hv = ndarray_get1d(height, i);

        int px = (int)((xv - xmin) / (xmax - xmin) * (c->width - 1));
        int py = (int)((1 - (hv - ymin) / (ymax - ymin)) * (c->height - 1));

        // bars drawn from the base (X axis) to the height
        int base_y = (int)((1 - (0 - ymin) / (ymax - ymin)) * (c->height - 1));
        int h = base_y - py;  // height in pixels

        if (h > 0)
        {
            draw_rect(c, px - bar_width / 2, py, bar_width, h, col);
        }
    }
}

void plt_bar_ndarray2(Canvas *c, ndarray *x, ndarray *y, Color col, double xmin, 
                    double xmax, double ymin, double ymax, int bar_width)
{
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

/*

px_start and px_end divide the canvas horizontally between bins.

py_top calculates the vertical position based on the maximum count (max_count)
to normalize height.

The histogram is filled, like matplotlib plt.hist(..., histtype='bar').

xmin and xmax define the range of the x-axis; values ​​outside are ignored.

For ymin and ymax, you can adjust the vertical mapping, but in the example above
we used 0 → top and max_count → bottom.

*/

void plt_hist(Canvas *c, ndarray *data, int bins, Color col, double xmin, double xmax)
{
    int *counts = ft_malloc(bins);
    int n = data->shape[0];

    // Counting the values ​​in each bin
    for (int i=0;i<n;i++) {
        double val = ((double*)data->data)[i];
        if (val < xmin || val > xmax) continue; // ignore out of range
        int bin = (int)((val - xmin)/(xmax - xmin) * bins);
        if (bin == bins) bin = bins-1; // edge case
        counts[bin]++;
    }

    // Determines maximum height for scale
    int max_count = 0;
    for (int i=0;i<bins;i++)
        if (counts[i] > max_count) max_count = counts[i];

    if (max_count == 0) max_count = 1; // avoid division by zero

    // Draw rectangles for each bin
    for (int i=0;i<bins;i++) {
        int px_start = (int)((double)i / bins * c->width);
        int px_end   = (int)((double)(i+1)/bins * c->width);
        int py_top   = (int)((1.0 - ((double)counts[i]/max_count)) * (c->height-1));
        // draw column
        for (int x = px_start; x < px_end; x++) {
            for (int y = py_top; y < c->height; y++) {
                set_pixel(c, x, y, col);
            }
        }
    }

    ft_free(counts);
}

void plt_hist2(Canvas *c, ndarray *data, int bins, Color col, double xmin, double xmax)
{
    int *counts = ft_calloc(bins, sizeof(int));
    int n = data->shape[0];

    // Counting the values ​​in each bin
    for (int i=0;i<n;i++) {
        double val = ((double*)data->data)[i];
        if (val < xmin || val > xmax) continue;
        int bin = (int)((val - xmin)/(xmax - xmin) * bins);
        if (bin == bins) bin = bins-1;
        counts[bin]++;
    }

    // Determines maximum height for scale
    int max_count = 0;
    for (int i=0;i<bins;i++)
        if (counts[i] > max_count) max_count = counts[i];
    if (max_count == 0) max_count = 1;

    // Draw rectangles for each bin
    for (int i=0;i<bins;i++) {
        int px_start = (int)((double)i / bins * c->width);
        int px_end   = (int)((double)(i+1)/bins * c->width);
        int py_top   = (int)((1.0 - ((double)counts[i]/max_count)) * (c->height-1));
        for (int x = px_start; x < px_end; x++) {
            for (int y = py_top; y < c->height; y++) {
                set_pixel(c, x, y, col);
            }
        }
    }

    ft_free(counts);
}


void plt_hist_full(Canvas *c, ndarray *data, int bins, Color col, const char *title,
                    const char *xlabel, const char *ylabel)
{

    // auto scale
    double xmin = ((double*)data->data)[0], xmax = xmin;
    for (int i=0;i<data->shape[0];i++) {
        double v = ((double*)data->data)[i];
        if (v < xmin) xmin=v;
        if (v > xmax) xmax=v;
    }
    double dx = (xmax-xmin)*0.05;
    xmin -= dx; xmax += dx;

    // ymin/ymax adjusted the plt_hist function
    AxisLimits lim = {xmin, xmax, 0, 0}; 

    // Draw axis
    plt_axes2(c, lim, (Color){0,0,0}, 5);

    // Draw histogram
    plt_hist2(c, data, bins, col, xmin, xmax);

    // Labels and title
    plt_title(c, title, (Color){0,0,0});
    plt_xlabel(c, xlabel, (Color){0,0,0});
    plt_ylabel(c, ylabel, (Color){0,0,0});
}

void plt_plot_ndarray(Canvas *c, ndarray *x, ndarray *y, Color col, double xmin, double xmax, double ymin, double ymax)
{
    int n = x->shape[0];
    if (n < 2) return;

    for (int i = 0; i < n - 1; i++) {
        double xv1 = ndarray_get1d(x, i);
        double yv1 = ndarray_get1d(y, i);
        double xv2 = ndarray_get1d(x, i + 1);
        double yv2 = ndarray_get1d(y, i + 1);

        // convert to pixels
        int px1 = (int)((xv1 - xmin) / (xmax - xmin) * (c->width - 1));
        int py1 = (int)((1 - (yv1 - ymin) / (ymax - ymin)) * (c->height - 1));

        int px2 = (int)((xv2 - xmin) / (xmax - xmin) * (c->width - 1));
        int py2 = (int)((1 - (yv2 - ymin) / (ymax - ymin)) * (c->height - 1));

        draw_line(c, px1, py1, px2, py2, col);
    }
}

void plt_savefig(Canvas *c, const char *filename)
{
    stbi_write_png(filename, c->width, c->height, 3, c->pixels, c->width * 3);
}

AxisLimits plt_axis_auto(ndarray *x, ndarray *y)
{
    AxisLimits lim;

    int n = x->shape[0];
    if (n <= 0) {
        lim.xmin = lim.xmax = 0;
        lim.ymin = lim.ymax = 0;
        return (lim);
    }

    // initializes with the first value
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

    // adds 5% margin (like matplotlib does)
    double xmargin = (lim.xmax - lim.xmin) * 0.05;
    double ymargin = (lim.ymax - lim.ymin) * 0.05;

    if (xmargin == 0) xmargin = 1.0; // avoid degenerate axis
    if (ymargin == 0) ymargin = 1.0;

    lim.xmin -= xmargin;
    lim.xmax += xmargin;
    lim.ymin -= ymargin;
    lim.ymax += ymargin;

    return (lim);
}

AxisLimits plt_axis_auto_multi(ndarray **xs, ndarray **ys, int nplots)
{
    AxisLimits lim;
    lim.xmin = lim.ymin =  1e308;   // big positive
    lim.xmax = lim.ymax = -1e308;   // big negative

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

    // 5% margin
    double xmargin = (lim.xmax - lim.xmin) * 0.05;
    double ymargin = (lim.ymax - lim.ymin) * 0.05;

    if (xmargin == 0) xmargin = 1.0;
    if (ymargin == 0) ymargin = 1.0;

    lim.xmin -= xmargin;
    lim.xmax += xmargin;
    lim.ymin -= ymargin;
    lim.ymax += ymargin;

    return (lim);
}
