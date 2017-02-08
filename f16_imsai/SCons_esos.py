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
# **************************************************
# SCons_esos.py - Build ESOS chapter 14 applications
# **************************************************
import os
Import('env bin2hex')
from Scons_TaskProjectBuilder import TaskProjectBuilder

esosFiles = [ 'lib/imsai/revF16.c',
      'lib/imsai/esos_pic24_sensor.c',
      'lib/imsai/esos_comm_helpers.c',
      'lib/pic24/src/pic24_clockfreq.c',
	  'lib/pic24/src/pic24_configbits.c',
	  'lib/pic24/src/pic24_timer.c',
	  'lib/pic24/src/pic24_util.c',
	  'lib/pic24/src/pic24_serial.c',
	  'lib/pic24/src/pic24_uart.c',
	  'lib/esos/src/esos.c',
	  'lib/esos/src/esos_comm.c',
	  'lib/esos/src/esos_cb.c',
	  'lib/esos/src/esos_mail.c',
	  'lib/esos/src/pic24/esos_pic24_i2c.c',
	  'lib/esos/src/pic24/esos_pic24_irq.c',
	  'lib/esos/src/pic24/esos_pic24_rs232.c',
	  'lib/esos/src/pic24/esos_pic24_spi.c',
	  'lib/esos/src/pic24/esos_pic24_tick.c' ]
esosLibrary = env.StaticLibrary(source = esosFiles)


#TaskProjectBuilder('task02', env, esosLibrary, bin2hex, Glob).buildSubTaskProject('t2_rag1', 't2_rag1.c').buildSubTaskProject('t2_rag2', 't2_rag2.c').buildSubTaskProject('t2_rag3', 't2_rag3.c')
task3Env = env.Clone()
task3Env.Append(CPPPATH = ['task03'])
task3Main = Glob('task03/t3_app.c', True, True, True)[0]
task3Env.Program([ task3Main, 'task03/esos_f14ui.c', 'task03/t3_app_chayne.c', 'task03/t3_app_austin.c', esosLibrary ])
bin2hex(task3Main, task3Env, 'esos')

task4Env = env.Clone()
task4Env.Append(CPPPATH = ['task04'])
task4Main = Glob('test/adc_test.c', True, True, True)[0]
task4Env.Program([ task4Main, esosLibrary ])
bin2hex(task4Main, task4Env, 'esos')
