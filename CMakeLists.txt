cmake_minimum_required(VERSION 3.22)  # Ensures compatibility with Qt6 deployment API
project(Qt6_CMake_Deploy LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Gui Widgets Charts)  # Added Charts for QValueAxis

qt_standard_project_setup()  # Standard Qt CMake behaviors (e.g., for deployment)

qt_add_executable(Qt6_CMake_Deploy
    main.cpp
    plotwindow.cpp
    ringbuffer.cpp  # Added this to include RingBuffer implementations
)

target_link_libraries(Qt6_CMake_Deploy PRIVATE
    Qt6::Gui
    Qt6::Widgets
    Qt6::Charts  # Added for Charts module
)

# Install the binary to the prefix's bin dir
install(TARGETS Qt6_CMake_Deploy
    RUNTIME DESTINATION bin
)

# Generate and install the deployment script (bundles deps during --install)
qt_generate_deploy_app_script(
    TARGET Qt6_CMake_Deploy
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

# Install the stylesheet to the bin dir (adjust filename if needed)
install(FILES QTdark_PlotWin.qss DESTINATION bin)