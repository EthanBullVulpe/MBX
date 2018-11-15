#include "system.h"

//#define DEBUG
//#define TIMING

#ifdef TIMING
#include <chrono>
#include <iostream>
#endif

/**
 * @file system.cpp
 * @brief System class implementation
 */

////////////////////////////////////////////////////////////////////////////////

namespace bblock {  // Building Block :: System

////////////////////////////////////////////////////////////////////////////////

System::System() { initialized_ = false; }
System::~System() {}

size_t System::GetNumMol() { return nummol; }
size_t System::GetNumMon() { return nummon_; }
size_t System::GetNumSites() { return numsites_; }
size_t System::GetNumRealSites() { return numat_; }

size_t System::GetMonNumAt(size_t n) { return nat_[original2current_order_[n]]; }

size_t System::GetFirstInd(size_t n) {
    // Obtain position in system of monomer in position
    // 'n' in the original order
    size_t current_pos = original2current_order_[n];

    // Get the first index that this monomer had in the input order
    return initial_order_[current_pos].second;
}

std::vector<size_t> System::GetPairList(size_t nmax, double cutoff, size_t istart, size_t iend) {
    // Make sure that nmax is 2 or 3
    // Throw exception otherwise
    if (nmax != 2 and nmax != 3) {
        std::string text = "nmax value of " + std::to_string(nmax) + " is not acceptable. Possible values are 2 or 3.";
        throw CUException(__func__, __FILE__, __LINE__, text);
    }

    // Call the add clusters function to get all the pairs
    AddClusters(nmax, cutoff, 0, monomers_.size());

    // Change the monomer indexes of dimers_ or trimers_
    // to match the input order
    // Select also the pairs that we are interested on

    std::vector<size_t> pair_list;

    if (nmax == 2) {
        // Loop over all the dimers
        for (size_t i = 0; i < dimers_.size(); i += nmax) {
            // Get the initial order of each monomer in the dimer
            size_t mon1 = initial_order_[dimers_[i]].first;
            size_t mon2 = initial_order_[dimers_[i + 1]].first;
            // Add the dimers within the range [istart,iend)
            if ((mon1 >= istart && mon1 < iend) || (mon2 >= istart && mon2 < iend)) {
                pair_list.push_back(mon1);
                pair_list.push_back(mon2);
            }
        }
    } else if (nmax == 3) {
        // Loop over all the trimers
        for (size_t i = 0; i < trimers_.size(); i += nmax) {
            // Get the initial order of each monomer in the trimer
            size_t mon1 = initial_order_[trimers_[i]].first;
            size_t mon2 = initial_order_[trimers_[i + 1]].first;
            size_t mon3 = initial_order_[trimers_[i + 2]].first;
            // Add the dimers within the range [istart,iend)
            if ((mon1 >= istart && mon1 < iend) || (mon2 >= istart && mon2 < iend) || (mon3 >= istart && mon3 < iend)) {
                pair_list.push_back(mon1);
                pair_list.push_back(mon2);
                pair_list.push_back(mon3);
            }
        }
    }

    return pair_list;
}

std::vector<size_t> System::GetMolecule(size_t n) { return molecules_[n]; }

std::vector<std::string> System::GetAtomNames() {
    return systools::ResetOrderN(atoms_, initial_order_, first_index_, sites_);
}

std::vector<std::string> System::GetRealAtomNames() {
    return systools::ResetOrderRealN(atoms_, initial_order_realSites_, numat_, first_index_, nat_);
}

std::vector<double> System::GetXyz() { return systools::ResetOrder3N(xyz_, initial_order_, first_index_, sites_); }

std::vector<double> System::GetRealXyz() {
    return systools::ResetOrderReal3N(xyz_, initial_order_realSites_, numat_, first_index_, nat_);
}

std::vector<double> System::GetGrads() { return systools::ResetOrder3N(grad_, initial_order_, first_index_, sites_); }

std::vector<double> System::GetRealGrads() {
    return systools::ResetOrderReal3N(grad_, initial_order_realSites_, numat_, first_index_, nat_);
}

std::vector<double> System::GetCharges() { return systools::ResetOrderN(chg_, initial_order_, first_index_, sites_); }

std::vector<double> System::GetRealCharges() {
    return systools::ResetOrderRealN(chg_, initial_order_realSites_, numat_, first_index_, nat_);
}

std::vector<double> System::GetPolarizabilities() {
    return systools::ResetOrderN(pol_, initial_order_, first_index_, sites_);
}

std::vector<double> System::GetRealPolarizabilities() {
    return systools::ResetOrderRealN(pol_, initial_order_realSites_, numat_, first_index_, nat_);
}

std::vector<double> System::GetPolarizabilityFactors() {
    return systools::ResetOrderN(polfac_, initial_order_, first_index_, sites_);
}

std::vector<double> System::GetRealPolarizabilityFactors() {
    return systools::ResetOrderRealN(pol_, initial_order_realSites_, numat_, first_index_, nat_);
}

std::string System::GetMonId(size_t n) {
    size_t current_pos = original2current_order_[n];
    return monomers_[current_pos];
}

void System::Set2bCutoff(double cutoff2b) { cutoff2b_ = cutoff2b; }
void System::Set3bCutoff(double cutoff3b) { cutoff3b_ = cutoff3b; }
void System::SetNMaxEval1b(size_t nmax) { maxNMonEval_ = nmax; }
void System::SetNMaxEval2b(size_t nmax) { maxNDimEval_ = nmax; }
void System::SetNMaxEval3b(size_t nmax) { maxNTriEval_ = nmax; }

void System::SetDipoleTol(double tol) { diptol_ = tol; }
void System::SetDipoleMaxIt(size_t maxit) { maxItDip_ = maxit; }
void System::SetDipoleMethod(std::string method) { dipole_method_ = method; }

void System::SetPBC(bool use_pbc, std::vector<double> box = {1000.0, 0.0, 0.0, 0.0, 1000.0, 0.0, 0.0, 0.0, 1000.0}) {
    // Check that the box has 9 components
    // Any other size is not acceptable
    if (box.size() != 9) {
        std::string text = "Box size of " + std::to_string(box.size()) + " is not acceptable.";
        throw CUException(__func__, __FILE__, __LINE__, text);
    }

    // Set the box and the bool to use or not pbc
    use_pbc_ = use_pbc;
    box_ = box;

    // If we use PBC, we need to make sure that the monomer atoms are all
    // close to the central atom (1st atom of each monomer)
    if (use_pbc_) {
        // Fix monomer coordinates
        systools::FixMonomerCoordinates(xyz_, box_, nat_, first_index_);
        // Reset the virtual site positions, charges, pols and polfacs
        SetVSites();
        SetCharges();
        SetPols();
        SetPolfacs();
    }
}

void System::SetXyz(std::vector<double> xyz) {
    // Make sure that the xyz of input has the right size
    if (xyz.size() != 3 * numsites_) {
        std::string text =
            "Sizes " + std::to_string(xyz.size()) + " and " + std::to_string(3 * numsites_) + " don't match.";
        throw CUException(__func__, __FILE__, __LINE__, text);
    }

    // Copy each coordinate in the apropriate place in the internal
    // xyz vector
    for (size_t i = 0; i < sites_.size(); i++) {
        size_t ini = 3 * initial_order_[i].second;
        size_t fin = ini + 3 * sites_[i];
        size_t ini_new = 3 * first_index_[i];
        std::copy(xyz.begin() + ini, xyz.begin() + fin, xyz_.begin() + ini_new);
    }
}

void System::SetRealXyz(std::vector<double> xyz) {
    // Make sure that the xyz of input has the right size
    if (xyz.size() != 3 * numat_) {
        std::string text =
            "Sizes " + std::to_string(xyz.size()) + " and " + std::to_string(3 * numat_) + " don't match.";
        throw CUException(__func__, __FILE__, __LINE__, text);
    }

    // Copy each coordinate in the apropriate place in the internal
    // xyz vector
    for (size_t i = 0; i < nat_.size(); i++) {
        size_t ini = 3 * initial_order_realSites_[i].second;
        size_t fin = ini + 3 * nat_[i];
        size_t ini_new = 3 * first_index_[i];
        std::copy(xyz.begin() + ini, xyz.begin() + fin, xyz_.begin() + ini_new);
    }
}

void System::AddMonomer(std::vector<double> xyz, std::vector<std::string> atoms, std::string id) {
    // If the system has been initialized, adding a monomer is not possible
    if (initialized_) {
        std::string text = std::string("The system has already been initialized. ") +
                           std::string("Adding a new monomer is not possible");
        throw CUException(__func__, __FILE__, __LINE__, text);
    }

    // Adding coordinates
    for (auto i = xyz.begin(); i != xyz.end(); i++) xyz_.push_back(*i);
    // Adding atom names
    for (auto i = atoms.begin(); i != atoms.end(); i++) atoms_.push_back(*i);
    // Adding id
    monomers_.push_back(id);
}

void System::AddMolecule(std::vector<size_t> molec) { molecules_.push_back(molec); }

void System::Initialize() {
    // If we try to reinitialize the system, we will get an exception
    if (initialized_) {
        std::string text =
            std::string("The system has already been initialized. ") + std::string("Reinitialization is not possible");
        throw CUException(__func__, __FILE__, __LINE__, text);
    }

    /////////////
    // CUTOFFS //
    /////////////

    // Setting 2B cutoff
    // Affects the 2B dispersion and 2B polynomials
    // TODO make it effective for electrostatics too
    cutoff2b_ = 100.0;

    // Setting 3B cutoff
    // Affects the 3B polynomials
    cutoff3b_ = 5.0;

    ////////////////////////
    // Evaluation batches //
    ////////////////////////

    // Maximum number in the batch for the 1B evaluation
    maxNMonEval_ = 1024;
    // Maximum number in the batch for the 2B evaluation
    maxNDimEval_ = 1024;
    // Maximum number in the batch for the 3B evaluation
    maxNTriEval_ = 1024;

    //////////////////////////////////
    // Periodic boundary conditions //
    //////////////////////////////////

    // Setting PBC to false by default
    SetPBC(false);

    /////////////////////////////
    // Add monomer information //
    /////////////////////////////

    // Retrieves all the monomer information given the coordinates
    // and monomer id, such as number of sites, and orders the monomers
    AddMonomerInfo();

    // Setting the number of molecules and number of monomers
    nummol = molecules_.size();
    nummon_ = monomers_.size();

    ////////////////////
    // ELECTROSTATICS //
    ////////////////////

    // Setting dipole tolerance to a consrvative value
    // TODO make it be error/dipole, not total error as it is now
    diptol_ = 1E-16;
    // Sets the maximum number of iteartions in the induced dipole
    // calculation. Will assume no convergence if this number is reached
    maxItDip_ = 100;
    // Sets the default method to calculate induced dipoles to ASPC
    dipole_method_ = "aspc";

    // Sets the position of the virtual sites if any
    SetVSites();
    // Sets the charges of the system, even the position dependent ones
    SetCharges();
    // Sets the polarizabilities of the system
    SetPols();
    // Sets the polarizability factors of the system
    SetPolfacs();

    // With the information previously set, we initialize the
    // electrostatics class
    // TODO: Do grads set to true for now. Needs to be fixed
    electrostaticE_.Initialize(chg_, chggrad_, polfac_, pol_, xyz_, monomers_, sites_, first_index_, mon_type_count_,
                               true, diptol_, maxItDip_, dipole_method_);

    // We are done. Setting initialized_ to true
    initialized_ = true;
}

void System::AddMonomerInfo() {
    // If xyz_ is empty, not possible to add any information
    if (xyz_.size() < 3) {
        std::string text = std::string("Cannot initialize the system with ") +
                           std::string("less than 3 coordinates, and current ") + std::string("size of xyz is ") +
                           std::to_string(xyz_.size());
        throw CUException(__func__, __FILE__, __LINE__, text);
    }

    // Copy xyz_ and clear it.
    std::vector<double> xyz = xyz_;
    xyz_.clear();

    // If the size of xyz is not 3* size(atoms_), the system has
    // not been defined properly
    if (xyz.size() != 3 * atoms_.size()) {
        std::string text = std::string("Number of coordinates (") + std::to_string(xyz.size()) +
                           std::string(") is not consistent with number of atoms (") + std::to_string(atoms_.size()) +
                           std::string(")");
        throw CUException(__func__, __FILE__, __LINE__, text);
    }

    // Copy the atoms_ vector and clear it
    std::vector<std::string> atoms = atoms_;
    atoms_.clear();

    // Adding the number of sites of each monomer and storing the first index
    std::vector<size_t> fi_at;
    numsites_ = systools::SetUpMonomers(monomers_, sites_, nat_, fi_at);

    // Calculating the number of atoms
    numat_ = 0;
    for (size_t i = 0; i < nat_.size(); i++) {
        numat_ += nat_[i];
    }

    // Ordering monomers by monomer type, from less to more monomers of each type
    mon_type_count_ = systools::OrderMonomers(monomers_, sites_, nat_, original2current_order_, initial_order_,
                                              initial_order_realSites_);

    // Rearranging coordinates to account for virt sites
    xyz_ = std::vector<double>(3 * numsites_, 0.0);
    atoms_ = std::vector<std::string>(numsites_, "virt");

    size_t count = 0;
    first_index_.clear();
    std::vector<size_t> tmpsites;
    std::vector<size_t> tmpnats;
    // Loop over all the monomers
    for (size_t i = 0; i < monomers_.size(); i++) {
        // For each monomer, copy the coordinates and atoms of the input
        // order xyz and atoms to the new position in the ordered system
        size_t k = initial_order_[i].first;
        // Positions
        std::copy(xyz.begin() + 3 * fi_at[k], xyz.begin() + 3 * (fi_at[k] + nat_[k]), xyz_.begin() + 3 * count);
        // Atom names
        std::copy(atoms.begin() + fi_at[k], atoms.begin() + fi_at[k] + nat_[k], atoms_.begin() + count);
        // Adding the first index of sites
        first_index_.push_back(count);
        // Update count
        count += sites_[k];
        // Updating the sites and nat vectors
        tmpsites.push_back(sites_[k]);
        tmpnats.push_back(nat_[k]);
    }

    // Set sites_ and nat_
    sites_ = tmpsites;
    nat_ = tmpnats;

    // Initialize gradients, charges, pols and polfacs to the right size
    grad_ = std::vector<double>(3 * numsites_, 0.0);
    chg_ = std::vector<double>(numsites_, 0.0);
    pol_ = std::vector<double>(numsites_, 0.0);
    polfac_ = std::vector<double>(numsites_, 0.0);
}

void System::AddClusters(size_t nmax, double cutoff, size_t istart, size_t iend) {
    // istart is the monomer position for which we will look all dimers and
    // trimers that contain it. iend is the last monomer position.
    // This means, if istart is 0 and iend is 2, we will look for all dimers
    // and trimers that contain monomers 0 and/or 1. !!! 2 IS NOT INCLUDED. !!!

    // Make sure that nmax is 2 or 3
    // Throw exception otherwise
    // Commented for now since this functiuon is private and unlikely to
    // be called from the outside
    // if (nmax != 2 and nmax != 3) {
    //    std::string text = "nmax value of " + std::to_string(nmax) + " is not acceptable. Possible values are 2
    //    or 3."; throw CUException(__func__, __FILE__, __LINE__, text);
    //}

    size_t nmon = monomers_.size();
    systools::AddClusters(nmax, cutoff, istart, iend, nmon, use_pbc_, box_, xyz_, first_index_, dimers_, trimers_);
}

std::vector<size_t> System::AddClustersParallel(size_t nmax, double cutoff, size_t istart, size_t iend) {
    // Overloaded function to be compatible with omp
    // Returns dimers if nmax == 2, or trimers if nmax == 3

    // Make sure that nmax is 2 or 3
    // Throw exception otherwise
    // Commented for now since this functiuon is private and unlikely to
    // be called from the outside
    // if (nmax != 2 and nmax != 3) {
    //    std::string text = "nmax value of " + std::to_string(nmax) + " is not acceptable. Possible values are 2
    //    or 3."; throw CUException(__func__, __FILE__, __LINE__, text);
    //}

    size_t nmon = monomers_.size();
    std::vector<size_t> dimers, trimers;
    systools::AddClusters(nmax, cutoff, istart, iend, nmon, use_pbc_, box_, xyz_, first_index_, dimers, trimers);
    if (nmax == 2) return dimers;
    return trimers;
}

double System::Energy(bool do_grads) {
    // Check if system has been initialized
    // If not, throw exception
    if (!initialized_) {
        std::string text =
            std::string("System has not been initialized. ") + std::string("Energy calculation not possible.");
        throw CUException(__func__, __FILE__, __LINE__, text);
    }

    // Reset energy and grads in system to 0
    energy_ = 0.0;
    std::fill(grad_.begin(), grad_.end(), 0.0);

    // Reset the charges, pols, polfacs and new Vsite
    if (use_pbc_) {
        SetPBC(use_pbc_, box_);
    } else {
        SetVSites();
        SetCharges();
        SetPols();
        SetPolfacs();
    }

    // Get the NB contributions

#ifdef TIMING
    auto t1 = std::chrono::high_resolution_clock::now();
#endif

    allMonGood_ = true;
    double e1b = Get1B(do_grads);

    // If monomers are too distorted, skip 2b and 3b calculation
    // Return only
    if (!allMonGood_) {
        return e1b;
    }

#ifdef TIMING
    auto t2 = std::chrono::high_resolution_clock::now();
#endif

    //SBdouble e2b = Get2B(do_grads);
    double e2b = 0.0;

#ifdef TIMING
    auto t3 = std::chrono::high_resolution_clock::now();
#endif

    //SBdouble e3b = Get3B(do_grads);
    double e3b = 0.0;

#ifdef TIMING
    auto t4 = std::chrono::high_resolution_clock::now();
#endif

    // Electrostatic energy
    double Eelec = GetElectrostatics(do_grads);

#ifdef TIMING
    auto t5 = std::chrono::high_resolution_clock::now();
#endif

    // Set up energy with the new value
    energy_ = e1b + e2b + e3b + Eelec;

#ifdef DEBUG
    std::cerr << "1B = " << e1b << std::endl
              << "2B = " << e2b << std::endl
              << "3B = " << e3b << std::endl
              << "Elec = " << Eelec << std::endl;
#endif
#ifdef TIMING
    std::cerr << "System::1b(grad=" << do_grads << ") "
              << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " milliseconds\n";
    std::cerr << "System::2b(grad=" << do_grads << ") "
              << std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t2).count() << " milliseconds\n";
    std::cerr << "System::3b(grad=" << do_grads << ") "
              << std::chrono::duration_cast<std::chrono::milliseconds>(t4 - t3).count() << " milliseconds\n";
    std::cerr << "System::electrostatics(grad=" << do_grads << ") "
              << std::chrono::duration_cast<std::chrono::milliseconds>(t5 - t4).count() << " milliseconds\n";
    std::cerr << "TotalEnergy(grad=" << do_grads << ") "
              << std::chrono::duration_cast<std::chrono::milliseconds>(t5 - t1).count() << " milliseconds\n";
#endif

    return energy_;
}

double System::OneBodyEnergy(bool do_grads) {
    // Check if system has been initialized
    // If not, throw exception
    if (!initialized_) {
        std::string text =
            std::string("System has not been initialized. ") + std::string("1B Energy calculation not possible.");
        throw CUException(__func__, __FILE__, __LINE__, text);
    }

    // Reset energy and gradients
    energy_ = 0.0;
    std::fill(grad_.begin(), grad_.end(), 0.0);

    // Calculate the 1b energy
    energy_ = Get1B(do_grads);

    return energy_;
}

double System::Get1B(bool do_grads) {
    // 1B ENERGY
    // Loop overall the monomers and get their energy
    size_t curr_mon_type = 0;
    size_t current_coord = 0;
    double e1b = 0.0;

    for (size_t k = 0; k < mon_type_count_.size(); k++) {
        // Useful variables
        size_t istart = 0;
        size_t iend = 0;
        while (istart < mon_type_count_[k].second) {
            iend = std::min(istart + maxNMonEval_, mon_type_count_[k].second);
            size_t nmon = iend - istart;
            size_t ncoord = 3 * nat_[curr_mon_type] * nmon;
            std::string mon = mon_type_count_[k].first;

            // XYZ with real sites
            std::vector<double> xyz(ncoord, 0.0);
            std::vector<double> grad2(ncoord, 0.0);

            // Set up real coordinates
            for (size_t i = istart; i < iend; i++) {
                std::copy(xyz_.begin() + current_coord + 3 * i * sites_[curr_mon_type],
                          xyz_.begin() + current_coord + 3 * (i * sites_[curr_mon_type] + nat_[curr_mon_type]),
                          xyz.begin() + 3 * (i - istart) * nat_[curr_mon_type]);
            }

            // Get energy of the chunk as function of monomer
            if (do_grads) {
                e1b += e1b::get_1b_energy(mon, nmon, xyz, grad2, allMonGood_);

                // Reorganize gradients
                for (size_t i = 0; i < nmon; i++) {
                    for (size_t j = 0; j < 3 * nat_[curr_mon_type]; j++) {
                        grad_[current_coord + 3 * (i + istart) * sites_[curr_mon_type] + j] +=
                            grad2[3 * i * nat_[curr_mon_type] + j];
                    }
                }
            } else {
                e1b += e1b::get_1b_energy(mon, nmon, xyz, allMonGood_);
            }

            istart = iend;
        }

        // Update current_coord and curr_mon_type
        current_coord += 3 * mon_type_count_[k].second * sites_[curr_mon_type];
        curr_mon_type += mon_type_count_[k].second;
    }

    return e1b;
}

double System::TwoBodyEnergy(bool do_grads) {
    // Check if system has been initialized
    // If not, throw exception
    if (!initialized_) {
        std::string text =
            std::string("System has not been initialized. ") + std::string("2B Energy calculation not possible.");
        throw CUException(__func__, __FILE__, __LINE__, text);
    }

    // Reset energy and gradients
    energy_ = 0.0;
    std::fill(grad_.begin(), grad_.end(), 0.0);

    // Calculate the 2b energy
    energy_ = Get2B(do_grads);

    return energy_;
}

double System::Get2B(bool do_grads) {
    // No dimers makes the function return 0.

    // 2B ENERGY
    double e2b_t = 0.0;
    double edisp_t = 0.0;

    // Variables needed for OMP
    size_t step = 1;
    int num_threads = 1;

#ifdef _OPENMP
#pragma omp parallel
    {
        // Get the number of threads
        if (omp_get_thread_num() == 0) num_threads = omp_get_num_threads();
    }
    // Define variables to be used later in the condensation of data
    int grad_step = 3 * numsites_ / num_threads;
    step = std::max(size_t(1), std::min(nummon_ / num_threads, step));
#endif  // _OPENMP

    // Variables to be used for both serial and parallel implementation
    size_t first_grad = 0;
    size_t last_grad = 3 * numsites_;
    int rank = 0;

    // Vector pools that allow compatibility between
    // serial and parallel implementation
    std::vector<double> e2b_pool(num_threads, 0.0);
    std::vector<double> edisp_pool(num_threads, 0.0);
    std::vector<std::vector<double>> grad_pool(num_threads, std::vector<double>(3 * numsites_, 0.0));

#ifdef _OPENMP
#pragma omp parallel for schedule(dynamic) private(rank)
#endif  // _OPENMP
    for (size_t istart = 0; istart < nummon_; istart += step) {
#ifdef _OPENMP
        rank = omp_get_thread_num();
#endif  // _OPENMP

        // We loop over all the monomers, and we get all the dimers
        // in which this monomer is involved

        // In this function, istart and iend refer to the first
        // monomer of the dimer. The second one will be found from istart+1
        // to the total number of monomers

        // This iend definition is making sure that we don't go passed
        // the number of monomers
        size_t iend = std::min(istart + step, nummon_);

// Adding corresponding clusters depending on if we are within
// OPENMP or not

// This call will get the dimers that have as first index a monomer
// with index between isatrt and iend (iend not included)
#ifdef _OPENMP
        std::vector<size_t> dimers = AddClustersParallel(2, cutoff2b_, istart, iend);
#else
        AddClusters(2, cutoff2b_, istart, iend);
        std::vector<size_t> dimers = dimers_;
#endif

        // In order to continue, we need at least one dimer
        // If the size of the dimer vector is not at least 2, means
        // that we don't have any dimer
        if (dimers.size() < 2) {
            continue;
        }

        // The way the XYZ are set, they include the virtual site,
        // but we don't need the electrostatic virtual site for teh 2B
        // polynomials. Thus, we need to create a pair of vectors with the right
        // coordinates to pass to polynomials and dispersion
        std::vector<double> xyz1;
        std::vector<double> xyz2;
        std::vector<double> grad1;
        std::vector<double> grad2;

        // Define the two monomer ids that we are currently looking at
        std::string m1 = monomers_[dimers[0]];
        std::string m2 = monomers_[dimers[1]];

        // Initialize the iteration variables
        size_t i = 0;
        size_t nd = 0;
        size_t nd_tot = 0;

        // Loop over all the dimers
        while (2 * nd_tot < dimers.size()) {
            i = (nd_tot + nd) * 2;
            // Check if we are still in the same type of pair
            // We will pas the entire batch in the 2b calculator, but they need
            // to be the same pair (e.g., h2o-h2o, h2o-i, cl-na...)
            if (monomers_[dimers[i]] == m1 && monomers_[dimers[i + 1]] == m2) {
                // Push the coordinates
                for (size_t j = 0; j < 3 * nat_[dimers[i]]; j++) {
                    xyz1.push_back(xyz_[3 * first_index_[dimers[i]] + j]);
                    grad1.push_back(0.0);
                }
                for (size_t j = 0; j < 3 * nat_[dimers[i + 1]]; j++) {
                    xyz2.push_back(xyz_[3 * first_index_[dimers[i + 1]] + j]);
                    grad2.push_back(0.0);
                }
                nd++;
            }

            // If one of the monomers is different as the previous one
            // since dimers are also ordered, means that no more dimers of that
            // type exist. Thus, do calculation, update m? and clear xyz
            if (monomers_[dimers[i]] != m1 || monomers_[dimers[i + 1]] != m2 || i == dimers.size() - 2 ||
                nd == maxNDimEval_) {
                if (nd == 0) {
                    xyz1.clear();
                    xyz2.clear();
                    grad1.clear();
                    grad2.clear();
                    m1 = monomers_[dimers[i]];
                    m2 = monomers_[dimers[i + 1]];
                    continue;
                }

                // Fix dimer positions if pbc
                if (use_pbc_) {
                    systools::GetCloseDimerImage(box_, nat_[dimers[nd_tot * 2]], nat_[dimers[nd_tot * 2 + 1]], nd,
                                                 xyz1.data(), xyz2.data());
                }

                if (do_grads) {
                    // POLYNOMIALS
                    e2b_pool[rank] += e2b::get_2b_energy(m1, m2, nd, xyz1, xyz2, grad1, grad2);

                    // DISPERSION
                    edisp_pool[rank] +=
                        disp::GetDispersion(m1, m2, nd, do_grads, xyz1, xyz2, grad1, grad2, cutoff2b_, use_pbc_);
                    // Update gradients in system
                    size_t i0 = nd_tot * 2;
                    for (size_t k = 0; k < nd; k++) {
                        // Monomer 1
                        for (size_t j = 0; j < 3 * nat_[dimers[i0 + 2 * k]]; j++) {
                            grad_pool[rank][3 * first_index_[dimers[i0 + 2 * k]] + j] +=
                                grad1[k * 3 * nat_[dimers[i0 + 2 * k]] + j];
                        }
                        // Monomer 2
                        for (size_t j = 0; j < 3 * nat_[dimers[i0 + 2 * k + 1]]; j++) {
                            grad_pool[rank][3 * first_index_[dimers[i0 + 2 * k + 1]] + j] +=
                                grad2[k * 3 * nat_[dimers[i0 + 2 * k + 1]] + j];
                        }
                    }
                } else {
                    // POLYNOMIALS
                    e2b_pool[rank] += e2b::get_2b_energy(m1, m2, nd, xyz1, xyz2);
                    // DISPERSION
                    edisp_pool[rank] +=
                        disp::GetDispersion(m1, m2, nd, do_grads, xyz1, xyz2, grad1, grad2, cutoff2b_, use_pbc_);
                }

                // Update loop variables and clear other temporary variable
                nd_tot += nd;
                nd = 0;
                xyz1.clear();
                xyz2.clear();
                grad1.clear();
                grad2.clear();
                m1 = monomers_[dimers[i]];
                m2 = monomers_[dimers[i + 1]];
            }
        }
    }

#ifdef _OPENMP
#pragma omp parallel private(first_grad, last_grad, rank)
    {
        rank = omp_get_thread_num();

        first_grad = 0 + rank * grad_step;

        last_grad = (rank + 1) * grad_step;

        if (rank == num_threads - 1) {
            last_grad = 3 * numsites_;
        }
#pragma omp barrier
#endif

        // Condensate gradients
        for (int i = 0; i < num_threads; i++) {
            for (size_t j = first_grad; j < last_grad; j++) {
                grad_[j] += grad_pool[i][j];
            }
        }

#ifdef _OPENMP
    }  // parallel
#endif

    // Condensate energy
    for (int i = 0; i < num_threads; i++) {
        e2b_t += e2b_pool[i];
        edisp_t += edisp_pool[i];
    }

#ifdef DEBUG
    std::cerr << "disp = " << edisp_t << "    2b = " << e2b_t << std::endl;
#endif

    return e2b_t + edisp_t;
}

double System::ThreeBodyEnergy(bool do_grads) {
    // Check if system has been initialized
    // If not, throw exception
    if (!initialized_) {
        std::string text =
            std::string("System has not been initialized. ") + std::string("3B Energy calculation not possible.");
        throw CUException(__func__, __FILE__, __LINE__, text);
    }

    energy_ = 0.0;
    std::fill(grad_.begin(), grad_.end(), 0.0);

    energy_ = Get3B(do_grads);

    return energy_;
}

double System::Get3B(bool do_grads) {
    // 3B ENERGY
    double e3b_t = 0.0;

    // Variables needed for OMP
    size_t step = 1;
    int num_threads = 1;

#ifdef _OPENMP
#pragma omp parallel
    {
        // Get the number of threads
        if (omp_get_thread_num() == 0) num_threads = omp_get_num_threads();
    }
    // Define variables to be used later in the condensation of data
    int grad_step = 3 * numsites_ / num_threads;
    step = std::max(size_t(1), std::min(nummon_ / num_threads, step));
#endif  // _OPENMP

    // Variables to be used for both serial and parallel implementation
    size_t first_grad = 0;
    size_t last_grad = 3 * numsites_;
    int rank = 0;

    // Vector pools that allow compatibility between
    // serial and parallel implementation
    std::vector<double> e3b_pool(num_threads, 0.0);
    std::vector<std::vector<double>> grad_pool(num_threads, std::vector<double>(3 * numsites_, 0.0));

#ifdef _OPENMP
#pragma omp parallel for schedule(dynamic) private(rank)
#endif  // _OPENMP
    for (size_t istart = 0; istart < nummon_; istart += step) {
#ifdef _OPENMP
        rank = omp_get_thread_num();
#endif

        // We loop over all the monomers, and we get all the trimers
        // in which this monomer is involved

        // In this function, istart and iend refer to the first
        // monomer of the trimer. The second and third ones will
        // be found from istart+1 to the total number of monomers

        // This iend definition is making sure that we don't go passed
        // the number of monomers

        size_t iend = std::min(istart + step, nummon_);

#ifdef _OPENMP
        std::vector<size_t> trimers = AddClustersParallel(3, cutoff3b_, istart, iend);
#else
        AddClusters(3, cutoff3b_, istart, iend);
        std::vector<size_t> trimers = trimers_;
#endif

        // In order to continue, we need at least one dimer
        // If the size of the dimer vector is not at least 2, means
        // that we don't have any dimer
        if (trimers.size() < 3) {
            continue;
        }

        // The way the XYZ are set, they include the virtual site,
        // but we don't need the electrostatic virtual site for teh 2B
        // polynomials. Thus, we need to create a pair of vectors with the right
        // coordinates to pass to polynomials and dispersion
        std::vector<double> coord1;
        std::vector<double> coord2;
        std::vector<double> coord3;
        std::string m1 = monomers_[trimers[0]];
        std::string m2 = monomers_[trimers[1]];
        std::string m3 = monomers_[trimers[2]];

        // Initialize the iteration variables
        size_t i = 0;
        size_t nt = 0;
        size_t nt_tot = 0;

        // Loop over all the trimers
        while (3 * nt_tot < trimers.size()) {
            i = (nt_tot + nt) * 3;

            // Check if we are still in the same type of trimer
            if (monomers_[trimers[i]] == m1 && monomers_[trimers[i + 1]] == m2 && monomers_[trimers[i + 2]] == m3) {
                // Push the coordinates
                for (size_t j = 0; j < 3 * nat_[trimers[i]]; j++) {
                    coord1.push_back(xyz_[3 * first_index_[trimers[i]] + j]);
                }
                for (size_t j = 0; j < 3 * nat_[trimers[i + 1]]; j++) {
                    coord2.push_back(xyz_[3 * first_index_[trimers[i + 1]] + j]);
                }
                for (size_t j = 0; j < 3 * nat_[trimers[i + 2]]; j++) {
                    coord3.push_back(xyz_[3 * first_index_[trimers[i + 2]] + j]);
                }
                nt++;
            }

            // If one of the monomers is different as the previous one
            // since trimers are also ordered, means that no more trimers of that
            // type exist. Thus, do calculation, update m? and clear xyz
            if (monomers_[trimers[i]] != m1 || monomers_[trimers[i + 1]] != m2 || monomers_[trimers[i + 2]] != m3 ||
                i == trimers.size() - 3 || nt == maxNTriEval_) {
                if (nt == 0) {
                    coord1.clear();
                    coord2.clear();
                    coord3.clear();
                    m1 = monomers_[trimers[i]];
                    m2 = monomers_[trimers[i + 1]];
                    m3 = monomers_[trimers[i + 2]];
                    continue;
                }

                // Fix trimer positions if pbc
                if (use_pbc_) {
                    systools::GetCloseTrimerImage(box_, nat_[trimers[nt_tot * 3]], nat_[trimers[nt_tot * 3 + 1]],
                                                  nat_[trimers[nt_tot * 3 + 2]], nt, coord1.data(), coord2.data(),
                                                  coord3.data());
                }

                std::vector<double> xyz1(coord1.size(), 0.0);
                std::vector<double> xyz2(coord2.size(), 0.0);
                std::vector<double> xyz3(coord3.size(), 0.0);
                std::copy(coord1.begin(), coord1.end(), xyz1.begin());
                std::copy(coord2.begin(), coord2.end(), xyz2.begin());
                std::copy(coord3.begin(), coord3.end(), xyz3.begin());

                if (do_grads) {
                    // POLYNOMIALS
                    std::vector<double> grad1(coord1.size(), 0.0);
                    std::vector<double> grad2(coord2.size(), 0.0);
                    std::vector<double> grad3(coord3.size(), 0.0);
                    // POLYNOMIALS
                    e3b_pool[rank] += e3b::get_3b_energy(m1, m2, m3, nt, xyz1, xyz2, xyz3, grad1, grad2, grad3);

                    // Update gradients
                    size_t i0 = nt_tot * 3;
                    for (size_t k = 0; k < nt; k++) {
                        // Monomer 1
                        for (size_t j = 0; j < 3 * nat_[trimers[i0 + 3 * k]]; j++) {
                            grad_pool[rank][3 * first_index_[trimers[i0 + 3 * k]] + j] +=
                                grad1[k * 3 * nat_[trimers[i0 + 3 * k]] + j];
                        }
                        // Monomer 2
                        for (size_t j = 0; j < 3 * nat_[trimers[i0 + 3 * k + 1]]; j++) {
                            grad_pool[rank][3 * first_index_[trimers[i0 + 3 * k + 1]] + j] +=
                                grad2[k * 3 * nat_[trimers[i0 + 3 * k + 1]] + j];
                        }
                        // Monomer 3
                        for (size_t j = 0; j < 3 * nat_[trimers[i0 + 3 * k + 2]]; j++) {
                            grad_pool[rank][3 * first_index_[trimers[i0 + 3 * k + 2]] + j] +=
                                grad3[k * 3 * nat_[trimers[i0 + 3 * k + 2]] + j];
                        }
                    }
                } else {
                    // POLYNOMIALS
                    e3b_pool[rank] += e3b::get_3b_energy(m1, m2, m3, nt, xyz1, xyz2, xyz3);
                }

                // Update iteration variables
                nt_tot += nt;
                nt = 0;
                coord1.clear();
                coord2.clear();
                coord3.clear();
                m1 = monomers_[trimers[i]];
                m2 = monomers_[trimers[i + 1]];
                m3 = monomers_[trimers[i + 2]];
            }
        }
    }

#ifdef _OPENMP
#pragma omp parallel private(first_grad, last_grad, rank)
    {
        rank = omp_get_thread_num();

        first_grad = 0 + rank * grad_step;

        last_grad = (rank + 1) * grad_step;
        if (rank == num_threads - 1) {
            last_grad = 3 * numsites_;
        }
#pragma omp barrier
#endif

        // Condensate gradients
        for (int i = 0; i < num_threads; i++) {
            for (size_t j = first_grad; j < last_grad; j++) {
                grad_[j] += grad_pool[i][j];
            }
        }

#ifdef _OPENMP
    }  // parallel
#endif

    // Condensate energy
    for (int i = 0; i < num_threads; i++) {
        e3b_t += e3b_pool[i];
    }

    return e3b_t;
}

////////////////////////////////////////////////////////////////////////////////

void System::SetCharges() {
    // Set charges for each monomer type
    size_t fi_mon = 0;
    for (size_t k = 0; k < mon_type_count_.size(); k++) {
        std::string mon = mon_type_count_[k].first;
        size_t nmon = mon_type_count_[k].second;
        size_t nsites = sites_[fi_mon];

        systools::SetCharges(xyz_, chg_, mon, nmon, nsites, first_index_[fi_mon], chggrad_);
        fi_mon += nmon;
    }
}

////////////////////////////////////////////////////////////////////////////////

void System::SetPols() {
    // Set polarizabilities for each monomer type
    size_t fi_mon = 0;
    for (size_t k = 0; k < mon_type_count_.size(); k++) {
        std::string mon = mon_type_count_[k].first;
        size_t nmon = mon_type_count_[k].second;
        size_t nsites = sites_[fi_mon];

        systools::SetPol(pol_, mon, nmon, nsites, first_index_[fi_mon]);
        fi_mon += nmon;
    }
}

////////////////////////////////////////////////////////////////////////////////

void System::SetPolfacs() {
    // Set polarizability factors for each monomer type
    size_t fi_mon = 0;
    for (size_t k = 0; k < mon_type_count_.size(); k++) {
        std::string mon = mon_type_count_[k].first;
        size_t nmon = mon_type_count_[k].second;
        size_t nsites = sites_[fi_mon];

        systools::SetPolfac(polfac_, mon, nmon, nsites, first_index_[fi_mon]);
        fi_mon += nmon;
    }
}

////////////////////////////////////////////////////////////////////////////////

void System::SetVSites() {
    // Set virtual sites for each monomer type
    size_t fi_mon = 0;
    for (size_t k = 0; k < mon_type_count_.size(); k++) {
        std::string mon = mon_type_count_[k].first;
        size_t nmon = mon_type_count_[k].second;
        size_t nsites = sites_[fi_mon];

        systools::SetVSites(xyz_, mon, nmon, nsites, first_index_[fi_mon]);
        fi_mon += nmon;
    }
}

////////////////////////////////////////////////////////////////////////////////

double System::Electrostatics(bool do_grads) {
    // Check if system has been initialized
    // If not, throw exception
    if (!initialized_) {
        std::string text = std::string("System has not been initialized. ") +
                           std::string("Electrostatic Energy calculation ") + std::string("not possible.");
        throw CUException(__func__, __FILE__, __LINE__, text);
    }

    energy_ = 0.0;
    std::fill(grad_.begin(), grad_.end(), 0.0);

    energy_ = GetElectrostatics(do_grads);

    return energy_;
}

////////////////////////////////////////////////////////////////////////////////

double System::GetElectrostatics(bool do_grads) {
    electrostaticE_.SetNewParameters(xyz_, chg_, chggrad_, pol_, polfac_, dipole_method_, do_grads, box_, use_pbc_);
    return electrostaticE_.GetElectrostatics(grad_);
}

////////////////////////////////////////////////////////////////////////////////

void System::ResetDipoleHistory() { electrostaticE_.ResetAspcHistory(); }

////////////////////////////////////////////////////////////////////////////////

}  // namespace bblock

////////////////////////////////////////////////////////////////////////////////
