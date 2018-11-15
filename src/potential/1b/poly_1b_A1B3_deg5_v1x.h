#ifndef POLY_1B_A1B3_deg5_H
#define POLY_1B_A1B3_deg5_H

namespace x1b_A1B3_deg5 {

struct poly_1b_A1B3_deg5 {
    static const unsigned n_vars = 6;
    static const unsigned size = 102;

    static double eval(const double a[102],
                       const double x[6]);

    static double eval(const double a[102],
                       const double x[6],
                             double g[6]);


public:
    unsigned report_nvars(){ return n_vars; };
    unsigned report_size(){ return size; };
};

} // namespace x1b_A1B3_deg5

#endif // POLY_1B_A1B3_deg5_H
