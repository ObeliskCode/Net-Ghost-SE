#!/usr/bin/python3
import os, sys, subprocess, ctypes, time, json

## Build Flags
#
# --windows [default:linux] - compile target for windows
# --gdb [default:disabled] - enables cmd debugger
# --wasm  - Emscripten WASM WEBGL
#
## Install Notes:
#  fedora: sudo dnf install glfw-devel openal-devel glm-devel
##

__thisdir = os.path.split(os.path.abspath(__file__))[0]
EMSDK = os.path.join(__thisdir, "emsdk")


def emsdk_update():
    subprocess.check_call(["git", "pull"], cwd=EMSDK)
    subprocess.check_call(["./emsdk", "install", "latest"], cwd=EMSDK)
    subprocess.check_call(["./emsdk", "activate", "latest"], cwd=EMSDK)


if "--wasm" in sys.argv and not os.path.isdir(EMSDK):
    cmd = [
        "git",
        "clone",
        "--depth",
        "1",
        "https://github.com/emscripten-core/emsdk.git",
    ]
    print(cmd)
    subprocess.check_call(cmd)
    emsdk_update()

EMCC = os.path.join(EMSDK, "upstream/emscripten/emcc")
if not EMCC and "--wasm" in sys.argv:
    emsdk_update()

if "--blender-install" in sys.argv:
    if "--blender-git" in sys.argv:
        if not os.path.isdir("./blender"):
            cmd = "git clone --depth 1 https://github.com/blender/blender.git"
            print(cmd)
            subprocess.check_call(cmd.split())
        cmd = "python3 ./blender/build_files/utils/make_update.py --no-libraries"
        print(cmd)
        subprocess.check_call(cmd.split(), cwd="./blender")
        subprocess.check_call(["make"], cwd="./blender")
    elif "fedora" in os.uname().nodename:
        os.system("sudo dnf install blender")
    else:
        os.system("sudo apt install blender")


BLENDER = "blender"


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
elif "--wasm" in sys.argv:
    CC = EMCC
    C = EMCC

else:
    CC = "g++"
    C = "gcc"


srcdir = os.path.join(__thisdir, "Source")
assert os.path.isdir(srcdir)
asset_dir = os.path.join(__thisdir, "Resources")
assert os.path.isdir(asset_dir)
shaders_dir = os.path.join(asset_dir, "shaders")
assert os.path.isdir(shaders_dir)

hacks = [
    "-I/usr/include/bullet",  ## this is the hack/workaround for bullet
]

includes = [
    "-I" + srcdir,
    "-I/usr/include/freetype2",
]

if "--wasm" in sys.argv:
    includes += [
        "-I/tmp",
    ]
    os.system("cp -Rv /usr/include/glm /tmp/.")


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
]

if not "--wasm" in sys.argv:
    libs += [
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
    if "fedora" in os.uname().nodename:
        cmd = "sudo dnf install glew-devel"
    else:
        cmd = "sudo apt-get install libglew-dev"
    print(cmd)
    subprocess.check_call(cmd.split())

if not os.path.isdir("/usr/include/assimp"):
    if "fedora" in os.uname().nodename:
        cmd = "sudo dnf install assimp-devel"
    else:
        cmd = "sudo apt-get install libassimp-dev"
    print(cmd)
    subprocess.check_call(cmd.split())


if not os.path.isdir("/usr/include/bullet"):
    if "fedora" in os.uname().nodename:
        cmd = "sudo dnf install bullet-devel"
    else:
        cmd = "sudo apt-get install libbullet-dev libopenal-dev"
    print(cmd)
    subprocess.check_call(cmd.split())

if not os.path.isdir("/usr/include/freetype2"):
    if "fedora" in os.uname().nodename:
        cmd = "sudo dnf install freetype-devel"
    else:
        cmd = "sudo apt-get install libfreetype-dev"
    print(cmd)
    subprocess.check_call(cmd.split())

NGHOST_HEADER = """
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
"""

NGHOST_GLFW = """
extern "C" void netghost_window_close(){
	glfwTerminate();
}
EMSCRIPTEN_KEEPALIVE
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
	std::cout << "window pointer:" << window << std::endl;
	Globals::get().screenWidth=w;
	Globals::get().screenHeight=h;
	Globals::get().camera->setDims(w,h);
	Globals::get().handCam->setDims(w,h);
	glViewport(0, 0, w, h);
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

"""

NGHOST_UPDATE = """
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
#ifndef EMSCRIPTEN
		glfwSetWindowTitle(window, newTitle.c_str());
#endif
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

"""

NGHOST_MAIN_WEB = """

void downloadSucceeded(emscripten_fetch_t *fetch) {
	printf("Finished downloading %llu bytes from URL %s.\\n", fetch->numBytes, fetch->url);
	// The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];
	emscripten_fetch_close(fetch); // Free data associated with the fetch.
}

void downloadFailed(emscripten_fetch_t *fetch) {
	printf("Downloading %s failed, HTTP failure status code: %d.\\n", fetch->url, fetch->status);
	emscripten_fetch_close(fetch); // Also free data on failure.
}


int main(){
	emscripten_fetch_attr_t attr;
	emscripten_fetch_attr_init(&attr);
	strcpy(attr.requestMethod, "GET");
	attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
	attr.onsuccess = downloadSucceeded;
	attr.onerror = downloadFailed;
	emscripten_fetch(&attr, "/Cube");
}

"""


def minify(f):
    if f.endswith(".glsl"):
        glsl = open(os.path.join(shaders_dir, f)).read()
    else:
        glsl = f
    o = []
    for ln in glsl.splitlines():
        if ln.strip().startswith("//"):
            continue
        if "//" in ln:
            print("WARN: inline comment in GLSL", ln)
            ln = ln.split("//")[0]
        o.append(ln.strip())

    return "\\n".join(o)

def get_default_shaders():
    shaders = {}
    for file in os.listdir(shaders_dir):
        if "Vert" in file:
            tag = file.split("Vert")[0]
            if tag not in shaders:
                shaders[tag] = {}
            shaders[tag]["vert"] = file
        elif "Frag" in file:
            tag = file.split("Frag")[0]
            if tag not in shaders:
                shaders[tag] = {}
            shaders[tag]["frag"] = file
        elif "Geom" in file:
            tag = file.split("Geom")[0]
            if tag not in shaders:
                shaders[tag] = {}
            shaders[tag]["geom"] = file
    return shaders

def genmain( gen_ctypes=None ):
    o = [
        "#define GLEW_STATIC",
        "#include <GL/glew.h>",
        "#include <GLFW/glfw3.h>",
        '#include "Scene.h"',
        #'#include "VBO.h"',
        "struct __vertex__{float x; float y; float z;};",
    ]
    if "--wasm" in sys.argv:
        o += [
            "#include <stdio.h>",
            "#include <string.h>",
            "#include <emscripten/fetch.h>",
        ]
    else:
        o.append('#define EMSCRIPTEN_KEEPALIVE')

    o += [
        NGHOST_HEADER,
        NGHOST_GLFW,
        NGHOST_UPDATE,
    ]
    if "--wasm" in sys.argv:
        o.append(NGHOST_MAIN_WEB)

    if gen_ctypes is not None:
        gen_ctypes['netghost_window_init'] = [ctypes.c_int, ctypes.c_int]

    font = None
    blends = []
    shaders = {}
    user_shader = 0
    for arg in sys.argv:
        if arg.endswith((".blend", ".json")):
            blends.append(arg)
        if arg.endswith(".json"):
            ## check if there are any shaders
            info = json.loads(open(arg).read())
            if info["shaders"]:
                shaders.update(info["shaders"])
        if arg.endswith(".ttf"):
            font = open(arg, "rb").read()

    if not font:
        defont = os.path.join(asset_dir, "fonts/arial.ttf")
        assert os.path.isfile(defont)
        print("using default font:", defont)
        font = open(defont, "rb").read()

    o += [
        "unsigned char __netghost_font__[] = {%s};" % ",".join(str(b) for b in font),
        "unsigned int   __netghost_font_size__ = %s;" % len(font),
    ]

    helper_funcs = []


    init_meshes = [
        'EMSCRIPTEN_KEEPALIVE',
        'extern "C" void netghost_init_meshes(){',
        "	Transform *trf;",
        "	Model *mdl;",
        "	unsigned int entID;",
    ]

    draw_loop = [
        'EMSCRIPTEN_KEEPALIVE',
        'extern "C" void netghost_redraw(){',
        "	Entity self;",
        "	glClearColor(1.0,0.5,0.5, 1.0);",
        "	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);",
        "	glEnable(GL_BLEND);",
        '	GUI::get().RenderText(*shader_text, "Hello World", 10, 10, 0.75f, glm::vec3(1.f, 1.f, 1.f));',
        "	glDisable(GL_BLEND);",
        "	glfwPollEvents();",
    ]

    init_cameras = [
        'EMSCRIPTEN_KEEPALIVE',
        'extern "C" void netghost_init_cameras(){',
        "	//Transform *trf;",
        "	//Model *mdl;",
        "	//unsigned int entID;",
    ]

    init_lights = [
        'EMSCRIPTEN_KEEPALIVE',
        'extern "C" void netghost_init_lights(){',
        "	//Transform *trf;",
        "	//Model *mdl;",
        "	//unsigned int entID;",
    ]

    if not blends:
        ## exports just the default Cube
        blends.append(None)
    for blend in blends:
        if blend and blend.endswith(".json"):
            info = json.loads(open(blend).read())
        else:
            cmd = [BLENDER]
            if blend:
                cmd.append(blend)
            cmd += ["--background", "--python", "./ghostblender.py", "--", "--dump"]
            print(cmd)
            subprocess.check_call(cmd)
            info = json.loads(open("/tmp/dump.json").read())

        shaders.update(info['shaders'])

        cameras = info["cameras"]
        for n in cameras:
            init_cameras += [
                '	//std::cout << "drawing: %s" << std::endl;' % n,
                "	//ECS::get().DrawEntity(__ID__%s);" % n,
            ]

        lights = info["lights"]
        for n in lights:
            init_lights += [
                '	//std::cout << "drawing: %s" << std::endl;' % n,
                "	//ECS::get().DrawEntity(__ID__%s);" % n,
            ]

        meshes = info["objects"]
        allprops = {}
        for n in meshes:
            if "props" in meshes[n]:
                for k in meshes[n]["props"]:
                    if k not in allprops:
                        allprops[k] = 1
                        draw_loop.append("float %s;" % k)

        for n in meshes:
            print(meshes[n])

            verts = ["{%sf,%sf,%sf}" % tuple(vec) for vec in meshes[n]["verts"]]
            norms = ["{%sf,%sf,%sf}" % tuple(vec) for vec in meshes[n]["normals"]]

            o.append("Mesh *mesh_%s;" % n)
            o.append("Transform *transform_%s;" % n)
            o.append(
                "static const __vertex__ _arr_%s[%s] = {%s};"
                % (n, len(verts), ",".join(verts))
            )
            o.append(
                "static const __vertex__ _narr_%s[%s] = {%s};"
                % (n, len(norms), ",".join(norms))
            )
            o.append("unsigned short __ID__%s;" % n)

            if "props" in meshes[n]:
                for k in meshes[n]["props"]:
                    val = meshes[n]["props"][k]
                    o.append("float %s_prop_%s = %s;" % (n, k, val))

            helper_funcs += [
                'EMSCRIPTEN_KEEPALIVE',
                'extern "C" void set_%s_pos(float x, float y, float z){' % n,
                '   transform_%s->setTranslation(glm::vec3(x, y, z));' % n,
                '}'
            ]
            if gen_ctypes is not None:
                gen_ctypes['set_%s_pos' % n] = [ctypes.c_float, ctypes.c_float, ctypes.c_float]

            draw_loop += [
                '	std::cout << "drawing: %s" << std::endl;' % n,
                "	ECS::get().DrawEntity(__ID__%s);" % n,
            ]
            if "scripts" in meshes[n] and meshes[n]["scripts"]:
                draw_loop.append("	self = ECS::get().getEntity(__ID__%s);" % n)

                if "props" in meshes[n]:
                    for k in meshes[n]["props"]:
                        ## gets global and sets to local
                        draw_loop.append("%s = %s_prop_%s;" % (k, n, k))

                for cpp in meshes[n]["scripts"]:
                    draw_loop.append(cpp)

                if "props" in meshes[n]:
                    for k in meshes[n]["props"]:
                        ## sets global to local
                        draw_loop.append("%s_prop_%s = %s;" % (n, k, k))

            indices = [str(i) for i in meshes[n]["indices"]]

            init_meshes += [
                "	std::vector<Vertex> _verts_%s;" % n,
                "	for (auto i=0; i<%s; i++){" % len(verts),
                "		auto x = _arr_%s[i].x;" % n,
                "		auto y = _arr_%s[i].y;" % n,
                "		auto z = _arr_%s[i].z;" % n,
                "		auto v = glm::vec3(x,y,z);",
                "		x = _narr_%s[i].x;" % n,
                "		y = _narr_%s[i].y;" % n,
                "		z = _narr_%s[i].z;" % n,
                "		auto norms = glm::vec3(x,y,z);",
                "		auto uv = glm::vec2(0,0);",  ## TODO
                "		_verts_%s.push_back(Vertex{v,norms,uv});" % n,
                "	}",

                "	static const auto _indices_%s = std::vector<GLuint>{%s};" % (n, ",".join(indices)),

                "	mesh_%s = new Mesh(_verts_%s, _indices_%s);" % (n, n, n),
                "	transform_%s = new Transform();" % n,
                "   trf = transform_%s;" % n,
                "	trf->setTranslation(glm::vec3(%sf, %sf, %sf));" % tuple(meshes[n]["pos"]),
                "	trf->setScale(glm::vec3(%sf, %sf, %sf));" % tuple(meshes[n]["scl"]),

                "	trf->setRotation(glm::vec3(%sf, %sf, %sf));" % tuple(meshes[n]["rot"]),

                "	mdl = new Model();",
                ## this probably should be a pointer to the mesh, not a copy.  using std::move here breaks the shareablity of the Mesh with other models
                "	mdl->meshes.push_back(*mesh_%s);" % n,
                '	std::cout << "mesh init: %s" << std::endl;' % n,
                "	entID = ECS::get().createEntity();",
                "	__ID__%s = (unsigned short)entID;" % n,
                "	ECS::get().addModel(entID, mdl);",
                "	ECS::get().addCamera(entID, Globals::get().camera);",
                "	ECS::get().addTransform(entID, trf);",
                "	ECS::get().addWireFrame(entID, 3.0f, 4.0f, 6.0f);",
            ]
            if "shader" in meshes[n]:
                sname = meshes[n]["shader"]
                init_meshes.append("ECS::get().addShader(entID, *shader_%s);" % sname)
            else:
                init_meshes.append("ECS::get().addShader(entID, *shader_wire);")

    init_meshes.append("}")
    init_cameras.append("}")
    init_lights.append("}")

    if not shaders:
    	## use all the default shaders in ./Resources/shaders/*.glsl
    	shaders.update( get_default_shaders() )
    else:
    	# note: the text shader is minimal required for engine
    	if 'text' not in shaders:
    		## the user could define their own? our text shader probably should be called __text__
    		shaders['text'] = get_default_shaders()['text']


    init_shaders = [
        'EMSCRIPTEN_KEEPALIVE',
        'extern "C" void netghost_init_shaders(){',
    ]
    for tag in shaders:
        s = shaders[tag]
        if "vert" in s and "frag" in s:
            o.append("Shader *shader_%s;" % tag)
            init_shaders += [
                '	std::cout << "shader init: %s" << std::endl;' % tag,
                "	shader_%s = new Shader();" % tag,
                '	shader_%s->set_vshader("%s");' % (tag, minify(s["vert"])),
                '	shader_%s->set_fshader("%s");' % (tag, minify(s["frag"])),
            ]
    init_shaders.append("}")


    draw_loop.append("	glfwSwapBuffers(window);")
    draw_loop.append("}")

    o = "\n".join(
        o + helper_funcs + init_shaders + init_cameras + init_lights + init_meshes + draw_loop
    )
    return o


def build(
    shared=True, assimp=False, wasm=False, debug_shaders="--debug-shaders" in sys.argv,
    gen_ctypes=False,
):
    cpps = []
    obfiles = []
    for file in os.listdir(srcdir):
        if file.endswith(".c"):
            ## this is just for drwave
            ofile = "/tmp/%s.o" % file
            obfiles.append(ofile)
            if os.path.isfile(ofile) and "--fast" in sys.argv:
                continue
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
            if os.path.isfile(ofile) and "--fast" in sys.argv:
                continue
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
            if not assimp:
                cmd.append("-DNOASS")
            if debug_shaders:
                cmd.append("-DDEBUG_SHADERS")
            cmd += includes
            cmd += hacks
            print(cmd)
            subprocess.check_call(cmd)

    tmp_main = "/tmp/__main__.cpp"
    tmpo = tmp_main + ".o"
    obfiles.append(tmpo)
    open(tmp_main, "w").write(genmain( gen_ctypes=gen_ctypes))
    cmd = [CC, "-std=c++20", "-c", "-fPIC", "-o", tmpo, tmp_main]
    if not assimp:
        cmd.append("-DNOASS")
    if debug_shaders:
        cmd.append("-DDEBUG_SHADERS")

    cmd += includes
    cmd += hacks
    print(cmd)
    subprocess.check_call(cmd)

    os.system("ls -lh /tmp/*.o")

    if wasm:
        cmd = (
            [
                EMCC,  #'--no-entry',
                #'-s', 'ERROR_ON_UNDEFINED_SYMBOLS=0',
                "-s",
                "FETCH",
                "-s",
                "SINGLE_FILE",
                "-s",
                "ENVIRONMENT=web",
                "-s",
                "WASM=1",
                "-s",
                "AUTO_JS_LIBRARIES",
                #'-s', 'MINIMAL_RUNTIME=2',  ## not compatible with glfw
                "-s",
                "USE_BULLET=1",
                "-s",
                "USE_FREETYPE=1",
                "-s",
                "USE_WEBGL2=1",
                "-s",
                "USE_GLFW=3",
                "-s",
                "NO_FILESYSTEM=1",
                "-o",
                "/tmp/netghost.html",
            ]
            + obfiles
            + libs
        )
        print(cmd)
        subprocess.check_call(cmd)
        return "/tmp/netghost.html"

    ## finally call the linker,
    ## note: there's better linkers we could use here, like gold and mold

    cmd = (
        [
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
    if shared:
        return ctypes.CDLL("/tmp/obelisk.so")

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


def bind_lib(lib, cdefs):
    #lib.netghost_window_init.argtypes = [ctypes.c_int, ctypes.c_int]
    for n in cdefs:
        func = getattr(lib, n)
        print('binding %s: args = %s ptr =%s' %(n,cdefs[n], func))
        func.argtypes = tuple(cdefs[n])

def test_python():
    from random import random
    gctypes = {}
    lib = build( gen_ctypes=gctypes )

    print(lib.netghost_window_init)
    print(lib.netghost_update)
    bind_lib(lib, gctypes)
    print("init_window")
    lib.netghost_window_init(320, 240)
    print("init_shaders")
    lib.netghost_init_shaders()
    print("init_cameras")
    lib.netghost_init_cameras()
    print("init_lights")
    lib.netghost_init_lights()
    print("init_meshes")
    lib.netghost_init_meshes()
    while True:
        print("redraw")
        lib.netghost_redraw()
        if 'set_Cube_pos' in gctypes:
            lib.set_Cube_pos(random(), random(), random())
        time.sleep(1)

    # lib.netghost_window_close()


def test_exe():
    exe = build(shared=False)
    if "--windows" in sys.argv:
        cmd = ["/tmp/obelisk.exe"]
    elif "--gdb" in sys.argv:
        cmd = ["gdb", "/tmp/obelisk"]
    else:
        cmd = ["/tmp/obelisk"]

    print(cmd)

    subprocess.check_call(cmd, cwd=asset_dir)


def test_wasm():
    lib = build(wasm=True)
    os.system("ls -lh %s" % lib)
    import webbrowser

    ## this is required because some browsers will not open files in /tmp
    os.system("cp -v %s ~/Desktop/netghost.html" % lib)
    webbrowser.open(os.path.expanduser("~/Desktop/netghost.html"))


if __name__ == "__main__":
    output = None
    for arg in sys.argv:
        if arg.startswith("--output="):
            output = arg.split("=")[-1]

    if output:
        if "--wasm" in sys.argv:
            lib = build(wasm=True)
            open(output, "wb").write(open(lib, "rb").read())

    else:
        if "--wasm" in sys.argv:
            test_wasm()
        else:
            test_python()
