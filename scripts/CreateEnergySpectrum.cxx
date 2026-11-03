// Builds the primary cosmic-ray spectra used by the examples.
//
// The fluxes are the seven-year AMS-02 measurements (May 2011 to May 2018) of
// M. Aguilar et al. [AMS], Phys. Rept. 894 (2021) 1: Table 5 for protons and
// Table 7 for helium, which give the flux in m^-2 sr^-1 s^-1 GV^-1 averaged
// over a rigidity bin.  The generator samples the kinetic energy of the
// primary, so the spectra are written as histograms whose bins are the measured
// rigidity bins converted to kinetic energy in MeV, and whose content is the
// measured flux divided by the bin width, dPhi/dE in m^-2 sr^-1 s^-1 MeV^-1.
// The integral of a bin is therefore the measured number of particles in that
// bin, and the range of a histogram is exactly the measured range.  The two
// spectra are absolute, so they can be weighted by their integral fluxes when a
// mixed composition is wanted.

#include "TFile.h"
#include "TH1.h"

#include <cmath>
#include <cstddef>

struct RigidityBin {
    double lo;   // GV
    double hi;   // GV
    double flux; // m^-2 sr^-1 s^-1 GV^-1, average over the bin
};

// clang-format off
constexpr RigidityBin ProtonBins[]{
    {1.00, 1.16, 824.7},
    {1.16, 1.33, 798.6},
    {1.33, 1.51, 760.1},
    {1.51, 1.71, 705.5},
    {1.71, 1.92, 639.8},
    {1.92, 2.15, 567.2},
    {2.15, 2.40, 493.3},
    {2.40, 2.67, 424.8},
    {2.67, 2.97, 361.9},
    {2.97, 3.29, 306.2},
    {3.29, 3.64, 257.1},
    {3.64, 4.02, 214.5},
    {4.02, 4.43, 177.9},
    {4.43, 4.88, 146.5},
    {4.88, 5.37, 119.7},
    {5.37, 5.90, 97.29},
    {5.90, 6.47, 78.83},
    {6.47, 7.09, 63.71},
    {7.09, 7.76, 51.38},
    {7.76, 8.48, 41.39},
    {8.48, 9.26, 33.31},
    {9.26, 10.1, 26.79},
    {10.1, 11, 21.55},
    {11, 12, 17.31},
    {12, 13, 13.95},
    {13, 14.1, 11.32},
    {14.1, 15.3, 9.141},
    {15.3, 16.6, 7.363},
    {16.6, 18, 5.929},
    {18, 19.5, 4.774},
    {19.5, 21.1, 3.85},
    {21.1, 22.8, 3.11},
    {22.8, 24.7, 2.506},
    {24.7, 26.7, 2.013},
    {26.7, 28.8, 1.628},
    {28.8, 31.1, 1.316},
    {31.1, 33.5, 1.066},
    {33.5, 36.1, 0.8649},
    {36.1, 38.9, 0.7018},
    {38.9, 41.9, 0.5702},
    {41.9, 45.1, 0.4637},
    {45.1, 48.5, 0.3775},
    {48.5, 52.2, 0.3074},
    {52.2, 56.1, 0.2508},
    {56.1, 60.3, 0.2048},
    {60.3, 64.8, 0.1674},
    {64.8, 69.7, 0.1369},
    {69.7, 74.9, 0.1116},
    {74.9, 80.5, 0.09124},
    {80.5, 86.5, 0.07445},
    {86.5, 93, 0.06067},
    {93, 100, 0.04951},
    {100, 108, 0.04009},
    {108, 116, 0.03253},
    {116, 125, 0.02647},
    {125, 135, 0.02137},
    {135, 147, 0.01702},
    {147, 160, 0.01341},
    {160, 175, 0.01052},
    {175, 192, 0.008166},
    {192, 211, 0.006269},
    {211, 233, 0.00479},
    {233, 259, 0.003627},
    {259, 291, 0.002674},
    {291, 330, 0.001914},
    {330, 379, 0.001343},
    {379, 441, 0.0008957},
    {441, 525, 0.0005752},
    {525, 643, 0.0003432},
    {643, 822, 0.0001905},
    {822, 1130, 9.068e-05},
    {1130, 1800, 3.26e-05},
};

constexpr RigidityBin HeliumBins[]{
    {1.92, 2.15, 69.47},
    {2.15, 2.40, 64.11},
    {2.40, 2.67, 57.51},
    {2.67, 2.97, 51.12},
    {2.97, 3.29, 44.9},
    {3.29, 3.64, 38.9},
    {3.64, 4.02, 33.29},
    {4.02, 4.43, 28.22},
    {4.43, 4.88, 23.68},
    {4.88, 5.37, 19.68},
    {5.37, 5.90, 16.24},
    {5.90, 6.47, 13.34},
    {6.47, 7.09, 10.92},
    {7.09, 7.76, 8.92},
    {7.76, 8.48, 7.278},
    {8.48, 9.26, 5.927},
    {9.26, 10.1, 4.828},
    {10.1, 11, 3.927},
    {11, 12, 3.186},
    {12, 13, 2.597},
    {13, 14.1, 2.122},
    {14.1, 15.3, 1.73},
    {15.3, 16.6, 1.406},
    {16.6, 18, 1.141},
    {18, 19.5, 0.9268},
    {19.5, 21.1, 0.753},
    {21.1, 22.8, 0.6131},
    {22.8, 24.7, 0.4986},
    {24.7, 26.7, 0.4049},
    {26.7, 28.8, 0.3302},
    {28.8, 31.1, 0.2694},
    {31.1, 33.5, 0.2201},
    {33.5, 36.1, 0.1802},
    {36.1, 38.9, 0.1475},
    {38.9, 41.9, 0.1203},
    {41.9, 45.1, 0.09861},
    {45.1, 48.5, 0.08092},
    {48.5, 52.2, 0.06627},
    {52.2, 56.1, 0.05446},
    {56.1, 60.3, 0.04481},
    {60.3, 64.8, 0.03685},
    {64.8, 69.7, 0.03036},
    {69.7, 74.9, 0.02496},
    {74.9, 80.5, 0.02051},
    {80.5, 86.5, 0.01681},
    {86.5, 93, 0.01387},
    {93, 100, 0.0114},
    {100, 108, 0.009311},
    {108, 116, 0.007597},
    {116, 125, 0.006186},
    {125, 135, 0.005029},
    {135, 147, 0.004036},
    {147, 160, 0.003187},
    {160, 175, 0.002521},
    {175, 192, 0.001972},
    {192, 211, 0.001532},
    {211, 233, 0.00117},
    {233, 259, 0.0008895},
    {259, 291, 0.0006552},
    {291, 330, 0.0004758},
    {330, 379, 0.0003339},
    {379, 441, 0.0002274},
    {441, 525, 0.0001473},
    {525, 643, 9.095e-05},
    {643, 822, 4.983e-05},
    {822, 1130, 2.412e-05},
    {1130, 1800, 8.831e-06},
    {1800, 3000, 2.518e-06},
};

// clang-format on

auto KineticEnergy(double rigidity, double charge, double mass) -> double {
    const auto p{charge * rigidity};
    return (std::sqrt(p * p + mass * mass) - mass) * 1e3; // MeV
}

template<std::size_t N>
auto MakeHistogram(const char* name, const char* title, const RigidityBin (&bins)[N], double charge, double mass) -> TH1* {
    double edges[N + 1];
    for (std::size_t i = 0; i < N; ++i) {
        edges[i] = KineticEnergy(bins[i].lo, charge, mass);
    }
    edges[N] = KineticEnergy(bins[N - 1].hi, charge, mass);

    auto h{new TH1D(name, title, static_cast<int>(N), edges)};
    for (std::size_t i = 0; i < N; ++i) {
        const auto dR{bins[i].hi - bins[i].lo};          // GV
        const auto dE{edges[i + 1] - edges[i]};          // MeV
        h->SetBinContent(i + 1, bins[i].flux * dR / dE); // m^-2 sr^-1 s^-1 MeV^-1
        h->SetBinError(i + 1, 0);
    }
    return h;
}

auto CreateEnergySpectrum() -> int {
    if (not TFile::Open("primary_cosmic_ray.root", "NEW")) {
        return 1;
    }
    MakeHistogram("AMS_Proton", "Proton flux (AMS-02, 7 years)", ProtonBins, 1, 0.93827208816)->Write();
    MakeHistogram("AMS_Helium", "Helium flux (AMS-02, 7 years)", HeliumBins, 2, 3.727379)->Write();
    return 0;
}
