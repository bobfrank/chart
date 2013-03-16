// g++ -o chart_test $(pkg-config --cflags --libs cairo) -I. -std=c++11 chart_test.cpp chart.cpp
#include <chart.h>
#include <sstream>
#include <iostream>

int main()
{
    bct::ChartData data{{{0,15},{5,19},{6,18},{9,14},{12,35},{18,1},{22,55},{30,44},
                          {33,30},{40,42},{45,42},{50,47},{60,2},{65,10},{70,9}}};
    std::vector<bct::ChartData> first(1,data);
    bct::plot_line(bct::ChartOptions()
        .width(600)
        .height(400)
        .title("Chart Title")
        .data(first)
        .xlabel("X")
        .ylabel("Y Value")
        .output("chart_test.png")
    );
}

