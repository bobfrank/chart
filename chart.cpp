#include <chart.h>
#include <cairo/cairo.h>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <tuple>
#include <sstream>

namespace bct {

double calculate_border(cairo_t *cr, const ChartBorder& border, bool left_or_right)
{
    double max_border_size = 0;

    cairo_text_extents_t extents;
    for( int i = 0; i < border.ticks.size(); ++i )
    {
        const ChartTick& tick = border.ticks[i];
        cairo_select_font_face(cr, tick.font.c_str(), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
        cairo_set_font_size(cr, tick.font_size);
        cairo_text_extents(cr, tick.label.c_str(), &extents);
        if( left_or_right ) {
            max_border_size = std::max(max_border_size, extents.height*sin(tick.rotation) + extents.width*cos(tick.rotation) );
        } else {
            max_border_size = std::max(max_border_size, extents.width*sin(tick.rotation) + extents.height*cos(tick.rotation) );
        }
    }

    if( border.label != "" ) {
        cairo_select_font_face(cr, border.font.c_str(), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, border.font_size);
        cairo_text_extents(cr, border.label.c_str(), &extents);
        max_border_size += extents.height;
    }

    return max_border_size
                + border.pre_label_padding_size
                + border.post_label_padding_size
                + border.post_tick_label_padding_size
                + border.tick_size;
}

double calculate_title(cairo_t *cr, const ChartConfig& c)
{
    if( c.title != "" ) {
        cairo_text_extents_t extents;
        cairo_select_font_face(cr, c.font.c_str(), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, c.font_size);
        cairo_text_extents(cr, c.title.c_str(), &extents);
        return extents.height
                + c.pre_title_padding_size
                + c.post_title_padding_size;
    } else {
        return 0;
    }
}

void plot_title(cairo_t *cr, const ChartConfig& c)
{
    if( c.title != "" ) {
        cairo_select_font_face(cr, c.font.c_str(), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, c.font_size);
        cairo_text_extents_t extents;
        cairo_text_extents(cr, c.title.c_str(), &extents);
        const double x = (c.width - extents.width)/2.0;
        cairo_move_to(cr, x, extents.height + c.pre_title_padding_size);
        cairo_show_text(cr, c.title.c_str());
    }
}

void plot_gridlines(cairo_t *cr, const ChartConfig& c,
        const int border_top,  const int border_bottom,
        const int border_left, const int border_right)
{
    cairo_set_line_width(cr, 0.4);
    cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 0.5);

    // TODO reduce duplicate code
    const double y_range_in  = c.ymax - c.ymin;
    const double y_min_in    = c.ymin;
    const double y_min_out   = border_top;
    const double y_range_out = c.height - border_top - border_bottom;
    for( int i = 0; i < c.ygridlines.size(); ++i ) {
        const double y = y_range_out - (c.ygridlines[i] - y_min_in)/y_range_in*y_range_out + y_min_out;
        cairo_move_to(cr, border_left, y);
        cairo_line_to(cr, c.width - border_right, y);
        cairo_stroke(cr);
    }

    const double x_range_in  = c.xmax - c.xmin;
    const double x_min_in    = c.xmin;
    const double x_min_out   = border_left;
    const double x_range_out = c.width - border_left - border_right;
    for( int i = 0; i < c.xgridlines.size(); ++i ) {
        const double x = (c.xgridlines[i] - x_min_in)/x_range_in*x_range_out + x_min_out;
        cairo_move_to(cr, x, border_top);
        cairo_line_to(cr, x, c.height - border_bottom);
        cairo_stroke(cr);
    }
    cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 1);
}

// TODO reduce duplication, should just be a plot_axis() function
void plot_yaxis(cairo_t *cr, const ChartConfig& c, const ChartBorder& border,
        const int border_top,  const int border_bottom,
        const int border_left, const int border_right)
{
    cairo_text_extents_t extents;

    const double y_range_in  = c.ymax - c.ymin;
    const double y_min_in    = c.ymin;
    const double y_min_out   = border_top;
    const double y_range_out = c.height - border_top - border_bottom;

    double left_border = border.pre_label_padding_size + border.post_label_padding_size;
    if( border.label != "" ) {
        cairo_select_font_face(cr, border.font.c_str(), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, border.font_size);
        cairo_text_extents(cr, border.label.c_str(), &extents);
        left_border += extents.height;
    }

    // add y axis tick labels
    for( int i = 0; i < border.ticks.size(); ++i ) {
        const double y = y_range_out - (border.ticks[i].location - y_min_in)/y_range_in*y_range_out + y_min_out;
        cairo_select_font_face(cr, border.ticks[i].font.c_str(), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
        cairo_set_font_size(cr, border.ticks[i].font_size);
        cairo_text_extents(cr, border.ticks[i].label.c_str(), &extents);
        const double y_adj = border.ticks[i].rotation==0 || border.ticks[i].rotation==90*3.14159/180
                           ? (extents.height*cos(border.ticks[i].rotation) + extents.width*sin(border.ticks[i].rotation))/2.0
                           : (extents.height*cos(border.ticks[i].rotation) + extents.width*sin(border.ticks[i].rotation));
        cairo_move_to(cr, left_border, y+y_adj);
        cairo_rotate(cr, -border.ticks[i].rotation);
        cairo_show_text(cr, border.ticks[i].label.c_str());
        cairo_rotate(cr, border.ticks[i].rotation);

        // add tick lines
        cairo_set_line_width (cr, 1);
        cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 1);
        cairo_move_to(cr, border_left-border.tick_size, y);
        cairo_line_to(cr, border_left, y);
        cairo_stroke(cr);
    }
}

void plot_xaxis(cairo_t *cr, const ChartConfig& c, const ChartBorder& border,
        const int border_top,  const int border_bottom,
        const int border_left, const int border_right)
{
    cairo_text_extents_t extents;

    const double x_range_in  = c.xmax - c.xmin;
    const double x_min_in    = c.xmin;
    const double x_min_out   = border_left;
    const double x_range_out = c.width - border_left - border_right;

    double left_border = border.pre_label_padding_size + border.post_label_padding_size;
    if( border.label != "" ) {
        cairo_select_font_face(cr, border.font.c_str(), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, border.font_size);
        cairo_text_extents(cr, border.label.c_str(), &extents);
        left_border += extents.height;
    }

    // add y axis tick labels
    for( int i = 0; i < border.ticks.size(); ++i ) {
        const double x = (border.ticks[i].location - x_min_in)/x_range_in*x_range_out + x_min_out;
        cairo_select_font_face(cr, border.ticks[i].font.c_str(), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
        cairo_set_font_size(cr, border.ticks[i].font_size);
        cairo_text_extents(cr, border.ticks[i].label.c_str(), &extents);
        double x_adj = (border.ticks[i].rotation==0||border.ticks[i].rotation==90*3.14159/180
                     ? (extents.height*sin(border.ticks[i].rotation) + extents.width*cos(border.ticks[i].rotation))/2.0
                     : 0) - extents.width*cos(border.ticks[i].rotation);
        const double y_adj = extents.width*sin(border.ticks[i].rotation) + extents.height*cos(border.ticks[i].rotation);
        cairo_move_to(cr, x+x_adj, c.height-border_bottom+border.post_tick_label_padding_size+y_adj+border.tick_size);
        cairo_rotate(cr, -border.ticks[i].rotation);
        cairo_show_text(cr, border.ticks[i].label.c_str());
        cairo_rotate(cr, border.ticks[i].rotation);

        // add tick lines
        cairo_set_line_width (cr, 1);
        cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 1);
        cairo_move_to(cr, x, c.height-border_bottom+border.tick_size);
        cairo_line_to(cr, x, c.height-border_bottom);
        cairo_stroke(cr);
    }
}

void plot_axes(cairo_t *cr, const ChartConfig& c,
        const int border_top,  const int border_bottom,
        const int border_left, const int border_right)
{
    // TODO do per axis and don't duplicate code
    // TODO add ability to plot right and top axes
    cairo_text_extents_t extents;
    // add y axis name
    cairo_select_font_face(cr, c.left.font.c_str(), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, c.left.font_size);
    cairo_text_extents(cr, c.left.label.c_str(), &extents);
    const double x = c.left.pre_label_padding_size + extents.height;
    const double y = c.height/2 + extents.width/2.; // TODO should be based on the viewing window, not the whole width and height
    cairo_move_to(cr, x, y);
    cairo_rotate(cr, -90*3.14159/180);
    cairo_show_text(cr, c.left.label.c_str());
    cairo_rotate(cr, 90*3.14159/180);

    // add x axis name
    cairo_select_font_face(cr, c.bottom.font.c_str(), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, c.bottom.font_size);
    cairo_text_extents(cr, c.bottom.label.c_str(), &extents);
    const double x2 = c.width/2.0 - extents.width/2; // TODO should be based on the viewing window, not the whole width and height
    const double y2 = c.height - c.bottom.pre_label_padding_size;
    cairo_move_to(cr, x2, y2);
    cairo_show_text(cr, c.bottom.label.c_str());

    plot_yaxis(cr, c, c.left, border_top, border_bottom, border_left, border_right);
    plot_xaxis(cr, c, c.bottom, border_top, border_bottom, border_left, border_right);
}

void plot_line(cairo_t *cr, const ChartConfig& c,
        const int border_top,  const int border_bottom,
        const int border_left, const int border_right)
{
    // TODO this transform needs to be more generic and less repeated
    const double x_range_in  = c.xmax - c.xmin;
    const double x_min_in    = c.xmin;
    const double x_min_out   = border_left;
    const double x_range_out = c.width - border_left - border_right;

    const double y_range_in  = c.ymax - c.ymin;
    const double y_min_in    = c.ymin;
    const double y_min_out   = border_top;
    const double y_range_out = c.height - border_top - border_bottom;

    cairo_set_line_width (cr, 1);
    typedef std::tuple<double,double,double> Color;
    std::vector<Color> colors{
        Color{1.0,0.0,0.0},
        Color{0.0,0.0,1.0},
        Color{0.0,1.0,0.0}
    };
    for( int j = 0; j < c.data.size(); ++j ) {
        cairo_set_source_rgba(cr, std::get<0>(colors[j]), std::get<1>(colors[j]), std::get<2>(colors[j]), 1);
        for( int i = 0; i < c.data[j].size(); ++i ) {
            const double x = (c.data[j][i].first  - x_min_in)/x_range_in*x_range_out + x_min_out;
            const double y = y_range_out - (c.data[j][i].second - y_min_in)/y_range_in*y_range_out + y_min_out;
            if( i == 0 ) {
                cairo_move_to(cr, x, y);
            } else {
                cairo_line_to(cr, x, y);
            }
        }
        cairo_stroke(cr);
    }
}

ChartConfig::ChartConfig()
    : pre_title_padding_size(5)
    , post_title_padding_size(1)
    , xmin(999999)
    , xmax(-999999)
    , ymin(999999)
    , ymax(-999999)
    , font("Sans")
    , font_size(13.0)
{
}

ChartOptions&
ChartOptions::data(const std::vector<ChartData>& c)
{
    m_chart_config.data = c;
    for( int i = 0; i < c.size(); ++i ) {
        for( int k = 0; k < c[i].size(); ++k ) {
            m_chart_config.xmin = std::min(c[i][k].first,  m_chart_config.xmin);
            m_chart_config.xmax = std::max(c[i][k].first,  m_chart_config.xmax);
            m_chart_config.ymin = std::min(c[i][k].second, m_chart_config.ymin);
            m_chart_config.ymax = std::max(c[i][k].second, m_chart_config.ymax);
        }
    }
//    m_chart_config.xmin = m_chart_config.xmin - 0.02*(m_chart_config.xmax-m_chart_config.xmin);
//    m_chart_config.xmax = m_chart_config.xmax + 0.02*(m_chart_config.xmax-m_chart_config.xmin);
//    m_chart_config.ymin = m_chart_config.ymin - 0.02*(m_chart_config.ymax-m_chart_config.ymin);
//    m_chart_config.ymax = m_chart_config.ymax + 0.02*(m_chart_config.ymax-m_chart_config.ymin);
//    m_chart_config.left.post_label_padding_size = 10;
    m_chart_config.left.post_tick_label_padding_size = 5;
    m_chart_config.bottom.post_label_padding_size = 10;
    m_chart_config.bottom.post_tick_label_padding_size = 5;
    for( int i = 0; i < 5; ++i ) {
        ChartTick t;
        double value = m_chart_config.ymin + i*(m_chart_config.ymax-m_chart_config.ymin)/4.0;
        t.label = format_y(value);
        t.location = value;
        m_chart_config.left.ticks.push_back(t);
    }
    for( int i = 0; i < 6; ++i ) {
        ChartTick t;
        double value = m_chart_config.xmin + i*(m_chart_config.xmax-m_chart_config.xmin)/5.0;
        t.label = format_x(value);
        t.location = value;
        t.rotation = 17*3.14159/180; // TODO this should only be if they requested it
        m_chart_config.bottom.ticks.push_back(t);
    }
    return *this;
}

std::string
ChartOptions::format_x(double x) const
{
    // TODO choose appropriate rounding levels
    std::ostringstream ss;
    ss << x;
    return ss.str();
}

std::string
ChartOptions::format_y(double y) const
{
    // TODO choose appropriate rounding levels
    std::ostringstream ss;
    ss << std::setprecision(3) << y;
    return ss.str();
}

int plot_line(const ChartOptions& co)
{
    const ChartConfig& c = co.chart_config();
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, c.width, c.height);
    cairo_t *cr = cairo_create(surface);

    const double border_left        = calculate_border(cr, c.left, true);
    const double border_bottom      = calculate_border(cr, c.bottom, false);
    const double border_top_title   = calculate_title(cr, c);
    const double border_top         = calculate_border(cr, c.top, true) + border_top_title;
    const double border_right       = calculate_border(cr, c.right, false);

    cairo_set_line_width(cr, 1);
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    cairo_rectangle(cr, border_left, border_top, c.width-border_left - border_right, c.height - border_top - border_bottom);
    cairo_stroke(cr);

    plot_title(cr, c);
    plot_gridlines(cr, c, border_top, border_bottom, border_left, border_right);
    plot_axes(cr, c, border_top, border_bottom, border_left, border_right);
    plot_line(cr, c, border_top, border_bottom, border_left, border_right);

    cairo_destroy(cr);
    cairo_surface_write_to_png(surface, co.output().c_str());
    cairo_surface_destroy(surface);
    return 0;
}

} // namespace bct

