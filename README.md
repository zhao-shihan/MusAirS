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
    - [General Particle Source (GPS)](#general-particle-source-gps)
  - [Save primary vertex data](#save-primary-vertex-data)
  - [Who ordered neutrino?](#who-ordered-neutrino)
  - [Interpret the result](#interpret-the-result)
  - [Output writer target](#output-writer-target)
  - [Bug tracking](#bug-tracking)
  - [Contributing](#contributing)

## Quick start

### Dependencies

MusAirS requires [Mustard](https://github.com/zhao-shihan/Mustard) `0.26.1121` and its [dependencies](https://github.com/zhao-shihan/Mustard?tab=readme-ov-file#required-dependencies):

- ROOT ≥ 6.36.00 (components `Core Imt MathCore Hist Tree ROOTDataFrame ROOTNTuple RIO`),
- Geant4 ≥ 11.0.0 (components `ui_all gdml vis_all`),
- MPI ≥ 4.1,
- CMake ≥ 3.25, Ninja (or GNU Make), and GCC ≥ 13 or Clang ≥ 19.

MusAirS finds Mustard with CPM. An installed Mustard is used when `find_package` finds one; otherwise CPM downloads tag `v0.26.1121` into the source cache `.cache/cpm`. The first configure needs network, and later configures reuse the cache.

### How to build (GNU/Linux)

In the source directory, execute:
```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

Useful build options:

| Option                                             | Default       | Meaning                                            |
| -------------------------------------------------- | ------------- | -------------------------------------------------- |
| `MUSAIRS_UNITY_BUILD` / `MUSAIRS_FULL_UNITY_BUILD` | `OFF` / `OFF` | unity build (also forwarded to an in-tree Mustard) |
| `MUSAIRS_ENABLE_IPO`                               | `ON`          | LTO/IPO in Release, for the MusAirS target         |
| `MUSAIRS_WERROR`                                   | `OFF`         | treat MusAirS compilation warnings as errors       |

An in-tree Mustard gets `MUSTARD_UNITY_BUILD` / `MUSTARD_FULL_UNITY_BUILD` from the `MUSAIRS_*` options above, `MUSTARD_USE_FFTW=OFF`, and `MUSTARD_USE_SHARED_MSVC_RT=${MUSAIRS_USE_SHARED_MSVC_RT}`; the other `MUSTARD_*` options keep their Mustard defaults.

### How to run

In the build directory, look for help:
```sh
./MusAirS --help
```

Useful runtime options (provided by Mustard's CLI):

| Option                                             | Meaning                                                                                                                                                                               |
| -------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `-v`, `--verbose`                                  | raise verbosity (-2 quiet ... 2 verbose; repeatable, e.g. `-vv`)                                                                                                                      |
| `-q`, `--quiet`                                    | lower verbosity (repeatable; can be mixed with `-v`)                                                                                                                                  |
| `--lite`                                           | do not print the Mustard banner                                                                                                                                                       |
| `--rng <engine>`                                   | random engine, one of `xoshiro256**`, `xoshiro256++`, `xoshiro256+`, `xoshiro512**`, `xoshiro512++`, `xoshiro512+`, `mt1993764`, `mt1993732` and `splitmix64`; default `xoshiro512**` |
| `--seed <int64>`                                   | seed; `0` means a random device, otherwise deterministic (a predefined deterministic seed is used by default)                                                                         |
| `--physics-list <list>`                            | Geant4 reference physics list (default `FTFP_BERT`)                                                                                                                                   |
| `--import-dd`, `--export-dd`, `--emport-dd <file>` | detector description import/export/emport before `/run/initialize`                                                                                                                    |
| `--mustard-version`, `--mustard-version-short`     | print the Mustard version                                                                                                                                                             |

One engine serves both CLHEP's global engine and ROOT's `gRandom`. In MPI runs each `/run/beamOn` receives mutually exclusive seeds. Use `--seed` for reproducible production runs.

#### Interactive session

Run interactively:
```sh
./MusAirS
```
or
```sh
./MusAirS -i <macro_for_initialization>
```
Without a macro, a default initialization macro fires a 1 TeV proton from 100 km using GPS, with visualization when available.

#### Batch session

Run in batch, sequentially:
```sh
./MusAirS <run_macro>
```
Parallel computing:
```sh
mpiexec -n <nproc> ./MusAirS <run_macro>
```
In a parallel run each MPI process writes its own output file inside a process-specific directory named after the requested file, with a `_p<rank>` suffix (e.g. `MusAirS/MusAirS_p0.root`); merge or reduce the per-rank files afterwards.

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

| **Coordinate** | **Expression**                           | **Direction**                                                           |
| -------------- | ---------------------------------------- | ----------------------------------------------------------------------- |
| phi            | $\mathrm{atan2}(\hat p_x, \hat p_y)+\pi$ | $0$: North, $\frac{\pi}{2}$: East, $\pi$: South, $\frac{3\pi}{2}$: West |
| Zenith         | $1+\hat p_z$                             | $0$: Sky, $1$: Horizon                                                  |

$\hat{\boldsymbol{p}}$ is the momentum direction at the end of the track, or at the ground surface for the hitting track.

## Choose an alternative physics list

MusAirS uses FTFP_BERT as the default physics list. However, you may change it by option `--physics-list`. For example,
```sh
MusAirS --physics-list QGSP_BERT
```
It accepts a [Geant4 reference physics list](https://geant4-userdoc.web.cern.ch/UsersGuides/PhysicsListGuide/html/reference_PL/index.html) name as its argument. The electromagnetic part is replaced by `EM Opt1`, so electromagnetic suffixes have no effect.

### Physics configuration

The rest of the physics configuration is set by the program itself, so that every run uses it even if the macro says nothing about it.

- Spin-dependent decay is enabled for muons and pions by `G4SpinDecayPhysics`, and Mustard's `PionKaonDecayMakeSpinPhysics` extends it to kaons and $K^0_L$.
- The default production cuts are **1 km** for gamma, e- and e+ and **3 m** for the proton. The proton cut is the kinetic-energy threshold of the recoiling nucleus in an elastic scatter; in Geant4's linear range-to-energy convention 3 m corresponds to 300 MeV.
- The reference-list neutron tracking cut of 10 us is replaced by **10 ms** and a **300 MeV** kinetic-energy limit. The default cut would stop neutrons long before the first interaction of a primary from 100 km, about 280 us; 300 MeV is the pion-production threshold below which a neutron cannot feed the muon component.
- The hadronic and electromagnetic tables are extended to **1 PeV**.
- The optional conversion processes of `G4EmExtraPhysics` (gamma to muon pair, positron to muon pair, muon to muon pair, positron to hadrons) keep their default disabled state; muons are produced electromagnetically through photonuclear processes.

The cuts can be overridden in the macro. The `lite` and `wo_pipm` example macros lower the electromagnetic cuts to 1 pc and kill the electromagnetic shower, which is suitable when only muons from pion and kaon decay are of interest.

```sh
/run/setCutForAGivenParticle gamma 1 pc
/run/setCutForAGivenParticle e- 1 pc
/run/setCutForAGivenParticle e+ 1 pc
```
A 1 pc range cut saturates the range-to-energy conversion at 9.55 GeV, which switches off the electromagnetic shower.

The `/MusAirS/Action/KillEMShower`, `/MusAirS/Action/KillNeutrino` and `/MusAirS/Action/KillChargedPion` switches discard the corresponding particles right after generation. All other particles are tracked and recorded when they reach the ground.

## Run macro

Example Geant4 macros are in the `scripts/` directory.

For more, refer to the [Geant4 Book For Application Developers](https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/html/index.html). [General Particle Source](https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/html/GettingStarted/generalParticleSource.html) would especially be helpful.

## Geometry and field

Geometry and field parameters are set in [the geometry description file](scripts/MusAirS.yaml), a YAML file. Units are Geant4's (MeV, mm, ns).

You can adjust these parameters by importing the YAML file through macro commands. For example,
```sh
/Mustard/Detector/Description/Import another.yaml
/Mustard/Detector/Description/Emport another.yaml
```
`Emport` exports, imports and then exports again, which is useful for debugging. The same three operations are available as the command-line options `--import-dd`, `--export-dd` and `--emport-dd`, which run before `/run/initialize`.

### Atmosphere modeling

#### States

The atmosphere follows the [International Standard Atmosphere (ISA)](https://en.wikipedia.org/wiki/International_Standard_Atmosphere). MusAirS uses an implementation valid up to 90 km above sea level, which is the default atmosphere top.

#### Components

The air material is `G4_AIR` (dry air) with the pressure, temperature and density of each atmosphere slice.

#### Slicing

Geant4 materials have constant properties, so the atmosphere is split into slices of air at different pressures. `SliceMode` selects slicing by altitude, with equal slice thickness, or by pressure, with equal pressure difference. The atmosphere top, the slice mode and the number of slices are set in the geometry description YAML file.

### Geomagnetic field

MusAirS supports a uniform magnetic field, whose vector is set in the YAML file in the Geant4 unit system (one tesla is $10^{-3}~\mathrm{MeV}\cdot\mathrm{ns}/\mathrm{mm}^2$).

### Elevating the ground

The ground plane is at sea level by default and can be raised in the geometry description YAML file, e.g. to simulate a detector on a plateau. The ground altitude must stay between 0 and the atmosphere top.

## Primary particles

MusAirS has two primary generators. The built-in primary cosmic ray generator is the default and is described first; the Geant4 [General Particle Source](https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/html/GettingStarted/generalParticleSource.html) (GPS) follows.

### Primary cosmic ray generator

The built-in primary cosmic ray generator is the default. `/MusAirS/Action/SwitchToPCR` selects it if GPS is used.

It accepts any particle of the Geant4 particle table, and takes the kinetic energy spectrum either from a formula, from a histogram in a ROOT file, or from a graph in a ROOT file:

```sh
/MusAirS/PCR/Particle                    proton
/MusAirS/PCR/Energy/Spectrum/Histogram   primary_cosmic_ray.root AMS_Proton
/MusAirS/PCR/Energy/Min                  3 GeV
/MusAirS/PCR/Energy/Max                  10 TeV
```

The primary vertex lies on the atmosphere top, on the line that runs through the ground centre `(0, 0, GroundAltitude)` in the sampled direction. The zenith angle $\theta$ is sampled with density proportional to $\cos\theta$ over the range accepted by the world volume, with uniform azimuth. This matches an isotropic cosmic-ray flux crossing horizontal ground; the $\cos\theta$ factor comes from the projected ground area, and the finite world acceptance enters the direction weight. The sampling can be biased (see [Direction sampling](#direction-sampling)). The initial direction is aimed at the ground centre; a charged primary is then deflected by the geomagnetic field during tracking.

The example file `primary_cosmic_ray.root` is built by `scripts/CreatePCRSpectrum.cxx`, which reads the published AMS-02 tables and writes the six histograms below.

| Histogram      | Species                             | Rigidity range |
| -------------- | ----------------------------------- | -------------- |
| `AMS_Proton`   | protons                             | 1.00-1800 GV   |
| `AMS_Helium4`  | ⁴He                                 | 1.92-3000 GV   |
| `AMS_Deuteron` | deuterons                           | 1.00-1800 GV   |
| `AMS_Helium3`  | ³He                                 | 1.92-3000 GV   |
| `AMS_Hydrogen` | Z = 1 flux as published (p + D)     | 1.00-1800 GV   |
| `AMS_Helium`   | Z = 2 flux as published (⁴He + ³He) | 1.92-3000 GV   |

The four species are listed in order of decreasing flux, followed by the two measured totals that are kept for reference.

The measured fluxes are the seven-year AMS-02 data [Phys. Rept. **894** (2021) 1]. The four species use the measured D/p and ³He/⁴He flux ratios of [Phys. Rev. Lett. **132** (2024) 261001], so `AMS_Proton` + `AMS_Deuteron` and `AMS_Helium4` + `AMS_Helium3` reproduce the published Z = 1 and Z = 2 fluxes bin by bin. Above 21.1 GV, where deuteron and ³He measurements end, the ratios follow the fits in that Letter; the script header documents the construction. `AMS_Hydrogen` and `AMS_Helium` are kept for comparisons with the published tables and should not be combined with the four species spectra, which already add up to them.

The bins are the rigidity bins of the measurement converted to the primary's total kinetic energy in MeV, which for nuclei is the whole-nucleus energy. The content is the flux density per MeV, so a bin integral is the measured flux in that bin and a histogram integral is a flux in m⁻² s⁻¹ sr⁻¹. The histograms carry no bin errors, since the generator samples central values and the published correlations are unavailable; the uncertainties are in the referenced tables. The four species have different masses and slightly different abscissae, so they are combined with weights proportional to their integral fluxes.

`scripts/run_AMS_lite.mac` (and its `full` and `wo_pipm` variants) runs the four species as four Geant4 runs in one session, writing `G4Run0` protons, `G4Run1` ⁴He, `G4Run2` deuterons and `G4Run3` ³He. The minimum kinetic energies correspond to the same 13.3 GV vertical geomagnetic cutoff at Zhaoqing, $E_{k,\min}=\sqrt{(ZR_c)^2+M^2}-M$, i.e. 12.40 GeV for protons, 23.13 GeV for ⁴He, 11.56 GeV for deuterons and 23.94 GeV for ³He. The particle, spectrum and minimum energy can be changed between `beamOn` commands, so a composition follows by weighting the runs afterwards.

The spectrum is sampled with one of four modes, selected by

```sh
/MusAirS/PCR/Energy/Sampling/Mode  LogUniformBiased
```

With $f(E)$ the physical spectrum and $g(E)$ the sampling distribution, the energy weight is $w_E(E)=f(E)/g(E)$. Direction sampling adds a weight $w_\Omega(\Omega)$, so the stored event weight is $w_E(E)\,w_\Omega(\Omega)$; in the default `Normal` direction mode $w_\Omega(\Omega)=1-u_{\min}^2$. The weights are stored so that every distribution can be reweighted afterwards.

| Mode               | $g(E)$                               | $w_E(E)$                                 | Use                                                               |
| ------------------ | ------------------------------------ | ---------------------------------------- | ----------------------------------------------------------------- |
| `Normal`           | $f(E)/I_f$                           | $I_f$                                    | reference, best for the lowest decade, cheapest per event         |
| `UniformBiased`    | $1/(E_{\max}-E_{\min})$              | $f(E)(E_{\max}-E_{\min})$                | exploring a wide range quickly                                    |
| `LogUniformBiased` | $1/[E\ln(E_{\max}/E_{\min})]$        | $f(E)E\ln(E_{\max}/E_{\min})$            | recommended for production, equal statistics per decade of energy |
| `CustomBiased`     | $g_{\text{user}}(E)/I_{\text{user}}$ | $f(E)I_{\text{user}}/g_{\text{user}}(E)$ | shaping the sample toward the observable of interest              |

The four modes use normalized sampling distributions, so their weights are importance ratios. Formula and graph normalization use a log-spaced energy grid or a linear direction grid; histogram normalization sums bin content times the overlap with the sampled range. In `Normal`, $w_E(E)$ is the spectrum integral $I_f=\int f(E)\,\mathrm{d}E$ over the sampled range; the other modes include the corresponding integral through $f(E)/g(E)$. For `CustomBiased`, $I_{\text{user}}=\int g_{\text{user}}(E)\,\mathrm{d}E$ and $w_E(E)=f(E)I_{\text{user}}/g_{\text{user}}(E)$; the custom spectrum must have a finite positive integral and positive values over the sampled range. Custom energy histograms and graphs are densities in $E$, so counts must be divided by the bin width and the spectrum must cover the sampling range. Partial bins at a range edge are treated as densities over the truncated width, with the same interval used for sampling and normalization. `Normal` and `CustomBiased` use ROOT's cached CDF, while `UniformBiased` and `LogUniformBiased` sample analytically. The stored weight $w_E(E)$ already includes the absolute energy normalization, so combining runs needs exposure or composition weights. $w_E(E)$ is an energy-integrated flux and $w_\Omega(\Omega)$ is dimensionless.

#### Direction sampling

The zenith angle is sampled over $\cos\theta \in [\cos\theta_{\max}, 1]$, where $\theta_{\max}$ follows from the world height and width (an inscribed cylinder, conservative for the box corners), and the azimuth is uniform. The mode is selected by

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

For `CustomBiased`, the user-supplied distribution $g_{\text{user}}(x)$ is given by a formula or by a histogram or graph in $x=\cos\theta$ over $[\cos\theta_{\max},1]$. ROOT's cached CDF samples the spectrum on `NPoint` points (default 10000, allowed 10-10000000), built on first use and invalidated by `SetRange` or `SetNpx`. The generator rebuilds it only when the spectrum, `NPoint` or geometry changes, so the per-event cost is unaffected. Formula and graph normalization use at least 1,000,000 points, or `NPoint` if larger; histogram normalization is exact and independent of `NPoint`. Increase `NPoint` if the spectrum has features narrower than the CDF grid. Custom direction histograms and graphs are densities in $x$, so counts must be divided by the bin width and the spectrum must cover the sampling range.

Let $f(\Omega)=\cos\theta/\pi$ be the physical angular distribution over the full downward hemisphere and $g(\Omega)$ the sampling distribution; let $u_{\min}=\cos\theta_{\max}$. The world accepts the fraction $1-u_{\min}^2$ of $f(\Omega)$, and the direction weight includes that acceptance. The modes sample $g(\Omega)$ and assign

| Mode                | $g(\Omega)$                           | $w_\Omega(\Omega)$                                                 |
| ------------------- | ------------------------------------- | ------------------------------------------------------------------ |
| `Normal`            | $\propto \cos\theta$                  | $1-u_{\min}^2$                                                     |
| `ZenithPowerBiased` | $\propto \cos^n\theta$                | $\frac{2}{n+1}(1-u_{\min}^{n+1})\cos^{1-n}\theta$                  |
| `CustomBiased`      | $\propto g_{\text{user}}(\cos\theta)$ | $\frac{2\cos\theta\,I_{\text{user}}}{g_{\text{user}}(\cos\theta)}$ |

Here $g_{\text{user}}(\cos\theta)$ is the user-supplied distribution, and for `CustomBiased` $I_{\text{user}}=\int_{u_{\min}}^{1} g_{\text{user}}(u)\,\mathrm{d}u$. The `ZenithPowerBiased` exponent must be finite and non-negative; the default is $n=2$, so the `Power` command can be omitted for that value. $n=1$ reproduces `Normal`, and $n=0$ gives a uniform distribution in $\cos\theta$ (uniform solid angle). The physical $f(\Omega)$ is normalized over the full downward hemisphere; each mode's weight has expectation $1-u_{\min}^2$ over the accepted cone, and $g_{\text{user}}(\cos\theta)$ must have a finite positive integral and positive values over the sampled range.

#### Units

The unit system is the Geant4 one (MeV, mm, ns). Spectrum formulas take the kinetic energy in MeV, so a formula with an explicit energy scale must use MeV; a 10 TeV cutoff is `exp(-x/1e7)`. Scale-free power laws such as `x^-2.7` are unaffected. The same applies to `/MusAirS/PCR/Energy/Sampling/Custom/Formula` and to spectra from a histogram or graph, whose abscissa is the kinetic energy in MeV. The `/MusAirS/PCR/Energy/Min` and `/MusAirS/PCR/Energy/Max` commands take a value with a unit; out-of-range values are clamped to the nearest end with a warning, and a minimum not below the current maximum (or a maximum not above the current minimum) is an error. The minimum kinetic energy must be positive. Direction custom distributions use $x = \cos\theta$.

### General Particle Source (GPS)

The Geant4 [General Particle Source](https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/html/GettingStarted/generalParticleSource.html) (GPS) is selected with

```sh
/MusAirS/Action/SwitchToGPSX
```

For example, the following macro lines generate protons from 100 km above sea level with a downward cosine-law angular distribution and an $E^{-2.7}$ kinetic energy spectrum from 3 GeV to 10 TeV:
```sh
/gps/particle         proton
/gps/position         0 0 100 km
/gps/ang/type         cos
/gps/ene/type         Pow
/gps/ene/alpha        -2.7
/gps/ene/min          3 GeV
/gps/ene/max          10 TeV
```
Detailed usage can be found in the [Geant4 Book For Application Developers](https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/html/GettingStarted/generalParticleSource.html).

## Save primary vertex data

Primary vertices are not recorded by default. Enable them with
```sh
/MusAirS/Analysis/SavePrimaryVertexData yes
```
The saved vertices are useful for debugging.

## Who ordered neutrino?

Neutrinos are killed at generation by default, since modelling their interactions would require a complete model of the Earth. To track them, and record their ground hits, use
```sh
/MusAirS/Action/KillNeutrino no
```

## Interpret the result

Each Geant4 run is written to a directory named `G4Run<id>`, with the collections that are enabled for the run.

| Collection                                                                 | Content                                                                                            |
| -------------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------- |
| `PrimaryVertex` (enabled by `/MusAirS/Analysis/SavePrimaryVertexData yes`) | one entry per primary particle, with fields `EvtID`, `PDGID`, `t0`, `x0`, `Ek0`, `p0` and `Weight` |
| `ReactionChain(<PDG ID>)`                                                  | the hitting track of that PDG ID and its complete parent chain up to the primary                   |

`ReactionChain` collections exist only for PDG IDs that hit the ground at least once in a run, and they store the ancestry of the ground hits. A hitting track has `KillProc == "<0|"`; a primary has `ParTrkID == -1` and `CreatProc == "|0>"`; `ParPDGID` is the parent's PDG ID (0 for a primary). Particles and antiparticles (e.g. PDG IDs 13 and -13) appear in separate collections. Entries are sorted by `TrkID` within an event, so parents precede their children; events follow beam-on order, so group by `EvtID`.

The fields of a track entry use the Geant4 unit system (energy and momentum in MeV, time in ns, length in mm).

| Field                   | Meaning                                                                                        |
| ----------------------- | ---------------------------------------------------------------------------------------------- |
| `EvtID`                 | event number                                                                                   |
| `ParTrkID`, `TrkID`     | parent track ID (-1 for a primary) and track ID                                                |
| `PDGID`, `ParPDGID`     | particle PDG ID, and the PDG ID of its parent (0 for a primary)                                |
| `CreatProc`, `KillProc` | creator process name (`"\|0>"` for a primary), killer process name (`"<0\|"` for a ground hit) |
| `t0`, `x0`, `Ek0`, `p0` | time, position, kinetic energy and momentum at track creation                                  |
| `t`, `x`, `Ek`, `p`     | time, position, kinetic energy and momentum at track end                                       |
| `Sigma0`, `Sigma`       | spin direction at track creation and at track end                                              |
| `Helicity0`, `Helicity` | helicity (momentum direction times spin direction) at track creation and at track end          |
| `Zenith`, `phi`         | direction at the end of the track in the convention above                                      |
| `TrkLen`                | total track length                                                                             |
| `Weight`                | track weight (the Geant4 event weight propagated to every track), for reweighting              |

For the hitting track the end-of-track values are taken at the ground surface; for ancestors, at the end of tracking.

A track terminated by a discrete absorption process (pair production, photoelectric effect, inelastic hadronic interaction, annihilation) or by an at-rest process has its final energy and momentum set to zero by Geant4. Such tracks remain in the output as ancestors of a ground hit. Their `Zenith` and `phi` come from the momentum direction, which Geant4 keeps in these cases; if a process resets it to zero, the direction at the start of the last step is used. Both columns are therefore defined for every entry; for a track at rest they give the direction of its last motion.

By default the collections are ROOT `RNTuple`s, which `ROOT::RDataFrame` reads directly by their directory-qualified name. For example, the kinetic energy of the muons that reached the ground in the first run:

```c++
ROOT::RDataFrame df{"G4Run0/ReactionChain(13)", "MusAirS.root"};
auto muonKineticEnergy = df.Filter("PDGID == 13 && KillProc == \"<0|\"").Take<float>("Ek");
```

To write ROOT `TTree`s instead (e.g. for `hadd`), set `MUSTARD_DATA_WRITER_DEFAULT_TARGET=TTree` in the environment or run `/MusAirS/Analysis/WriterTarget TTree` before `/run/beamOn`; the collection names are preserved. A `TTree` collection lives in a ROOT directory, so retrieve it from the file first. `ROOT::RDataFrame` can also read a `TTree`, and `TTree::Draw` is available:

```c++
TFile file("MusAirS.root");
auto* tree = file.Get<TDirectory>("G4Run0")->Get<TTree>("ReactionChain(13)");
ROOT::RDataFrame df{*tree};
auto muonKineticEnergy = df.Filter("PDGID == 13 && KillProc == \"<0|\"").Take<float>("Ek");
tree->Draw("Ek", "PDGID == 13 && KillProc == \"<0|\"");
```

When running in parallel, every MPI process writes its own file (see [Batch session](#batch-session)); merge or read the per-rank files together afterwards, e.g. `hadd` when writing `TTree`s.

## Output writer target

The analysis writes ROOT `RNTuple`s by default. The output format can be changed either by the `MUSTARD_DATA_WRITER_DEFAULT_TARGET` environment variable (`RNTuple` or `TTree`) or at run time with

```sh
/MusAirS/Analysis/WriterTarget RNTuple  # default
/MusAirS/Analysis/WriterTarget TTree    # one TTree per collection instead
```

## Bug tracking

Open an issue if you found any defects or bugs.

## Contributing

Open a pull request whenever you want.
