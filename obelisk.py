#!/usr/bin/python3
## Happy 24th B-Day Noel,
## from Brett June 23, 2024
## version 5


## this script is run from the Obelisk-Engine folder
## git clone --depth 1 https://github.com/n6garcia/Obelisk-Engine.git

import os, sys, subprocess



if '--windows' in sys.argv:
	os.system('rm /tmp/*.o /tmp/*.exe')

	## https://stackoverflow.com/questions/43864159/mutex-is-not-a-member-of-std-in-mingw-5-3-0
	## TODO, not use std::mutex? seems like the only issue using win32 instead os posix
	#CC  = 'i686-w64-mingw32-g++-win32'
	#C   = 'i686-w64-mingw32-gcc-win32'


	CC  = 'i686-w64-mingw32-g++-posix'
	C   = 'i686-w64-mingw32-gcc-posix'

	if not os.path.isfile(os.path.join('/usr/bin/', CC)):
		cmd = 'sudo apt-get install mingw-w64 gcc-multilib g++-multilib'
		subprocess.check_call(cmd.split())

else:
	CC = 'g++'
	C  = 'gcc'

## https://github.com/n6garcia/Obelisk-Engine
## https://github.com/n6garcia/Obelisk-Models

## HACKS TODO FIX
'''

## ISSUE 1:
/home/rap/Obelisk-3D-Engine/Source/Physics.h:7:10: fatal error: btBulletDynamicsCommon.h: No such file or directory
    7 | #include "btBulletDynamicsCommon.h"


TODO FIX
    7 | #include <btBulletDynamicsCommon.h>


## ISSUE 2:
/home/rap/Obelisk-3D-Engine/Source/Skeleton.h:7:10: fatal error: Bone.h: No such file or directory
    7 | #include <Bone.h>

TODO FIX
    7 | #include "Bone.h"

(the double quotes mean include from your project)
(using < and > is for system include paths)


## ISSUE 3:
/home/rap/Obelisk-3D-Engine/Source/ECS.h:19:1: note: ‘std::unordered_set’ is defined in header ‘<unordered_set>’; did you forget to ‘#include <unordered_set>’?
   18 | #include <queue>
  +++ |+#include <unordered_set>
   19 | #include <vector>


## ISSUE 4:
/home/rap/Obelisk-3D-Engine/Source/SkeletalModel.cpp:4:10: error: ‘unordered_set’ is not a member of ‘std’
    4 |     std::unordered_set<GLuint> texIDs;
      |          ^~~~~~~~~~~~~
/home/rap/Obelisk-3D-Engine/Source/SkeletalModel.cpp:2:1: note: ‘std::unordered_set’ is defined in header ‘<unordered_set>’; did you forget to ‘#include <unordered_set>’?
    1 | #include "SkeletalModel.h"
  +++ |+#include <unordered_set>
    2 | 

## NON-ISSUE 5:
/home/rap/Obelisk-3D-Engine/Source/Audio.cpp:207:22: error: ‘erase_if’ is not a member of ‘std’; did you mean ‘enable_if’?
  207 |                 std::erase_if(mixer, [this](const int x) -> bool {
      |                      ^~~~~~~~
      |                      enable_if

EASY FIX: g++ -std=c++20

## ISSUE 6 (MINGW):
https://github.com/vlang/v/issues/13978
/usr/include/linux/errno.h:1:10: fatal error: asm/errno.h: No such file or directory
    1 | #include <asm/errno.h>

EASY FIX: sudo apt-get install gcc-multilib g++-multilib

## ISSUE 7 (MINGW):
/home/rap/Obelisk-3D-Engine/Source/Audio.h:54:10: error: ‘mutex’ in namespace ‘std’ does not name a type
   54 |     std::mutex mixerMutex;
      |          ^~~~~
/home/rap/Obelisk-3D-Engine/Source/Audio.h:13:1: note: ‘std::mutex’ is defined in header ‘<mutex>’; did you forget to ‘#include <mutex>’?
   12 | #include "dr_wav.h"
  +++ |+#include <mutex>

## ISSUE 8 (MINGW):

workaround is auto-download the pre-built libs from:
https://packages.msys2.org/
probably NOT.

ucrt on Fedora has packages!
https://github.com/msys2/setup-msys2/issues/293

/usr/bin/i686-w64-mingw32-ld: cannot find -lGL: No such file or directory
/usr/bin/i686-w64-mingw32-ld: cannot find -lGLU: No such file or directory
/usr/bin/i686-w64-mingw32-ld: cannot find -lGLEW: No such file or directory
/usr/bin/i686-w64-mingw32-ld: cannot find -lglfw: No such file or directory
/usr/bin/i686-w64-mingw32-ld: cannot find -lopenal: No such file or directory
/usr/bin/i686-w64-mingw32-ld: cannot find -lfreetype: No such file or directory
/usr/bin/i686-w64-mingw32-ld: cannot find -lBulletDynamics: No such file or directory
/usr/bin/i686-w64-mingw32-ld: cannot find -lBulletCollision: No such file or directory
/usr/bin/i686-w64-mingw32-ld: cannot find -lLinearMath: No such file or directory
/usr/bin/i686-w64-mingw32-ld: cannot find -lassimp: No such file or directory
/usr/bin/i686-w64-mingw32-ld: cannot find -lc: No such file or directory


'''

srcdir = os.path.abspath('./Source')
assert os.path.isdir(srcdir)


hacks = [
	'-I/usr/include/bullet',  ## this is the workaround for ISSUE 1
]

includes = [
	'-I' + srcdir,
	'-I/usr/include/freetype2',
]

'''
/usr/include/bits/wctype-wchar.h:38:27: error: conflicting declaration ‘typedef long unsigned int wctype_t’
   38 | typedef unsigned long int wctype_t;
      |                           ^~~~~~~~
In file included from /usr/share/mingw-w64/include/crtdefs.h:10,
                 from /usr/share/mingw-w64/include/stddef.h:7,
                 from /usr/lib/gcc/i686-w64-mingw32/12-win32/include/stddef.h:1,
                 from /usr/include/wchar.h:35,
                 from /usr/lib/gcc/i686-w64-mingw32/12-win32/include/c++/cwchar:44:
/usr/share/mingw-w64/include/corecrt.h:107:24: note: previous declaration as ‘typedef short unsigned int wctype_t’
  107 | typedef unsigned short wctype_t;
'''
## https://stackoverflow.com/questions/30417871/conflicting-declaration-when-cross-compiling-with-mingw-w64-on-linux

## https://stackoverflow.com/questions/44100474/c-include-path-on-x86-64-w64-mingw32-g-with-opengl

def fake_includes():
	if os.path.isdir('/tmp/fake'):
		return
	os.system('mkdir /tmp/fake/')
	os.system('cp -Rv /usr/include/GL /tmp/fake/.')
	os.system('cp -Rv /usr/include/GLFW /tmp/fake/.')
	os.system('cp -Rv /usr/include/glm /tmp/fake/.')
	os.system('cp -Rv /usr/include/assimp /tmp/fake/.')
	os.system('cp -Rv /usr/include/boost /tmp/fake/.')
	os.system('cp -Rv /usr/include/AL /tmp/fake/.')

if '--windows' in sys.argv:
	#includes += ['-I/usr/include']
	includes += ['-lopengl32', '-I/tmp/fake']
	fake_includes()

#/home/rap/Obelisk-3D-Engine/Source/Shader.h:10:10: fatal error: GL/glew.h: No such file or directory
#   10 | #include <GL/glew.h>


libs = [
	'-lGL',
	'-lGLU',
	'-lGLEW',
	'-lglfw',
	'-lopenal',
	#'-lbullet',
	'-lfreetype',
	'-lBulletDynamics',
	'-lBulletCollision',
	'-lLinearMath',
	'-lassimp',
	#'-lstdc',
	'-lm',
	'-lc',
	'-lstdc++',
]

glew = '/usr/include/GL/glew.h'
if not os.path.isfile(glew):
	cmd = 'sudo apt-get install libglew-dev'
	print(cmd)
	subprocess.check_call(cmd)

if os.path.isdir('./Libraries_linux/include/assimp'):
	#TODO build assimp from Libraries_linux/Libraries
else:
	if not os.path.isdir('/usr/include/assimp'):
		cmd = 'sudo apt-get install libassimp-dev'
		print(cmd)
		subprocess.check_call(cmd)


if not os.path.isdir('/usr/include/bullet'):
	cmd = 'sudo apt-get install libbullet-dev libopenal-dev'
	print(cmd)
	subprocess.check_call(cmd)

if not os.path.isdir('/usr/include/freetype2'):
	cmd = 'sudo apt-get install libfreetype-dev'
	print(cmd)
	subprocess.check_call(cmd)

obelisk_so = '/tmp/obelisk.so'

def build():
	cpps = []
	main = None
	obfiles = []

	for file in os.listdir(srcdir):

		if file=='Main.cpp':
			main = file
		print(file)
		if file.endswith('.c'):
			## this is just for drwave
			ofile = '/tmp/%s.o' % file
			obfiles.append(ofile)
			if os.path.isfile(ofile):
				## FASTER
				continue
			cpps.append(file)
			cmd = [
				#'gcc',
				C,
				'-c',     ## do not call the linker
				'-fPIC',  ## position indepenent code
				'-o', ofile, 
				os.path.join(srcdir, file),
			]
			print(cmd)
			subprocess.check_call(cmd)

		elif file.endswith('.cpp'):
			ofile = '/tmp/%s.o' % file
			obfiles.append(ofile)
			if os.path.isfile(ofile):
				## FASTER
				continue
			cpps.append(file)
			cmd = [
				#'g++', 
				CC,
				'-std=c++20',
				'-c',     ## do not call the linker
				'-fPIC',  ## position indepenent code
				'-o', ofile, 
				os.path.join(srcdir, file),
			]
			cmd += libs
			cmd += includes
			cmd += hacks
			print(cmd)
			subprocess.check_call(cmd)



	os.system('ls -lh /tmp/*.o')

	## finally call the linker,
	## note: there's better linkers we could use here, like gold and mold

	if '--windows' not in sys.argv:
		cmd = [
			#'ld',
			'g++',
			'-shared',
			'-o', '/tmp/obelisk.so'
		] + obfiles + libs

		print(cmd)
		subprocess.check_call(cmd)

	exe = '/tmp/obelisk'
	if '--windows' in sys.argv:
		exe += '.exe'
	cmd = [
		#'g++',
		CC,
		'-o', exe,
	]
	if '--windows' in sys.argv:
		cmd += '-static-libgcc -static-libstdc++ -static'.split()
	cmd += obfiles + libs

	print(cmd)

	subprocess.check_call(cmd)


#if not os.path.isfile(obelisk_so):
#	build()
build()


'''
https://pybullet.org/Bullet/phpBB3/viewtopic.php?t=12742

/usr/bin/ld: /tmp/Physics.cpp.o: undefined reference to symbol '_Z22btAlignedAllocInternalmi'
/usr/bin/ld: /lib/x86_64-linux-gnu/libLinearMath.so.3.24: error adding symbols: DSO missing from command line
collect2: error: ld returned 1 exit status

'''


asset_dir = os.path.abspath('./3D_OpenGL_Engine')
assert os.path.isdir(asset_dir)

models_dir = os.path.join(asset_dir, 'models')
if len(os.listdir(models_dir)) <= 1:  ## .gitignore :)
	cmd = 'git clone --depth 1 https://github.com/n6garcia/Obelisk-Models.git'
	print(cmd)
	subprocess.check_call(cmd.split(), cwd=models_dir)
	os.system('mv -v %s/Obelisk-Models/* %s/.' %(models_dir, models_dir))

if '--windows' in sys.argv:
	cmd = ['/tmp/obelisk.exe']
else:
	cmd = ['gdb', '/tmp/obelisk']
print(cmd)


subprocess.check_call(cmd, cwd=asset_dir)
#sys.exit()

## TODO: get working from python for testing
import ctypes
os.chdir(asset_dir)

dll = ctypes.CDLL(obelisk_so)
print(dll)

print(dll.main)
dll.main()  ## this segfaults TODO FIX ME NOEL

## TODO: NOEL, connect some golang networking code with obelisk_so

GO = '''
package main
import "C"
//export add
func add(a, b int64) int64 {
    return a + b
}
func main() {}
'''
open('/tmp/myprog.go', 'wb').write( GO.encode('utf-8'))
cmd = ['go', 'build', '-o', '/tmp/mygolib.so', '-buildmode=c-shared', '/tmp/myprog.go']
print(cmd)
subprocess.check_call(cmd)
golib = ctypes.CDLL('/tmp/mygolib.so')
print(golib)
print(golib.add)
print(golib.add(10, 100))

