cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project("t_junction_traffic_lights")

# Setup path for source dir
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(TARGET_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../src)

set(TARGET_FILES
	${TARGET_DIR}/hsm.c
	)

set (DEMO_FILES
	${SRC_DIR}/main.c
	${SRC_DIR}/main_lights.c
    ${SRC_DIR}/side_lights.c
	)

set (HEADER_FILES
		${SRC_DIR}/main_lights.h
        ${SRC_DIR}/side_lights.h
		${TARGET_DIR}/hsm.h
	)
SOURCE_GROUP("Src" FILES ${DEMO_FILES} ${TARGET_FILES} ${HEADER_FILES})

include_directories(
						${SRC_DIR}
						${TARGET_DIR}
					)

set(C_VERSION 99)
if ("c_std_11" IN_LIST CMAKE_C_COMPILE_FEATURES)
	set(C_VERSION 11)
endif()

set(CMAKE_C_STANDARD ${C_VERSION})
set(CMAKE_C_STANDARD_REQUIRED ON)
message("Your compiler supports : c${C_VERSION}")

set(HIERARCHICAL_STATES 0)
set(STATE_MACHINE_LOGGER 1)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

add_executable(t_junction_traffic_lights ${DEMO_FILES} ${TARGET_FILES} ${HEADER_FILES})

if ( CMAKE_C_COMPILER_ID MATCHES "Clang|AppleClang|GNU" )
    target_compile_options( t_junction_traffic_lights PRIVATE -Wall -Wextra -Wunreachable-code -Wpedantic)
    target_compile_options( t_junction_traffic_lights PRIVATE -Werror )
endif()

target_compile_definitions(t_junction_traffic_lights PRIVATE HSM_CONFIG)
configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/../../CMake/hsm_config.h.in"
            "${CMAKE_CURRENT_BINARY_DIR}/hsm_config.h" )

# Setup compiler include path
target_include_directories(t_junction_traffic_lights PRIVATE ${CMAKE_CURRENT_BINARY_DIR})


