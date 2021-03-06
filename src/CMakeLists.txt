# create parser file
# this has to be done before the lexer file, because the lexer file includes the header from the parser
macro(BISON_FILE parser_FILE)
    GET_FILENAME_COMPONENT(output_FILE ${parser_FILE} NAME)
    set(OUTFILE "${CMAKE_CURRENT_BINARY_DIR}/${output_FILE}.cpp")
    set(HEADER "${CMAKE_CURRENT_BINARY_DIR}/${output_FILE}.hpp")
    add_custom_command(
        OUTPUT "${OUTFILE}" "${HEADER}"
        COMMAND "${BISON_EXECUTABLE}"
        ARGS "--name-prefix=yy"
        "--defines"
        "--output-file=${OUTFILE}"
        "${CMAKE_CURRENT_SOURCE_DIR}/${parser_FILE}"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${parser_FILE}"
    )
    set_source_files_properties("${OUTFILE}" "${HEADER}" PROPERTIES GENERATED TRUE)
    set_source_files_properties("${HEADER}" PROPERTIES HEADER_FILE_ONLY TRUE)
endmacro(BISON_FILE)

BISON_FILE(common/parser.yy)


# create lexer file
macro(FLEX_FILE lexer_FILE)
    GET_FILENAME_COMPONENT(output_FILE ${lexer_FILE} NAME)
    set(OUTFILE "${CMAKE_CURRENT_BINARY_DIR}/${output_FILE}.cpp")
    add_custom_command(
        OUTPUT "${OUTFILE}"
        COMMAND "${FLEX_EXECUTABLE}"
        ARGS "--prefix=yy"
        "--outfile=${OUTFILE}"
        "${CMAKE_CURRENT_SOURCE_DIR}/${lexer_FILE}"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${lexer_FILE}"
    )
    set_source_files_properties("${OUTFILE}" PROPERTIES GENERATED TRUE)
endmacro(FLEX_FILE)

FLEX_FILE(common/lexer.ll)

add_subdirectory(qml-kde)

include_directories(common)
include_directories(desktop)

# Continuing with normally schedule CMake action...
set(abakus_SRCS
    desktop/main.cpp
    desktop/mainwindow.cpp
    common/evaluator.cpp
    common/function.cpp
    common/functionmodel.cpp
    common/functionmodelitem.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/lexer.ll.cpp
    common/node.cpp
    common/numeralmodel.cpp
    common/numeralmodelitem.cpp
    common/numerictypes.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/parser.yy.cpp
    common/result.cpp
    common/resultmodel.cpp
    common/resultmodelitem.cpp
    common/settings.cpp
    common/settingscore.cpp
)

kde4_add_executable(abakus ${abakus_SRCS})
target_link_libraries(abakus ${MPFR_LIBRARIES} ${QT_QTDECLARATIVE_LIBRARY} ${KDECLARATIVE_LIBRARIES} ${KDE4_PLASMA_LIBS})

install(TARGETS abakus ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES desktop/abakus.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
