// -*- C++ -*-
//
// Construction of the primary cosmic-ray spectra used by MusAirS.
//
// 1. Purpose
//
// This macro builds the kinetic-energy spectra of the four charged species
// that carry the bulk of the galactic cosmic-ray flux below a few TV,
// protons (p), deuterons (D), helium-4 (4He) and helium-3 (3He), from the
// published AMS-02 measurements, and writes them to primary_cosmic_ray.root,
// from where PrimaryCosmicRayGenerator reads them through
//
//     /MusAirS/PCR/Energy/Spectrum/Histogram primary_cosmic_ray.root AMS_Proton
//
// The generator draws the kinetic energy E_k of one primary and converts it
// into momentum as p = sqrt((2 M + E_k) E_k), M being the mass of the particle
// in Geant4.  The abscissa of every histogram is therefore the total kinetic
// energy of the nucleus in MeV, not the kinetic energy per nucleon, and the
// bin content is dPhi/dE_k in m^-2 s^-1 sr^-1 MeV^-1.  Each bin holds the
// measured flux averaged over the corresponding kinetic-energy interval, so
// the integral of the histogram over a bin is the flux observed by AMS in that
// rigidity bin, per unit area, time and solid angle.  The normalization is
// physical, hence spectra can be combined with weights proportional to their
// integral fluxes.
//
// 2. Input data
//
// The input are the bin-averaged fluxes measured by AMS-02 on the ISS and
// published as functions of the rigidity R = p/(Z e) at the top of the
// detector, that is at 1 AU, solar modulated, without any correction for the
// heliospheric modulation:
//
//   (i)   Z = 1 flux, 1.00-1800 GV, 72 bins: Table 5 of
//         M. Aguilar et al. (AMS Collaboration), Phys. Rept. 894 (2021) 1,
//         doi:10.1016/j.physrep.2020.09.003, seven years of data
//         (May 2011 - May 2018).
//   (ii)  Z = 2 flux, 1.92-3000 GV, 68 bins: Table 7 of the same reference.
//   (iii) D/p flux ratio, 1.92-21.1 GV, 26 bins: Table S101 of
//         M. Aguilar et al. (AMS Collaboration), Phys. Rev. Lett. 132 (2024)
//         261001, doi:10.1103/PhysRevLett.132.261001, May 2011 - April 2021.
//   (iv)  3He/4He flux ratio, 1.92-21.1 GV, 26 bins: Table S102 of the same
//         reference.
//
// The fluxes are reproduced below in the units of the publications, together
// with the rigidity bins of the measurement.  The published uncertainties are
// not used by this script (section 7).  The data files are distributed by the
// collaboration at
//
//     https://ams02.space/publications/202102   (Tables 5 and 7)
//     https://ams02.space/publications/202401   (Tables S100-S102)
//
// The 26 rigidity bins of (iii) and (iv) are identical to the bins of (i) and
// (ii) between 1.92 and 21.1 GV, which is what makes the bin-by-bin ratios
// used below well defined.  The absolute deuteron and helium-isotope fluxes
// (Tables S100 and S102) are not used: the decomposition works with the
// ratios, for the reasons given in section 3.
//
// 3. Why the published fluxes have to be decomposed
//
// The seven-year fluxes of Tables 5 and 7 are not separated by species.  The
// Z = 1 sample contains a few percent of deuterons that are not removed: "at 1 GV it is less than 2%
// and at 20 GV it is 0.6% ... Deuterons were not removed" (Phys. Rev. Lett.
// 114 (2015) 171103, p. 171103-4), and "the sample includes a few percent of
// deuterons" (Phys. Rev. Lett. 127 (2021) 271102, p. 271102-3).  The Z = 2
// sample contains both helium isotopes and is treated as pure 4He, the
// isotopic composition entering only as a 0.3% systematic uncertainty (Phys.
// Rev. Lett. 115 (2015) 211101, p. 211101-5); for that sample "the sum of the
// two fluxes is consistent with" the published helium flux (Phys. Rev. Lett.
// 123 (2019) 181102, p. 181102-4).  Superimposing deuterons or 3He on the
// Z = 1 and Z = 2 fluxes would therefore count them twice.
//
// The decomposition uses the flux ratios measured by AMS in the same rigidity
// bins,
//
//     r_D(R) = Phi_D / Phi_p,      r_3(R) = Phi_3He / Phi_4He,
//
// through
//
//     Phi_p   = Phi_Z1 / (1 + r_D),    Phi_D   = r_D Phi_p,
//     Phi_4He = Phi_Z2 / (1 + r_3),    Phi_3He = r_3 Phi_4He,
//
// so that Phi_p + Phi_D = Phi_Z1 and Phi_4He + Phi_3He = Phi_Z2 at every
// rigidity.  Ratios are preferred to the absolute deuteron and 3He fluxes for
// two reasons.  First, the ratios are time independent above 4.5 GV, the range
// over which the power-law fits of Phys. Rev. Lett. 132 (2024) 261001 are
// performed, so they can be applied to the 2011-2018 Z = 1 and Z = 2 fluxes
// without inheriting the few percent difference between the 2011-2018 and
// 2011-2021 normalizations; below 4.5 GV the ratios do vary with time, so the
// split of the two totals at low rigidity is approximate.  Second, the two
// identities above hold exactly,
// whereas subtracting the absolute 2011-2021 deuteron and 3He fluxes from the
// 2011-2018 totals would misnormalize p and 4He by about 10% of the deuteron
// flux at 2 GV.
//
// 4. Extrapolation above 21.1 GV
//
// AMS measures D and 3He only up to 21.1 GV.  Above that rigidity the ratios
// are continued with the fits reported in Phys. Rev. Lett. 132 (2024) 261001,
//
//     r_D(R) = 0.027 +- 0.001                                   (R > 13 GV),
//     r_3(R) = 0.142 +- 0.003 (R / 4.5 GV)^(-0.289 +- 0.003),
//
// evaluated at the geometric mean of the bin edges.  At 21.1 GV these
// expressions reproduce the measured ratios to better than 0.1% (D/p) and
// 0.6% (3He/4He); the first extrapolated bin of the 3He/4He ratio lies 1.7%
// below the last measured value.  AMS does not measure D or 3He above 21.1 GV,
// and no propagation model is used: the same reference shows that GALPROP and
// USINE do not reproduce the measured D/4He flux ratio above ~2 GeV/n, which is
// precisely why the high-rigidity behavior is taken from the measured ratios
// rather than from a model.  Below 1.92 GV, where deuterons are not measured,
// D/p is frozen at the value of the first measured bin.
//
// 5. Conversion from rigidity to kinetic energy
//
// For a nucleus of charge Z e and mass M,
//
//     E_k(R) = sqrt((Z R)^2 + M^2) - M,       dR/dE_k = E / (Z^2 R),
//
// with E = sqrt((Z R)^2 + M^2).  The published Phi(R) is the flux averaged
// over a rigidity bin [R1, R2], that is the bin contains Phi (R2 - R1)
// particles per m^2 sr s.  The histogram is filled with the flux density
// averaged over the corresponding kinetic-energy interval,
//
//     dPhi/dE_k = Phi (R2 - R1) / (E_k(R2) - E_k(R1)),
//
// which preserves the particle number of every bin exactly.  The Jacobian is
// evaluated as this finite difference rather than at the bin center because
// the bins are wide above ~100 GV.  Inside a bin the histogram is constant,
// which is the standard reading of a bin-averaged measurement.
//
// 6. Output
//
// The four species are listed in order of decreasing flux, followed by the two
// measured totals:
//
//     AMS_Proton     protons    (1.00-1800 GV)
//     AMS_Helium4    4He        (1.92-3000 GV)
//     AMS_Deuteron   deuterons  (1.00-1800 GV, frozen below 1.92 GV and
//                                extrapolated above 21.1 GV)
//     AMS_Helium3    3He        (1.92-3000 GV, ratio-extrapolated above 21.1 GV)
//     AMS_Hydrogen   Z = 1 flux as published (p + D)
//     AMS_Helium     Z = 2 flux as published (4He + 3He)
//
// The two measured totals are kept for traceability and for comparisons with
// the published tables; they must not be used in the same composition as the
// four species spectra, which already add up to them.
//
// 7. Uncertainties
//
// The published uncertainties are not propagated into the histograms.  The
// generator samples central values only, so bin errors would not affect the
// simulation, and the uncertainties of the four species are correlated with
// those of the measured Z = 1 and Z = 2 fluxes in a way that the published
// tables do not quantify.  All six histograms are therefore stored with zero
// bin errors; the uncertainties of the measurements are given in the tables of
// the references cited in section 2.

#include "TFile.h"
#include "TH1D.h"

#include <array>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <iterator>
#include <vector>

namespace {

// Nuclear masses (GeV).
constexpr auto kProtonMass{0.93827208816};
constexpr auto kDeuteronMass{1.87561294257};
constexpr auto kHelium4Mass{3.7273794066};
constexpr auto kHelium3Mass{2.8083916074};

// One row of a published flux table.
struct FluxBin {
    double rigidityMin; // GV
    double rigidityMax; // GV
    double flux;        // m^-2 s^-1 sr^-1 GV^-1
};

// Table 5 of Phys. Rept. 894 (2021) 1: Z = 1 flux, 72 bins, 1.00-1800 GV.
// The Z = 1 sample is p + D; the deuteron admixture is removed in the
// decomposition below.
constexpr FluxBin kZ1Flux[]{
    {1,    1.16, 824.7    },
    {1.16, 1.33, 798.6    },
    {1.33, 1.51, 760.1    },
    {1.51, 1.71, 705.5    },
    {1.71, 1.92, 639.8    },
    {1.92, 2.15, 567.2    },
    {2.15, 2.4,  493.3    },
    {2.4,  2.67, 424.8    },
    {2.67, 2.97, 361.9    },
    {2.97, 3.29, 306.2    },
    {3.29, 3.64, 257.1    },
    {3.64, 4.02, 214.5    },
    {4.02, 4.43, 177.9    },
    {4.43, 4.88, 146.5    },
    {4.88, 5.37, 119.7    },
    {5.37, 5.9,  97.29    },
    {5.9,  6.47, 78.83    },
    {6.47, 7.09, 63.71    },
    {7.09, 7.76, 51.38    },
    {7.76, 8.48, 41.39    },
    {8.48, 9.26, 33.31    },
    {9.26, 10.1, 26.79    },
    {10.1, 11,   21.55    },
    {11,   12,   17.31    },
    {12,   13,   13.95    },
    {13,   14.1, 11.32    },
    {14.1, 15.3, 9.141    },
    {15.3, 16.6, 7.363    },
    {16.6, 18,   5.929    },
    {18,   19.5, 4.774    },
    {19.5, 21.1, 3.85     },
    {21.1, 22.8, 3.11     },
    {22.8, 24.7, 2.506    },
    {24.7, 26.7, 2.013    },
    {26.7, 28.8, 1.628    },
    {28.8, 31.1, 1.316    },
    {31.1, 33.5, 1.066    },
    {33.5, 36.1, 0.8649   },
    {36.1, 38.9, 0.7018   },
    {38.9, 41.9, 0.5702   },
    {41.9, 45.1, 0.4637   },
    {45.1, 48.5, 0.3775   },
    {48.5, 52.2, 0.3074   },
    {52.2, 56.1, 0.2508   },
    {56.1, 60.3, 0.2048   },
    {60.3, 64.8, 0.1674   },
    {64.8, 69.7, 0.1369   },
    {69.7, 74.9, 0.1116   },
    {74.9, 80.5, 0.09124  },
    {80.5, 86.5, 0.07445  },
    {86.5, 93,   0.06067  },
    {93,   100,  0.04951  },
    {100,  108,  0.04009  },
    {108,  116,  0.03253  },
    {116,  125,  0.02647  },
    {125,  135,  0.02137  },
    {135,  147,  0.01702  },
    {147,  160,  0.01341  },
    {160,  175,  0.01052  },
    {175,  192,  0.008166 },
    {192,  211,  0.006269 },
    {211,  233,  0.00479  },
    {233,  259,  0.003627 },
    {259,  291,  0.002674 },
    {291,  330,  0.001914 },
    {330,  379,  0.001343 },
    {379,  441,  0.0008957},
    {441,  525,  0.0005752},
    {525,  643,  0.0003432},
    {643,  822,  0.0001905},
    {822,  1130, 9.068e-05},
    {1130, 1800, 3.26e-05 },
};

// Table 7 of Phys. Rept. 894 (2021) 1: Z = 2 flux, 68 bins, 1.92-3000 GV.
// The Z = 2 sample is 4He + 3He, treated as pure 4He in the publication; the
// 3He admixture is removed in the decomposition below.
constexpr FluxBin kZ2Flux[]{
    {1.92, 2.15, 69.47    },
    {2.15, 2.4,  64.11    },
    {2.4,  2.67, 57.51    },
    {2.67, 2.97, 51.12    },
    {2.97, 3.29, 44.9     },
    {3.29, 3.64, 38.9     },
    {3.64, 4.02, 33.29    },
    {4.02, 4.43, 28.22    },
    {4.43, 4.88, 23.68    },
    {4.88, 5.37, 19.68    },
    {5.37, 5.9,  16.24    },
    {5.9,  6.47, 13.34    },
    {6.47, 7.09, 10.92    },
    {7.09, 7.76, 8.92     },
    {7.76, 8.48, 7.278    },
    {8.48, 9.26, 5.927    },
    {9.26, 10.1, 4.828    },
    {10.1, 11,   3.927    },
    {11,   12,   3.186    },
    {12,   13,   2.597    },
    {13,   14.1, 2.122    },
    {14.1, 15.3, 1.73     },
    {15.3, 16.6, 1.406    },
    {16.6, 18,   1.141    },
    {18,   19.5, 0.9268   },
    {19.5, 21.1, 0.753    },
    {21.1, 22.8, 0.6131   },
    {22.8, 24.7, 0.4986   },
    {24.7, 26.7, 0.4049   },
    {26.7, 28.8, 0.3302   },
    {28.8, 31.1, 0.2694   },
    {31.1, 33.5, 0.2201   },
    {33.5, 36.1, 0.1802   },
    {36.1, 38.9, 0.1475   },
    {38.9, 41.9, 0.1203   },
    {41.9, 45.1, 0.09861  },
    {45.1, 48.5, 0.08092  },
    {48.5, 52.2, 0.06627  },
    {52.2, 56.1, 0.05446  },
    {56.1, 60.3, 0.04481  },
    {60.3, 64.8, 0.03685  },
    {64.8, 69.7, 0.03036  },
    {69.7, 74.9, 0.02496  },
    {74.9, 80.5, 0.02051  },
    {80.5, 86.5, 0.01681  },
    {86.5, 93,   0.01387  },
    {93,   100,  0.0114   },
    {100,  108,  0.009311 },
    {108,  116,  0.007597 },
    {116,  125,  0.006186 },
    {125,  135,  0.005029 },
    {135,  147,  0.004036 },
    {147,  160,  0.003187 },
    {160,  175,  0.002521 },
    {175,  192,  0.001972 },
    {192,  211,  0.001532 },
    {211,  233,  0.00117  },
    {233,  259,  0.0008895},
    {259,  291,  0.0006552},
    {291,  330,  0.0004758},
    {330,  379,  0.0003339},
    {379,  441,  0.0002274},
    {441,  525,  0.0001473},
    {525,  643,  9.095e-05},
    {643,  822,  4.983e-05},
    {822,  1130, 2.412e-05},
    {1130, 1800, 8.831e-06},
    {1800, 3000, 2.518e-06},
};

// One row of a published flux-ratio table.
struct RatioBin {
    double rigidityMin; // GV
    double rigidityMax; // GV
    double ratio;
};

// Table S101 of Phys. Rev. Lett. 132 (2024) 261001: D/p flux ratio, 26 bins,
// 1.92-21.1 GV (column D_to_p of the table).
constexpr RatioBin kDeuteronToProtonRatio[]{
    {1.92, 2.15, 0.01939},
    {2.15, 2.4,  0.02   },
    {2.4,  2.67, 0.02097},
    {2.67, 2.97, 0.02197},
    {2.97, 3.29, 0.02132},
    {3.29, 3.64, 0.02236},
    {3.64, 4.02, 0.02318},
    {4.02, 4.43, 0.02373},
    {4.43, 4.88, 0.02392},
    {4.88, 5.37, 0.02432},
    {5.37, 5.9,  0.02478},
    {5.9,  6.47, 0.02464},
    {6.47, 7.09, 0.0242 },
    {7.09, 7.76, 0.02568},
    {7.76, 8.48, 0.02569},
    {8.48, 9.26, 0.0261 },
    {9.26, 10.1, 0.02569},
    {10.1, 11,   0.02619},
    {11,   12,   0.02635},
    {12,   13,   0.02647},
    {13,   14.1, 0.02662},
    {14.1, 15.3, 0.02676},
    {15.3, 16.6, 0.02688},
    {16.6, 18,   0.02694},
    {18,   19.5, 0.02697},
    {19.5, 21.1, 0.027  },
};

// Table S102 of Phys. Rev. Lett. 132 (2024) 261001: 3He/4He flux ratio,
// 26 bins, 1.92-21.1 GV (column 3He_to_4He of the table).
constexpr RatioBin kHelium3ToHelium4Ratio[]{
    {1.92, 2.15, 0.1511 },
    {2.15, 2.4,  0.1576 },
    {2.4,  2.67, 0.1594 },
    {2.67, 2.97, 0.1577 },
    {2.97, 3.29, 0.1527 },
    {3.29, 3.64, 0.1481 },
    {3.64, 4.02, 0.144  },
    {4.02, 4.43, 0.1407 },
    {4.43, 4.88, 0.1377 },
    {4.88, 5.37, 0.1344 },
    {5.37, 5.9,  0.1299 },
    {5.9,  6.47, 0.1288 },
    {6.47, 7.09, 0.128  },
    {7.09, 7.76, 0.1219 },
    {7.76, 8.48, 0.1195 },
    {8.48, 9.26, 0.1162 },
    {9.26, 10.1, 0.1133 },
    {10.1, 11,   0.1102 },
    {11,   12,   0.1075 },
    {12,   13,   0.1053 },
    {13,   14.1, 0.1033 },
    {14.1, 15.3, 0.1009 },
    {15.3, 16.6, 0.09843},
    {16.6, 18,   0.09613},
    {18,   19.5, 0.09344},
    {19.5, 21.1, 0.09139},
};

// Continuation of the flux ratios above the last measured rigidity (21.1 GV),
// from Phys. Rev. Lett. 132 (2024) 261001: the D/p ratio is constant above
// 13 GV [Eq. (4)] and the 3He/4He ratio is a single power law above 4.5 GV
// [Fig. 4].
constexpr auto kDeuteronToProtonRatioAbove21GV{0.027};
constexpr auto kHelium3ToHelium4Normalization{0.142};
constexpr auto kHelium3ToHelium4ReferenceRigidity{4.5}; // GV
constexpr auto kHelium3ToHelium4Index{-0.289};

// Kinetic energy (MeV) of a nucleus of charge Z (in units of e) and mass M
// (GeV) at rigidity R (GV).  Since R = p/(Z e), p = Z R and
//     E_k = sqrt(p^2 + M^2) - M.
auto KineticEnergy(double rigidity, int charge, double mass) -> double {
    const auto momentum{charge * rigidity};                             // GeV
    return (std::sqrt(momentum * momentum + mass * mass) - mass) * 1e3; // MeV
}

// Row of a ratio table matching the given rigidity bin, or nullptr.
template<std::size_t N>
auto FindRatio(const RatioBin (&table)[N], double rigidityMin, double rigidityMax) -> const RatioBin* {
    for (const auto& bin : table) {
        if (std::abs(bin.rigidityMin - rigidityMin) < 1e-6 and std::abs(bin.rigidityMax - rigidityMax) < 1e-6) {
            return &bin;
        }
    }
    return nullptr;
}

// D/p in the given rigidity bin: measured up to 21.1 GV, continued above, and
// frozen below 1.92 GV where deuterons are not measured.
auto DeuteronToProton(double rigidityMin, double rigidityMax) -> double {
    if (const auto* bin{FindRatio(kDeuteronToProtonRatio, rigidityMin, rigidityMax)}) {
        return bin->ratio;
    }
    if (std::sqrt(rigidityMin * rigidityMax) < kDeuteronToProtonRatio[0].rigidityMin) {
        return kDeuteronToProtonRatio[0].ratio;
    }
    return kDeuteronToProtonRatioAbove21GV;
}

// 3He/4He in the given rigidity bin: measured up to 21.1 GV, continued above.
auto Helium3ToHelium4(double rigidityMin, double rigidityMax) -> double {
    if (const auto* bin{FindRatio(kHelium3ToHelium4Ratio, rigidityMin, rigidityMax)}) {
        return bin->ratio;
    }
    const auto rigidity{std::sqrt(rigidityMin * rigidityMax)};
    return kHelium3ToHelium4Normalization * std::pow(rigidity / kHelium3ToHelium4ReferenceRigidity, kHelium3ToHelium4Index);
}

// Spectrum bin in the variables of the tables: rigidity edges in GV and
// bin-averaged flux in m^-2 s^-1 sr^-1 GV^-1.
struct SpeciesBin {
    double rigidityMin;
    double rigidityMax;
    double flux;
};

// Convert a rigidity spectrum into the histogram read by the generator: the
// abscissa becomes the total kinetic energy of the primary in MeV and the
// content the flux density per MeV, dPhi/dE_k = Phi (R2 - R1)/(E_k2 - E_k1).
auto MakeHistogram(const char* name, const char* title, const std::vector<SpeciesBin>& bins, int charge, double mass) -> TH1* {
    std::vector<double> edges(bins.size() + 1);
    for (std::size_t i = 0; i < bins.size(); ++i) {
        edges[i] = KineticEnergy(bins[i].rigidityMin, charge, mass);
    }
    edges[bins.size()] = KineticEnergy(bins.back().rigidityMax, charge, mass);

    auto* histogram{new TH1D(name, title, static_cast<int>(bins.size()), edges.data())};
    histogram->GetXaxis()->SetTitle("E_{k} [MeV]");
    histogram->GetYaxis()->SetTitle("d#Phi/dE_{k} [m^{-2} s^{-1} sr^{-1} MeV^{-1}]");
    for (std::size_t i = 0; i < bins.size(); ++i) {
        const auto rigidityWidth{bins[i].rigidityMax - bins[i].rigidityMin}; // GV
        const auto energyWidth{edges[i + 1] - edges[i]};                     // MeV
        histogram->SetBinContent(i + 1, bins[i].flux * rigidityWidth / energyWidth);
        // SetBinContent assigns the Poisson error sqrt(content) by default;
        // the published uncertainties are not propagated (see section 7).
        histogram->SetBinError(i + 1, 0);
    }
    return histogram;
}

} // namespace

auto CreatePCRSpectrum() -> int {
    TFile file{"primary_cosmic_ray.root", "NEW"};
    if (file.IsZombie()) {
        std::printf("CreatePCRSpectrum: cannot create primary_cosmic_ray.root (does the file already exist?)\n");
        return 1;
    }

    // Every bin of the ratio tables must be a bin of the corresponding flux
    // table; otherwise the decomposition would silently fall back to the
    // high-rigidity continuation.
    const auto hasBin{[](const auto& table, double rigidityMin, double rigidityMax) {
        for (const auto& bin : table) {
            if (std::abs(bin.rigidityMin - rigidityMin) < 1e-6 and std::abs(bin.rigidityMax - rigidityMax) < 1e-6) {
                return true;
            }
        }
        return false;
    }};
    for (const auto& bin : kDeuteronToProtonRatio) {
        if (not hasBin(kZ1Flux, bin.rigidityMin, bin.rigidityMax)) {
            std::printf("CreatePCRSpectrum: D/p bin %.2f-%.2f GV is not a bin of the Z = 1 table\n", bin.rigidityMin, bin.rigidityMax);
            return 1;
        }
    }
    for (const auto& bin : kHelium3ToHelium4Ratio) {
        if (not hasBin(kZ2Flux, bin.rigidityMin, bin.rigidityMax)) {
            std::printf("CreatePCRSpectrum: 3He/4He bin %.2f-%.2f GV is not a bin of the Z = 2 table\n", bin.rigidityMin, bin.rigidityMax);
            return 1;
        }
    }

    // Decomposition: Phi_p = Phi_Z1/(1 + r_D) and Phi_4He = Phi_Z2/(1 + r_3)
    // with the complements Phi_D = r_D Phi_p and Phi_3He = r_3 Phi_4He.  The
    // AMS_Hydrogen and AMS_Helium histograms keep the published values.
    std::vector<SpeciesBin> hydrogen;
    std::vector<SpeciesBin> helium;
    std::vector<SpeciesBin> proton;
    std::vector<SpeciesBin> deuteron;
    std::vector<SpeciesBin> helium4;
    std::vector<SpeciesBin> helium3;
    hydrogen.reserve(std::size(kZ1Flux));
    proton.reserve(std::size(kZ1Flux));
    deuteron.reserve(std::size(kZ1Flux));
    helium.reserve(std::size(kZ2Flux));
    helium4.reserve(std::size(kZ2Flux));
    helium3.reserve(std::size(kZ2Flux));

    for (const auto& bin : kZ1Flux) {
        const auto ratio{DeuteronToProton(bin.rigidityMin, bin.rigidityMax)};
        const auto protonFlux{bin.flux / (1 + ratio)};
        hydrogen.push_back({bin.rigidityMin, bin.rigidityMax, bin.flux});
        proton.push_back({bin.rigidityMin, bin.rigidityMax, protonFlux});
        deuteron.push_back({bin.rigidityMin, bin.rigidityMax, protonFlux * ratio});
    }
    for (const auto& bin : kZ2Flux) {
        const auto ratio{Helium3ToHelium4(bin.rigidityMin, bin.rigidityMax)};
        const auto helium4Flux{bin.flux / (1 + ratio)};
        helium.push_back({bin.rigidityMin, bin.rigidityMax, bin.flux});
        helium4.push_back({bin.rigidityMin, bin.rigidityMax, helium4Flux});
        helium3.push_back({bin.rigidityMin, bin.rigidityMax, helium4Flux * ratio});
    }

    // The four species are ordered by decreasing flux; the two measured totals
    // come last because they are stored for reference only.  All titles follow
    // the same scheme: "<flux>, AMS-02 (<rigidity range>)".
    const std::array<TH1*, 6> histograms{
        MakeHistogram("AMS_Proton", "Proton flux, AMS-02 (1.00-1800 GV)", proton, 1, kProtonMass),
        MakeHistogram("AMS_Helium4", "4He flux, AMS-02 (1.92-3000 GV)", helium4, 2, kHelium4Mass),
        MakeHistogram("AMS_Deuteron", "Deuteron flux, AMS-02 (1.00-1800 GV)", deuteron, 1, kDeuteronMass),
        MakeHistogram("AMS_Helium3", "3He flux, AMS-02 (1.92-3000 GV)", helium3, 2, kHelium3Mass),
        MakeHistogram("AMS_Hydrogen", "Hydrogen flux (p + D), AMS-02 (1.00-1800 GV)", hydrogen, 1, kProtonMass),
        MakeHistogram("AMS_Helium", "Helium flux (4He + 3He), AMS-02 (1.92-3000 GV)", helium, 2, kHelium4Mass),
    };

    for (auto* histogram : histograms) {
        histogram->Write();
        std::printf("%-14s %3d bins  %10.4g - %10.4g MeV  integral %11.4g m^-2 s^-1 sr^-1\n",
                    histogram->GetName(),
                    histogram->GetNbinsX(),
                    histogram->GetXaxis()->GetXmin(),
                    histogram->GetXaxis()->GetXmax(),
                    histogram->Integral("width"));
    }

    // Continuity of the two ratios at the junction (21.1 GV), where the
    // measured ratios are replaced by their fits.
    const auto& lastDeuteronBin{kDeuteronToProtonRatio[std::size(kDeuteronToProtonRatio) - 1]};
    const auto& lastHeliumBin{kHelium3ToHelium4Ratio[std::size(kHelium3ToHelium4Ratio) - 1]};
    const auto junction{kDeuteronToProtonRatio[std::size(kDeuteronToProtonRatio) - 1].rigidityMax};
    std::printf("\njunction at %.1f GV: D/p measured %.4f, fit %.4f;  3He/4He measured %.4f, fit %.4f\n",
                junction,
                lastDeuteronBin.ratio,
                kDeuteronToProtonRatioAbove21GV,
                lastHeliumBin.ratio,
                kHelium3ToHelium4Normalization * std::pow(junction / kHelium3ToHelium4ReferenceRigidity, kHelium3ToHelium4Index));

    file.Close();
    return 0;
}
