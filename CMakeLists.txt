################################################################################
## CMake script for J-PET Geant4 Monte Carlo simulation software
################################################################################

cmake_minimum_required (VERSION 2.6)
project(jpet_mc LANGUAGES CXX)

################################################################################
## Det default build type to "Release"
set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

################################################################################
## Specify source folders
set(FOLDERS_WITH_SOURCE
  Objects/Framework
  Objects/Geant4
  Actions
  Core
  Info
)

## Point SOURCES and headers
foreach(dir ${FOLDERS_WITH_SOURCE})
  file(GLOB HEADERS_TMP ${dir}/*.h)
  file(GLOB SOURCES_TMP ${dir}/*.cpp)
  list(APPEND HEADERS ${HEADERS_TMP})
  list(APPEND SOURCES ${SOURCES_TMP})
endforeach()

list(APPEND FOLDERS ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_LIBRARY_PATH ${PROJECT_SOURCE_DIR}/lib)

################################################################################
## Project version number
set(VERSION_MAJOR 4 CACHE STRING "Project major version number.")
set(VERSION_MINOR 0 CACHE STRING "Project minor version number.")
set(VERSION_PATCH 0 CACHE STRING "Project patch version number.")
mark_as_advanced(VERSION_MAJOR VERSION_MINOR VERSION_PATCH)

################################################################################
## Provide scripts in a folder with a binary
set(SCRIPT_FILES
  vis.mac
  run3.mac
  run5.mac
  run5_lifetime.mac
  run6.mac
  run7.mac
  nema.mac
  cosmics.mac
  init_vis.mac
  singleBeam.mac
  extendedSource.mac
  modSmCh.mac
)

################################################################################
## Add a target to generate API documentation with Doxygen
find_package(Doxygen)
option(
  BUILD_DOCUMENTATION
  "Create and install the HTML based API documentation (requires Doxygen)"
  ${DOXYGEN_FOUND}
)

if(BUILD_DOCUMENTATION)
  if(NOT DOXYGEN_FOUND)
    message(FATAL_ERROR "Doxygen is needed to build the documentation.")
  endif()
  set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
  set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile)
  configure_file(${doxyfile_in} ${doxyfile} @ONLY)
  add_custom_target(
    doc
    COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM
  )
  install(
    DIRECTORY
    ${CMAKE_CURRENT_BINARY_DIR}/html
    DESTINATION
    ${CMAKE_CURRENT_BINARY_DIR}/doc
  )
endif()
################################################################################

################################################################################
## Include GEANT4 WITH VIS DRIVERS
## remember to compile geant4 with following flags
## -DGEANT4_USE_QT=ON -DGEANT4_INSTALL_DATA=ON -DGEANT4_USE_OPENGL_X11=ON -DGEANT4_USE_GDML=ON
set(GEANT4_LINK_OPTIONS "")
if(LINK_STATIC_GEANT4)
  set(GEANT4_LINK_OPTIONS static)
  message(STATUS "Linking statically to Geant4 libraries.")
endif()

find_package(Geant4 REQUIRED ${GEANT4_LINK_OPTIONS} ui_all vis_all)
include(${Geant4_USE_FILE})

################################################################################
## Include header-only CADMesh library
include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/CADMesh/)

################################################################################
## Include Boost
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.70 REQUIRED)
if(NOT TARGET Boost::filesystem)
    add_library(Boost::filesystem IMPORTED INTERFACE)
    set_property(TARGET Boost::filesystem PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})
    set_property(TARGET Boost::filesystem PROPERTY
        INTERFACE_LINK_LIBRARIES ${Boost_LIBRARIES})
endif()

include_directories(${Boost_INCLUDE_DIRS})
add_definitions(${Boost_DEFINITIONS})
link_directories(${Boost_LIBRARY_DIRS})
################################################################################
## Include ROOT
find_package(ROOT REQUIRED)
message(STATUS "ROOT version: ${ROOT_VERSION}")
if(ROOT_FOUND AND ROOT_VERSION VERSION_LESS "6.0")
  message(FATAL_ERROR "ROOT 6.0 is required ")
  # tested version above 6.10 can not display graphics while /run/beamOn due to llvm problem
endif()

include(${ROOT_USE_FILE})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)
include_directories(SYSTEM ${Geant4_INCLUDE_DIR} ${ROOT_INCLUDE_DIR})

################################################################################
## Create ROOT dictionaries
ROOT_GENERATE_DICTIONARY(
  JPetMCClasses
  Objects/Framework/JPetGeantEventInformation.h
  Objects/Framework/JPetGeantDecayTreeBranch.h
  Objects/Framework/JPetGeantDecayTree.h
  Objects/Framework/JPetGeantEventPack.h
  Objects/Framework/JPetGeantScinHits.h
  LINKDEF
  LinkDef.h
)
## Create a shared library with geneated dictionary
add_library(
  JPetMCClassesDict
  SHARED
  Objects/Framework/JPetGeantEventInformation.cpp
  Objects/Framework/JPetGeantDecayTreeBranch.cpp
  Objects/Framework/JPetGeantDecayTree.cpp
  Objects/Framework/JPetGeantEventPack.cpp
  Objects/Framework/JPetGeantScinHits.cpp
  JPetMCClasses.cxx
)
target_link_libraries(JPetMCClassesDict ${ROOT_LIBRARIES})

################################################################################
## Create a main program using the library
add_executable(jpet_mc JPetMC.cpp ${SOURCES})

# needed to suppress warnings about ROOT's locals
#  shadowing Geant4's global variables
target_compile_options(jpet_mc PRIVATE -Wno-shadow)

target_link_libraries(
  jpet_mc 
  ${ROOT_LIBRARIES}
  ${Geant4_LIBRARIES}
  JPetMCClassesDict
  )

## Copy script files to bin directory
foreach(file_i ${SCRIPT_FILES})
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/scripts/${file_i}
    ${CMAKE_CURRENT_BINARY_DIR}/bin/${file_i}
    COPYONLY
  )
endforeach(file_i)

foreach(file_i ${PARAMETERS_FILES})
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/docs/${file_i}
    ${CMAKE_CURRENT_BINARY_DIR}/bin/${file_i}
    COPYONLY
  )
endforeach(file_i)


## Copy stl file of frame construction to bin directory
add_custom_command(
  TARGET
  ${PROJECT_NAME}
  POST_BUILD
  COMMAND ln -sf
  ${CMAKE_CURRENT_SOURCE_DIR}/stl_geometry
  ${CMAKE_CURRENT_BINARY_DIR}/bin
)

## Link to manual
add_custom_command(
  TARGET
  ${PROJECT_NAME}
  POST_BUILD
  COMMAND ln -sf
  ${CMAKE_CURRENT_SOURCE_DIR}/docs/manual_v4.pdf
  ${CMAKE_CURRENT_BINARY_DIR}/bin
)

