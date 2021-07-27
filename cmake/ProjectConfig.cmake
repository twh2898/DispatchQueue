include(CMakeFindDependencyMacro)

# TODO: capture @var@ config variables

find_dependency(fmtlib REQUIRED)

# TODO: extra steps

include("${CMAKE_CURRENT_LIST_DIR}/FooBarTargets.cmake")
