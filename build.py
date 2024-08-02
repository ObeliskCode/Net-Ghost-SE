#!/usr/bin/python3

## LINUX ONLY

## Build Flags
#
# --windows [default:linux] - compile target for windows
# --gdb [default:disabled] - enables cmd debugger
# --gen-main [default:disabled] - generates c++ from python
#
##


import os, sys, subprocess

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

## https://github.com/n6garcia/Obelisk-Engine
## https://github.com/n6garcia/Obelisk-Models


srcdir = os.path.abspath("./Source")
assert os.path.isdir(srcdir)


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
    #'-lbullet',
    "-lfreetype",
    "-lBulletDynamics",
    "-lBulletCollision",
    "-lLinearMath",
    "-lassimp",
    #'-lstdc',
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

obelisk_so = "/tmp/obelisk.so"

## BUILD CODE ##

gen_main = "--gen-main" in sys.argv
test_main = "--test-main" in sys.argv


def testmain():
    o = []

    BPLATE = """
    int* data = new int[20];
    for(int i = 0; i < 20; i++){
        data[i] = i;
        std::cout << std::to_string(data[i]) << std::endl;
    }
    void* ret = dae.blockingProcess(TestFunc(20), (void*)data);
    int* retList = (int*)ret;
    for(int i = 0; i < 20; i++){
        std::cout << std::to_string(retList[i]) << std::endl;
    }
    """

    o.extend(
        [
            '#include "Run.h"',
            '#include "Daemon.h"',
            "int main(int argc, char **argv) {",
            BPLATE,
        ]
    )

    o.append("}")
    o = "\n".join(o)
    return o


def genmain():
    o = []

    BPLATE = """
    return run();
    """

    o.extend(
        [
            '#include "Run.h"',
            "int main(int argc, char **argv) {",
            BPLATE,
        ]
    )

    o.append("}")
    o = "\n".join(o)
    return o


def build():
    cpps = []
    obfiles = []

    for file in os.listdir(srcdir):

        if file == "Main.cpp":
            if gen_main:
                open("/tmp/gen.main.cpp", "wb").write(genmain().encode("utf-8"))
                file = "/tmp/gen.main.cpp"
                ofile = "%s.o" % file
                obfiles.append(ofile)
                # if os.path.isfile(ofile):
                #    continue
                cpps.append(file)
                cmd = [
                    #'g++',
                    CC,
                    "-std=c++20",
                    "-c",  ## do not call the linker
                    "-fPIC",  ## position indepenent code
                    "-o",
                    ofile,
                    os.path.join(srcdir, file),
                ]
                cmd += libs
                cmd += includes
                cmd += hacks
                print(cmd)
                subprocess.check_call(cmd)
            elif test_main:
                open("/tmp/gen.main.cpp", "wb").write(testmain().encode("utf-8"))
                file = "/tmp/gen.main.cpp"
                ofile = "%s.o" % file
                obfiles.append(ofile)
                # if os.path.isfile(ofile):
                #    continue
                cpps.append(file)
                cmd = [
                    #'g++',
                    CC,
                    "-std=c++20",
                    "-c",  ## do not call the linker
                    "-fPIC",  ## position indepenent code
                    "-o",
                    ofile,
                    os.path.join(srcdir, file),
                ]
                cmd += libs
                cmd += includes
                cmd += hacks
                print(cmd)
                subprocess.check_call(cmd)
            else:
                ofile = "/tmp/%s.o" % file
                obfiles.append(ofile)
                # if os.path.isfile(ofile):
                #    continue
                cpps.append(file)
                cmd = [
                    #'g++',
                    CC,
                    "-std=c++20",
                    "-c",  ## do not call the linker
                    "-fPIC",  ## position indepenent code
                    "-o",
                    ofile,
                    os.path.join(srcdir, file),
                ]
                cmd += libs
                cmd += includes
                cmd += hacks
                print(cmd)
                subprocess.check_call(cmd)

        elif file.endswith(".c"):
            ## this is just for drwave
            ofile = "/tmp/%s.o" % file
            obfiles.append(ofile)
            # if os.path.isfile(ofile):
            #    continue
            cpps.append(file)
            cmd = [
                #'gcc',
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
            # if os.path.isfile(ofile):
            #    continue
            cpps.append(file)
            cmd = [
                #'g++',
                CC,
                "-std=c++20",
                "-c",  ## do not call the linker
                "-fPIC",  ## position indepenent code
                "-o",
                ofile,
                os.path.join(srcdir, file),
            ]
            cmd += libs
            cmd += includes
            cmd += hacks
            print(cmd)
            subprocess.check_call(cmd)

    os.system("ls -lh /tmp/*.o")

    ## finally call the linker,
    ## note: there's better linkers we could use here, like gold and mold

    cmd = (
        [
            #'ld',
            "g++",
            "-shared",
            "-o",
            "/tmp/obelisk.so",
        ]
        + obfiles
        + libs
    )

    print(cmd)
    subprocess.check_call(cmd)

    exe = "/tmp/obelisk"
    if "--windows" in sys.argv:
        exe += ".exe"
    cmd = [
        #'g++',
        CC,
        "-o",
        exe,
    ]
    if "--windows" in sys.argv:
        cmd += "-static-libgcc -static-libstdc++ -static".split()
    cmd += obfiles + libs

    print(cmd)

    subprocess.check_call(cmd)


## BUILDING / RUNNING ##

build()

asset_dir = os.path.abspath("./3D_OpenGL_Engine")
assert os.path.isdir(asset_dir)

# possibly install models if needed
models_dir = os.path.join(asset_dir, "models")
if len(os.listdir(models_dir)) <= 1:  ## .gitignore :)
    cmd = "git clone --depth 1 https://github.com/n6garcia/Obelisk-Models.git"
    print(cmd)
    subprocess.check_call(cmd.split(), cwd=models_dir)
    os.system("mv -v %s/Obelisk-Models/* %s/." % (models_dir, models_dir))

if "--windows" in sys.argv:
    cmd = ["/tmp/obelisk.exe"]
elif "--gdb" in sys.argv:
    cmd = ["gdb", "/tmp/obelisk"]
else:
    cmd = ["/tmp/obelisk"]

print(cmd)

subprocess.check_call(cmd, cwd=asset_dir)
# sys.exit()

## TODO: get working from python for testing
import ctypes

os.chdir(asset_dir)

dll = ctypes.CDLL(obelisk_so)
print(dll)

print(dll.main)
dll.main()

## TODO: NOEL, connect some golang networking code with obelisk_so

GO = """
package main
import "C"
//export add
func add(a, b int64) int64 {
    return a + b
}
func main() {}
"""
open("/tmp/myprog.go", "wb").write(GO.encode("utf-8"))
cmd = ["go", "build", "-o", "/tmp/mygolib.so", "-buildmode=c-shared", "/tmp/myprog.go"]
print(cmd)
subprocess.check_call(cmd)
golib = ctypes.CDLL("/tmp/mygolib.so")
print(golib)
print(golib.add)
print(golib.add(10, 100))
