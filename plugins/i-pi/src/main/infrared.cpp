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
    std::vector<std::vector<double> > dk_mu(nframes);

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

        // Calculate Dipole derivatives
        noneq::Perturbation my_pert;
        my_pert.Initialize(systems[0].GetXyz(), systems[0].GetCharges(), systems[0].GetPolarizabilities(), systems[0].GetPolarizabilityFactors(), box);
        my_pert.SetDipoles(mu_perm_at[i],mu_ind_at[i]);
        my_pert.SetChargeDerivative(chg_der[i]);
        my_pert.GetDkMu(dk_mu[i]);

        // Calculate numerical dipole derivatives
        std::vector<double> tmp1,tmp2,tmp3,dk_mu_ind;
        my_pert.GetDkMuInd(dk_mu_ind);

        std::vector<double> my_xyz = systems[0].GetXyz();
    
        for (size_t k = 0; k < my_xyz.size(); k++) {
            for (size_t u = 0; u < 3; u++) {
                double step = 0.00001;
                double anal_dkmu = dk_mu_ind[3*k + u];
                my_xyz[k] += step;
                //std::cout << "xyz[" << k << "] + step = " << my_xyz[k] << std::endl;
                systems[0].SetXyz(my_xyz);
                //std::vector<double> xxx = systems[0].GetXyz();
                //std::cout << "xyz_sys[" << k << "] + step = " << xxx[k] << std::endl;
                systems[0].SetPBC(box);
                systems[0].Energy(false);
                systems[0].GetTotalDipole(tmp1,tmp2,tmp3);
                double dip_p = tmp2[u];
                //std::cout << "Total dipole (+s): " << tmp2[u] << std::endl; 
                my_xyz[k] -= 2*step;
                //std::cout << "xyz[" << k << "] - step = " << my_xyz[k] << std::endl;
                systems[0].SetXyz(my_xyz);
                //xxx = systems[0].GetXyz();
                //std::cout << "xyz_sys[" << k << "] - step = " << xxx[k] << std::endl;
                systems[0].SetPBC(box);
                systems[0].Energy(false);
                systems[0].GetTotalDipole(tmp1,tmp2,tmp3);
                //std::cout << "Total dipole (-s): " << tmp2[u] << std::endl; 
                double dip_m = tmp2[u];
                my_xyz[k] += step;
                //std::cout << "xyz[" << k << "] = " << my_xyz[k] << std::endl;
                double fd_dip = 0.5*(dip_p - dip_m)/step;
                //std::cout << std::scientific << std::setprecision(6)
                std::cout << std::setprecision(6)
                          << std::setw(5) << 3*k + u 
                          << std::setw(15) << anal_dkmu
                          << std::setw(15) << fd_dip
                          << std::setw(15) << fabs(anal_dkmu - fd_dip) << std::endl;
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
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;

        std::cout << "Frame " << i << " :: induced dipoles" << std::endl;
        for (size_t j = 0; j < nmon; j++) {
            std::cout << std::setw(5) << j;
            for (size_t k = 0; k < 3; k++) {
                std::cout << std::setw(20) << std::scientific << std::setprecision(10) << mu_ind[i][3*j + k];
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    for (size_t i = 0; i < nframes; i++) {
        std::cout << "Frame " << i << " :: total dipole" << std::endl;
        for (size_t k = 0; k < 3; k++) {
            std::cout << std::setw(20) << std::scientific << std::setprecision(10) << mu_tot_ind[i][k];
        }
        std::cout << std::endl;
    }
    return 0;
}
