# MusAirS

Simulation of cosmic ray air shower.

![MusAirS](document/picture/proton_shower.png "A proton shower.")

## Content

- [MusAirS](#musairs)
  - [Content](#content)
  - [Quick start](#quick-start)
    - [Dependencies](#dependencies)
    - [How to build (GNU/Linux)](#how-to-build-gnulinux)
    - [How to run](#how-to-run)
      - [Interactive session](#interactive-session)
      - [Batch session](#batch-session)
  - [Coordinate convention](#coordinate-convention)
    - [Coordinate system](#coordinate-system)
    - [Angular Coordinate (in result)](#angular-coordinate-in-result)
  - [Choose an alternative physics list](#choose-an-alternative-physics-list)
    - [Physics configuration](#physics-configuration)
  - [Run macro](#run-macro)
  - [Geometry and field](#geometry-and-field)
    - [Atmosphere modeling](#atmosphere-modeling)
      - [States](#states)
      - [Components](#components)
      - [Slicing](#slicing)
    - [Geomagnetic field](#geomagnetic-field)
    - [Elevating the ground](#elevating-the-ground)
  - [Primary particles](#primary-particles)
    - [Primary cosmic ray generator](#primary-cosmic-ray-generator)
      - [Direction sampling](#direction-sampling)
      - [Units](#units)
  - [Save primary vertex data](#save-primary-vertex-data)
  - [Who ordered neutrino?](#who-ordered-neutrino)
  - [Interpret the result](#interpret-the-result)
  - [Bug tracking](#bug-tracking)
  - [Contributing](#contributing)


## Quick start

### Dependencies

Required, built-in if not found (network or pre-downloaded source is required):

1. [Mustard (A modern, distributed offline software framework for HEP experiments)](https://github.com/zhao-shihan/Mustard) (≥ 0.26.315, built-in if not found) and its [dependencies](https://github.com/zhao-shihan/Mustard?tab=readme-ov-file#dependencies).

### How to build (GNU/Linux)

In source directory, execute line-by-line:
```sh
mkdir build
cd build
cmake ..
make -j8
```

### How to run

In build directory, look help:
```sh
./MusAirS --help
```

#### Interactive session

Run interactively:
```sh
./MusAirS
```
or
```sh
./MusAirS -i <macro_for_initialzation>
```

#### Batch session

Run in batch, sequentially:
```sh
./MusAirS <run_macro>
```
Parallel computing:
```sh
mpirun -n <nproc> ./MusAirS <run_macro>
```

## Coordinate convention

### Coordinate system

| **Coordinate** | **Direction** |
| -------------- | ------------- |
| +X             | East          |
| -X             | West          |
| +Y             | North         |
| -Y             | South         |
| +Z             | Sky           |
| -Z             | Ground        |

### Angular Coordinate (in result)

| **Coordinate** | **Expression**                     | **Direction**                                                           |
| -------------- | ---------------------------------- | ----------------------------------------------------------------------- |
| phi            | $\mathrm{atan2}(p_x, p_y)+\pi$     | $0$: North, $\frac{\pi}{2}$: East, $\pi$: South, $\frac{3\pi}{2}$: West |
| Zenith         | $1+\frac{p_z}{p}\ (=1-\cos\theta)$ | $0$: Sky, $1$: Horizon                                                  |

## Choose an alternative physics list

MusAirS uses FTFP_BERT as the default physics list. However, you may change it by option `--physics-list`. For example,
```sh
MusAirS --physics-list QGSP_BERT
```
It accepts a [Geant4 reference physics list](https://geant4-userdoc.web.cern.ch/UsersGuides/PhysicsListGuide/html/reference_PL/index.html) name as its argument. Note that electromagnetic physics is forced to `EM Opt1`, so EM physics suffix won't work.

### Physics configuration

The rest of the physics configuration is set by the program itself, so that every run uses it even if the macro says nothing about it:

- The spin-dependent decay process of Geant4 is registered for pions by `G4SpinDecayPhysics` and extended to kaons and $K^0_L$ by `PionKaonDecayMakeSpinPhysics` of [Mustard](https://github.com/zhaoshihan/Mustard).
- The default production cuts are **1 km** for gamma, e- and e+ and **3 m** for the proton. The proton cut is the kinetic-energy threshold of the recoiling nucleus in an elastic scatter, not a threshold for producing protons: in Geant4's linear range-to-energy convention 3 m corresponds to 300 MeV, and inelastic final states are not filtered.
- The neutron tracking cut that comes with the reference lists (10 us, measured from the primary) is replaced by **10 ms** and a **300 MeV** kinetic-energy limit. The default cut would stop every neutron at its first step in an air shower, because a primary that starts at 100 km reaches its first interaction about 280 us later. 10 ms is longer than the longest transit through the world volume, and 300 MeV is at the threshold for pion production on a nucleus, below which a neutron cannot feed the muon component.
- The hadronic and electromagnetic tables are extended to **1 PeV**.
- The optional conversion processes of `G4EmExtraPhysics` (gamma to muon pair, positron to muon pair, muon to muon pair, positron to hadrons) are left at their default disabled state; the electromagnetic production of muons proceeds through the photonuclear processes, which are always on.

The cuts can still be overridden in the macro. For example, the `lite` and `wo_pipm` example macros lower the electromagnetic cuts to 1 pc and kill the electromagnetic shower, which is appropriate when only the muons from pion and kaon decay are of interest:
```sh
/run/setCutForAGivenParticle gamma 1 pc
/run/setCutForAGivenParticle e- 1 pc
/run/setCutForAGivenParticle e+ 1 pc
```
A range cut of 1 pc saturates the range-to-energy conversion at 9.55 GeV; it is a way of switching off the electromagnetic shower, not a physical range of one parsec.

## Run macro

We provided a few example Geant4 macros in `scripts/` directory. Based on them you can write your own.

For more, refer to the [Geant4 Book For Application Developers](https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/html/index.html). [General Particle Source](https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/html/GettingStarted/generalParticleSource.html) would especially be helpful.

## Geometry and field

There are some adjustable geometric and field parameters. See [the geometry description file](scripts/MusAirS.yaml) (a YAML file) for detail. The unit system is the Geant4 unit system (MeV, mm, ns).

You can adjust these parameters by importing the YAML file through macro commands. For example,
```sh
/Mustard/Detector/Description/Import another.yaml # or,
# Ixport = Export, import, and then export. This is useful for debugging.
/Mustard/Detector/Description/Ixport another.yaml
```

### Atmosphere modeling

#### States

The atmosphere is modeled by [The International Standard Atmosphere (ISA)](https://en.wikipedia.org/wiki/International_Standard_Atmosphere). It is a static atmospheric model of how the pressure, temperature and density of the Earth's atmosphere change over a range of altitudes up to 85 km above sea level. The altitude range should be enough for studying air shower up to 10 TeV.

#### Components

The components of atmosphere within the available range of ISA are almost the same as on the ground. Therefore, the air material is built based on `G4_AIR` (dry air) with different pressure, temperature and density.

#### Slicing

Geant4 does not allow basic material properties (density, pressures, etc.) to change continuously. So the atmosphere should be modeled by many slices of air with different pressures. There are 2 slicing scheme: by altitude or by pressure. The former slices the atmosphere with adjacent layers have the same thickness, while the latter slices with adjacent layers have (almost) the same pressure difference. The atmosphere top (max altitude), slicing scheme and the number of slices can be changed in the geometry description YAML file.

### Geomagnetic field

MusAirS allow user to specify a uniform magnetic field. The magnetic field vector can be set in the YAML file. It uses Geant4 unit system ($1~\mathrm{T} = 0.001~\mathrm{MeV}\cdot\mathrm{ns}/\mathrm{mm}^2 = 0.001$).

### Elevating the ground

By default, the detection plane (the ground") is at the sea level (altitude = 0 m). This can be changed in the geometry description YAML file. This allows to simulate the shower measured in a higher altitude (e.g. on a plateau). Note that the ground level should be higher than 0 m and lower than the atmosphere top.

## Primary particles

We use Geant4 [General Particle Source (GPS)](https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/html/GettingStarted/generalParticleSource.html) for primary particle generation. For example, the following macro lines configures GPS to generate protons isotopically from 100 km above the sea level, with $E^{-2.7}$ energy spectrum ranges from 3 GeV to 10 TeV.
```sh
/gps/particle         proton
/gps/position         0 0 100 km
/gps/ang/type         iso
/gps/ene/type         Pow
/gps/ene/biasAlpha    -2.7
/gps/ene/min          3 GeV
/gps/ene/max          10 TeV
```
Detailed usage can be found in the [Geant4 Book For Application Developers](https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/html/GettingStarted/generalParticleSource.html).

### Primary cosmic ray generator

For air showers, switch to the built-in primary cosmic ray generator:

```sh
/MusAirS/Action/SwitchToPCR
```

It accepts any particle of the Geant4 particle table, and takes the kinetic energy spectrum either from a formula, from a histogram in a ROOT file, or from a graph in a ROOT file:

```sh
/MusAirS/PCR/Particle                    proton
/MusAirS/PCR/Energy/Spectrum/Histogram   primary_cosmic_ray.root AMS_Proton
/MusAirS/PCR/Energy/Min                  3 GeV
/MusAirS/PCR/Energy/Max                  10 TeV
```

Primary vertices are placed on the top of the atmosphere at the point where the initial primary direction points at the ground center `(0, 0, GroundAltitude)`.  By default, the zenith angle $\theta$ is sampled with probability density proportional to $\cos\theta$ over the range of angles accepted by the world volume, and the azimuth angle is uniform.  This is the angular distribution of an isotropic cosmic-ray flux crossing the horizontal ground: the $\cos\theta$ factor is the projected area of the ground, and the finite acceptance of the world volume is included in the direction weight.  The direction sampling can be biased; see [Direction sampling](#direction-sampling).  Only the initial direction is aimed; a charged primary is subsequently deflected by the geomagnetic field during tracking.

The example file `primary_cosmic_ray.root` is built by `scripts/CreatePCRSpectrum.cxx`, which reads the published AMS-02 tables and writes six histograms:

| Histogram | Species | Rigidity range |
| --- | --- | --- |
| `AMS_Proton` | protons | 1.00-1800 GV |
| `AMS_Helium4` | ⁴He | 1.92-3000 GV |
| `AMS_Deuteron` | deuterons | 1.00-1800 GV |
| `AMS_Helium3` | ³He | 1.92-3000 GV |
| `AMS_Hydrogen` | Z = 1 flux as published (p + D) | 1.00-1800 GV |
| `AMS_Helium` | Z = 2 flux as published (⁴He + ³He) | 1.92-3000 GV |

The four species are listed in order of decreasing flux; the two measured totals follow them and are stored for reference only.

The two measured fluxes are the seven-year AMS-02 measurements [Phys. Rept. **894** (2021) 1]; the four species are obtained from them with the measured D/p and ³He/⁴He flux ratios of [Phys. Rev. Lett. **132** (2024) 261001], so that `AMS_Proton` + `AMS_Deuteron` and `AMS_Helium4` + `AMS_Helium3` reproduce the measured Z = 1 and Z = 2 fluxes bin by bin.  Above 21.1 GV, where AMS does not measure deuterons or ³He, the two ratios are continued with the fits given in that Letter; the header of the script documents the construction and the reasons for it.  `AMS_Hydrogen` and `AMS_Helium` are kept for comparisons with the published tables and must not be used in the same composition as the four species spectra, which already add up to them.

The bins of every histogram are the rigidity bins of the measurement converted to the total kinetic energy of the primary in MeV, which for nuclei is the energy of the whole nucleus and not the energy per nucleon, and the content is the flux density per MeV, so that the integral of a bin is the measured flux in that bin, per unit area, time and solid angle, and the integral of a histogram is a physical flux in m⁻² s⁻¹ sr⁻¹.  The histograms carry no bin errors: the published uncertainties are not propagated, because the generator samples central values only and because the correlations between the fluxes and the ratios are not published; the uncertainties are given in the tables of the two references.  The four species have different masses and therefore slightly different abscissae: they are combined with weights proportional to their integral fluxes, not by adding bin contents.

`scripts/run_AMS_lite.mac` (and its `full` and `wo_pipm` variants) runs the four species in a single session, one Geant4 run per species, and writes all of them to one file per process: `G4Run0` protons, `G4Run1` ⁴He, `G4Run2` deuterons, `G4Run3` ³He.  The minimum kinetic energies are chosen so that every species starts at the same 13.3 GV vertical geomagnetic cutoff at Zhaoqing, $E_{k,\min}=\sqrt{(ZR_c)^2+M^2}-M$, i.e. 12.40 GeV for protons, 23.13 GeV for ⁴He, 11.56 GeV for deuterons and 23.94 GeV for ³He; they differ because the kinetic energy is not proportional to the rigidity for different masses.  The particle, the spectrum and the minimum energy can be changed between `beamOn` commands, so a composition is obtained by running the macro once and weighting the runs afterwards.

The spectrum is sampled with one of four modes, selected by

```sh
/MusAirS/PCR/Energy/Sampling/Mode  LogUniformBiased
```

With $f(E)$ the physical spectrum and $g(E)$ the distribution that is sampled, the energy weight of an event is $w_E = f(E)/g(E)$.  The direction sampling contributes an additional weight $w_\Omega$ for every mode, and the total event weight stored in the output is $w_E w_\Omega$; in the default `Normal` direction mode $w_\Omega = 1-u_{\min}^2$.  The weights are stored in the output so that every distribution can be reweighted afterwards.

| Mode               | $g(E)$ | $w_E(E)$                    | Use                                                               |
| ------------------ | ------ | --------------------------- | ----------------------------------------------------------------- |
| `Normal`           | $f$    | $1$                         | reference; best for the lowest decade, cheapest per event         |
| `UniformBiased`    | const  | $f(E)(E_{\max}-E_{\min})$   | exploring a wide range quickly                                    |
| `LogUniformBiased` | $1/E$  | $fE\ln(E_{\max}/E_{\min})$  | recommended for production: equal statistics per decade of energy |
| `CustomBiased`     | user   | $f/g$                       | shaping the sample toward the observable of interest              |

The `Normal`, `UniformBiased`, and `LogUniformBiased` modes use normalized sampling distributions, so their weights are exact ratios.  `CustomBiased` uses the user-supplied distribution as is for its weight, so its weight is defined up to an overall constant; the sampling distribution itself is normalized internally.  Custom energy histograms and graphs are interpreted as densities with respect to $E$; counts in non-uniform bins must be divided by the bin width, and the histogram or graph must cover the sampling range.  The `Normal` and `CustomBiased` energy samplers use the CDF cached internally by `TF1`; `UniformBiased` and `LogUniformBiased` sample analytically.

#### Direction sampling

The zenith angle of the primary is sampled over $\cos\theta \in [\cos\theta_{\max}, 1]$, where $\theta_{\max}$ is set by the world width and the atmosphere/ground geometry (the inscribed cylinder of the world box is used, a conservative truncation of the box corners), and the azimuth angle is uniform.  The mode is selected by

```sh
/MusAirS/PCR/Direction/Sampling/Mode  Normal
```

The `ZenithPowerBiased` exponent, the `CustomBiased` distribution, and its sampling resolution are set by

```sh
/MusAirS/PCR/Direction/Sampling/Power             2
/MusAirS/PCR/Direction/Sampling/Custom/Formula    x^2
/MusAirS/PCR/Direction/Sampling/Custom/Histogram  direction.root h
/MusAirS/PCR/Direction/Sampling/Custom/Graph      direction.root g
/MusAirS/PCR/Direction/Sampling/Custom/NPoint     10000
```

For `CustomBiased`, the variable of a formula and the abscissa of a histogram or graph are $x = \cos\theta$ over $[\cos\theta_{\max}, 1]$.  The custom direction spectrum is sampled with ROOT's cached CDF on `NPoint` points (default 10000, range 10-10000000).  ROOT builds the CDF on first use; `SetRange` and `SetNpx` invalidate it.  The generator sets the range only when the spectrum, `NPoint`, or geometry changes, not on every event, so the cached CDF is reused.  `NPoint` controls the CDF accuracy and one-time build cost, not the per-event cost.  Custom direction histograms and graphs are interpreted as densities with respect to $x$; counts in non-uniform bins must be divided by the bin width, and the histogram or graph must cover the sampling range.

Let $f(\Omega) = \cos\theta/\pi$ be the physical angular distribution over the full downward hemisphere, and let $u_{\min}=\cos\theta_{\max}$.  The world accepts the fraction $1-u_{\min}^2$ of this distribution, and the direction weight includes that acceptance.  The modes sample $g(\Omega)$ and assign

| Mode                | $g(\Omega)$             | $w_\Omega(\Omega)$                                                                 |
| ------------------- | ----------------------- | ---------------------------------------------------------------------------------- |
| `Normal`            | $\propto \cos\theta$    | $1-u_{\min}^2$                                                                     |
| `ZenithPowerBiased` | $\propto \cos^n\theta$  | $\frac{2}{n+1}(1-u_{\min}^{n+1})\cos^{1-n}\theta$                                  |
| `CustomBiased`      | $\propto G(\cos\theta)$ | $\propto \cos\theta/G(\cos\theta)$                                                |

Here $G(\cos\theta)$ is the user-supplied custom distribution.  The `ZenithPowerBiased` exponent must be finite and non-negative; the default is $n=2$, so the `Power` command can be omitted when that value is wanted.  $n=1$ reproduces `Normal`, and $n=0$ gives a uniform distribution in $\cos\theta$ (uniform solid angle).  The `Normal` and `ZenithPowerBiased` weights are normalized to the full downward hemisphere.  `CustomBiased` is unnormalized: its weight is proportional to $\cos\theta/G(\cos\theta)$, with an arbitrary overall constant.  For an absolute flux, multiply by $2I/(1-u_{\min}^2)$ with $I=\int_{u_{\min}}^1 G(u)\,du$, or normalize $G$ so that $I=1$.

#### Units

The unit system is the Geant4 one (MeV, mm, ns). In particular **the variable of a spectrum formula is the kinetic energy in MeV**, so a formula containing an explicit energy scale must be written in MeV: a spectrum with a cutoff at 10 TeV is `exp(-x/1e7)`, not `exp(-x/1e4)`. Scale-free power laws such as `x^-2.7` are unaffected. The same applies to a formula given to `/MusAirS/PCR/Energy/Sampling/Custom/Formula`, and to a spectrum read from a graph or a histogram, whose abscissa is also the kinetic energy in MeV. The `/MusAirS/PCR/Energy/Min` and `/MusAirS/PCR/Energy/Max` commands, in contrast, take a value with a unit; a value outside the range of the spectrum is clamped to the nearest end of that range, with a warning, and a minimum that is not below the current maximum is an error.  Direction custom distributions use the dimensionless variable $x = \cos\theta$.

## Save primary vertex data

Primary vertices are not recorded by default. You can enable saving primary vertices by the command
```sh
/MusAirS/Analysis/SavePrimaryVertexData yes
```
This can sometimes be useful in debugging.

## Who ordered neutrino?

Neutrino hits are not recorded by default (since there are to much neutrino). You can let `EarthSD` to record neutrino hits by the command
```sh
/MusAirS/SD/EarthSD/DetectNeutrino yes
```

## Interpret the result

// TODO

## Bug tracking

Open an issue if you found any defects or bugs.

## Contributing

Open a pull request whenever you want.
