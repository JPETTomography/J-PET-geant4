# Installation

## Required packages:  
* `cmake`  
* `root 6.0`  
* [`geant4.10.04`](https://github.com/Geant4/geant4)  
 Built with cmake flags:  
 `-DGEANT4_USE_QT=ON -DGEANT4_INSTALL_DATA=ON -DGEANT4_USE_OPENGL_X11=ON -DGEANT4_USE_GDML=ON`  
* [cadmesh](https://github.com/christopherpoole/CADMesh)
* doxygen

## Environemt setup with CVMFS
You can fulfill above prerequsities with CernVM File System. See [CVMFS](https://cvmfs.readthedocs.io/en/stable/cpt-quickstart.html) documenttion. 
### Centos7:
For Geant4 v10 `source setups/setup-env-cvmfs-g4v10-centos7.sh`  
For Geant4 v11 `source setups/setup-env-cvmfs-g4v11-centos7.sh`   

### Debian/Ubunu:
For Geant4 v10: TODO  
For Geant4 v11: TODO

Once you run this script you have compiled Geant4 and all depandancies!

## Environemt setup with Conda
`conda env create --name j-pet --file setups/conda_env_g4v11-jpet.yml`  
`conda activate j-pet`   
Once you install & activate this environment you can build your application!

## Git submodule
External submodules can be easly cloned and initialized with running the script:
`./setups/init_submodules.sh`

## How to compile?
`mkdir build`  
`cd build`  
`cmake ..`  
`make`  
Once you want to compile with **mutlithreded (MT)** mode:  
`cmake .. -DMT=ON`  
`make` 

output file: (in build folder)  
`bin/jpet_mc`  

## How to create documentation?
(in build folder)  
`cmake .. && make doc`  
- open the `doc/html/index.html` in your favorite web browser  

# Advanced installation options

## Statically linking to Geant4 libraries
Using a statically-linked binary can speed up the execution of J-PET MC simulations by about 10%.

In order to use static Geant4 linkage:

1. Build Geant4 static libraries alongside with the (default) shared
libraries by passing the following option to CMake in  addition to other flags:
    ```
    -DBUILD_STATIC_LIBS=ON
    ```

    As a result, in the installation directory of Geant4, in the `lib` sudbirectory
    files with `*.a` extension should be present besides `*.so` files.

2. When running CMake for J-PET-Geant4, pass in the following additional option:

    ```
    -DLINK_STATIC_GEANT4=ON
    ```
