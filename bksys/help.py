## Thomas Nagy, 2005

"""
Detect and store the most common options
* kdecxxflags  : debug=1 (-g) or debug=full (-g3, slower)
  else use the user CXXFLAGS if any, - or -O2 by default
* prefix : the installation path
* extraincludes : a list of paths separated by ':'
ie: scons configure debug=full prefix=/usr/local extraincludes=/tmp/include:/usr/local
"""

BOLD   ="\033[1m"
RED    ="\033[91m"
GREEN  ="\033[92m"
YELLOW ="\033[1m" #"\033[93m" # unreadable on white backgrounds
CYAN   ="\033[96m"
NORMAL ="\033[0m"

def exists(env):
	return true

def generate(env):
	## Bksys requires scons 0.96
	env.EnsureSConsVersion(0, 96)

	env._help = ''

	def addHelpText(env, text):
		env._help = env._help + text

	def helpText(env):
		text = env._help.replace(']', NORMAL)
		text = text.replace('b[', BOLD)
		text = text.replace('g[', GREEN)
		text = text.replace('r[', RED)
		text = text.replace('y[', YELLOW)
		text = text.replace('c[', CYAN)

		return text

	from SCons.Script.SConscript import SConsEnvironment
	SConsEnvironment.addHelpText = addHelpText
	SConsEnvironment.helpText = helpText
