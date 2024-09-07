def blender_scene_view():
	html = ['<!DOCTYPE html><html><head></head><body>']

	html.append('<h1>objects:</h1><ul>')
	for ob in bpy.data.objects:
		html += [
			'<li><a href="/bpy/data/objects/%s">%s</a> : %s</li>' % (ob.name, ob.name, ob.type),
		]
		if ob.type=='MESH' and not ob.parent:
			html += [
				'<li><img src="/bpy/data/objects/%s.png"/></li>' % ob.name,
				'<li><a href="/bpy/data/objects/%s.ktx2"/>KTX2</a></li>' % ob.name,
			]
		elif ob.type=='EMPTY' and ob.empty_display_type=='IMAGE' and ob.data:
			## a basic sprite image
			html += [
				'<li><img src="/bpy/data/objects/%s.png"/></li>' % ob.name,
			]

	html.append('</ul>')
	html.append('</body></html>')
	return '\n'.join(html)

@netghost.http
class BlenderServer(BaseHTTPRequestHandler):
	def do_GET (self):
		self.send_response(200)
		self.send_header("Cache-Control", "no-cache, no-store, must-revalidate")
		self.send_header("Pragma", "no-cache")
		self.send_header("Expires", "0")
		ret = 'OK'
		hint = ''
		if self.path.endswith('.ico'):
			pass
		elif self.path == '/':
			if '__index__.html' in bpy.data.texts:
				ret = bpy.data.texts['__index__.html'].as_string()
			else:
				ret = blender_scene_view()
		elif self.path.startswith('/bpy/data/objects/'):
			name = self.path.split('/')[-1]
			if name in bpy.data.objects:
				ret = str(bpy.data.objects[name])
			elif name.endswith('.png'):
				ret = netghost.render(name[:-4])
			elif name.endswith('.ktx2'):
				ret = netghost.render(name[:-5])
				iput = '/tmp/__basisu__.png'
				open(iput, 'wb').write(ret)
				ret = netghost.basisu( iput )  ## defaults to KTX2

		elif os.path.isfile(self.path[1:]): # the .wasm file
			ret = open(self.path[1:], 'rb').read()
		elif self.path.endswith('.glb'):
			bpy.ops.object.select_all(action='DESELECT')
			name = self.path.split('/')[-1][: -len('.glb') ]
			if name in bpy.data.objects:
				ob = bpy.data.objects[name]
				ob.select_set(True)
				tmp = '/tmp/__httpd__.glb'
				bpy.ops.export_scene.gltf(filepath=tmp, export_selected = True)
				ret = open(tmp,'rb').read()

		if ret is None:
			ret = 'None?'
		if type(ret) is not bytes:
			ret = ret.encode('utf-8')

		self.send_header("Content-Length", str(len(ret)))
		self.end_headers()

		try:
			self.wfile.write( ret )
		except BrokenPipeError:
			print('CLIENT WRITE ERROR: failed bytes', len(ret))
