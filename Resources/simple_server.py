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
				for t in bpy.data.texts:
					if t.name.endswith('.html'):
						ret = t.as_string()
						break
		elif self.path.startswith('/bpy/data/objects/'):
			name = self.path.split('/')[-1]
			if name in bpy.data.objects:
				ret = str(bpy.data.objects[name])
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
