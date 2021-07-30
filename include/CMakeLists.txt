
set(TARGET DispatchQueue)

set(HEADER_LIST "${${PROJECT_NAME}_SOURCE_DIR}/include/dispatch.hpp")

add_library(${TARGET} INTERFACE ${HEADER_LIST})

target_include_directories(${TARGET} INTERFACE
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_features(${TARGET} INTERFACE cxx_std_17)
target_precompile_headers(${TARGET} INTERFACE ${HEADER_LIST})
target_link_libraries(${TARGET} INTERFACE Threads::Threads)

# Check for Inter Procedural Optimization (IPO)
include(CheckIPOSupported)
check_ipo_supported(RESULT result)
if(result)
    set_target_properties(${TARGET} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

# Add ALIAS target for use in add_subdirectory or build directory
add_library(${PROJECT_NAME}::${TARGET} ALIAS ${TARGET})

# IDE support, show headers in a folder
source_group(TREE "${PROJECT_SOURCE_DIR}/include"
    PREFIX "Header Files"
    FILES ${HEADER_LIST})

# Export targets so build dir can be used by other projects
export(TARGETS ${TARGET}
    NAMESPACE ${PROJECT_NAME}::
    FILE ${PROJECT_NAME}Targets.cmake)

# Add target to Project Targets for install
install(TARGETS ${TARGET}
    EXPORT ${PROJECT_NAME}Targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}) # This is for Windows?

# Include the headers in install
install(DIRECTORY ../include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
