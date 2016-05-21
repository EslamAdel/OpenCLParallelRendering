################################################################################
# Copyright (C) 2015 Marwan Abdellah <abdellah.marwan@gmail.com>
################################################################################


## Configure the shell script to export LD_LIBRARY_PATH=/path/to/qt/libraries
#get_target_property(QtCore_location Qt5::Core  LOCATION)
#get_filename_component( QT_LIBS_LOCATION ${QtCore_location} DIRECTORY)
#set( APPLICATION_RUNSCRIPT_RAW ${CMAKE_CURRENT_SOURCE_DIR}/runscript.sh.in )

add_subdirectory(develop)
add_subdirectory(release)

