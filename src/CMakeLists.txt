
project(abakus CXX)

# Need to include checks for MPFR, GNU MP, FLEX, and BISON

include_directories(${KDE4_INCLUDES})

set(abakus_SRCS 
    abakus.cpp
    abakuslistview.cpp
    dcopIface.cpp
    dragsupport.cpp
    editor.cpp
    evaluator.cpp
    function.cpp
    mainwindow.cpp
    node.cpp
    numerictypes.cpp
    result.cpp
    resultlistview.cpp
    resultlistviewtext.cpp
    rpnmuncher.cpp
    token.cpp
    valuemanager.cpp
)

set(math_srcs)
set(math_libs)
set(HAVE_FLEX_BISON 1) # For now, XXX fix this

if(MPFR_FOUND)
    set(math_libs mpfr gmp)
    message(STATUS "Using MPFR")
else(MPFR_FOUND)
    set(math_srcs hmath.cpp number.cpp)
    message(STATUS "Using built-in high precision math.")
endif(MPFR_FOUND)

set(parser_srcs)
if(HAVE_FLEX_BISON)
    add_custom_target(Parser echo "Generating parser.")

    # Add command to run the lexer.
    add_custom_command(
	OUTPUT  "${CMAKE_BINARY_DIR}/src/lexer_lex.cpp"
	DEPENDS "${CMAKE_SOURCE_DIR}/src/lexer.ll"
	COMMAND flex
	ARGS    -o"${CMAKE_BINARY_DIR}/src/lexer_lex.cpp"
	        "${CMAKE_SOURCE_DIR}/src/lexer.ll"
	)

    # Add command to run the parser.
    add_custom_command(
	OUTPUT "${CMAKE_BINARY_DIR}/src/parser_yacc.cpp"
	#        "${CMAKE_BINARY_DIR}/src/parser_yacc.hpp"
	DEPENDS "${CMAKE_SOURCE_DIR}/src/parser.yy"
	        "${CMAKE_BINARY_DIR}/src/lexer_lex.cpp"
	COMMAND bison
	ARGS    "-o${CMAKE_BINARY_DIR}/src/parser_yacc.cpp"
	        -d "${CMAKE_SOURCE_DIR}/src/parser.yy"
	)

    set(parser_srcs ${CMAKE_BINARY_DIR}/src/parser_yacc.cpp
	            ${CMAKE_BINARY_DIR}/src/lexer_lex.cpp
		    )
    set_source_files_properties(
	${CMAKE_BINARY_DIR}/src/parser_yacc.cpp
	${CMAKE_BINARY_DIR}/src/parser_yacc.hpp
	${CMAKE_BINARY_DIR}/src/lexer_lex.cpp
	GENERATED
	)

    include_directories(${CMAKE_BINARY_DIR}/src)
else(HAVE_FLEX_BISON)
    set(parser_srcs parser_yacc.cpp lexer_lex.cpp)
endif(HAVE_FLEX_BISON)

kde4_automoc(${abakus_SRCS})
kde4_add_ui_files(abakus_SRCS "MainWindow.ui")

kde4_add_executable(abakus ${abakus_SRCS} ${math_srcs} ${parser_srcs})
target_link_libraries(abakus ${KDE4_KDEUI_LIBS} ${math_libs} m )

install(TARGETS abakus DESTINATION ${BIN_INSTALL_DIR})

########### install files ###############

install( FILES abakus.desktop DESTINATION ${XDG_APPS_DIR} )
install( FILES abakusui.rc DESTINATION ${DATA_INSTALL_DIR}/abakus )

kde4_install_icons(${ICON_INSTALL_DIR} hicolor)


