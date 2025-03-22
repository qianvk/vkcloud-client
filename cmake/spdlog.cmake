include(ExternalProject)

# Define submodule build/install directories
set(SPDLOG_INSTALL_DIR ${CMAKE_BINARY_DIR}/third_party/spdlog)
# set(SPDLOG_BUILD_DIR ${CMAKE_BINARY_DIR}/third_party_build)

# ✅ Build the submodule first
ExternalProject_Add(spdlog_project
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/third_party/spdlog
    # BINARY_DIR ${SPDLOG_BUILD_DIR}
    INSTALL_DIR ${SPDLOG_INSTALL_DIR}
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${SPDLOG_INSTALL_DIR}
        -DCMAKE_BUILD_TYPE=Release
        -DBUILD_SHARED_LIBS=OFF
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON
        -DBUILD_OBJECT_LIBS=OFF
    UPDATE_COMMAND ""
    BUILD_COMMAND make -j$(nproc)
    INSTALL_COMMAND make install
    BUILD_BYPRODUCTS ${SPDLOG_INSTALL_DIR}/lib/libspdlog.a
)

# ✅ Create an imported target for the submodule static library
add_library(spdlog STATIC IMPORTED)
set_target_properties(spdlog PROPERTIES
    IMPORTED_LOCATION ${SPDLOG_INSTALL_DIR}/lib/libspdlog.a
)

# ✅ Find the submodule's include directory dynamically
find_path(SPDLOG_INCLUDE_DIR
    # NAMES submodule.h  # Change this to an actual header in the submodule
    PATHS ${SPDLOG_INSTALL_DIR}/include
    NO_DEFAULT_PATH
)

# ✅ Make these variables available in the parent CMakeLists.txt
set(SPDLOG_INCLUDE_DIR ${SPDLOG_INCLUDE_DIR} CACHE PATH "spdlog include dir")
set(SPDLOG_INSTALL_DIR ${SPDLOG_INSTALL_DIR} CACHE PATH "spdlog install dir")


