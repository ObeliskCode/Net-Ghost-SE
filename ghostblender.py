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
		bpy.props.PointerProperty(name='NetGhost C++ Script', type=bpy.types.Text)
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

