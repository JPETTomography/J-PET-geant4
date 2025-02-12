# Installation

## (Ana)conda Package Manager
For building the application, the Geant4 and ROOT libraries are required. They can be easily installed using the Anaconda package manager.

Current versions used:  
**Geant4**: 11.2.2  
**ROOT**:  6.32.2

### Conda in your local machine
You can easly install it, see [conda/docs](https://docs.conda.io/projects/conda/en/latest/user-guide/install/index.html).

#### Environemt creation with Conda
`conda env create --name j-pet --file setups/conda_env_g4v11-jpet.yml`  
`conda activate j-pet`   

### Conda at JPSRV04 machine
A preconfigured central environment is available. You are expected to be a member of the system group `anacondag`. To have conda available for you run: `source /data/3/anaconda/anaconda3/init_conda.sh`. Then activate the environment: `conda activate G4v11.2.2-ROOTv6.32.2`

## Git submodules
IMPORTANT. External submodules are being used in the project. After cloning the codebase of the application you have to get them into the local repo. They can be easly cloned and initialized with running the script: `./setups/init_submodules.sh`

## How to compile?
Once you activate the conda environemt you can build your application!
`mkdir build`  
`cd build`  
`cmake ..`  
`make`  
Once you want to compile with **mutlithreded (MT)** mode:  
`cmake .. -DMT=ON`  
`make` 

output file: (in build folder)  
`bin/jpet_mc`  

## How to run
You can view the available command-line options by running: `./jpet_mc --help`.

## How to create documentation?
(in build folder)  
`cmake .. && make doc`  
- open the `doc/html/index.html` in your favorite web browser  