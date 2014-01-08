	
#-----------------------------------------------------------------------------
# Git protocole option
#-----------------------------------------------------------------------------
option(Slicer_USE_GIT_PROTOCOL "If behind a firewall turn this off to use http instead." ON)

set(git_protocol "git")
if(NOT Slicer_USE_GIT_PROTOCOL)
  set(git_protocol "http")
endif()

#-----------------------------------------------------------------------------
# Enable and setup External project global properties
#-----------------------------------------------------------------------------
include(ExternalProject)
set(ep_base        "${CMAKE_BINARY_DIR}")

# Compute -G arg for configuring external projects with the same CMake generator:
if(CMAKE_EXTRA_GENERATOR)
  set(gen "${CMAKE_EXTRA_GENERATOR} - ${CMAKE_GENERATOR}")
else()
  set(gen "${CMAKE_GENERATOR}")
endif()



#-----------------------------------------------------------------------------
# PointBasedPatientRegistration
#-----------------------------------------------------------------------------

#if(NOT PointBasedPatientRegistration_DIR)
#  include("${CMAKE_CURRENT_SOURCE_DIR}/SuperBuild/External_PointBasedPatientRegistration.cmake")
#endif()

#-----------------------------------------------------------------------------
# OpenCV
#-----------------------------------------------------------------------------

#if(NOT OpenCV_DIR)
#  include("${CMAKE_CURRENT_SOURCE_DIR}/SuperBuild/External_OpenCV.cmake")
#endif()

#-----------------------------------------------------------------------------
# Project dependencies
#-----------------------------------------------------------------------------

set(project EndoscopeConsole)
set(${project}_DEPENDENCIES 
  #PointBasedPatientRegistration
  OpenCV)

SlicerMacroCheckExternalProjectDependency(${project})

set(ep_cmake_args)
foreach(dep ${EXTENSION_DEPENDS})
  list(APPEND ep_cmake_args -D${dep}_DIR:PATH=${${dep}_DIR})
endforeach()

ExternalProject_Add(${project}
  DOWNLOAD_COMMAND ""
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
  BINARY_DIR ${EXTENSION_BUILD_SUBDIRECTORY}
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
    -DMIDAS_PACKAGE_EMAIL:STRING=${MIDAS_PACKAGE_EMAIL}
    -DMIDAS_PACKAGE_API_KEY:STRING=${MIDAS_PACKAGE_API_KEY}
    -DADDITIONAL_C_FLAGS:STRING=${ADDITIONAL_C_FLAGS}
    -DADDITIONAL_CXX_FLAGS:STRING=${ADDITIONAL_CXX_FLAGS}
    -DGIT_EXECUTABLE:FILEPATH=${GIT_EXECUTABLE}
    -D${EXTENSION_NAME}_SUPERBUILD:BOOL=OFF
    -DEXTENSION_SUPERBUILD_BINARY_DIR:PATH=${${EXTENSION_NAME}_BINARY_DIR}
    # Slicer
    -DSlicer_DIR:PATH=${Slicer_DIR}
    # OpenCV
    -DOpenCV_DIR:PATH=${OpenCV_DIR}
    # PointBasedPatientRegistration
    #-DPointBasedPatientRegistration_DIR:PATH=${PBPR_DIR}
    ${ep_cmake_args}
  DEPENDS
    ${${project}_DEPENDENCIES}
  )
