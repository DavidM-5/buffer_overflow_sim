cmake_minimum_required(VERSION 3.10)
project(MyProject VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)

# ——————————————————————————————————————
# Copy directories at configure time
# ——————————————————————————————————————
# Copy access_manager → build/access_manager
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/access_manager")
file(COPY
  "${CMAKE_SOURCE_DIR}/access_manager/."
  DESTINATION "${CMAKE_BINARY_DIR}/access_manager"
)

# Copy vulnerable_system → build/targets/compiled
set(VULN_SRC "${CMAKE_SOURCE_DIR}/targets/src/Task_one/vulnerable_system")
set(VULN_DST "${CMAKE_BINARY_DIR}/targets/compiled")

file(MAKE_DIRECTORY "${VULN_DST}")

# copy the hidden .keyfile.bin
file(COPY
  "${VULN_SRC}/.keyfile.bin"
  DESTINATION "${VULN_DST}"
)

# copy the database directory
file(COPY
  "${VULN_SRC}/database/."
  DESTINATION "${VULN_DST}/database"
)

# ——————————————————————————————————————
# Continue with your normal subprojects
# ——————————————————————————————————————
add_subdirectory(core)
add_subdirectory(application)