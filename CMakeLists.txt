CMAKE_MINIMUM_REQUIRED (VERSION 3.1)
PROJECT (mofem)

# Version number
SET (mofem_VERSION_MAJOR 0)
SET (mofem_VERSION_MINOR 1)

# Eigen include directory
SET (EIGEN_DIR "C:/Users/Mohamed/Dropbox/projects/Eigen" CACHE PATH "Eigen include path")

# Boost include directory
SET (BOOST_DIR "C:/boost/boost_1_57_0" CACHE PATH "Boost include path")
    
# Setup includes
INCLUDE_DIRECTORIES(${EIGEN_DIR} ${BOOST_DIR})
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src)

# Build types
UNSET(CMAKE_CONFIGURATION_TYPES CACHE)
SET(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Supported config types")

# No install
UNSET(CMAKE_INSTALL_PREFIX CACHE)

# Source files
SET(SRC_MAIN 
    src/main.cpp)
SET(SRC_CS 
    src/CS/CS.h 
    src/CS/CS.cpp)
SET(SRC_DOF 
    src/DOF/DOF.h 
    src/DOF/DOF.cpp)
SET(SRC_ELEMENT 
    src/Element/Element.h 
    src/Element/Element.cpp)
SET(SRC_ELEMENT_TRUSS
    src/Element/Truss/Truss.h 
    src/Element/Truss/TrussLinear.cpp)
SET(SRC_INPUT
    src/Input/InputData.h 
    src/Input/InputCreate.h 
    src/Input/InputCreate.cpp)
SET(SRC_MATERIAL
    src/Material/Material.h 
    src/Material/Material.cpp
    src/Material/MaterialCalculate.h)
SET(SRC_MATERIAL_LINEAR_ELASTIC
    src/Material/LinearElastic/LinearElastic.h
    src/Material/LinearElastic/LinearElastic.cpp)
SET(SRC_MODEL
    src/Model/ModelData.h
    src/Model/ModelGatherScatter.h
    src/Model/ModelCreate.h
    src/Model/ModelCreate.cpp)
SET(SRC_NODE
    src/Node/Node.h)
SET(SRC_SOLUTION
    src/Solution/SolutionData.h
    src/Solution/SolutionCreate.h
    src/Solution/SolutionCreate.cpp)
SET(SRC_UTIL
    src/Util/Array.h
    src/Util/BoostEnumString.h
    src/Util/CompileType.h
    src/Util/Precision.h
    src/Util/SparseArray.h
    src/Util/Status.h)
    
 SET(SRC_ALL
     ${SRC_MAIN} 
     ${SRC_CS}
     ${SRC_DOF} 
     ${SRC_ELEMENT}
     ${SRC_ELEMENT_TRUSS}
     ${SRC_INPUT}
     ${SRC_MATERIAL}
     ${SRC_MATERIAL_LINEAR_ELASTIC}
     ${SRC_MODEL}
     ${SRC_NODE}
     ${SRC_SOLUTION}
     ${SRC_UTIL})
    
# Executable
ADD_EXECUTABLE(mofem ${SRC_ALL})

# Source groups
SOURCE_GROUP("Source" FILES ${SRC_MAIN})
SOURCE_GROUP("Source\\CS" FILES ${SRC_CS})
SOURCE_GROUP("Source\\DOF" FILES ${SRC_DOF})
SOURCE_GROUP("Source\\Element" FILES ${SRC_ELEMENT})
SOURCE_GROUP("Source\\Element\\Truss" FILES ${SRC_ELEMENT_TRUSS})
SOURCE_GROUP("Source\\Input" FILES ${SRC_INPUT})
SOURCE_GROUP("Source\\Material" FILES ${SRC_MATERIAL})
SOURCE_GROUP("Source\\Material\\Linear Elastic" FILES ${SRC_MATERIAL_LINEAR_ELASTIC})
SOURCE_GROUP("Source\\Model" FILES ${SRC_MODEL})
SOURCE_GROUP("Source\\Node" FILES ${SRC_NODE})
SOURCE_GROUP("Source\\Solution" FILES ${SRC_SOLUTION})
SOURCE_GROUP("Source\\Util" FILES ${SRC_UTIL})