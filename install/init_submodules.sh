#!/bin/bash
# HOWTO: Run the script from the main directory
# ./setups/init_submodules.sh
echo "Init submodules..."
git submodule update --init --recursive

CADMesh_VERSION=2.0.3
echo "CADMesh version: ${CADMesh_VERSION}"
cd Submodules/CADMesh;git checkout tags/v${CADMesh_VERSION};cd ../..

CXXOPTS_VERSION=3.2.1
echo "CxxOpts version: ${CXXOPTS_VERSION}"
cd Submodules/CxxOpts;git checkout tags/v${CXXOPTS_VERSION};cd ../..
