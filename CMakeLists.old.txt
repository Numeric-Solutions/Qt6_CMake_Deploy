cmake_minimum_required(VERSION 3.19)
project(Qt6_CMake_Deploy 
    VERSION 1.0.0
    LANGUAGES CXX)


find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Charts)

qt_standard_project_setup()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

qt_add_executable(Qt6_CMake_Deploy
    WIN32 MACOSX_BUNDLE
    main.cpp
    plotwindow.cpp
    plotwindow.h
    plotwindow.ui
    ringbuffer.cpp
    ringbuffer.h
)

target_link_libraries(Qt6_CMake_Deploy
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt6::Charts
)

include(GNUInstallDirs)

install(TARGETS Qt6_CMake_Deploy
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Add this to install the QSS file alongside the executable
install(FILES ${CMAKE_SOURCE_DIR}/QTdark_PlotWin.qss 
        DESTINATION ${CMAKE_INSTALL_BINDIR})

qt_generate_deploy_app_script(
    TARGET Qt6_CMake_Deploy
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
    NO_TRANSLATIONS  # Add this to skip translation deployment
)
install(SCRIPT ${deploy_script})

# copy qss file to build dir
configure_file(${CMAKE_SOURCE_DIR}/QTdark_PlotWin.qss ${CMAKE_BINARY_DIR}/QTdark_PlotWin.qss COPYONLY)
