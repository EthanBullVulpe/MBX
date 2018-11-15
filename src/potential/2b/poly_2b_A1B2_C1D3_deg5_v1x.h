#ifndef POLY_2B_A1B2_C1D3_deg5_H
#define POLY_2B_A1B2_C1D3_deg5_H

namespace x2b_A1B2_C1D3_deg5 {

struct poly_2b_A1B2_C1D3_deg5_v1x {
    static const unsigned n_vars = 12;
    static const unsigned size = 776;

    static double eval(const double a[776],
                       const double x[12]);

    static double eval(const double a[776],
                       const double x[12],
                             double g[12]);

    static double eval_direct(const double a[776],
                              const double x[12]);

public:
    unsigned report_nvars(){ return n_vars; };
    unsigned report_size(){ return size; };
};
} // namespace x2b_A1B2_C1D3_deg5

#endif // POLY_2B_A1B2_C1D3_deg5_H
