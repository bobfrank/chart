#ifndef CHART_H
#define CHART_H

#include <vector>
#include <string>

namespace bct {

struct ChartTick {
    std::string font;
    double font_size;
    std::string label;
    double rotation;
    double location;
    ChartTick() : font("Sans"), font_size(11.0), rotation(0), location(0) {}
};

struct ChartBorder {
    std::vector<ChartTick> ticks;
    std::string label;
    std::string font;
    double font_size;
    double pre_label_padding_size;
    double post_label_padding_size;
    double post_tick_label_padding_size;
    double tick_size;
    ChartBorder() : pre_label_padding_size(5), font_size(11.0), post_label_padding_size(0), post_tick_label_padding_size(0), tick_size(5) {}
};

typedef std::vector<std::pair<double,double>> ChartData;

struct ChartConfig {
    std::vector<double> xgridlines, ygridlines;
    ChartBorder top, left, right, bottom;
    std::string title;
    double pre_title_padding_size;
    double post_title_padding_size;
    double font_size;
    std::string font;
    int width, height;
    double xmin, xmax;
    double ymin, ymax;
    std::vector<ChartData> data;
    ChartConfig();
};

class ChartOptions {
public:
    ChartOptions& width(int w) { m_chart_config.width = w; return *this; }
    ChartOptions& height(int h) { m_chart_config.height = h; return *this; }
    ChartOptions& title(const std::string& s) { m_chart_config.title = s; return *this; }
    ChartOptions& data(const std::vector<ChartData>& c);
    ChartOptions& xlabel(const std::string& s) { m_chart_config.bottom.label = s; return *this; }
    ChartOptions& ylabel(const std::string& s) { m_chart_config.left.label = s; return *this; }
    ChartOptions& output(const std::string& s) { m_output = s; return *this; }
    const ChartConfig& chart_config() const { return m_chart_config; }
    const std::string& output() const { return m_output; }
    virtual std::string format_x(double x) const;
    virtual std::string format_y(double y) const;

private:
    ChartConfig m_chart_config;
    std::string m_output;
};

int plot_line(const ChartOptions& co);

} // namespace bct

#endif
