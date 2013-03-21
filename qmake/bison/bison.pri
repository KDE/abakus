bison.name = Bison ${QMAKE_FILE_IN}
bison.input = BISONSOURCES
bison.output = $$BUILD_DIR/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}.cpp
bison.commands = bison -d -o $$BUILD_DIR/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}.cpp ${QMAKE_FILE_IN}
bison.CONFIG += target_predeps
bison.variable_out = GENERATED_SOURCES
silent:bison.commands = @echo Bison ${QMAKE_FILE_IN} && $$bison.commands
QMAKE_EXTRA_COMPILERS += bison
bison_header.input = BISONSOURCES
bison_header.output = $$BUILD_DIR/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}.hpp
bison_header.commands = bison -d -o $$BUILD_DIR/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}.cpp ${QMAKE_FILE_IN}
bison_header.CONFIG += target_predeps no_link
silent:bison_header.commands = @echo Bison ${QMAKE_FILE_IN} && $$bison.commands
QMAKE_EXTRA_COMPILERS += bison_header
