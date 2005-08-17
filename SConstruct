#! /usr/bin/env python

###################################################################
# LOAD THE ENVIRONMENT AND SET UP THE TOOLS
###################################################################

## Load the builders in config
tools    = [ 'default', 'help', 'generic', 'kde', 'abakus' ]
toolpath = [ './', './admin' ]

# Required as part of SCons
env = Environment(tools = tools, toolpath = toolpath)

# Pull in some default settings.
env.KDEuse("environ rpath nohelp")
#env.KDEuse("environ rpath lang_qt thread nohelp")

# If we're asking for help just go ahead and exit now.
if env['HELP']:
    print env.helpText()
    Exit()

if env['flex'] and env['bison']:
    env['PARSER_INCLUDED'] = True

# Export the environment so that SConscript files in subdirs can access it.
Export('env')

###################################################################
# SCRIPTS FOR BUILDING THE TARGETS
###################################################################

env.subdirs('src')

env.docfolder('doc/en', 'en', 'abakus/')
