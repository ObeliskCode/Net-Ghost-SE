#!/usr/bin/python3

## LINUX ONLY

## Build Flags
#
# --windows [default:linux] - compile target for windows
# --gdb [default:disabled] - enables cmd debugger
#
##


import os, sys, subprocess, ctypes, time

## SETUP / INSTALL ##

if "--windows" in sys.argv:
	os.system("rm /tmp/*.o /tmp/*.exe")

	## https://stackoverflow.com/questions/43864159/mutex-is-not-a-member-of-std-in-mingw-5-3-0
	## TODO, not use std::mutex? seems like the only issue using win32 instead os posix
	# CC  = 'i686-w64-mingw32-g++-win32'
	# C   = 'i686-w64-mingw32-gcc-win32'

	CC = "i686-w64-mingw32-g++-posix"
	C = "i686-w64-mingw32-gcc-posix"

	if not os.path.isfile(os.path.join("/usr/bin/", CC)):
		cmd = "sudo apt-get install mingw-w64 gcc-multilib g++-multilib"
		subprocess.check_call(cmd.split())

else:
	CC = "g++"
	C = "gcc"


srcdir = os.path.abspath("./Source")
assert os.path.isdir(srcdir)

asset_dir = os.path.abspath("./3D_OpenGL_Engine")
assert os.path.isdir(asset_dir)

shaders_dir = os.path.join(asset_dir, 'shaders')
assert os.path.isdir(shaders_dir)

hacks = [
	"-I/usr/include/bullet",  ## this is the hack/workaround for bullet
]

includes = [
	"-I" + srcdir,
	"-I/usr/include/freetype2",
]


def fake_includes():
	if os.path.isdir("/tmp/fake"):
		return
	os.system("mkdir /tmp/fake/")
	os.system("cp -Rv /usr/include/GL /tmp/fake/.")
	os.system("cp -Rv /usr/include/GLFW /tmp/fake/.")
	os.system("cp -Rv /usr/include/glm /tmp/fake/.")
	os.system("cp -Rv /usr/include/assimp /tmp/fake/.")
	os.system("cp -Rv /usr/include/boost /tmp/fake/.")
	os.system("cp -Rv /usr/include/AL /tmp/fake/.")


if "--windows" in sys.argv:
	# includes += ['-I/usr/include']
	includes += ["-lopengl32", "-I/tmp/fake"]
	fake_includes()

libs = [
	"-lGL",
	"-lGLU",
	"-lGLEW",
	"-lglfw",
	"-lopenal",
	"-lfreetype",
	"-lBulletDynamics",
	"-lBulletCollision",
	"-lLinearMath",
	"-lassimp",
	"-lm",
	"-lc",
	"-lstdc++",
]

glew = "/usr/include/GL/glew.h"
if not os.path.isfile(glew):
	cmd = "sudo apt-get install libglew-dev"
	print(cmd)
	subprocess.check_call(cmd)

if not os.path.isdir("/usr/include/assimp"):
	cmd = "sudo apt-get install libassimp-dev"
	print(cmd)
	subprocess.check_call(cmd)


if not os.path.isdir("/usr/include/bullet"):
	cmd = "sudo apt-get install libbullet-dev libopenal-dev"
	print(cmd)
	subprocess.check_call(cmd)

if not os.path.isdir("/usr/include/freetype2"):
	cmd = "sudo apt-get install libfreetype-dev"
	print(cmd)
	subprocess.check_call(cmd)

NGHOST_HEADER = '''
GLFWwindow *window;
Scene *bp;
double crntTime = 0.0;
double timeStart = -1.0;
double prevTime = timeStart;
double timeDiff;
unsigned int counter = 0;
double lastFrame = timeStart;
double deltaTime = 1.0 / 188.0;
double frameTime = 0.0f;
double lastTick = timeStart;
double thisTick = 0.0;
double delta;
'''

NGHOST_GLFW = '''
extern "C" void netghost_window_close(){
	glfwTerminate();
}
extern "C" void netghost_window_init(int w, int h) {
	glfwInit();
	window = glfwCreateWindow(w, h, "NetGhost", NULL, NULL); // windowed
	if (window == NULL) {
		printf("Failed to create GLFW window!\\n");
		return;
	}
	glfwMakeContextCurrent(window);
	if (GLEW_OK != glewInit()){
		printf("Failed to initialize GLEW!.\\n");
		return;
	}

	// enable depth buffer
	glEnable(GL_DEPTH_TEST);
	// emable stencil test
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	glStencilMask(0xFF);
	// enable 8x MSAA
	glfwWindowHint(GLFW_SAMPLES, 8);
	glEnable(GL_MULTISAMPLE);
	// enable smooth shading vs flat
	glShadeModel(GL_SMOOTH);
	// Enable Culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CW);
	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
	// disable vsync if enabled
	glfwSwapInterval(0);
	// enable transparency function
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);

	timeStart = glfwGetTime();
}

'''

NGHOST_UPDATE = '''
extern "C" void netghost_update(){
	crntTime = glfwGetTime();

	/* FPS counter */
	timeDiff = crntTime - prevTime;
	counter++;
	if (timeDiff >= 1.0)
	{
		std::string FPS = std::to_string((1.0 / timeDiff) * counter);
		std::string ms = std::to_string((timeDiff / counter) * 1000);
		std::string newTitle = "Obelisk Engine - " + FPS + "FPS / " + ms + "ms";
		glfwSetWindowTitle(window, newTitle.c_str());
		prevTime = crntTime;
		counter = 0;
	}

	glfwPollEvents(); // get inputs
	frameTime = crntTime - lastFrame;
	lastFrame = crntTime;
	thisTick = glfwGetTime();
	delta = thisTick - lastTick;
	if (delta >= deltaTime){
		lastTick = thisTick;
		bp->tick(window, delta);
	}
	bp->drawFrame(window, frameTime);

}

'''

def genmain():
	o = [
		'#define GLEW_STATIC',
		'#include <GL/glew.h>',
		'#include <GLFW/glfw3.h>',
		'#include "Scene.h"',
		NGHOST_HEADER,
		NGHOST_GLFW,
		NGHOST_UPDATE,
	]
	o = "\n".join(o)
	return o


def build(shared=True):
	cpps = []
	obfiles = []
	for file in os.listdir(srcdir):
		if file.endswith(".c"):
			## this is just for drwave
			ofile = "/tmp/%s.o" % file
			obfiles.append(ofile)
			if os.path.isfile(ofile) and '--fast' in sys.argv: continue
			cpps.append(file)
			cmd = [
				C,
				"-c",  ## do not call the linker
				"-fPIC",  ## position indepenent code
				"-o",
				ofile,
				os.path.join(srcdir, file),
			]
			print(cmd)
			subprocess.check_call(cmd)

		elif file.endswith(".cpp"):
			ofile = "/tmp/%s.o" % file
			obfiles.append(ofile)
			if os.path.isfile(ofile) and '--fast' in sys.argv: continue
			cpps.append(file)
			cmd = [
				CC,
				"-std=c++20",
				"-c",  ## do not call the linker
				"-fPIC",  ## position indepenent code
				"-o",
				ofile,
				os.path.join(srcdir, file),
			]
			cmd += includes
			cmd += hacks
			print(cmd)
			subprocess.check_call(cmd)

	tmp_main = '/tmp/__main__.cpp'
	tmpo = tmp_main + '.o'
	obfiles.append(tmpo)
	open(tmp_main,'w').write(genmain())
	cmd = [CC, "-std=c++20", "-c", "-fPIC",  "-o",tmpo, tmp_main]
	cmd += includes
	cmd += hacks
	print(cmd)
	subprocess.check_call(cmd)

	os.system("ls -lh /tmp/*.o")

	## finally call the linker,
	## note: there's better linkers we could use here, like gold and mold

	cmd = [
		"g++",
		"-shared",
		"-o",
		"/tmp/obelisk.so",
		] + obfiles + libs
	print(cmd)
	subprocess.check_call(cmd)
	if shared:
		return ctypes.CDLL('/tmp/obelisk.so')

	exe = "/tmp/obelisk"
	if "--windows" in sys.argv:
		exe += ".exe"
	cmd = [
		CC,
		"-o",
		exe,
	]
	if "--windows" in sys.argv:
		cmd += "-static-libgcc -static-libstdc++ -static".split()
	cmd += obfiles + libs
	print(cmd)
	subprocess.check_call(cmd)
	return exe

def bind_lib(lib):
	lib.netghost_window_init.argtypes = [ctypes.c_int, ctypes.c_int]

def test_python():
	lib = build()
	print(lib.netghost_window_init)
	print(lib.netghost_update)
	bind_lib(lib)
	lib.netghost_window_init(320, 240)
	time.sleep(5)
	lib.netghost_window_close()

def test_exe():
	exe = build(shared=False)

	# possibly install models if needed
	#models_dir = os.path.join(asset_dir, "models")
	#if len(os.listdir(models_dir)) <= 1:  ## .gitignore :)
	#    cmd = "git clone --depth 1 https://github.com/n6garcia/Obelisk-Models.git"
	#    print(cmd)
	#    subprocess.check_call(cmd.split(), cwd=models_dir)
	#    os.system("mv -v %s/Obelisk-Models/* %s/." % (models_dir, models_dir))

	if "--windows" in sys.argv:
		cmd = ["/tmp/obelisk.exe"]
	elif "--gdb" in sys.argv:
		cmd = ["gdb", "/tmp/obelisk"]
	else:
		cmd = ["/tmp/obelisk"]

	print(cmd)

	subprocess.check_call(cmd, cwd=asset_dir)


if __name__=='__main__':
	test_python()

