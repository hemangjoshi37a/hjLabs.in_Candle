find_package(Qt5 COMPONENTS Core Widgets UiPlugin REQUIRED)

file(GLOB SRC_FILES *.cpp)

add_library(customwidgetsplugin SHARED ${SRC_FILES})

# Build next to the candle binary so QUiLoader finds StyledToolButton etc.
set_target_properties(customwidgetsplugin PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${DESIGNER_BUILD_OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${DESIGNER_BUILD_OUTPUT_DIR}")

target_include_directories(customwidgetsplugin PRIVATE ../../customwidgets)

target_link_libraries(customwidgetsplugin PRIVATE customwidgets Qt5::Core Qt5::Widgets Qt5::UiPlugin)

install(TARGETS customwidgetsplugin
    RUNTIME DESTINATION ${DESIGNER_DESTINATION}
    LIBRARY DESTINATION ${DESIGNER_DESTINATION})