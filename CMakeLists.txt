cmake_minimum_required(VERSION 3.6)
project(OC++)

set(CMAKE_CXX_STANDARD 11)

########################################################################################################################
# Make definitions
set(COMMON_FILES include/math_utils.h include/physconsts.h include/utils.h  src/utils.cpp     src/math_utils.cpp)

set(SOURCE_FILES_TYPES ${COMMON_FILES}
        include/types.h src/types.cpp)

set(SOURCE_FILES_REFERENCES ${COMMON_FILES}
        include/references.h src/references.cpp)

set(SOURCE_FILES_ELEMENTS ${COMMON_FILES}
        include/elements.h src/elements.cpp)

set(SOURCE_FILES_SPECIES ${COMMON_FILES} ${SOURCE_FILES_ELEMENTS}
        include/species.h src/species.cpp)

set(SOURCE_FILES_STRUCTURES ${COMMON_FILES}
        include/structure.h src/structure.cpp)

set(SOURCE_FILES_Y ${COMMON_FILES} ${SOURCE_FILES_ELEMENTS}
        include/y.h src/y.cpp)

set(SOURCE_FILES_CONFIGURATIONS ${COMMON_FILES} ${SOURCE_FILES_SPECIES}
         include/configuration.h src/configuration.cpp)

set(SOURCE_FILES_TPFUNCTIONS ${COMMON_FILES}
        include/tpfunctions.h src/tpfunctions.cpp)

set(SOURCE_FILES_MAGNETIC ${COMMON_FILES}
        include/magnetic.h src/magnetic.cpp)

set(SOURCE_FILES_READABINITIO ${COMMON_FILES} ${SOURCE_FILES_STRUCTURES} ${SOURCE_FILES_ELEMENTS}
        include/readabinitio.h src/readabinitio.cpp)

set(SOURCE_FILES_PHASES ${COMMON_FILES}  ${SOURCE_FILES_CONFIGURATIONS} ${SOURCE_FILES_TYPES} ${SOURCE_FILES_Y}
        ${SOURCE_FILES_TPFUNCTIONS}   ${SOURCE_FILES_MAGNETIC} ${SOURCE_FILES_READABINITIO}
        include/phases.h src/phases.cpp)

set(SOURCE_FILES_READTDB ${SOURCE_FILES_PHASES} ${SOURCE_FILES_REFERENCES}
        include/readtdb.h src/readtdb.cpp)

set(SOURCE_FILES_ELECTRONIC ${COMMON_FILES}
        include/electronic.h src/electronic.cpp)

set(SOURCE_FILES_HARMONIC ${COMMON_FILES}
        include/harmonic.h src/harmonic.cpp)

set(SOURCE_FILES_SYSTEMS ${SOURCE_FILES_PHASES} ${SOURCE_FILES_REFERENCES} ${SOURCE_FILES_ELECTRONIC}
        ${SOURCE_FILES_HARMONIC} ${SOURCE_FILES_READTDB}
        include/systems.h src/systems.cpp)

set(CMAKE_CXX_FLAGS  "-lcln -lginac" )

# Add libraries directories
link_directories("lib")

########################################################################################################################
# Builds several test executables  (comment or delete these lines in final release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../tests")
add_executable(test_types tests/test_types.cpp ${SOURCE_FILES_TYPES})
add_executable(test_references tests/test_references.cpp ${SOURCE_FILES_REFERENCES})
add_executable(test_elements tests/test_elements.cpp ${SOURCE_FILES_ELEMENTS})
add_executable(test_species tests/test_species.cpp ${SOURCE_FILES_SPECIES})
add_executable(test_structures tests/test_structures.cpp ${SOURCE_FILES_STRUCTURES})
add_executable(test_y tests/test_y.cpp ${SOURCE_FILES_Y})
add_executable(test_configurations tests/test_configurations.cpp ${SOURCE_FILES_CONFIGURATIONS})
add_executable(test_tpfunctions  tests/test_tpfunctions.cpp ${SOURCE_FILES_TPFUNCTIONS})
add_executable(test_magnetic tests/test_magnetic.cpp ${SOURCE_FILES_MAGNETIC})
add_executable(test_readabinitio tests/test_readabinitio.cpp ${SOURCE_FILES_READABINITIO})
add_executable(test_phases tests/test_phases.cpp ${SOURCE_FILES_PHASES})
add_executable(test_readtdb tests/test_readtdb.cpp ${SOURCE_FILES_READTDB})
add_executable(test_electronic tests/test_electronic.cpp ${SOURCE_FILES_ELECTRONIC})
add_executable(test_harmonic tests/test_harmonic.cpp ${SOURCE_FILES_HARMONIC})
add_executable(test_systems tests/test_systems.cpp ${SOURCE_FILES_SYSTEMS})

# Add the static libraries cln and ginac
target_link_libraries(test_tpfunctions libcln.so libginac.so)
target_link_libraries(test_phases libcln.so libginac.so)
target_link_libraries(test_readtdb libcln.so libginac.so)
target_link_libraries(test_systems libcln.so libginac.so)

########################################################################################################################
# Build OC++
set(SOURCE_FILES_OC ${SOURCE_FILES_SYSTEMS}
        include/UI.h src/UI.cpp src/main.cpp)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin/")
add_executable(OC++ src/main.cpp ${SOURCE_FILES_OC})
target_link_libraries(OC++ libcln.so libginac.so)