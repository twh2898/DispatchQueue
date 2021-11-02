
set(TARGET DispatchQueue)

add_library(${TARGET} DispatchQueue.cpp ../include/DispatchQueue.hpp)

target_include_directories(${TARGET} PUBLIC
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_features(${TARGET} INTERFACE cxx_std_17)

target_link_libraries(${TARGET} INTERFACE Threads::Threads)

# Check for Inter Procedural Optimization (IPO)
include(CheckIPOSupported)
check_ipo_supported(RESULT result)
if(result)
    set_target_properties(${TARGET} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

# IDE support, show headers in a folder
source_group(TREE "${PROJECT_SOURCE_DIR}/include"
    PREFIX "Header Files"
    FILES ${HEADER_LIST})

# Add target to Project Targets for install
install(TARGETS ${TARGET}
    EXPORT ${PROJECT_NAME}Targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

# Include the headers in install
install(DIRECTORY ${${PROJECT_NAME}_SOURCE_DIR}/include
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
