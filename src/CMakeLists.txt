kde4_header()
project(abakus)

# Need to include checks for MPFR, GNU MP, FLEX, and BISON

include_directories(${KDE4_INCLUDES})

set(abakus_SRCS 
    abakus.cpp
    abakuslistview.cpp
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
    valuemanager.cpp
)

set(math_srcs)
set(math_libs)
set(MPFR_FOUND 1) # For now, XXX fix this
set(HAVE_FLEX_BISON 1) # For now, XXX fix this

if(MPFR_FOUND)
    set(math_libs mpfr gmp)
else(MPFR_FOUND)
    set(math_srcs hmath.cpp number.c)
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
kde4_add_dcop_skels(abakus_SRCS dcopIface.h)

kde4_add_executable(abakus ${abakus_SRCS} ${math_srcs} ${parser_srcs})
target_link_libraries(abakus ${KDE4_KDECORE_LIBS} ${math_libs} m kdeui)

install_targets(/bin abakus)

########### install files ###############

install_files(${XDG_APPS_DIR} FILES abakus.desktop)
install_files(${DATA_INSTALL_DIR}/abakus FILES abakusui.rc)

kde4_install_icons(${ICON_INSTALL_DIR} hicolor)

kde4_footer()
