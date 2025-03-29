add_library(GuiService STATIC
  HttpGuiService/Conversion/Wallet.cpp
  HttpGuiService/HttpGuiService.cpp
)

#search for headers in root project directory
target_include_directories(GuiService PRIVATE ${PROJECT_SOURCE_DIR})

target_compile_options(GuiService PUBLIC ${COMPILE_FLAGS})

#Crow library
find_package( Crow REQUIRED)
set_target_properties(Crow::Crow PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS "" #workaround for "ISO C++11 requires whitespace after the macro name" warning when linking Crow
)

target_link_libraries(GuiService ${LINKER_FLAGS}
  Crow::Crow
  glog
)
