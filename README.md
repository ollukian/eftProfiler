# eftProfiler
Statistical tools used for Nuisance Parameters ranking and Nll scans

Table of Contents
* [Nll scans](#nll-scan):
  * [Computing the Nll scan](#running-the-nll-scan)
  * [Plotting the Nll scan](#plotting-the-nll-scan)
* [Nuissance parameters ranking](#nuisance-parameters-ranking):
  * [Computing the ranking](#computing-the-ranking)
  * [Plotting the ranking](#plotting-the-nuisance-parameter-ranking-)
* [Free fit](#free-fit)
* [Utilities](#utilities):
  * [Search for missing nuisances](#search-for-missing-nps)
  * [Getting quick information about the workspace](#getting-quick-information-about-the-workspace)
  * [Estimation of the most important nuisance parameters](#estimation-of-the-most-important-nuisance-parameters)


# Installation & Requirements
## Requirements:
- RooFit && ROOT:
* Either singularity (to run under /cvmfs/unpacked.cern.ch/gitlab-registry.cern.ch/ service)
* Or RooFit (https://github.com/roofit-dev) with ROOT
- CMake >= 3.13
- C++ compiler supporting c++17 (tested with MSVC 2022 and gcc (Debian 10.2.1-6) 10.2.1 20210110)

## Installation
To install the package, one needs to satisfy the recommendations above and to run the following commands:
```bash
git submodule update --init --recursive
```
to update the submodules and
```bash
mkdir build && cd build
cmake -j ..
make -j 
```

## Update
To update the package, simply run:
```
sh update_from_singularity.sh
```
The code will  update itself if an update is required and will run a series of tests to check if everything is fine.

## Nll scans:
![alt text](https://github.com/ollukian/eftProfiler/blob/dev/photo_2023-01-23_11-00-10.jpg)
Showing a result of a code for a POI Nll scan (expected post-fit).
* The red curve shows the *stat-only* result, being the result where all paired nuisance parameters are fixed at their best-fit-values on the real data.
* The blue curve is a *full* likelihood curve, where all nuisance parameters are floated.

# Launching
## Nuisance parameters ranking
### Computing the ranking
To run a Nuisance parameters ranking, one needs to specify the POI for which the ranking should be computed and to set up the workspace.

A general command to launch a computation for one nuisance parameter:
```
    sh job_script.sh --task compute_ranking \
        --poi POI_NAME \
        --ws_path PATH_TO_WORKSPACE \
        --ws_name WORKSPACE_NAME \
```
Apart from this, one needs to specify somehow what is a nuisance parameter for the ranking.
There are two ways to do it:
* By specifying the worker id: 
```--worker_id WORKER_ID```
* By specifying name of the nuisance parameters to be ranked: 
```--np_name NP_NAME```

Some other important options:
* ```--errors``` the way how to compute errors on the POIs. See the dedicated section below.
* ```--no_gamma``` whether to omit all the gamma-systematics from the ranking list.

Now, one needs to define the input data and names of some variables in the workspace. This is done via the following options:

| Option               | Type   | Mandatory | default value | description                                                   |
|----------------------|--------|-----------|---------------|---------------------------------------------------------------|
| ```--poi_init_val``` | float  | &cross;   | 0.            | Initial value of the POI to start the fit from.               |
| ```--res_path```     | string | &cross;   | .             | Path to save the resulting .json files                        |
| ```--ws_path```      | string | &cross;   | -             | Path to the workspace                                         |
| ```--ws_name```      | string | &cross;   | -             | Name of the workspace                                         |
| ```--model_config``` | string | &cross;   | ModelConfig   | Name of the model config in the workspace                     |
| ```--comb_data```    | string | &cross;   | combData      | Name of the dataset in the workspace                          |
| ```--comb_pdf```     | string | &cross;   | combPdf       | Name of the combined pdf in the workspace                     |
| ```--eps```          | float  | &cross;   | 1e-6          | Precision of the fit                                          |
| ```--strategy ```    | int    | &cross;   | 1             | Strategy of the fit (RooMinimizer options)                    |
| ```--retry```        | int    | &cross;   | 0             | Number of retries if a fit failed (with a change of strategy) |


#### Errors:
There are a few errors to treat errors on the POIs:
* ```--errors Hesse``` to re-compute the errors using the Hesse method after the initial fit. Should be nominally used!
* ```--errors Minos par1 par2``` to re-compute the errors using the Minos method (for par1, par2, ..) after the initial fit. Should be used if the Hesse method fails.

#### Output .json file:
As a result of a computation of impact for one Nuisance parameter, the code produces one .json file with the following structure:
```json
{
  "nll": "NLL_VALUE_AS_FLOAT",
  "np_err": "NP_CENTRAL_ERROR_IN_A_FREE_FIT_AS_FLOAT",
  "np_name": "NP_NAME_AS_STRING",
  "np_val": "NP_CENTRAL_VALUE_IN_A_FREE_FIT_AS_FLOAT",
  "poi_fixed_np_err": "ERROR_ON_POI_WITH_NP_FIXED_AS_FLOAT",
  "poi_fixed_np_val": "VALUE_OF_POI_WITH_NP_FIXED_AS_FLOAT",
  "poi_free_fit_err": "ERROR_ON_POI_IN_A_FREE_FIT_AS_FLOAT",
  "poi_free_fit_val": "VALUE_OF_POI_IN_A_FREE_FIT_AS_FLOAT",
  "poi_minus_one_variation_err": "ERROR_ON_POI_WITH_NP_VARYING_ON_MINUS_ONE_AS_FLOAT",
  "poi_minus_one_variation_val": "VALUE_OF_POI_WITH_NP_VARYING_ON_MINUS_ONE_AS_FLOAT",
  "poi_minus_sigma_variation_err": "ERROR_ON_POI_WITH_NP_VARYING_ON_MINUS_SIGMA_AS_FLOAT",
  "poi_minus_sigma_variation_val": "VALUE_OF_POI_WITH_NP_VARYING_ON_MINUS_SIGMA_AS_FLOAT",
  "poi_name": "NAME_OF_POI_AS_STRING",
  "poi_plus_one_variation_err": "ERROR_ON_POI_WITH_NP_VARYING_ON_PLUS_ONE_AS_FLOAT",
  "poi_plus_one_variation_val": "VALUE_OF_POI_WITH_NP_VARYING_ON_PLUS_ONE_AS_FLOAT",
  "poi_plus_sigma_variation_err": "ERROR_ON_POI_WITH_NP_VARYING_ON_PLUS_SIGMA_AS_FLOAT",
  "poi_plus_sigma_variation_val": "VALUE_OF_POI_WITH_NP_VARYING_ON_PLUS_SIGMA_AS_FLOAT",
  "prePostFit": {
    "prePostFit": "OBSERVED, POSTFIT, PREFIT"
  },
  "statType": {
    "statType": "NP_RANKING"
  },
  "studyType": {
    "studyType": ""
  }
}
```

### Plotting the Nuisance parameter ranking:
To plot the ranking of the Nuisance parameters, one needs to run the following command:
```
sh job_script.sh --task plot_ranking \
    --input PATH_TO_INPUT_JSON_FILES \
    --poi POI_NAME
```

It is possible to customize the plot by adding one of the various options:

| Option                    | Type        | Mandatory | default value          | description                                                  |  
|---------------------------|-------------|-----------|------------------------|--------------------------------------------------------------|
| ```top```                 | size_t      | &check;   | 20                     | Number of nuisance parameters to plot                        |
| ```color_prefit_plus```   | string      | &cross;   | "RGB(101,153,255)"     | Color of the prefit +1 sigma band. [Colour Format](#colour)  |
| ```color_prefit_minus```  | string      | &cross;   | "RGB(1,255,255)"       | Color of the prefit -1 sigma band. [Colour Format](#colour)  |
| ```color_postfit_plus```  | string      | &cross;   | "RGB(101,153,255,160)" | Color of the postfit +1 sigma band. [Colour Format](#colour) |
| ```color_postfit_minus``` | string      | &cross;   | "RGB(1,255,255,160)"   | Color of the postfit -1 sigma band. [Colour Format](#colour) |
| ```vertical```            | bool        | &cross;   | false                  | Plot the ranking in the vertical orientation                 |
| ```h_draw_options```      | string      | &cross;   | "E2"                   | Draw options for the error bands                             |
| ```rmargin```             | float       | &cross;   | 0.05                   | Right margin of the plot                                     |
| ```lmargin```             | float       | &cross;   | 0.10                   | Left margin of the plot                                      |
| ```tmargin```             | float       | &cross;   | 0.03                   | Top margin of the plot                                       |
| ```bmargin```             | float       | &cross;   | 0.40                   | Bottom margin of the plot                                    |
| ```mu_offset```           | float       | &cross;   | 0.0                    | Offset of the mu label from the axis                         |
| ```plt_size```            | float float | &cross;   | 1200 800               | Size of the canvas                                           |
| ```fileformat```          | vec<string> | &cross;   | "pdf"                  | Formats of the output file (space separated)                 |
| ```ignore_name```         | vec<string> | &cross;   |                        | Pattern in the names of the nuissance parameters to ignore   |

#### Colour
Color can be specified in the following ways:
* ```kROOTNAME``` name in the ROOT color table (ex: kRed, kBlue)
* ```RGB(R,G,B)``` RGB values in the range [0,1] or [0,255]
* ```RGBA(R,G,B,A)``` RGBA values in the range [0,1] or [0,255]

## Nll Scan
### Running the Nll scan
To run a Nll scan, one needs to specify the POI for which the Nll should be computed. A general command for launching:
* Required options:
```
sh job_script.sh --task nll_scan --pois(**see pois section below**)
```

* Optional options:

| Option       | Type        | Mandatory | default value | description                                                        | 
|--------------|-------------|-----------|---------------|--------------------------------------------------------------------|
| pois         | string      | &check;   | -             | destription of the POIs to scan. See the section below             | 
| pois_float   | vec strings | &cross;   | -             | POIs which must be left float during the scan                      | 
| prefit       | bool        | &cross;   | false         | Whether to use *Asimov pre-fit* data (via it's generation)         | 
| postfit      | bool        | &cross;   | false         | Whether to use *Asimov post-fit* data (via it's generation)        | 
| stat_only    | bool        | &cross;   | false         | Whether to fix all NPs consts (to get the stat-only resutls)       | 
| one_at_time  | bool        | &cross;   | false         | Whether to fix all POIs in the WS const                            |
| fit_all_pois | bool        | &cross;   | false         | Whether to allow all POIs to float in fit                          |
| force_data   | bool        | &cross;   | false         | Whether to force using a dataset from a dedicated data from the WS |
| snapshot     | string      | &cross;   | -             | Before each fit load this snapshot                                 |

Apart from this, all flags related to the Workspace and fit settings can also be used. See the dedicated section
#### Format of the POIS:
```
--pois POI_NAME(val CENTRAL_VALUE CENTRAL_ERROR : grid NUMBER_OF_POINTS_GRID : range SCAN_RANGE : at EXACT_VALUE_FOR_COMPUTATION)
```
* If you know central values of the POI and its error, then you may define them with the "grid" option and "range":
* If you want to compute Nll at a very specific point, just use the "at" option

* Options:
    *   POI_NAME => just name of the POI. Must be present in the workspace.
    *  *val*  => central value of the POI. If the grid and error are known, then for the worker number "i" the required value of the POI will be computed. Such that worker #0 takes the smallest allowd value (from the range) and the worker *grid* gives the largest allowed value
    *  *err*  => central error of the POI. Used to compute the scan range.
    *  *grid* => number of points in the scan grid.
    *  *at*   => exact position at which the Nll should be computed

### Plotting the Nll scan
To plot the Nll scan, one needs to run the following command:
```
sh job_script.sh --task plot_scan \
    --input PATH_TO_INPUT_JSON_FILES \
    --poi POI_NAME
```

* Optional options:

| Option       | Type   | Mandatory | default value | description                                               | 
|--------------|--------|-----------|---------------|-----------------------------------------------------------|
| yh           | float  | &check;   | -             | y-range of the plot (-2$\Delta$ ln(likelihood))           | 
| rmul         | float  | &check;   | -             | x-range low of the plot (units of the POI) (left point)   |
| rmuh         | float  | &check;   | -             | x-range high of the plot (units of the POI) (right point) |
| out          | string | &cross;   | -             | path and prefix where to save plots                       | 
| full         | bool   | &cross;   | false         | whether to plot the full scan (not the stat-only)         |
| stat         | bool   | &cross;   | false         | whether to plot the stat-only scan                        |
| exp          | bool   | &cross;   | false         | whether to plot the expected scan (postfit)               |
| obs          | bool   | &cross;   | false         | whether to plot the observed scan                         |
| force_data   | bool   | &cross;   | false         | Whether to force using a specific dataset from  the WS    |
| snapshot     | string | &cross;   | -             | Before each fit load this snapshot                        |
| energy       | string | &cross;   | 13            | CEM Energy of the plot (TeV)                              |
| res_status   | string | &cross;   | Internal      | Internal / Preliminary / Simulation...                    |
| obs_expected | string | &cross;   | Observed      | Info: Expected/Observed/...                               |
| experiment   | string | &cross;   | ATLAS         | Info: Experiment name                                     |
| lumi         | string | &cross;   | "36.1-139"    | Luminosity                                                |

## Free fit
To run a free fit with no conditions, floating only the required POIs. Errors re-evaluation may be performed (Hesse, Minos, etc). The command is:
### Launching the free fit
```
sh job_script.sh --task free_fit \
  --pois_float POI1 POI2 ... \
  --ws_path PATH_TO_WORKSPACE 
```

* Optional options:

| Option      | Type         | Mandatory | default value     | description                                                                                                   |
|-------------|--------------|-----------|-------------------|---------------------------------------------------------------------------------------------------------------|
| prefit      | bool         | &cross;   | false             | whether to use the prefit (generate Asimov prefit data)                                                       |
| postfit     | bool         | &cross;   | false             | whether to use the postfit (generate Asimov postfit data)                                                     |
| out         | string       | &cross;   | free_fit_res.root | path of the output ROOT file                                                                                  |
| errors      | string       | &cross;   | -                 | errors to re-evaluate (Hesse, Minos, etc). See [Errors settings](##errors)                                    |
| pois_float  | vec<strings> | &cross;   | -                 | POIs to be float in the fit                                                                                   |
| error_level | double       | &cross;   | 1.0               | For Minos. This is the level of 2 ($Delta ln L) to be used to estimate errors. "1" -> 1 sigma. 3.84 - 2 sigma |
| errors_for  | vec<strings> | &cross;   | -                 | For Minos. Parameters for which the errors are to be estimated.                                               |

## Utilities
To comfortify the work with the tool, some utilities are provided. They are:
* Search for missing Nuisance parameters files in a folder. [See this section](#search-for-missing-nps)
* Getting quick information about the workspace (POIs, NPs, etc) [See this section](#getting-quick-information-about-the-workspace)
* Estimation of the most important NPs for a given POI. [See this section](#estimation-of-the-most-important-nuisance-parameters)

### Search for missing NPs
Sometimes, some jobs for computing impact of a given NP on a given POI may fail. 
To find such NPs, one can use run the following command:
```
sh job_script.sh --task get_missing_nps \
    --input PATH_TO_INPUT_JSON_FILES \
    --poi POI_NAME \
    --ws_path PATH_TO_WORKSPACE
```

* The code will provide you with a list of missing nps (from the full list of NPs available in the workspace).
* The result will be printed in the terminal (by default) or saved in a file (if the option `--out` is provided).
* The NPs are separated by a symbol defined by the option `--sep`.
     - By default, it is `\ \n` (to print each NP on a new line with '\' after to allow quick pasting to a bash script.

Options:

| Option  | Type   | Mandatory | default value | description                                                  |
|---------|--------|-----------|---------------|--------------------------------------------------------------|
| out     | string | &cross;   | \[cout\]      | path of the output file. If not set, terminal output is used |
| sep     | string | &cross;   | \"\ \n\"      | separator between NPs in the output                          |
| input   | string | &check;   | -             | path to the input json files                                 |
| poi     | string | &check;   | -             | name of the POI for which NPs must be checked                |
| ws_path | string | &check;   | -             | path to the workspace                                        |

### Getting quick information about the workspace

To get quick information about the workspace (names of POIs, NPs, their amount, etc), one can use the following command:

```
sh job_script.sh --get GET_SPECIFICATION \
    --ws_path PATH_TO_WORKSPACE
```

Where "GET_SPECIFICATION" must be composed of one or two words separated by a space:
* object type:
  * poi - to get the list of POIs
  * np - to get the list of Nuisance parameters
  * obs - to get the list of Observables
  * globs - to get the list of Global observables
  * paired_nps - to get the list of paired NPs (which have a corresponding Global observable)
  * paired_globs - to get the list of paired Global observables (which have a corresponding NP)
  * non_gamma_nps - to get the list of NPs which are not Gamma-distributed
* "count" - if this option is provided, then the code will print only the number of objects of the given type.

Examples:
```
--get poi
```
would produce the list of POIs in the workspace:
```
HZZ_01
HYY_01
cHG
...
```

and with the ```count``` option:
```
--get poi count
```
would just return the number of POIs in the workspace: 3 (for example).

There also is a possibility to print the global pdf or dataset from the workspace:
```
--get pdf
--get ds_total
```

### Estimation of the most important Nuisance parameters
A rough estimation of what are the most important nuisance parameters can be estimated by their correlation with the POI
obtained from the Hesse matrix (matrix of second derivatives, hence a good estimation of the correlation) in a free fit. 

To launch the estimation, one can use the following command:
```
sh job_script.sh --task compute_hesse_nps \
    --poi POI_NAME \
    --ws_path PATH_TO_WORKSPACE
```

The code will produce a .txt file called "compute_hesse_nps_POI_NAME.txt" 
with the list of NPs sorted by their correlation with the POI along with 
a numerical value of this correlation.