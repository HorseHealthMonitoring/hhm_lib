# .. "Copyright (c) 2008 Robert B. Reese, Bryan A. Jones, J. W. Bruce ("AUTHORS")"
#    All rights reserved.
#    (R. Reese, reese_AT_ece.msstate.edu, Mississippi State University)
#    (B. A. Jones, bjones_AT_ece.msstate.edu, Mississippi State University)
#    (J. W. Bruce, jwbruce_AT_ece.msstate.edu, Mississippi State University)
#
#    Permission to use, copy, modify, and distribute this software and its
#    documentation for any purpose, without fee, and without written agreement is
#    hereby granted, provided that the above copyright notice, the following
#    two paragraphs and the authors appear in all copies of this software.
#
#    IN NO EVENT SHALL THE "AUTHORS" BE LIABLE TO ANY PARTY FOR
#    DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
#    OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE "AUTHORS"
#    HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#    THE "AUTHORS" SPECIFICALLY DISCLAIMS ANY WARRANTIES,
#    INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
#    AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
#    ON AN "AS IS" BASIS, AND THE "AUTHORS" HAS NO OBLIGATION TO
#    PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
#
#    Please maintain this header in its entirety when copying/modifying
#    these files.
#
# ****************************************************************
# SConstruct.py - Build all libraries and examples over many chips
# ****************************************************************
#  #. Install SCons.
#  #. Install the Microchip compiler. Make sure your path
#     includes the directories in which the compiler binaries
#     exist.
#  #. From the command line, change to the directory in which
#     this file lies.
#  #. Execute ``SCons``, which builds everything. Optionally use :doc:`runscons.bat <runscons.bat>` to filter through the resulting warnings.
#
#  The build process can be modified by passing options to
#  SCons. See ``SCons --help`` for options specific
#  to this build and ``SCons -H`` for generic SCons
#  options.
#
# .. contents::

import os
import psutil
from Scons_TaskProjectBuilder import TaskProjectBuilder
# Make sure SCons is recent enough.
EnsureSConsVersion(2, 0)

# Create a Microchip XC16 Construction Environment
# ================================================
# Define command-line options to set bootloader.
# The Environment below depends on opts, so this must go here instead of with
# the rest of the `Command-line options`_.
opts = Variables()
opts.Add(EnumVariable('BOOTLDR', 'Determines bootloader type', 'msu',
                    allowed_values=('msu', 'none')))

# Create the environment.
env = Environment(
        # Force SCons to set up with gnu tools to start
        # with reasonable defaults. Note: using platform = 'posix'
        # causes SCons to try to call fork() when executing programs
        # (such as compilers), which errors out on Windows.
        tools = ['gcc', 'gnulink', 'ar', 'zip', 'packaging'],
        options = opts,
        CPPPATH = [ '../pic24lib_all/lib/include', '../pic24lib_all/esos/include', '../pic24lib_all/esos/include/pic24', 'lib/imsai'],
        CC = 'xc16-gcc',
        LIBPATH = '.',
        AR = 'xc16-ar',
        LINK = 'xc16-gcc',
        # Copied and cobbled together from SCons\Tools\cc.py with mods
        CCCOM = '$CC -c -o $TARGET $CFLAGS $CCFLAGS $CPPFLAGS $_CPPDEFFLAGS $_CPPINCFLAGS $SOURCES',
        CCCCOMSTR = 'Compiling $SOURCES',
        # The warnings provide some lint-like checking. Omitted options: -Wstrict-prototypes -Wold-style-definition complains about void foo(), which should be void foo(void), but isn't worth the work to change.
        CCFLAGS = '-mcpu=33EP512GP806 -O1 -msmart-io=1 -omf=elf -Wall -Wextra -Wdeclaration-after-statement -Wlong-long -fdiagnostics-show-option -DHARDWARE_PLATFORM=EMBEDDED_F14 -DBUILT_ON_ESOS',
        LINKFLAGS = '-mcpu=33EP512GP806 -omf=elf -Wl,--heap=100,$LINKERSCRIPT,--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,--no-cpp',
        LINKERSCRIPT = '--script="bootloader/p33EP512GP806_bootldr.gld"',
        ARFLAGS = 'rcs',
        ARSTR = 'Create static library: $TARGET',
        OBJSUFFIX = '.o',
        PROGSUFFIX = '.elf',
        # Copy the host envrionment variables for our scons environment
        # so scons can find the build tools and related env vars.
        ENV = os.environ,

  )

# Create a bin2hex builder
# ------------------------
# Add the bin2hex function to the environment as a new builder
# This functions converts a binary (.elf or .cof) file to a hex file.
def bin2hex(
  # The name of the .elf/.cof file to be converted.
  binName,
  # An Environment in which to build these sources.
  buildEnvironment,
  # A string to serve as an alias for this build.
  aliasString):

  f = os.path.splitext(binName)[0]
  myHex = buildEnvironment.Hex(f, f)
  # Add this hex file to a convenient alias
  buildEnvironment.Alias(aliasString, myHex)

b2h = Builder(
        action = 'xc16-bin2hex $SOURCE -a -omf=elf',
        suffix = 'hex',
        src_suffix = 'elf')
env.Append(BUILDERS = {'Hex' : b2h})


# Command-line options
# --------------------
# adjust our default environment based on user command-line requests
dict = env.Dictionary()
if dict['BOOTLDR'] != 'msu':
    env.Replace(LINKERSCRIPT = '--script="p${MCU}.gld"')

# By default, run number_of_cpus*4 jobs at once. This only works if the --no-cpp option is passed to the linker; otherwise, the linker produces a temporary file in the root build directory, which gets overwritten and confused when multiple builds run. There's some nice examples and explanation for this in the `SCons user guide <http://www.scons.org/doc/production/HTML/scons-user/c2092.html#AEN2183>`_.
#
# Some results from running on my 8-core PC:, gathered from the Total build time returned by the --debug=time scons command-line option:
#
# ==  ==========  ===============  ============
# -j  Time (sec)  Time (hh:mm:ss)  Speedup
# ==  ==========  ===============  ============
# 32   303        0:05:03          11.66006601
# 16   348.7      0:05:49          10.13191855
#  8   510.9      0:08:31           6.915247602
#  4   916        0:15:16           3.8569869
#  2  1777        0:29:37           1.98818233
#  1  3533        0:58:53           1
# ==  ==========  ===============  ============

env.SetOption('num_jobs', psutil.cpu_count()*4)
print("Running with -j %d." % GetOption('num_jobs'))

# generate some command line help for our custom options
Help(opts.GenerateHelpText(env))
Help("""Additional targets:
  template-build: Build all .c/.h files which are produced by templates.
  zipit: Build an archive for distributing end-user library contents.
  bootloader: Build the bootloader binaries only.""")


libraryFiles = [ 'lib/imsai/esos_f14ui.c',
    'lib/imsai/revF16.c',
    'lib/imsai/esos_pic24_sensor.c',
    'lib/imsai/esos_comm_helpers.c',
    'lib/imsai/esos_menu_service.c',
    'lib/imsai/fcn_synth.c',
    'lib/imsai/t6_app_austin.c',
    'lib/imsai/CANTeamMembers.c',
    '../pic24lib_all/lib/src/pic24_clockfreq.c',
	  '../pic24lib_all/lib/src/pic24_configbits.c',
	  '../pic24lib_all/lib/src/pic24_timer.c',
	  '../pic24lib_all/lib/src/pic24_util.c',
	  '../pic24lib_all/lib/src/pic24_serial.c',
	  '../pic24lib_all/lib/src/pic24_uart.c',
    '../pic24lib_all/lib/src/pic24_ecan.c',
	  '../pic24lib_all/esos/src/esos.c',
	  '../pic24lib_all/esos/src/esos_comm.c',
	  '../pic24lib_all/esos/src/esos_cb.c',
	  '../pic24lib_all/esos/src/esos_mail.c',
    'lib/imsai/esos_ecan.c',
	  '../pic24lib_all/esos/src/pic24/esos_pic24_i2c.c',
	  '../pic24lib_all/esos/src/pic24/esos_pic24_irq.c',
	  '../pic24lib_all/esos/src/pic24/esos_pic24_rs232.c',
	  '../pic24lib_all/esos/src/pic24/esos_pic24_spi.c',
	  '../pic24lib_all/esos/src/pic24/esos_pic24_tick.c',
    'lib/imsai/esos_pic24_ecan.c' ]
library = env.Library(source = libraryFiles)
lcdLibrary = env.Library(source = [ 'lib/imsai/esos_pic24_lcd44780.c', 'lib/imsai/esos_lcd44780.c' ])
lcdNibbleLibrary = env.Library(source = [ 'lib/imsai/lcd_nibble/esos_pic24_lcd44780.c',  'lib/imsai/lcd_nibble/esos_lcd44780.c' ])

TaskProjectBuilder('task02', env, library, bin2hex, Glob).buildSubTaskProject('t2_rag1', 't2_rag1.c').buildSubTaskProject('t2_rag2', 't2_rag2.c').buildSubTaskProject('t2_rag3', 't2_rag3.c')
task3Env = env.Clone()
task3Env.Append(CPPPATH = ['task03'])
task3Main = Glob('task03/t3_app.c', True, True, True)[0]
task3Env.Program([ task3Main, 'task03/t3_app_chayne.c', 'task03/t3_app_austin.c', library ])
bin2hex(task3Main, task3Env, 'esos')


TaskProjectBuilder('task04', env, library, bin2hex, Glob) \
    .buildSubTaskProject('T4_SENSOR1', 't4_sensor1.c') \
    .buildSubTaskProject('T4_SENSOR2', 't4_sensor2.c', True, ['t4_sampleapp2_austin.c']) \
    .buildSubTaskProject('T4_SENSOR3', 't4_sensor3.c', True, ['t4_sensor3_app.c'])

taskFiveEnv = env.Clone()
taskFiveEnv.Append(CPPPATH = ['task05'])
lcdMain = Glob('task05/lcd_app.c', True, True, True)[0]
taskFiveEnv.Program([lcdMain, 'task05/lcd_app_lm60m.c', 'task05/lcd_app_pot.c', 'task05/t6_app_austin.c', lcdLibrary, library])
bin2hex(lcdMain, taskFiveEnv, 'esos')

taskFiveNibbleEnv = env.Clone()
taskFiveNibbleEnv.Append(CPPPATH = ['task05/task05_nibble'])
lcdNibbleMain = Glob('task05/task05_nibble/lcd_app.c', True, True, True)[0]
taskFiveNibbleEnv.Program([lcdNibbleMain, 'task05/task05_nibble/lcd_app_lm60m.c', 'task05/task05_nibble/lcd_app_pot.c', lcdNibbleLibrary, library])
bin2hex(lcdNibbleMain, taskFiveNibbleEnv, 'esos')

taskSixEnv = env.Clone()
taskSixEnv.Append(CPPPATH = ['task06'])
taskSixMain = Glob('task06/t6_app.c', True, True, True)[0]
taskSixEnv.Program([taskSixMain, library])
bin2hex(taskSixMain, taskSixEnv, 'esos')

taskSevenEnv = env.Clone()
taskSevenEnv.Append(CPPPATH = ['task07', 'task06'])
taskSevenMain = Glob('task07/test_menu_app.c', True, True, True)[0]
taskSevenEnv.Program([taskSevenMain, lcdLibrary, library])
bin2hex(taskSevenMain, taskSevenEnv, 'esos')

netIdMacros = [ 'IS_CHAYNE_BOARD', 'IS_TYLER_BOARD', 'IS_EMILY_BOARD', 'IS_AUSTIN_BOARD' ]
for netIdMacro in netIdMacros:
  vdir = '_'.join(['netId', netIdMacro])
  
  taskEightEnv = env.Clone()
  taskEightEnv.Append(CCFLAGS= ' -D' + netIdMacro)
  taskEightEnv.VariantDir(vdir, '.')
  taskEightEnv.Append(CPPPATH = ['task06'])
  taskEightMain = Glob(vdir + '/task08/can_app.c', True, True, True)[0]
  taskEightEnv.Program([taskEightMain, vdir + '/task08/can_menu.c', lcdLibrary, library])
  bin2hex(taskEightMain, taskEightEnv, 'esos')

  taskNineEnv = env.Clone()
  taskNineEnv.Append(CCFLAGS= ' -D' + netIdMacro)
  taskNineEnv.VariantDir(vdir, '.')
  taskNineEnv.Append(CPPPATH = ['task06'])
  taskNineMain = Glob(vdir + '/task09/can_app.c', True, True, True)[0]
  taskNineEnv.Program([taskNineMain, vdir + '/task09/can_menu.c', vdir + '/task09/can_fcn_synth.c', lcdLibrary, library])
  bin2hex(taskNineMain, taskNineEnv, 'esos')

# taskEightEnv.Append(CPPPATH = ['task08'])
# taskEightMain = Glob('task08/can_app.c', True, True, True)[0]
# taskEightEnv.Program([taskEightMain, 'task07/fcn_synth.c', 'task06/t6_app_austin.c', lcdLibrary, library])

testEnv = env.Clone()
for testFile in Glob('test/*.c', True, True, True):
  testEnv.Program([ testFile, lcdLibrary, library ])
  bin2hex(testFile, testEnv, 'esos')

