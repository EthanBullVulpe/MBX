#include "x1b_A1B3_deg5_v1x.h" 
 

////////////////////////////////////////////////////////////////////////////////

namespace {

//----------------------------------------------------------------------------//

struct variable {
    double v_exp0(const double& r0, const double& k,
                 const double * p1, const double * p2 );
                 
    double v_exp(const double& k,
                 const double * p1, const double * p2 );

    double v_coul0(const double& r0, const double& k,
                  const double * p1, const double * p2 );
                  
    double v_coul(const double& k,
                  const double * p1, const double * p2 );
                  
    double v_gau0(const double& r0, const double& k,
                 const double * p1, const double * p2 );
                 
    void grads(const double& gg, double * grd1, double * grd2,
               const double * p1, const double * p2);

    double g[3]; // diff(value, p1 - p2)
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

double variable::v_gau0(const double& r0, const double& k,
                       const double * p1, const double * p2)
{
    g[0] = p1[0] - p2[0];
    g[1] = p1[1] - p2[1];
    g[2] = p1[2] - p2[2];

    const double r = std::sqrt(g[0]*g[0] + g[1]*g[1] + g[2]*g[2]);

    const double exp1 = std::exp(-k*(r0 - r)*(r0 - r));
    const double gg = 2*k*(r0 - r)*exp1/r;

    g[0] *= gg;
    g[1] *= gg;
    g[2] *= gg;

    return exp1;
}


//----------------------------------------------------------------------------//

void variable::grads(const double& gg, double * grd1, double * grd2, 
                     const double * p1, const double * p2) {
    for (size_t i = 0; i < 3 ; i++) {
        double d = gg*g[i];
        grd1[i] += d;
        grd2[i] -= d;
    }
}

//----------------------------------------------------------------------------//

double variable::v_exp0(const double& r0, const double& k,
                       const double * p1, const double * p2)
{
    g[0] = p1[0] - p2[0];
    g[1] = p1[1] - p2[1];
    g[2] = p1[2] - p2[2];

    const double r = std::sqrt(g[0]*g[0] + g[1]*g[1] + g[2]*g[2]);

    const double exp1 = std::exp(k*(r0 - r));
    const double gg = - k*exp1/r;

    g[0] *= gg;
    g[1] *= gg;
    g[2] *= gg;

    return exp1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

double variable::v_coul0(const double& r0, const double& k,
                        const double * p1, const double * p2)
{
    g[0] = p1[0] - p2[0];
    g[1] = p1[1] - p2[1];
    g[2] = p1[2] - p2[2];

    const double rsq = g[0]*g[0] + g[1]*g[1] + g[2]*g[2];
    const double r = std::sqrt(rsq);

    const double exp1 = std::exp(k*(r0 - r));
    const double rinv = 1.0/r;
    const double val = exp1*rinv;

    const double gg = - (k + rinv)*val*rinv;

    g[0] *= gg;
    g[1] *= gg;
    g[2] *= gg;

    return val;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

double variable::v_exp(const double& k,
                       const double * p1, const double * p2)
{
    g[0] = p1[0] - p2[0];
    g[1] = p1[1] - p2[1];
    g[2] = p1[2] - p2[2];

    const double r = std::sqrt(g[0]*g[0] + g[1]*g[1] + g[2]*g[2]);

    const double exp1 = std::exp(k*(- r));
    const double gg = - k*exp1/r;

    g[0] *= gg;
    g[1] *= gg;
    g[2] *= gg;

    return exp1;
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //


double variable::v_coul(const double& k,
                        const double * p1, const double * p2)
{
    g[0] = p1[0] - p2[0];
    g[1] = p1[1] - p2[1];
    g[2] = p1[2] - p2[2];

    const double rsq = g[0]*g[0] + g[1]*g[1] + g[2]*g[2];
    const double r = std::sqrt(rsq);

    const double exp1 = std::exp(k*(-r));
    const double rinv = 1.0/r;
    const double val = exp1*rinv;

    const double gg = - (k + rinv)*val*rinv;

    g[0] *= gg;
    g[1] *= gg;
    g[2] *= gg;

    return val;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

struct monomer {
    double oh1[3];
    double oh2[3];

    void setup(const double* ohh,
               const double& in_plane_g, const double& out_of_plane_g,
               double x1[3], double x2[3]);

    void grads(const double* g1, const double* g2,
               const double& in_plane_g, const double& out_of_plane_g,
               double* grd) const;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

void monomer::setup(const double* ohh,
                    const double& in_plane_g, const double& out_of_plane_g,
                    double* x1, double* x2)
{
    for (int i = 0; i < 3; ++i) {
        oh1[i] = ohh[i + 3] - ohh[i];
        oh2[i] = ohh[i + 6] - ohh[i];
    }

    const double v[3] = {
        oh1[1]*oh2[2] - oh1[2]*oh2[1],
        oh1[2]*oh2[0] - oh1[0]*oh2[2],
        oh1[0]*oh2[1] - oh1[1]*oh2[0]
    };

    for (int i = 0; i < 3; ++i) {
        const double in_plane = ohh[i] + 0.5*in_plane_g*(oh1[i] + oh2[i]);
        const double out_of_plane = out_of_plane_g*v[i];

        x1[i] = in_plane + out_of_plane;
        x2[i] = in_plane - out_of_plane;
    }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

void monomer::grads(const double* g1, const double* g2,
                    const double& in_plane_g, const double& out_of_plane_g,
                    double* grd) const
{
    const double gm[3] = {
        g1[0] - g2[0],
        g1[1] - g2[1],
        g1[2] - g2[2]
    };

    const double t1[3] = {
        oh2[1]*gm[2] - oh2[2]*gm[1],
        oh2[2]*gm[0] - oh2[0]*gm[2],
        oh2[0]*gm[1] - oh2[1]*gm[0]
    };

    const double t2[3] = {
        oh1[1]*gm[2] - oh1[2]*gm[1],
        oh1[2]*gm[0] - oh1[0]*gm[2],
        oh1[0]*gm[1] - oh1[1]*gm[0]
    };

    for (int i = 0; i < 3; ++i) {
        const double gsum = g1[i] + g2[i];
        const double in_plane = 0.5*in_plane_g*gsum;

        const double gh1 = in_plane + out_of_plane_g*t1[i];
        const double gh2 = in_plane - out_of_plane_g*t2[i];

        grd[i + 0] += gsum - (gh1 + gh2); // O
        grd[i + 3] += gh1; // H1
        grd[i + 6] += gh2; // H2
    }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

//struct vsites {
//    //void TwoParticleAverageSite() {}
//    //void ThreeParticleAverageSite() {}
//    void OutOfPlaneSite(const double& w12, const double& w13,
//                        const double& wcross, const double x1[3],
//                        const double y1[3], const double y2[3],
//                        double vs[3]);
//    //void LocalCoordinatesSite{}
//};
//
//void vsites::OutOfPlaneSite(const double& w12,
//                            const double& w13,
//                            const double& wcross,
//                            const double x1[3],
//                            const double y1[3],
//                            const double y2[3],
//                            double vs[3]) {
//    double r12[3], r13[3];
//
//    for (int i = 0; i < 3; ++i) {
//        r12[i] = y1[i] - x1[i];
//        r13[i] = y2[i] - x1[i];
//    }
//                            
//    double rc[3];
//    rc[0] = r12[1]*r13[2] - r12[2]*r13[1];
//    rc[1] = r12[2]*r13[0] - r12[0]*r13[2];
//    rc[2] = r12[0]*r13[1] - r12[1]*r13[0];
//    
//    vs[0] = x1[0] + w12 * r12[0] + w13 * r13[0] + wcross * rc[0];
//    vs[1] = x1[1] + w12 * r12[1] + w13 * r13[1] + wcross * rc[1];
//    vs[2] = x1[2] + w12 * r12[2] + w13 * r13[2] + wcross * rc[2];
//}

} // namespace

////////////////////////////////////////////////////////////////////////////////

namespace x1b_A1B3_deg5 {

//----------------------------------------------------------------------------//



x1b_A1B3_v1x::x1b_A1B3_v1x(std::string mon) {


    // =====>> SECTION CONSTRUCTOR <<=====
    // =>> PASTE RIGHT BELOW THIS LINE <== 
    if (mon == "no3") {
        coefficients = std::vector<double> {
           1.467013276924008e+04, // 0
          -1.202387177449758e+04, // 1
          -1.374236051756376e+05, // 2
           1.882835905681036e+05, // 3
           1.610017570858289e+05, // 4
          -1.505662231881726e+05, // 5
          -1.789342898858161e+05, // 6
          -9.336773587118372e+04, // 7
           1.453509770336072e+05, // 8
           1.079230783551492e+06, // 9
           9.379038028805507e+05, // 10
          -7.672068371940963e+04, // 11
          -6.278526393892618e+05, // 12
           4.820774824125394e+05, // 13
          -1.563354187008254e+06, // 14
          -1.492808021508751e+06, // 15
           3.618032872736755e+05, // 16
           3.571802393644014e+05, // 17
           5.617138404639941e+05, // 18
          -8.051501011084202e+05, // 19
           2.234109819705603e+06, // 20
          -3.327264814773584e+05, // 21
          -1.772037109458685e+07, // 22
           5.236067490257511e+06, // 23
          -1.413401537284706e+07, // 24
           9.391448165628776e+07, // 25
          -8.246589009616192e+07, // 26
           2.897621741689878e+06, // 27
           2.862042325232227e+07, // 28
          -4.234781044982562e+07, // 29
           4.705659302587409e+07, // 30
           9.382402329529318e+07, // 31
          -1.843032799150361e+07, // 32
           1.239650954809004e+07, // 33
          -1.041431314593068e+08, // 34
           1.958782839548584e+07, // 35
          -5.215304156924993e+07, // 36
           5.729095208304486e+07, // 37
          -5.242808273790624e+07, // 38
          -4.203321860934235e+07, // 39
           3.625858408271678e+07, // 40
          -5.269457786856639e+07, // 41
          -4.219067438513709e+07, // 42
           2.423896214137727e+07, // 43
           4.776499919739433e+07, // 44
          -1.437381092053698e+07, // 45
           4.727667377674374e+07, // 46
           1.878815555017274e+07, // 47
           2.389831461288344e+07, // 48
           4.812028621092267e+07, // 49
           4.211341265051085e+08, // 50
          -1.372361135588659e+08, // 51
           1.426777813162109e+08, // 52
           2.099063052693085e+08, // 53
           1.992160619731002e+08, // 54
           3.940187816101905e+08, // 55
          -4.107804388790347e+08, // 56
          -4.236001014761753e+08, // 57
           7.449325870514711e+06, // 58
           4.179908270758910e+08, // 59
           7.078381420831852e+07, // 60
           3.380519179167102e+07, // 61
          -6.446110799964182e+07, // 62
          -4.106685576986044e+08, // 63
           7.097332947645731e+07, // 64
          -1.436407042040988e+08, // 65
          -4.225896673352960e+08, // 66
          -1.424454421427803e+08, // 67
           8.338768974233510e+08, // 68
          -3.677162551723567e+07, // 69
          -6.997041214394571e+07, // 70
          -2.122148529920201e+08, // 71
          -1.265685697980537e+08, // 72
           2.108888829862254e+08, // 73
           1.333994466570028e+08, // 74
           7.201521915172590e+07, // 75
           1.334119651077173e+08, // 76
          -1.877717006860089e+08, // 77
           1.423519997794528e+08, // 78
          -8.104957571726564e+08, // 79
           2.002792347492156e+08, // 80
           1.434121371346337e+08, // 81
          -3.224891808965320e+07, // 82
           4.187104946536853e+08, // 83
           3.923066519204194e+08, // 84
           2.114497603185102e+08, // 85
           2.116989931248341e+08, // 86
           4.181368870681080e+08, // 87
           3.649384732325295e+07, // 88
           3.323591337363309e+07, // 89
           1.341793354537147e+08, // 90
          -4.091796635231270e+08, // 91
          -3.696780337973800e+07, // 92
          -2.064033935970311e+08, // 93
          -6.256034158191132e+06, // 94
          -4.105371753976170e+08, // 95
          -2.116982236980565e+08, // 96
          -2.082296657303753e+08, // 97
          -1.437689139582170e+08, // 98
          -2.052493604133397e+08, // 99
          -1.385616241019757e+08, // 100
          -6.913700410623027e+07}; // 101

  m_k_AB =  8.050030335495362e-01; // A^(-1))
  m_k_BB =  5.318160031156520e-01; // A^(-1))


    } // end if mon == "no3"


}

//----------------------------------------------------------------------------//

std::vector<double> x1b_A1B3_v1x::eval(const double* xyz, const size_t nmon) const
{

    std::vector<double> energies(nmon,0.0);

    for (size_t j = 0; j < nmon; j++) {

        double xcrd[12]; // coordinates of real sites ONLY

        std::copy(xyz + j*12, xyz + (j+1)*12, xcrd);
    
        double v[6];
    
        const double* A_1_a= xcrd + 0;
        const double* B_1_a= xcrd + 3;
        const double* B_2_a= xcrd + 6;
        const double* B_3_a= xcrd + 9;



        variable vr[6];

        v[0]  = vr[0].v_exp(m_k_AB, A_1_a, B_1_a);
        v[1]  = vr[1].v_exp(m_k_AB, A_1_a, B_2_a);
        v[2]  = vr[2].v_exp(m_k_AB, A_1_a, B_3_a);
        v[3]  = vr[3].v_exp(m_k_BB, B_1_a, B_2_a);
        v[4]  = vr[4].v_exp(m_k_BB, B_1_a, B_3_a);
        v[5]  = vr[5].v_exp(m_k_BB, B_2_a, B_3_a);

     
    
        energies[j] = polynomial::eval(coefficients.data(), v);

    }

    return energies;
}

std::vector<double> x1b_A1B3_v1x::eval(const double* xyz, 
                double * grad, const size_t nmon) const
{

    std::vector<double> energies(nmon,0.0);

    for (size_t j = 0; j < nmon; j++) {

        double xcrd[12]; // coordinates of real sites ONLY

        std::copy(xyz + j*12, xyz + (j+1)*12, xcrd);

        double v[6];
    
        const double* A_1_a= xcrd + 0;
        const double* B_1_a= xcrd + 3;
        const double* B_2_a= xcrd + 6;
        const double* B_3_a= xcrd + 9;



        variable vr[6];

        v[0]  = vr[0].v_exp(m_k_AB, A_1_a, B_1_a);
        v[1]  = vr[1].v_exp(m_k_AB, A_1_a, B_2_a);
        v[2]  = vr[2].v_exp(m_k_AB, A_1_a, B_3_a);
        v[3]  = vr[3].v_exp(m_k_BB, B_1_a, B_2_a);
        v[4]  = vr[4].v_exp(m_k_BB, B_1_a, B_3_a);
        v[5]  = vr[5].v_exp(m_k_BB, B_2_a, B_3_a);

     
    
        double g[6];
        
        energies[j] = polynomial::eval(coefficients.data(), v, g);
        
        double xgrd[12];
        std::fill(xgrd, xgrd + 12, 0.0);

        double* A_1_a_g= xgrd + 0;
        double* B_1_a_g= xgrd + 3;
        double* B_2_a_g= xgrd + 6;
        double* B_3_a_g= xgrd + 9;


        vr[0].grads(g[0], A_1_a_g, B_1_a_g, A_1_a, B_1_a);
        vr[1].grads(g[1], A_1_a_g, B_2_a_g, A_1_a, B_2_a);
        vr[2].grads(g[2], A_1_a_g, B_3_a_g, A_1_a, B_3_a);
        vr[3].grads(g[3], B_1_a_g, B_2_a_g, B_1_a, B_2_a);
        vr[4].grads(g[4], B_1_a_g, B_3_a_g, B_1_a, B_3_a);
        vr[5].grads(g[5], B_2_a_g, B_3_a_g, B_2_a, B_3_a);


        for (size_t i = 0; i < 12; i++)
            grad[i + j*12] = xgrd[i];

    }

    return energies;
}

} // namespace x1b_A1B3_deg5

////////////////////////////////////////////////////////////////////////////////
