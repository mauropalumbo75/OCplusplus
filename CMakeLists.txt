cmake_minimum_required(VERSION 3.6)
project(OC++)

set(CMAKE_CXX_STANDARD 11)

set(COMMON_FILES include/math_utils.h include/physconsts.h include/utils.h  src/utils.cpp     src/math_utils.cpp)

# Builds tests
set(SOURCE_FILES_ELEMENTS ${COMMON_FILES}
        include/elements.h src/elements.cpp)
add_executable(test_elements tests/test_elements.cpp ${SOURCE_FILES_ELEMENTS})

set(SOURCE_FILES_SPECIES ${COMMON_FILES} ${SOURCE_FILES_ELEMENTS}
        include/species.h src/species.cpp)
add_executable(test_species tests/test_species.cpp ${SOURCE_FILES_SPECIES})

set(SOURCE_FILES_CONFIGURATIONS ${COMMON_FILES} ${SOURCE_FILES_SPECIES}
         include/configuration.h src/configuration.cpp)
add_executable(test_configuration tests/test_configurations.cpp ${SOURCE_FILES_CONFIGURATIONS})


# Build Ocplusplus
#set(SOURCE_FILES ${SOURCE_FILES_CONFIGURATIONS})
#add_executable(OC++ src/main.cpp ${SOURCE_FILES})