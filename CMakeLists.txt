cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

# add project files
set(HEADER_FILES)
set(SRC_FILES main.cpp)

# set meta data
set(META_PROJECT_NAME findicons)
set(META_PROJECT_TYPE application)
set(META_PROJECT_VARNAME FIND_ICONS)
set(META_APP_NAME "Find-Icons")
set(META_APP_AUTHOR "Martchus")
set(META_APP_URL "https://github.com/${META_APP_AUTHOR}/${META_PROJECT_NAME}")
set(META_APP_DESCRIPTION "Finds icons required by Qt Widgets and Qt Quick applications")
set(META_CXX_STANDARD 17)
set(META_VERSION_MAJOR 0)
set(META_VERSION_MINOR 0)
set(META_VERSION_PATCH 1)

# find c++utilities
find_package(c++utilities 4.12.0 REQUIRED)
use_cpp_utilities()

# use standard filesystem library
list(APPEND PRIVATE_LIBRARIES "-lstdc++fs")

# include modules to apply configuration
include(BasicConfig)
include(WindowsResources)
include(AppTarget)
include(ConfigHeader)
