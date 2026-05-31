if(APPLE)
    set(DESIGNER_DESTINATION "Candle.app/Contents/MacOS/designerplugins")
else()
    set(DESIGNER_DESTINATION "designerplugins")
endif()

# QUiLoader (used by the JS plugins) resolves custom widgets such as
# StyledToolButton, ColorPicker and CameraWidget from "<app dir>/designerplugins"
# (see src/candleplugins/*/script.js: loader.addPluginPath(app.path + "/designerplugins")).
# The installed package ships the plugin .so files there, next to the candle
# binary; mirror that in the build tree so they resolve when running from build/.
if(APPLE)
    set(DESIGNER_BUILD_OUTPUT_DIR "${CMAKE_BINARY_DIR}/src/candle/Candle.app/Contents/MacOS/designerplugins")
else()
    set(DESIGNER_BUILD_OUTPUT_DIR "${CMAKE_BINARY_DIR}/src/candle/designerplugins")
endif()

add_subdirectory(cameraplugin)
add_subdirectory(customwidgetsplugin)