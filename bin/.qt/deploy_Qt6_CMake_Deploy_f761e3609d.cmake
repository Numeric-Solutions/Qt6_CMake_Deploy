include("/home/reflexion/Documents/Repositories/Qt6_CMake_Deploy/bin/.qt/QtDeploySupport.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/Qt6_CMake_Deploy-plugins.cmake" OPTIONAL)
set(__QT_DEPLOY_I18N_CATALOGS "qtbase")

qt6_deploy_runtime_dependencies(
    EXECUTABLE "/home/reflexion/Documents/Repositories/Qt6_CMake_Deploy/bin/Qt6_CMake_Deploy"
    GENERATE_QT_CONF
    NO_TRANSLATIONS
)
