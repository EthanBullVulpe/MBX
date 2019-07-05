#include <cmath>
#include <cassert>

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstring>
#include <stdexcept>
#include <cstdlib>

#include "io_tools/read_nrg.h"
#include "io_tools/write_nrg.h"

#include "bblock/system.h"

#include "perturb.h"
#include "read_xyz.h"

namespace {

static std::vector<bblock::System> systems;

} // namespace

int main(int argc, char** argv)
{

    if (argc < 3) {
      std::cerr << "Usage: " << argv[0] << " <input.nrg> <trajectory.xyz> [<box_side>]"
                << std::endl;
      return 0;
    }

    std::vector<std::vector<double> > xyz;
    std::vector<std::vector<std::string> > atoms;

    try {

        tools::ReadNrg(argv[1], systems);

        ReadXyzTrajectory(argv[2],xyz,atoms);

      
    } catch (const std::exception& e) {
        std::cerr << " ** Error ** : " << e.what() << std::endl;
        return 1;
    }

    size_t nframes = xyz.size();
    std::vector<double> box;
    if (argc > 3) {
        double box_l = atof(argv[3]);
        box = std::vector<double>(9,0.0);
        box[0] = box[4] = box[8] = box_l;
    }

    if (box.size() > 0) {
        systems[0].Set2bCutoff(9.0);
        systems[0].SetEwald(0.25, 2.5, 6);
    }

    std::vector<std::vector<double> > mu_perm(nframes);
    std::vector<std::vector<double> > mu_ind(nframes);
    std::vector<std::vector<double> > mu_perm_at(nframes);
    std::vector<std::vector<double> > mu_ind_at(nframes);
    std::vector<std::vector<double> > chg_der(nframes);
    std::vector<std::vector<double> > mu_perm_kh(nframes); // Kelly addition
    std::vector<std::vector<double> > mu_ind_kh(nframes); // Kelly addition
    std::vector<std::vector<double> > dk_mu(nframes);
//    std::vector<std::vector<double> > dk_mu_ind(nframes);

    std::vector<std::vector<double> > mu_tot(nframes,std::vector<double>(3,0.0));
    std::vector<std::vector<double> > mu_tot_ind(nframes,std::vector<double>(3,0.0));
    std::vector<std::vector<double> > mu_tot_perm(nframes,std::vector<double>(3,0.0));

    size_t nmon = systems[0].GetNumMon();

    for (size_t i = 0; i < nframes; i++) {
        std::cout << "Calculating frame " << i << std::endl;
        systems[0].SetRealXyz(xyz[i]);
        systems[0].SetPBC(box);
        systems[0].Energy(true);
        systems[0].GetMolecularDipoles(mu_perm[i],mu_ind[i]);
        systems[0].GetDipoles(mu_perm_at[i],mu_ind_at[i]);
        chg_der[i] = systems[0].GetChargeDerivatives();
        systems[0].GetTotalDipole(mu_tot_perm[i],mu_tot_ind[i],mu_tot[i]);
        std::vector<double> pols = systems[0].GetPolarizabilities();
        std::vector<double> polfacs = systems[0].GetPolarizabilityFactors();

        for (size_t j = 0; j<pols.size(); j++) {
            std::cout << "pols = " << pols[j] << std::endl;
        }
//        for (size_t j = 0; j<polfacs.size(); j++) {
//            std::cout << "polfacs = " << polfacs[j] << std::endl;
//        }
        for (size_t j = 0; j<mu_ind_at[i].size(); j++) {
            mu_ind_at[i][j] = -mu_ind_at[i][j];
            std::cout << "mu_ind_MBX = " << mu_ind_at[i][j] << std::endl;
            //mu_ind_at_[i][j] = -mu_ind_at_[i][j];
        }

        // Calculate Dipole derivatives
        noneq::Perturbation my_pert;
        my_pert.Initialize(systems[0].GetXyz(), systems[0].GetCharges(), systems[0].GetPolarizabilities(), systems[0].GetPolarizabilityFactors(), box);
//        my_pert.SetDipoles(mu_perm_at[i],mu_ind_at[i]);
        my_pert.SetChargeDerivative(chg_der[i]);
        my_pert.GetMuPerm(mu_perm_kh[i]); // Kelly addition
        my_pert.GetMuInd(mu_ind_kh[i]); // Kelly addition
//        my_pert.CalculateMuPerm(); // Kelly addition
//        my_pert.CalculateMuInd();  // Kelly addition
        my_pert.GetDkMu(dk_mu[i]);  // Kelly playing with this?
       
//        std::vector<double> dk_mu_ind_all(3*pols.size()*3*pols.size());
//        my_pert.GetDkMuIndAll(dk_mu_ind_all);
//        for (size_t j = 0; j<dk_mu_ind_all.size(); j++) {
//            std::cout << "dk_mu_ind_all = " << dk_mu_ind_all[j] << std::endl;
//        }

        // Calculate numerical dipole derivatives
        std::vector<double> tmp1,tmp2,tmp3,dk_mu_ind;
        my_pert.GetDkMuInd(dk_mu_ind);

        std::vector<double> my_xyz = systems[0].GetXyz();
    
        for (size_t k = 0; k < my_xyz.size(); k++) {
            for (size_t u = 0; u < 3; u++) {
                double step = 0.00001;
                double anal_dkmu = dk_mu_ind[3*k + u];
                my_xyz[k] += step;
//                std::cout << "xyz[" << k << "] + step = " << my_xyz[k] << std::endl;
                systems[0].SetXyz(my_xyz);
                std::vector<double> xxx = systems[0].GetXyz();
//                std::cout << "xyz_sys[" << k << "] + step = " << xxx[k] << std::endl;
                systems[0].SetPBC(box);
                systems[0].Energy(false);
                systems[0].GetTotalDipole(tmp1,tmp2,tmp3);
//                double dip_p = tmp2[u];
                double dip_p = -tmp2[u];
//                std::cout << "Total dipole (+s): " << tmp2[u] << std::endl; 
                my_xyz[k] -= 2*step;
//                std::cout << "xyz[" << k << "] - step = " << my_xyz[k] << std::endl;
                systems[0].SetXyz(my_xyz);
                xxx = systems[0].GetXyz();
//                std::cout << "xyz_sys[" << k << "] - step = " << xxx[k] << std::endl;
                systems[0].SetPBC(box);
                systems[0].Energy(false);
                systems[0].GetTotalDipole(tmp1,tmp2,tmp3);
//                std::cout << "Total dipole (-s): " << tmp2[u] << std::endl; 
//                double dip_m = tmp2[u];
                double dip_m = -tmp2[u];
                my_xyz[k] += step;
//                std::cout << "xyz[" << k << "] = " << my_xyz[k] << std::endl;
                double fd_dip = 0.5*(dip_p - dip_m)/step;
                //std::cout << std::scientific << std::setprecision(6)
                std::cout << std::setprecision(12)
                          << std::setw(5) << 3*k + u 
                          << " analytical: " << std::setw(14) << anal_dkmu
                          << " numerical: " << std::setw(14) << fd_dip
                          << " difference: " << std::setw(14) << fabs(anal_dkmu - fd_dip) << std::endl;
           }
        }
    }

    // Calculate correlation
    for (size_t i = 0; i < nframes; i++) {
        std::cout << "Frame " << i << " :: permanent dipoles" << std::endl;
        for (size_t j = 0; j < nmon; j++) {
            std::cout << std::setw(5) << j;
            for (size_t k = 0; k < 3; k++) {
                std::cout << std::setw(20) << std::scientific << std::setprecision(10) << mu_perm[i][3*j + k];
//                std::cout << std::setw(20) << std::scientific << std::setprecision(10) << mu_tot_perm[i][3*j + k];
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;

        std::cout << "Frame " << i << " :: induced dipoles" << std::endl;
        for (size_t j = 0; j < nmon; j++) {
            std::cout << std::setw(5) << j;
            for (size_t k = 0; k < 3; k++) {
                std::cout << std::setw(20) << std::scientific << std::setprecision(10) << mu_ind[i][3*j + k];
//                std::cout << std::setw(20) << std::scientific << std::setprecision(10) << mu_tot_ind[i][3*j + k];
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    for (size_t i = 0; i < nframes; i++) {
        std::cout << "Frame " << i << " :: total dipole" << std::endl;
        for (size_t k = 0; k < 3; k++) {
            std::cout << std::setw(20) << std::scientific << std::setprecision(10) << mu_tot_ind[i][k];
//            std::cout << std::setw(20) << std::scientific << std::setprecision(10) << mu_tot[i][k];
        }
        std::cout << std::endl;
    }
    return 0;
}
