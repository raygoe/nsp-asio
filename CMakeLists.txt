cmake_minimum_required ( VERSION 2.8.12 )

project ( nsp )

macro(get_WIN32_WINNT version)
    if (WIN32 AND CMAKE_SYSTEM_VERSION)
        set(ver ${CMAKE_SYSTEM_VERSION})
        string(REPLACE "." "" ver ${ver})
        string(REGEX REPLACE "([0-9])" "0\\1" ver ${ver})

        set(${version} "0x${ver}")
    endif()
endmacro()
# Local CMake Modules
set ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules" )

option( NSP_BUILD_DOCS "Build the NSP Doxygen documentation" ON)
if ( NSP_BUILD_DOCS )
    find_package(Doxygen)
endif ( )

# Import macros
include ( CheckCXXCompilerFlag )

# C++11 Requirement for Linux
if ( NOT WIN32 )
    check_cxx_compiler_flag ( "-std=c++11" COMPILER_SUPPORTS_CXX11 )
    check_cxx_compiler_flag ( "-std=c++0x" COMPILER_SUPPORTS_CXX0X )
    set ( CMAKE_CXX_COMPILER_ARG1 "-std=c++11" )

    if ( COMPILER_SUPPORTS_CXX11 )
        set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++11" )
    elseif ( COMPILER_SUPPORTS_CXX0X )
        set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++0x" )
    else ( )
        message ( STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
    endif ( )
    
    set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3" )  # Optimize
else ( )
    get_WIN32_WINNT(ver)
    add_definitions(-D_WIN32_WINNT=${ver})

    if ( MINGW )
        check_cxx_compiler_flag ( "-std=c++11" COMPILER_SUPPORTS_CXX11 )
        check_cxx_compiler_flag ( "-std=c++0x" COMPILER_SUPPORTS_CXX0X )
        set ( CMAKE_CXX_COMPILER_ARG1 "-std=c++11" )
    
        if ( COMPILER_SUPPORTS_CXX11 )
            set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )
        elseif ( COMPILER_SUPPORTS_CXX0X )
            set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x" )
        else ( )
            message ( STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
        endif ( )
        
        set ( CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++" )
    else ( )
        add_definitions ( "/wd4244 /wd4996" )
    endif ( )
endif ( )

add_definitions(-DASIO_STANDALONE)

include_directories ( "${PROJECT_SOURCE_DIR}/shared" )

add_subdirectory ( shared )
add_subdirectory ( client )
add_subdirectory ( server )

# Docs
if ( DOXYGEN_FOUND AND NSP_BUILD_DOCS )
    if ( DOXYGEN_DOT_FOUND )
        set ( NSP_HAVE_DOT "YES" )
    else ( )
        set ( NSP_HAVE_DOT "NO" )
    endif ( )

    configure_file( "${PROJECT_SOURCE_DIR}/docs/Doxyfile.in"
                    "${CMAKE_INSTALL_PREFIX}/docs/Doxyfile" )

    add_subdirectory ( docs )
endif ( )
