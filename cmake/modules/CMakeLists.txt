# install the cmake files

file(GLOB cmakeFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.cmake")

set(module_install_dir /share/apps/cmake/modules )

install_files(${module_install_dir} FILES  ${cmakeFiles} )

