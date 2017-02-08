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
# .. highlight:: Python
#
# ******************************************
# SCons_bootloader.py - Build the bootloader
# ******************************************
# This builds the bootloader for a given configuration. It should be invoked
# from the main SConstruct.py script.
import os
Import('env bin2hex')

## Inform SCons about the dependencies in the template-based files
SConscript('templates/SConscript.py', 'env')

targetName = 'p${MCU}_${HW}_bootloader'
# Compile the bootloader to a .cof file.
env.Program(target=targetName, source=
  ['bootloader/pic24_dspic33_bootloader.X/main.c',
   'bootloader/pic24_dspic33_bootloader.X/mem.c',
   'bootloader/pic24_dspic33_bootloader.X/pic24_lib-small.c',
   'lib/src/pic24_clockfreq.c',
   'lib/src/pic24_uart.c',
   'lib/src/pic24_configbits.c'])
# Convert it to a .hex
bin2hex(targetName, env, 'bootloader')
# Copy the .hex to the hex/ directory.
env.Command('../../hex/' + targetName + '.hex', targetName + '.hex', Copy("$TARGET", "$SOURCE"))
