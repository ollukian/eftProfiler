# eftProfiler
Is a statistical framework used for Nuisance Parameters ranking and Nll (negative-log-likelihood) scans.

* Framework allows performing precise statistical analysis (based on the ROOT framework https://root.cern/).

## Nll scans:
![alt text](https://github.com/ollukian/eftProfiler/blob/dev/photo_2023-01-23_11-00-10.jpg)
Showing a result of a code for a POI Nll scan (expected post-fit).
* The red curve shows the *stat-only* result, being the result where all statistical uncertainty on the model are fixed at their best-fit-values on the real data.
* The blue curve is a *full* likelihood curve, where all nuisance parameters are floated.

## Nuisance parameters ranking plot
**under construction**

# Installation & Requirements
## Requirements:
- RooFit && ROOT:
* Either singularity (to run under /cvmfs/unpacked.cern.ch/gitlab-registry.cern.ch/ service)
* Or RooFit (https://github.com/roofit-dev) with ROOT
- CMake >= 3.13
- c++ 17
## Update
To update the package, simply run:
```
sh update_from_singularity.sh
```
The code will  update itself if an update is required and will run a series of tests

# Running
The code has a couple of functionalities:
- Core:
    - Running a scan for Nuisance Parameters ranking
    - Running a negative-log-likelihood scan (Nll scan)
- Plotting:
    - Nuisance Parameters ranking
    - Nll scan
- Utils
  - POIs in the workspace
  - Nuisance parameters in the workspace
  - Global observables in the workspace
  - Number of any of those above

/// the readme is under construction and is not full
