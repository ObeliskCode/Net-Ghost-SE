#!/usr/bin/env python3
import subprocess, sys, os
try:
	import bpy
except:
	bpy = None

if not bpy:
	blender = 'blender'  # Linux
	if sys.platform == 'win32': # Windows
		blender = 'C:/Program Files/Blender Foundation/Blender 4.2/blender.exe'
	elif sys.platform == 'darwin': # Apple
		blender = '/Applications/Blender.app/Contents/MacOS/Blender'

	command = []
	user_opts   = []
	test_bevy   = False
	for i,arg in enumerate(sys.argv):
		if arg.endswith('.blend'):
			command.append(os.path.expanduser(arg))
		elif arg.endswith('.exe'): # Windows
			blender = arg
		elif arg.endswith('.app'): # Apple
			blender = arg + '/Contents/MacOS/Blender'
		elif arg.endswith(('blender', 'Blender')):
			blender = arg
		elif arg.startswith('--'):
			user_opts.append(arg)

	command = [blender] + command + [ '--python', __file__ ]

	if user_opts:
		command.append('--')
		command += user_opts
	print(command)

	subprocess.check_call(command)
	sys.exit()

## BLENDER PLUGIN ##
assert bpy
import json

## NetGhost Blender DNA/RNA
MAX_SCRIPTS_PER_OBJECT = 8
for i in range(MAX_SCRIPTS_PER_OBJECT):
	setattr(
		bpy.types.Object, 
		'netghost_script' + str(i), 
		bpy.props.PointerProperty(name='script%s'%i, type=bpy.types.Text)
	)


def netghost2json():
	dump = {}
	for ob in bpy.data.objects:
		if ob.type=='MESH':
			print('dumping mesh:', ob)
			dump[ob.name] = {
				'pos'  : list(ob.location),
				'rot'  : list(ob.rotation_euler),
				'scl'  : list(ob.scale),
				'verts': [(v.co.x,v.co.y,v.co.z) for v in ob.data.vertices],
				'normals': [(v.normal.x, v.normal.y, v.normal.z) for v in ob.data.vertices],
				'indices':[],
				'scripts':[],
			}
			if ob.parent:
				dump[ob.name]['parent'] = ob.parent.name
			for face in ob.data.polygons:
				for i in range(3):
					dump[ob.name]['indices'].append(face.vertices[i])
			for i in range(MAX_SCRIPTS_PER_OBJECT):
				txt = getattr(ob, 'netghost_script'+str(i))
				if txt:
					dump[ob.name]['scripts'].append( txt.as_string() )
			if ob.keys():
				dump[ob.name]['props'] = {}
				props = {}
				for k in ob.keys():
					if type(ob[k]) is float:  ## GOTCHA, there is other blender DNA/RNA hacks here
						props[k]= ob[k]
				if props:
					dump[ob.name]['props'] = props

	print(dump)
	return json.dumps(dump)

@bpy.utils.register_class
class NetGhostScriptsPanel(bpy.types.Panel):
	bl_idname = 'OBJECT_PT_NetGhost_Scripts_Panel'
	bl_label = 'NetGhost Scripts'
	bl_space_type = 'PROPERTIES'
	bl_region_type = 'WINDOW'
	bl_context = 'object'

	def draw(self, context):
		self.layout.label(text='Attach C++ Scripts')
		foundUnassignedScript = False
		for i in range(MAX_SCRIPTS_PER_OBJECT):
			hasProperty = getattr(context.active_object, 'netghost_script' + str(i)) != None
			if hasProperty or not foundUnassignedScript:
				self.layout.prop(context.active_object, 'netghost_script' + str(i))
			if not foundUnassignedScript:
				foundUnassignedScript = not hasProperty

@bpy.utils.register_class
class NetGhostExport(bpy.types.Operator):
	bl_idname = 'netghost.export'
	bl_label = 'Export EXE'
	@classmethod
	def poll (cls, context): return True
	def execute (self, context):
		tmpj = '/tmp/b2ghost.json'
		open(tmpj,'w').write( netghost2json() )
		cmd = ['python3', './build.py', tmpj]
		print(cmd)
		subprocess.check_call(cmd)
		return {'FINISHED'}

@bpy.utils.register_class
class NetGhostExportWasm(bpy.types.Operator):
	bl_idname = 'netghost.export_wasm'
	bl_label = 'Export WASM'
	@classmethod
	def poll (cls, context): return True
	def execute (self, context):
		tmpj = '/tmp/b2ghost.json'
		open(tmpj,'w').write( netghost2json() )
		cmd = ['python3', './build.py', '--wasm', tmpj]
		print(cmd)
		subprocess.check_call(cmd)
		return {'FINISHED'}


@bpy.utils.register_class
class NetGhostWorldPanel(bpy.types.Panel):
	bl_idname = 'WORLD_PT_NetGhostWorld_Panel'
	bl_label = 'NetGhost Export'
	bl_space_type = 'PROPERTIES'
	bl_region_type = 'WINDOW'
	bl_context = 'world'
	def draw(self, context):
		self.layout.operator('netghost.export_wasm', icon='CONSOLE')
		self.layout.operator('netghost.export', icon='CONSOLE')

TEST1 = '''

std::cout << "hello GHOST" << std::endl;
std::cout << "object ID=" << self.m_id << std::endl;
std::cout << "object transform-flag=" << self.transform_flag << std::endl;

'''

TEST2 = TEST1 + '''

std::cout << "object blender-prop myprop=" << myprop << std::endl;
myprop += 0.1;

'''


def test():
	txt = bpy.data.texts.new(name='my.c++.py')
	txt.from_string(TEST2)
	ob = bpy.data.objects['Cube']
	ob.netghost_script0 = txt
	ob['myprop'] = 1.0




if __name__=='__main__':
	test()

