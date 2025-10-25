# ft_matplotlib

A lightweight C library for creating data visualizations, inspired by Python's matplotlib. Built as part of the ft_maki project ecosystem.

**Note**: This is a learning project inspired by Matplotlib functionality.

## Overview

ft_matplotlib provides a simple API for generating plots and charts directly from C code, with support for ndarray data structures and dataframes. All visualizations are rendered to PNG images using a custom canvas-based drawing system.

### Installation
```bash
git clone https://github.com/thiagoviks/ft_matplotlib.git
cd ft_matplotlib
sudo make install
```

## Features

### Plot Types
- **Line plots**: Connect data points with lines
- **Scatter plots**: Plot individual points with optional sizing
- **Bar charts**: Vertical bar graphs with customizable widths
- **Histograms**: Distribution visualization with configurable bins
- **Heatmaps**: 2D matrix visualization with color mapping

### Customization
- **Axes & grids**: Automatic or manual axis limits, grid overlay
- **Labels & titles**: Add text annotations to plots
- **Legends**: Multi-item legends with color coding
- **Colors**: Full RGB color support
- **Auto-scaling**: Automatic axis range detection

### Data Sources
- **ndarray support**: Direct plotting from N-dimensional arrays
- **DataFrame integration**: Plot columns from structured data
- **Multi-plot support**: Overlay multiple datasets

## Core Data Structures

### Canvas
```c
typedef struct {
  int width, height;
  unsigned char *pixels;
} Canvas;
```

### Color
```c
typedef struct {
  unsigned char r, g, b;
} Color;
```

### AxisLimits
```c
typedef struct {
  double xmin, xmax;
  double ymin, ymax;
} AxisLimits;
```

### Legend
```c
typedef struct {
  LegendItem *items;
  int nitems;
  int max_items;
} Legend;
```

## API Reference

### Canvas Management

```c
Canvas *create_canvas(int w, int h);
void plt_destroy(Canvas *c);
void plt_savefig(Canvas *c, const char *filename);
void plt_show(Canvas *c);
```

### Line Plots

```c
// Plot from ndarrays
void plt_plot_ndarray(Canvas *c, ndarray *x, ndarray *y, Color col, 
                      double xmin, double xmax, double ymin, double ymax);

// Plot from dataframe columns
void plt_plot_dataframe(Canvas *c, dataframe *df, int colx, int coly, Color col,
                        double xmin, double xmax, double ymin, double ymax);
```

### Scatter Plots

```c
// Basic scatter
void plt_scatter_ndarray(Canvas *c, ndarray *x, ndarray *y, Color col,
                         double xmin, double xmax, double ymin, double ymax);

// Scatter with sized circles
void plt_scatter_circle_ndarray(Canvas *c, ndarray *x, ndarray *y, Color col,
                                double xmin, double xmax, double ymin, double ymax,
                                int radius);
```

### Bar Charts

```c
// Standard bar chart
void plt_bar_ndarray(Canvas *c, ndarray *x, ndarray *height, Color col,
                     double xmin, double xmax, double ymin, double ymax);

// Bar chart with custom width
void plt_bar_ndarray2(Canvas *c, ndarray *x, ndarray *y, Color col, 
                      double xmin, double xmax, double ymin, double ymax, 
                      int bar_width);
```

### Histograms

```c
// Basic histogram
void plt_hist(Canvas *c, ndarray *data, int bins, Color col, 
              double xmin, double xmax);

// Alternative histogram implementation
void plt_hist2(Canvas *c, ndarray *data, int bins, Color col, 
               double xmin, double xmax);

// Full-featured histogram with labels
void plt_hist_full(Canvas *c, ndarray *data, int bins, Color col,
                   const char *title, const char *xlabel, const char *ylabel);
```

### Heatmap

```c
// Display 2D matrix as image
void plt_imshow(Canvas *c, ndarray *matrix);
```

### Customization Functions

```c
// Titles and labels
void plt_title(Canvas *c, const char *text, Color col);
void plt_xlabel(Canvas *c, const char *text, Color col);
void plt_ylabel(Canvas *c, const char *text, Color col);

// Axes
void plt_axes(Canvas *c, Color col);
void plt_axes2(Canvas *c, AxisLimits lim, Color col, int n_ticks);
void draw_axes(Canvas *c, double xmin, double xmax, double ymin, double ymax);

// Grid
void plt_grid(Canvas *c, int on);

// Legend
Legend *legend_create(int max_items);
void legend_add(Legend *lg, const char *label, Color col);
void plt_draw_legend(Canvas *c, Legend *lg);
void plt_legend(Canvas *c, const char *text, int x, int y, Color col);
```

### Auto-scaling

```c
// Auto-detect axis limits for single plot
AxisLimits plt_axis_auto(ndarray *x, ndarray *y);

// Auto-detect axis limits for multiple plots
AxisLimits plt_axis_auto_multi(ndarray **xs, ndarray **ys, int nplots);
```

### Low-level Drawing Primitives

```c
void set_pixel(Canvas *c, int x, int y, Color col);
void draw_line(Canvas *c, int x0, int y0, int x1, int y1, Color col);
void draw_rect(Canvas *c, int x, int y, int w, int h, Color col);
void draw_circle(Canvas *c, int cx, int cy, int r, Color col);
void draw_text(Canvas *c, int x, int y, const char *text, Color col);
```

## Usage Example

```c
#include <ft_math.h>
#include <ft_matplotlib.h>
#include <ft_ndarray.h>

int main(void) {
  // Create canvas
  Canvas *canvas = create_canvas(800, 600);

  // Prepare data
  int shape[] = {100};
  ndarray *x = ft_create_ndarray(shape, 1, sizeof(double));
  ndarray *y = ft_create_ndarray(shape, 1, sizeof(double));

  // Fill with data (sin wave example)
  for (int i = 0; i < 100; i++) {
    ((double *)x->data)[i] = i * 0.1;
    ((double *)y->data)[i] = ft_sin(i * 0.1);
  }

  // Auto-scale axes
  AxisLimits lim = plt_axis_auto(x, y);

  // Plot with styling
  Color blue = {0, 0, 255};
  Color black = {0, 0, 0};

  plt_plot_ndarray(canvas, x, y, blue, lim.xmin, lim.xmax, lim.ymin, lim.ymax);

  plt_title(canvas, "sine wave", black);
  plt_xlabel(canvas, "x axis", black);
  plt_ylabel(canvas, "y axis", black);
  plt_axes2(canvas, lim, black, 10);

  // Save and cleanup
  plt_savefig(canvas, "sine_wave.png");
  plt_destroy(canvas);
  ft_free_ndarray(x);
  ft_free_ndarray(y);

  return 0;
}
```
**Note** This example uses ft_math
```c
#include <ft_maki.h>
```

**Compile:**
```bash
clang hello.c -o hello -lft_maki -lft_ndarray -lft_math
./hello
```

## Creating Multi-line Plots with Legend

```c
#include <ft_maki.h>
#include <ft_matplotlib.h>
#include <ft_ndarray.h>

int main(void) {

  double xdata[] = {0, 1, 2, 3, 4, 5};
  double ydata[] = {0, 1, 4, 9, 16, 25};

  double xdata1[] = {0, 1, 2 * 5, 3 * 5, 4 * 5, 5 * 5};
  double ydata1[] = {0, 1, 4 * 8, 9 * 8, 16 * 8, 25 * 8};

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

  ndarray *xs[2] = {&x, &y};
  ndarray *ys[2] = {&x1, &y1};
  AxisLimits lim = plt_axis_auto_multi(xs, ys, 2);

  plt_axes2(c, lim, black, 5);

  plt_plot_ndarray(c, &x, &y, orange, lim.xmin, lim.xmax, lim.ymin, lim.ymax);
  plt_plot_ndarray(c, &x1, &y1, blue, lim.xmin, lim.xmax, lim.ymin, lim.ymax);

  plt_scatter_ndarray(c, &x, &y, blue, lim.xmin, lim.xmax, lim.ymin, lim.ymax);
  plt_scatter_circle_ndarray(c, &x1, &y1, orange, lim.xmin, lim.xmax, lim.ymin,
                             lim.ymax, 2);

  plt_bar_ndarray2(c, &x, &y, red, lim.xmin, lim.xmax, lim.ymin, lim.ymax, 8);
  plt_bar_ndarray2(c, &x1, &y1, orange, lim.xmin, lim.xmax, lim.ymin, lim.ymax, 8);

  plt_title(c, "graph of the sine function", green);
  plt_xlabel(c, "axis x", red);
  plt_ylabel(c, "axis y", black);

  Legend *lg = legend_create(2);
  legend_add(lg, "y1", red);
  legend_add(lg, "y2", blue);
  plt_draw_legend(c, lg);

  // save
  plt_savefig(c, "ft_matplotlib2.png");
  plt_destroy(c);

  return 0;
}

```

**Compile:**
```bash
clang hello2.c -o hello2 -lft_maki -lft_ndarray
./hello
```

## Technical Details

### Rendering Engine
- Uses **STB Image Write** library for PNG output
- Custom **8x8 bitmap font** for text rendering
- **Bresenham algorithms** for line and circle drawing
- Direct pixel manipulation for maximum control

### Dependencies
- `ft_maki.h` - Core utilities
- `ft_ndarray.h` - Data structures
- `stb_image_write.h` - PNG export
- `font8x8_basic.h` - Text rendering

### Output Format
- All plots are saved as PNG images
- Default canvas sizes are customizable
- RGB color space (24-bit)

## Building

This library requires the ft_maki and ft_ndarray projects:

```c
#include <ft_maki.h>
#include <ft_ndarray.h>
#include <ft_matplotlib.h>
```

Ensure you have the STB libraries in your include path.

## Limitations

- No interactive plotting (static images only)
- Limited font support (8x8 bitmap font)
- No advanced color gradients or alpha blending
- Single-threaded rendering

## Future Enhancements

Potential additions:
- More plot types (box plots, violin plots, contour plots)
- Advanced styling options
- Subplot support
- Color maps for heatmaps
- LaTeX-style math rendering

## License

MIT

---

**Note**: This is a learning project inspired by matplotlib's plotting API, reimagined for C programming.

## Author
**Maki** - Inspired by Python matplotlib, built from scratch in C.