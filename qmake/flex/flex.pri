flex.name = Flex ${QMAKE_FILE_IN}
flex.input = FLEXSOURCES
flex.output = $$BUILD_DIR/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}.cpp
flex.commands = flex -o $$BUILD_DIR/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}.cpp ${QMAKE_FILE_IN}
flex.CONFIG += target_predeps
flex.variable_out = GENERATED_SOURCES
silent:flex.commands = @echo Lex ${QMAKE_FILE_IN} && $$flex.commands
QMAKE_EXTRA_COMPILERS += flex
