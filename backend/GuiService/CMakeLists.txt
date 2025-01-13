add_library(GuiService STATIC
    GuiService.cpp
    ServiceFactory.cpp
)

# include directories
target_include_directories(GuiService PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

#Crow library
find_package( Crow REQUIRED)
set_target_properties(Crow::Crow PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS "" #workaround for "ISO C++11 requires whitespace after the macro name" warning when linking Crow
)

target_link_libraries(GuiService
    Crow::Crow
    glog
)
