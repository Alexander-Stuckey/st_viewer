macro(INITIALISE_PROJECT)

#    set(CMAKE_VERBOSE_MAKEFILE ON)
    set(CMAKE_INCLUDE_CURRENT_DIR ON)

    # Required packages
    find_package(Qt5Widgets REQUIRED)
    # Keep track of some information about Qt
    set(QT_BINARY_DIR ${_qt5Widgets_install_prefix}/bin)
    set(QT_LIBRARY_DIR ${_qt5Widgets_install_prefix}/lib)
    set(QT_PLUGINS_DIR ${_qt5Widgets_install_prefix}/plugins)
    set(QT_VERSION_MAJOR ${Qt5Widgets_VERSION_MAJOR})
    set(QT_VERSION_MINOR ${Qt5Widgets_VERSION_MINOR})
    set(QT_VERSION_PATCH ${Qt5Widgets_VERSION_PATCH})

    if(CMAKE_BUILD_TYPE MATCHES [Dd][Ee][Bb][Uu][Gg])
        message(STATUS "Building a debug version...")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_DEBUG -DDEBUG")
        add_definitions(-DQT_DEBUG)
    else()
        message(STATUS "Building a release version...")
        add_definitions(-DQT_NO_DEBUG_OUTPUT)
        add_definitions(-DQT_NO_DEBUG)
    endif()



    if(WIN32)
        #TODO
    else()
        # Adding -std=c++11 flag explicitly
        # It is a temporary fix to get building with CLANG working again.
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
        set(WARNING_ERROR "-Werror")
        set(DISABLED_WARNINGS "-Wno-c++11-long-long -Wno-float-equal -Wno-shadow -Wno-unreachable-code -Wno-switch-enum -Wno-type-limits -Wno-deprecated")
        set(EXTRA_WARNINGS "-Woverloaded-virtual -Wundef -Wall -Wextra -Wformat-nonliteral -Wformat -Wunused-variable -Wreturn-type -Wempty-body -Wdisabled-optimization -Wredundant-decls -Wpacked -Wuninitialized -Wcast-align -Wcast-qual -Wswitch -Wsign-compare -pedantic-errors -fuse-cxa-atexit -ffor-scope")
        if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
            set(EXTRA_WARNINGS "${EXTRA_WARNINGS} -Wold-style-cast -Wpedantic  -Weffc++ -Wnon-virtual-dtor -Wswitch-default -Wint-to-void-pointer-cast")
        endif()
    endif()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_WARNINGS} ${DISABLED_WARNINGS} ${WARNING_ERROR}")

    # Ask for Unicode to be used
    add_definitions(-DUNICODE)
    if(WIN32)
        add_definitions(-D_UNICODE)
    endif()

    # Set the RPATH information on Linux
    # Note: this prevent us from having to use the uncool LD_LIBRARY_PATH...
    if(NOT WIN32 AND NOT APPLE)
        set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib:$ORIGIN/../plugins/${PROJECT_NAME}")
    endif()

    set(REQUIRED_CXX_FEATURES
        cxx_long_long_type
        cxx_range_for
        cxx_constexpr
        cxx_auto_type
        cxx_nullptr
        cxx_static_assert
    )
  
    if(APPLE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmacosx-version-min=10.7 -stdlib=libc++")
    endif()
    
endmacro()

macro(use_qt5lib qt5lib)
    find_package(${qt5lib} REQUIRED)
    include_directories(${${qt5lib}_INCLUDE_DIRS})
    add_definitions(${${qt5lib}_DEFINITIONS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${${qt5lib}_EXECUTABLE_COMPILE_FLAGS}")
endmacro()

macro(LINUX_DEPLOY_QT_PLUGIN PLUGIN_CATEGORY)
    foreach(PLUGIN_NAME ${ARGN})
        install(FILES ${QT_PLUGINS_DIR}/${PLUGIN_CATEGORY}/${CMAKE_SHARED_LIBRARY_PREFIX}${PLUGIN_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}
                DESTINATION plugins/${PLUGIN_CATEGORY})
    endforeach()
endmacro()

macro(WINDOWS_DEPLOY_QT_LIBRARIES)
    foreach(LIBRARY ${ARGN})
        windows_deploy_library(${QT_BINARY_DIR}
        ${CMAKE_SHARED_LIBRARY_PREFIX}${LIBRARY}${CMAKE_SHARED_LIBRARY_SUFFIX} .)
    endforeach()
endmacro()

macro(WINDOWS_DEPLOY_QT_PLUGIN PLUGIN_CATEGORY)
    foreach(PLUGIN_NAME ${ARGN})
        windows_deploy_library(${QT_PLUGINS_DIR}/${PLUGIN_CATEGORY}
        ${CMAKE_SHARED_LIBRARY_PREFIX}${PLUGIN_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}
        plugins/${PLUGIN_CATEGORY})
    endforeach()
endmacro()

macro(WINDOWS_DEPLOY_LIBRARY DIRNAME FILENAME DESTINATION)
    # Copy the library file to both the build and build/bin folders, so we can
    # test things without first having to deploy stVi
    copy_file_to_build_dir(${DIRNAME} ${DESTINATION} ${FILENAME})
    # Install the library file
    install(FILES ${DIRNAME}/${FILENAME} DESTINATION ${DESTINATION})
endmacro()

macro(PROJECT_GROUP TARGET_NAME FOLDER_PATH)
    #Organize projects into folders
    set_property(TARGET ${TARGET_NAME} PROPERTY FOLDER ${FOLDER_PATH})
endmacro()

function(ST_LIBRARY)
    get_filename_component(PARENTDIR ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    set(UI_GENERATED_FILES)
    if(DEFINED LIBRARY_ARG_UI_FILES) 
        qt5_wrap_ui(UI_GENERATED_FILES ${LIBRARY_ARG_UI_FILES})
    endif()
    add_library("${PARENTDIR}" OBJECT ${UI_GENERATED_FILES} ${LIBRARY_ARG_INCLUDES} ${LIBRARY_ARG_SOURCES})
    source_group("${PARENTDIR}" FILES ${LIBRARY_ARG_INCLUDES} ${LIBRARY_ARG_SOURCES})
    target_compile_features("${PARENTDIR}" PUBLIC ${REQUIRED_CXX_FEATURES} PRIVATE ${REQUIRED_CXX_FEATURES})
endfunction()

function(INSTALL_LIBRARY_AND_SYMLINKS SRCPATH DEST)
  # SRCPATH is the full path to the library
  # DEST is the destination (for instance "lib")
  # 
  # This function will help us to install a shared library together with the symlinks pointing to it.
  # For instance, to install the following library and its symbolic links into "lib"
  #
  # esjolund@zebra:~/code/st_client$ ls -l  ~/Qt5.2.1/5.2.1/gcc_64/lib/libQt5Core.so*
  # lrwxrwxrwx 1 esjolund esjolund      19 feb  6 13:45 /home/esjolund/Qt5.2.1/5.2.1/gcc_64/lib/libQt5Core.so -> libQt5Core.so.5.2.1
  # lrwxrwxrwx 1 esjolund esjolund      19 feb  6 13:45 /home/esjolund/Qt5.2.1/5.2.1/gcc_64/lib/libQt5Core.so.5 -> libQt5Core.so.5.2.1
  # lrwxrwxrwx 1 esjolund esjolund      19 feb  6 13:45 /home/esjolund/Qt5.2.1/5.2.1/gcc_64/lib/libQt5Core.so.5.2 -> libQt5Core.so.5.2.1
  # -rwxr-xr-x 1 esjolund esjolund 5030256 feb  6 13:45 /home/esjolund/Qt5.2.1/5.2.1/gcc_64/lib/libQt5Core.so.5.2.1
  #
  # we would call the function like this:
  # INSTALL_LIBRARY_AND_SYMLINKS(/home/esjolund/Qt5.2.1/5.2.1/gcc_64/lib/libQt5Core.so.5.2.1 lib)
  #
  set(CURRENTSTRING "dummyvalue")
  set(NEXTSTRING "${SRCPATH}")
  while(NOT ${NEXTSTRING} STREQUAL ${CURRENTSTRING})
    set(CURRENTSTRING "${NEXTSTRING}")
    if(EXISTS "${CURRENTSTRING}")
      install(FILES ${CURRENTSTRING}
              DESTINATION ${DEST})
    endif()
    # The REGEX tries to remove a version number from the end of the string
    string(REGEX REPLACE "\\.[0-9]+$" "" NEXTSTRING "${CURRENTSTRING}")
  endwhile()
endfunction()
