#include "mbnrg_1b_A1B3_deg5_v1.h"

////////////////////////////////////////////////////////////////////////////////

namespace mbnrg_A1B3_deg5 {

mbnrg_A1B3_deg5_v1::mbnrg_A1B3_deg5_v1(const std::string mon1) {

    // =====>> BEGIN SECTION CONSTRUCTOR <<=====
    // =>> PASTE RIGHT BELOW THIS LINE <==


    if (mon1 == "nh3") {
        coefficients = std::vector<double> {
             3.049586327032568e+03, // 0
            -4.975789114237548e+02, // 1
             3.559096578815746e+02, // 2
             7.204374553206170e+02, // 3
             1.297225593183740e+00, // 4
            -1.635130527495839e+03, // 5
             1.478440975403314e+03, // 6
            -2.681249327836969e+03, // 7
            -3.015503615803338e+03, // 8
             3.754941107989681e+02, // 9
            -9.147050531471287e+01, // 10
             1.303030805161406e+03, // 11
             5.765365728655918e+02, // 12
             4.104851814189507e+02, // 13
             2.480153790543263e+02, // 14
            -1.618370110673628e+03, // 15
             5.593898200734063e+02, // 16
            -1.206496836628975e+03, // 17
             1.201400612272894e+03, // 18
            -1.072281277796699e+03, // 19
             5.338689733886608e+02, // 20
            -6.156779721425069e+02, // 21
            -2.695954973813786e+02, // 22
            -1.179019988043031e+02, // 23
            -8.098536252157837e+02, // 24
             1.480537474316868e+03, // 25
             1.463348238360443e+03, // 26
             2.980771189084281e+01, // 27
            -2.610126372791364e+02, // 28
             3.739492929579640e+02, // 29
            -9.537913510008663e+02, // 30
            -2.219172207989508e+03, // 31
             2.775485765387266e+02, // 32
            -1.433613120425149e+03, // 33
             8.200276122336041e+01, // 34
            -8.845633817924179e+02, // 35
            -1.041619234054371e+03, // 36
            -1.404867915185791e+03, // 37
             1.685098954474550e+03, // 38
            -2.921567409352183e+02, // 39
             4.440213745401439e+02, // 40
             2.414522068248781e+03, // 41
            -2.997879895518365e+01, // 42
            -6.659661511595555e+02, // 43
            -4.774709220564669e+02, // 44
             1.340002342673051e+03, // 45
             4.209505058432742e+02, // 46
             4.948235508669396e+01, // 47
             1.254436479721805e+03, // 48
             7.852993667553311e+02, // 49
            -7.734407981224908e+02, // 50
             6.628626725724992e+02, // 51
            -2.699736301185195e+02, // 52
             1.206510778268609e+03, // 53
             2.333210822324699e+02, // 54
             6.273273256946678e+01, // 55
             1.724513059096634e+03, // 56
            -2.104760433016936e+03, // 57
             9.734177413792257e+02, // 58
             1.337508154029563e+03, // 59
            -1.241095513781793e+03, // 60
             1.034548995585538e+02, // 61
            -9.023623704796768e+02, // 62
            -5.187286773916443e+02, // 63
            -7.160704444038445e+02, // 64
             2.066756488440617e+02, // 65
             5.786753585787684e+02, // 66
            -7.222988006821809e+02, // 67
             5.477282747358634e+02, // 68
             6.576043217176033e+01, // 69
            -1.488296125694603e+03, // 70
             5.874613489742153e+02, // 71
             4.844115293777543e+01, // 72
            -1.456776672506554e+03, // 73
             6.484040997190893e+02, // 74
             1.816341027891361e+03, // 75
            -8.445677270004668e+02, // 76
            -1.399771881480828e+03, // 77
             1.142372958153422e+03, // 78
             1.628955005731378e+03, // 79
            -1.210100717604578e+02, // 80
            -4.888787264603807e+02, // 81
             2.494911310179261e+02, // 82
             3.680995467795371e+02, // 83
            -2.246039479879227e+03, // 84
            -7.841118186239568e+02, // 85
             2.352031154144906e+02, // 86
            -9.299484927355860e+02, // 87
             1.235933637268831e+03, // 88
             1.934086177740243e+03, // 89
             1.727389285698890e+03, // 90
            -1.333442833591749e+02, // 91
            -3.138176954669480e+02, // 92
            -1.071915196757031e+03, // 93
            -7.789809398609173e+02, // 94
            -6.231166048248058e+01, // 95
             7.537123800462796e+02, // 96
             1.237520154539678e+03, // 97
            -5.462345754516014e+02, // 98
            -5.283610762646591e+02, // 99
            -3.579121189213171e+03, // 100
             2.553626043486894e+03}; // 101
    m_k_x_intra_A_B_1 =  7.477073986362700e-01; // A^(-1))
    m_k_x_intra_B_B_1 =  2.570438745863988e-01; // A^(-1))
    m_ri =  7.000000000000000e+00; // A
    m_ro =  8.000000000000000e+00; // A

    } // end if mon1 == "nh3"
    // =====>> END SECTION CONSTRUCTOR <<=====
}

//----------------------------------------------------------------------------//

double mbnrg_A1B3_deg5_v1::f_switch(const double r, double& g)
{
    if (r > m_ro) {
        g = 0.0;
        return 0.0;
    } else if (r > m_ri) {
        const double t1 = M_PI/(m_ro - m_ri);
        const double x = (r - m_ri)*t1;
        g = - std::sin(x)*t1/2.0;
        return (1.0 + std::cos(x))/2.0;
    } else {
        g = 0.0;
        return 1.0;
    }
}

//----------------------------------------------------------------------------//

 std::vector<double> mbnrg_A1B3_deg5_v1::eval(const double *xyz1, const size_t n) {
    std::vector<double> energies(n,0.0);
    std::vector<double> energies_sw(n,0.0);

    std::vector<double> xyz(12);
    double sw = 0.0;
    polynomial my_poly;

    for (size_t j = 0; j < n; j++) {
        const double *mon1 = xyz1 + 12*j;


        if (false ) {
             continue;
        }

        std::copy(mon1, mon1 + 12, xyz.begin() + 0);


        const double* coords_A_1_a = xyz.data() + 0;

        const double* coords_B_1_a = xyz.data() + 3;

        const double* coords_B_2_a = xyz.data() + 6;

        const double* coords_B_3_a = xyz.data() + 9;


        double w12 =     -9.721486914088159e-02;  //from MBpol
        double w13 =     -9.721486914088159e-02;
        double wcross =   9.859272078406150e-02;

    
        variable vs[6];

        double xs[6];

        xs[0] = vs[0].v_exp(m_k_x_intra_A_B_1, coords_A_1_a, coords_B_1_a);
        xs[1] = vs[1].v_exp(m_k_x_intra_A_B_1, coords_A_1_a, coords_B_2_a);
        xs[2] = vs[2].v_exp(m_k_x_intra_A_B_1, coords_A_1_a, coords_B_3_a);
        xs[3] = vs[3].v_exp(m_k_x_intra_B_B_1, coords_B_1_a, coords_B_2_a);
        xs[4] = vs[4].v_exp(m_k_x_intra_B_B_1, coords_B_1_a, coords_B_3_a);
        xs[5] = vs[5].v_exp(m_k_x_intra_B_B_1, coords_B_2_a, coords_B_3_a);


        sw = 1.0;

        energies[j] = my_poly.eval(xs,coefficients.data());
        energies_sw[j] = energies[j]*sw;

    }
    return energies_sw;

}

//----------------------------------------------------------------------------//

std::vector<double> mbnrg_A1B3_deg5_v1::eval(const double *xyz1, double *grad1 , const size_t n, std::vector<double> *virial) {
    std::vector<double> energies(n,0.0);
    std::vector<double> energies_sw(n,0.0);

    std::vector<double> xyz(12);
    double sw = 0.0;
    polynomial my_poly;

    for (size_t j = 0; j < n; j++) {
        const double *mon1 = xyz1 + 12*j;


        if (false ) {
             continue;
        }

        std::vector<double> gradients(12,0.0);

        std::copy(mon1, mon1 + 12, xyz.begin() + 0);
        const double* coords_A_1_a = xyz.data() + 0;

        const double* coords_B_1_a = xyz.data() + 3;

        const double* coords_B_2_a = xyz.data() + 6;

        const double* coords_B_3_a = xyz.data() + 9;


        double* coords_A_1_a_g = gradients.data() + 0;

        double* coords_B_1_a_g = gradients.data() + 3;

        double* coords_B_2_a_g = gradients.data() + 6;

        double* coords_B_3_a_g = gradients.data() + 9;



        double w12 =     -9.721486914088159e-02;  //from MBpol
        double w13 =     -9.721486914088159e-02;
        double wcross =   9.859272078406150e-02;

    
        variable vs[6];

        double xs[6];


        double gxs[6];

        xs[0] = vs[0].v_exp(m_k_x_intra_A_B_1, coords_A_1_a, coords_B_1_a);
        xs[1] = vs[1].v_exp(m_k_x_intra_A_B_1, coords_A_1_a, coords_B_2_a);
        xs[2] = vs[2].v_exp(m_k_x_intra_A_B_1, coords_A_1_a, coords_B_3_a);
        xs[3] = vs[3].v_exp(m_k_x_intra_B_B_1, coords_B_1_a, coords_B_2_a);
        xs[4] = vs[4].v_exp(m_k_x_intra_B_B_1, coords_B_1_a, coords_B_3_a);
        xs[5] = vs[5].v_exp(m_k_x_intra_B_B_1, coords_B_2_a, coords_B_3_a);


        sw = 1.0;

        energies[j] = my_poly.eval(xs,coefficients.data(),gxs);
        energies_sw[j] = energies[j]*sw;

        for (size_t i = 0; i < 6; i++) {
            gxs[i] *= sw;
        }

        vs[0].grads(gxs[0], coords_A_1_a_g, coords_B_1_a_g, coords_A_1_a, coords_B_1_a);
        vs[1].grads(gxs[1], coords_A_1_a_g, coords_B_2_a_g, coords_A_1_a, coords_B_2_a);
        vs[2].grads(gxs[2], coords_A_1_a_g, coords_B_3_a_g, coords_A_1_a, coords_B_3_a);
        vs[3].grads(gxs[3], coords_B_1_a_g, coords_B_2_a_g, coords_B_1_a, coords_B_2_a);
        vs[4].grads(gxs[4], coords_B_1_a_g, coords_B_3_a_g, coords_B_1_a, coords_B_3_a);
        vs[5].grads(gxs[5], coords_B_2_a_g, coords_B_3_a_g, coords_B_2_a, coords_B_3_a);


        for (size_t i = 0; i < 3; i++) {
            gradients[0 + i] += 0.0 ;
        }


        for (size_t i = 0; i < 12; i++) {
            grad1[i + j*12] += gradients[0 + i];
        }

        
        if (virial != 0) {
        
            (*virial)[0] += -coords_A_1_a[0]*coords_A_1_a_g[0]
                        -coords_B_1_a[0]*coords_B_1_a_g[0]
                        -coords_B_2_a[0]*coords_B_2_a_g[0]
                        -coords_B_3_a[0]*coords_B_3_a_g[0];

        
            (*virial)[1] += -coords_A_1_a[0]*coords_A_1_a_g[1]
                        -coords_B_1_a[0]*coords_B_1_a_g[1]
                        -coords_B_2_a[0]*coords_B_2_a_g[1]
                        -coords_B_3_a[0]*coords_B_3_a_g[1];

        
            (*virial)[2] += -coords_A_1_a[0]*coords_A_1_a_g[2]
                        -coords_B_1_a[0]*coords_B_1_a_g[2]
                        -coords_B_2_a[0]*coords_B_2_a_g[2]
                        -coords_B_3_a[0]*coords_B_3_a_g[2];

        
            (*virial)[4] += -coords_A_1_a[1]*coords_A_1_a_g[1]
                        -coords_B_1_a[1]*coords_B_1_a_g[1]
                        -coords_B_2_a[1]*coords_B_2_a_g[1]
                        -coords_B_3_a[1]*coords_B_3_a_g[1];

        
            (*virial)[5] += -coords_A_1_a[1]*coords_A_1_a_g[2]
                        -coords_B_1_a[1]*coords_B_1_a_g[2]
                        -coords_B_2_a[1]*coords_B_2_a_g[2]
                        -coords_B_3_a[1]*coords_B_3_a_g[2];

        
            (*virial)[8] += -coords_A_1_a[2]*coords_A_1_a_g[2]
                        -coords_B_1_a[2]*coords_B_1_a_g[2]
                        -coords_B_2_a[2]*coords_B_2_a_g[2]
                        -coords_B_3_a[2]*coords_B_3_a_g[2];

            (*virial)[3] = (*virial)[1];
            (*virial)[6] = (*virial)[2];
            (*virial)[7] = (*virial)[5];


        }


    }
    return energies_sw;

}

//----------------------------------------------------------------------------//
} // namespace mbnrg_A1B3_deg5
