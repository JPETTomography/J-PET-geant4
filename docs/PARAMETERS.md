# Parameters for MC simulations with Geant4 macros
Following options can be added to macro files, that are read by Geat4. Example files are in `scripts` folder, and also those files are copied to `bin` directory during program build.  

## Using different geometries
* 3 layers of scintillators (48, 48, 96)  
  each scintillator: 1.9x0.7x50 cm^3 wrapped in kapton foil  
  detector frame: loaded from CAD file  
  standard setup for runs 1-7:  
 `/jpetmc/detector/loadJPetBasicGeom`  
* loads the chamber specific for desired measurement: `runNr=3/5/6/7`:  
 `/jpetmc/detector/loadTargetForRun runNr`  
* shows only scintillator wrapped in a kapton foil:  
 `/jpetmc/detector/loadOnlyScintillators`  
* not implemented option that should load n-layer fully packed with
  scintillators barrel:  
 `/jpetmc/detector/loadIdealGeom`  
* loads modular layer in two configurations (Single and Double)
 `/jpetmc/detector/loadModularLayer [option]`  
* load detector configuration from JSON file
 `/jpetmc/detector/jsonSetupFile [fileName]`
* set the number of Run that is to be loaded from JSON file
 `/jpetmc/detector/jsonSetupRunNum [number]`

## General parameters:  
* Hit merging time:  
  define time range, between hits in scintillator, which will be classified as single hit:  
 `/jpetmc/detector/hitMergingTime`  
* Adding date and time to the name of the output file, so multiple executions of the simulation
  does not overwrite the default file (be careful with simulatenous simulations in the same directory)  
 `/jpetmc/output/AddDatetime 1`  

## Additional parameters:
* simulate only oPs 3 gamma decays:  
 `/jpetmc/material/threeGammaOnly`  
* simulate only pPs 2 gamma decays:  
 `/jpetmc/material/twoGammaOnly`  
* simulate only pick-off process oPs 2 gamma decays:  
 `/jpetmc/material/pickOffOnly`  
* adding oPs component for material given further by reload material command:  
 `/jpetmc/material/oPsComponent [mean lifetime in ns] [probability 0-100 %]`  
* setting pPs component for material given further by reload material command:  
 `/jpetmc/material/pPsComponent [mean lifetime in ns] [fraction of pPs to oPs]`  
* adding oPs component for material given further by reload material command:  
 `/jpetmc/material/directComponent [mean lifetime in ns] [probability 0-100 %]`  
* reloading lifetime parameters for material given by command material (xad4, kapton, aluminium, plexiglass, pa6):  
 `/jpetmc/material/reloadMaterials [material]`  
* set center of the annihilation chamber (3D with units):  
 `/jpetmc/run/setChamberCenter [dimensions X Y Z with units - cm, m, mm]`  
* for run5: define range where we expect annihilation to occur:   
 `/jpetmc/run/setEffectivePositronRange [value with unit]`  
* save true(!) generated events based on multiplicity (0,2-10):  
 `/jpetmc/event/saveEvtsDetAcc true`  
* change lower value of saved multiplicity (0):  
 `/jpetmc/event/minRegMulti [value]`  
  (above valid only with: /jpetmc/event/saveEvtsDetAcc)  
* change upper value of saved multiplicity (10):  
 `/jpetmc/event/maxRegMulti [value]`  
  (above valid only with: /jpetmc/event/saveEvtsDetAcc)  
* change excluded value of multiplicity (1):  
 `/jpetmc/event/excludedMulti [value]`  
  (above valid only with: /jpetmc/event/saveEvtsDetAcc)  
 `/jpetmc/event/save2g`  
* save event when 2g were registered (default false):  
 `/jpetmc/event/save3g`  
* save event when 3g were registered (default false):
  Options save2g/save3g  and saveEvtsDetAcc are separable !
* save event with fixed minimal number of hits to save (default 0 means all):
 `/jpetmc/event/saveEventWithMinSize [int]`
* save event with fixed minimal number of hits to save (default 0 means all):
 `/jpetmc/event/saveEventWithMaxSize [int]`
* save event with fixed number of hits to save and within the given energy window (default -1 keV energy means all energies):
The third argument is an additional option: 0 -> save all events in which there was N hits within energy window
1 -> save only events that have only minimal fixed number of hits, which is in the energy window
 `/jpetmc/event/setEnergyRange [min energy in keV - double] [max energy in keV - double] [bool]`
* print how many events were generated:  
 `/jpetmc/event/printEvtStat`  
* print out option during execution of the simulation - X in divisor (10^X) for number of printed events:  
 `/jpetmc/event/printEvtFactor`  
* show generation progress (in %):  
 `/jpetmc/event/ShowProgress`  
* Give nema point number to simulate (1-6):  
 `/jpetmc/source/nema`  
* Simulate couple of nema points at once with the same acitivity:  
 `/jpetmc/source/nema/mixed`  
* Setting position of given nema points:  
 `/jpetmc/source/nema/mixed/setPosition [point] [x position] [y position] [z position]`__
* Set weight of a given nema point:  
 `/jpetmc/source/nema/mixed/setWeight [point] [weight - int from 0 to infty, when 0 is removing given point]`  
* Set mean lifetime of a given nema point:  
 `/jpetmc/source/nema/mixed/setLifetime [point] [lifetime - int from 0 to infty]`  
* Allow a given point to annihilate into 3 photons based on the mean lifetime set -> Prob3G = lifetime/oPsLifetimevacuum:  
 `/jpetmc/source/nema/mixed/allow3G [point]'  
* Points are simulated at now only as cylinders. Setting size of that cylinder:  
 `/jpetmc/source/nema/mixed/setCylinderSize [point] [radius] [length]`  
* Allow a given point to generate prompt photon (by default it is true):  
 `/jpetmc/source/nema/mixed/allowPrompt [point]'  
* Points are simulated at now only as cylinders. Setting size of the cylinder for prompt:  
 `/jpetmc/source/nema/mixed/setPromptSourceSize [point] [radius] [length]`  
* Rotating the shape of the nema point:  
 `/jpetmc/source/nema/mixed/setCylinderRotation [point] [phi] [theta]`  
* Changing the shape of the nema point by shifting position in X:  
 `/jpetmc/source/nema/mixed/setCylinderShapeParametersY [point] [direction and spread] [shape parameter1 - power] [shape parameter2 - range]`  
* set parameters of gamma beam:  
 `/jpetmc/source/gammaBeam/setEnergy setPosition setMomentum`  
* set parameters of the isotope source, shape, if cylinder: radius, z-lenght (half):  
 `/jpetmc/source/isotope/setShape cylinder`  
 `/jpetmc/source/isotope/setShape/cylinderRadius`  
 `/jpetmc/source/isotope/setShape/cylinderZ`  
 `/jpetmc/source/isotope/setPosition`  
* set number of gamma quanta to generate 1 / 2 / 3 by the isotope:  
 `/jpetmc/source/isotope/setNGamma`  
* setting seed for simulations (if 0 random number will be used):  
 `/jpetmc/SetSeed [value]`  
* saving seed used for simulations:  
 `/jpetmc/SaveSeed true`  
* creation decay tree:  
 `/jpetmc/output/CreateDecayTree`  
* generate only cosmic muons  
 `/jpetmc/source/cosmicOnly
* generate cosmic muons in cylinder or cuboid (by default in cuboid)  
 `/jpetmc/source/cosmicGenShape [parameter - cuboid or cylinder]

## Creating .json file with geometry setup for J-PET Framework. If one of these two option will be put into macro, the output file will be created.
* select a type of output file strucure - Big Barrel or Modular format (default "barrel" other possible "modular"):  
 `jpetmc/detector/createGeometryType [geometry]`
* name an output file (default name "mc_geant_setup.json"):  
 `/jpetmc/detector/geometryFileName [name]`  
