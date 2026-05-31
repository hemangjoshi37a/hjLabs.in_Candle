cmake_minimum_required(VERSION 3.10)

project(Candle VERSION 11.2)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

if(MSVC)
    add_compile_options(/MP)
endif()

add_subdirectory(src/customwidgets)
add_subdirectory(src/cadcam)
add_subdirectory(src/candle)
add_subdirectory(src/designerplugins)
add_subdirectory(src/scriptbindings/qtbindings)

# The JS plugins (coordinate system, emergency button, user commands, camera)
# call importExtension("qt.core"/"qt.gui"/...). Qt Script finds those binding
# extensions in "<app dir>/script". Build them directly next to the candle
# binary and make candle depend on them, so the plugins work when running from
# the build tree (the installed package already ships them this way).
set(_qtscript_exts
    qtscript_core qtscript_custom qtscript_gui qtscript_multimedia
    qtscript_network qtscript_opengl qtscript_printsupport qtscript_sql
    qtscript_uitools qtscript_widgets qtscript_xml)
foreach(_ext ${_qtscript_exts})
    if(TARGET ${_ext})
        set_target_properties(${_ext} PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/src/candle/script)
        if(TARGET candle)
            add_dependencies(candle ${_ext})
        endif()
    endif()
endforeach()