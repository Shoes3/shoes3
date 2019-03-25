/*
 * plot.h
*/
#ifndef PLOT_H
#define PLOT_H
#include "shoes/app.h"
#include "shoes/canvas.h"
#include "shoes/ruby.h"
#include "shoes/internal.h"
#include "shoes/world.h"
#include "shoes/native/native.h"
#include "shoes/types/effect.h"
#include "shoes/types/color.h"
#include "shoes/version.h"
#include "shoes/http.h"
#include <math.h>

// shoes_plot and shoes_chart_series structs are defined in canvas.h

//#define NEW_MACRO_CHARTSERIES
//#define NEW_MACRO_PLOT

#ifdef NEW_MACRO_CHARTSERIES
extern const rb_data_type_t shoes_chart_series_type;
#endif

#ifdef NEW_MACRO_PLOT
extern const rb_data_type_t shoes_plot_type;
#endif

// ChartSeries struct
typedef struct {
    VALUE maxv;
    VALUE minv;
    VALUE values;
    VALUE name;
    VALUE desc;
    VALUE labels;
    VALUE strokes;
    VALUE point_type;
    VALUE color;
} shoes_chart_series;

//
// Plot struct - It's HUGE!
//
typedef struct {
    VALUE parent;
    VALUE attr;
    shoes_place place;
    int chart_type;
    int seriescnt;
    VALUE series;
    int auto_grid;
    int boundbox;
    int missing;   // repurposed in pie_charts so beware
    VALUE background;
    VALUE title;
    VALUE legend;
    VALUE caption;
    VALUE default_colors;
    VALUE column_opts;
    void *c_things;
    int x_ticks;   // number of x_axis (which means a vertical grid line draw)
    int y_ticks;   // number of (left side) y axis horizontial grid lines)
    double radar_label_mult; // radius multipler (1.1 ex)
    char  *fontname; // not a Shoes name, cairo "toy" name - might be the same
    int beg_idx;  //used for zooming in
    int end_idx;  // and zooming out
    int title_h;
    PangoFontDescription *title_pfd;
    int caption_h;
    PangoFontDescription *caption_pfd;
    int legend_h;
    PangoFontDescription *legend_pfd;
    PangoFontDescription *label_pfd;
    PangoFontDescription *tiny_pfd;
    int yaxis_offset; // don't like
    int graph_h;  // to where the dots are drawn
    int graph_w;
    int graph_x;
    int graph_y;
    char hover;
    shoes_transform *st;
} shoes_plot;

/*
 * NOTE: functions that changes the cairo state (aka ontext, cairo_t) for
 * color or line width and the like should call shoes_plot_set_cairo_default()
 * in plot_util.c to restore the default Shoes plot drawing state.
 *
 * Only plot_*.c includes plot.h so the enums don't bleed into Shoes.
*/

enum {
    VERTICALLY,
    HORIZONTALLY
};

enum {
    LEFT,
    BELOW,
    RIGHT
};
// missing value or observation handling
enum {
    MISSING_SKIP,
    MISSING_MIN,
    MISSING_MAX
};

// chart type - line is default
enum  {
    LINE_CHART,
    TIMESERIES_CHART,
    COLUMN_CHART,
    SCATTER_CHART,
    PIE_CHART,
    RADAR_CHART
};

enum {
    NUB_NONE,
    NUB_DOT,     //filled circle
    NUB_CIRCLE,  //unfilled circle
    NUB_BOX,     //filled rect
    NUB_RECT,    //unfilled rect
};

// positional values for Radar Chart column_settings
enum {
    RADAR_LABEL,
    RADAR_MIN,
    RADAR_MAX,
    RADAR_EXTRA,
};

// quadrant (pie, radar)
enum {
    QUAD_ONE,
    QUAD_TWO,
    QUAD_THREE,
    QUAD_FOUR,
    QUAD_ERR
};

// Pie charts are sufficently complex that we need some structs for the
// internal stuff. Probably should be done for the other types to
typedef struct {
    double value;
    double startAngle;
    double endAngle;
    shoes_color *color;
    char *label;
    int lh; // label height and width and placement
    int lw;
    int lx, ly;
    PangoLayout *layout;
} pie_slice_t;

typedef struct {
    int percent;  // true when display % instead of value
    double radius;
    int centerx;
    int centery;
    int count;
    int top, left, bottom, right, height, width;
    double maxv;
    double minv;
    pie_slice_t *slices; // treated as an array because it is.
} pie_chart_t;


typedef struct {
    int percent;  // true when display % instead of value
    double radius;
    double rotation;
    double angle;
    int centerx;
    int centery;
    int count;
    int top, left, bottom, right, height, width;
    double maxv;
    double minv;
    double *colmax;   //array of
    double *colmin;   //array of
    char **labels;    //array of string ptrs;
    char **fmt_strs;  //array of string ptrs;
    double *lh;       // array
    double *lw;       // array
    double *lx;       // array
    double *ly;       // array
    PangoLayout **layouts;  // array
} radar_chart_t;

typedef cairo_public cairo_surface_t * (cairo_surface_function_t) (const char *filename, double width, double height);

extern void shoes_plot_line_draw(cairo_t *, shoes_place *, shoes_plot *);
extern void shoes_plot_column_draw(cairo_t *, shoes_place *, shoes_plot *);
extern void shoes_plot_scatter_draw(cairo_t *, shoes_place *, shoes_plot *);
extern void shoes_plot_pie_draw(cairo_t *, shoes_place *, shoes_plot *);
extern void shoes_plot_pie_init(shoes_plot *);
extern void shoes_plot_pie_dealloc(shoes_plot *);
extern void shoes_plot_radar_init(shoes_plot *);
extern void shoes_plot_radar_dealloc(shoes_plot *);
extern void shoes_plot_radar_draw(cairo_t *, shoes_place *, shoes_plot *);
extern void shoes_chart_series_Cinit(shoes_chart_series *, VALUE, VALUE,
                                     VALUE, VALUE, VALUE, VALUE, VALUE, VALUE, VALUE);
extern VALUE shoes_plot_parse_column_settings(VALUE);
// plot utility functions (in plot_util.c)
extern void shoes_plot_set_cairo_default(cairo_t *, shoes_plot *);
extern void shoes_plot_util_default_colors(shoes_plot *);
extern void shoes_plot_draw_title(cairo_t *, shoes_plot *);
extern void shoes_plot_draw_caption(cairo_t *,shoes_plot *);
extern void shoes_plot_draw_fill(cairo_t *, shoes_plot *);
extern void shoes_plot_draw_boundbox(cairo_t *, shoes_plot *);
extern void shoes_plot_draw_ticks_and_labels(cairo_t *, shoes_plot *);
extern void shoes_plot_draw_legend(cairo_t *, shoes_plot *);
extern void shoes_plot_draw_tick(cairo_t *, shoes_plot *, int, int, int);
extern void shoes_plot_draw_label(cairo_t *, shoes_plot *, int, int, char*, int);
extern void shoes_plot_draw_nub(cairo_t *, shoes_plot *, double, double, int, int);
extern int shoes_plot_util_quadrant(double angle);
extern void shoes_plot_util_adornments(cairo_t *, shoes_place *, shoes_plot *, int);

// TODO: refactor into shoes/types?
extern VALUE cPlot, cChartSeries;

#endif
