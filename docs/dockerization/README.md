# Dockerization for the PL-Grid infrastructure
## Ubuntu-G4 Docker Image for J-PET-geant4

This Docker image provides a ready-to-use environment for `J-PET-geant4` simulations with `ROOT` and a Conda-based Python environment. It includes all necessary dependencies for building and running `J-PET-geant4` on `Ubuntu 24.04`.

This image is built through a two-step process:
1) dockerfile.base: based on Ubuntu 22.04, with ROOT and Geant4 installed inside a Conda environment being copied from Miniconda image (for Python/Conda environment): `continuumio/miniconda3:25.3.1-1`
2) dockerfile: the actual build on top using the f-petframework branch.

Note: Avoid using the official ROOT Docker image from Docker Hub as it often leads to dependency and library linking issues caused by mismatched system libraries and complex external dependencies. Installing ROOT via Conda inside your custom image provides better control over package versions, environment consistency, and reduces runtime compatibility problems

## Singularity

## Building the docker image with J-PET s/w

In order to build an image, run from the directory where dockerfile is placed, or put the full path to it. For this repo the dockerfile is named `dockerfile`:
```
docker build -f dockerfile --tag 'ubuntu-24.04-root-6.32.02-g4-11.2.2-<branch-name>-<commit-id>' .
```

To validate the image, you can run the container:
```
docker run -ti --rm ubuntu-24.04-root-6.32.02-g4-11.2.2-<branch-name>-<commit-id> /bin/bash
```

## Export the docker image
```
docker save -o ubuntu-24.04-root-6.32.02-g4-11.2.2-<branch-name>-<commit-id>.tar ubuntu-24.04-root-6.32.02-g4-11.2.2-<branch-name>-<commit-id>
```

## Convert the docker image to singularity image
First of all you have to install `Singularity`, well you can utilize the conda and define env with given `sif-env.yml` file (only on Linux):
```
conda env create --file=sif-env.yml
```

In general, the convertion command is like:
```
singularity build my_sif_image.sif docker-archive:///ubuntu-24.04-root-6.32.02-g4-11.2.2-<branch-name>-<commit-id>.tar
```
But running it on single CPU takes long time... you can then utilize the `Makefile` to run this conversion in parralell:
```
make -j 8 DOCKER_ARCHIVE=/your_path/ubuntu-24.04-root-6.32.02-g4-11.2.2-<branch-name>-<commit-id>.tar
```

**NOTE**: The temporary directory used during a build must be on a filesystem that has enough space to hold the entire container image, uncompressed, including any temporary files that are created and later removed during the build. You may need to set `SINGULARITY_TMPDIR` when building a large container on a system which has a small `/tmp` filesystem.

# Run builded SIF image on Cyfronet/Ares
Start an interactive session on worker node (from Ares machine):
```
srun -N 1 -C localfs --cpus-per-task=4 -p plgrid-now --time=02:00:00 -A plgtbjpet-cpu --job-name irun --pty /bin/bash
```
```
singularity exec --writable-tmpfs ubuntu-22.04-g4-11.1.3-plgrid.sif bash -c "source /opt/conda/etc/profile.d/conda.sh && conda activate geant4 && exec bash"
```
Note: Entering the sif image we have to activate the conda environmet on-the-fly. Once we work on interactive sesion we put at the end the command `exec bash`. For batch job it would be slightly different...



