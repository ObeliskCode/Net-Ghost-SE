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

TEST1 = '''

std::cout << "hello GHOST" << std::endl;

'''



def test():
	txt = bpy.data.texts.new(name='my.c++.py')
	txt.from_string(TEST1)
	bpy.data.objects['Cube'].netghost_script0 = txt





if __name__=='__main__':
	test()

