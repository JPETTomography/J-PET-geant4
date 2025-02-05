# Dockerization for the PL-Grid infrastructure

## Singularity

## Building the docker image with J-PET s/w

In order to build an image, run from the directory where dockerfile is placed, or put the full path to it. For this repo the dockerfile is named `dockerfile_g4`:
```
docker build -f dockerfile_g4 --tag 'ubuntu-g4:22.04-11.1.3-plgrid' .
```

To validate the image, you can run the container:
```
docker run -ti --rm ubuntu-g4:22.04-11.1.3-plgrid /bin/bash
```

## Export the docker image
```
docker save -o ubuntu-22.04-g4-11.1.3-plgrid.tar ubuntu-g4:22.04-11.1.3-plgrid
```

## Convert the docker image to singularity image
First of all you have to install `Singularity`, well you can utilize the conda and define env with given `sif-env.yml` file (only on Linux):
```
conda env create --file=sif-env.yml
```

In general, the convertion command is like:
```
singularity build my_sif_image.sif docker-archive:///ubuntu-22.04-g4-11.1.3-plgrid.tar
```
But running it on single CPU takes long time... you can then utilize the `Makefile` to run this conversion in parralell:
```
make -j 8 DOCKER_ARCHIVE=/your_path/ubuntu-22.04-g4-11.1.3-plgrid.tar
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



