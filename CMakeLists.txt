cmake_minimum_required(VERSION 3.22.1)

project(iecServer)

set(iecServer_HEADERS
    src/modelBuilder.h
)

set(iecServer_SOURCES
    src/main.c
    src/modelBuilder.c
)

include_directories(
    libiec61850/hal/inc
    libiec61850/src/common/inc/
    libiec61850/src/iec61850/inc/
    libiec61850/src/mms/inc/
    libiec61850/src/goose/
)

add_subdirectory(libiec61850)

add_executable(iecServer
    ${iecServer_HEADERS}
    ${iecServer_SOURCES}
)

target_link_libraries(iecServer 
    iec61850
)
