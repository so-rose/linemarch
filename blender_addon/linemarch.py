import math, copy, sys
from mathutils import Vector

import bpy
from bpy.props import *

#~ sys.path.append('/home/sofus/subhome/src/linemarch/blender_addon')
#~ print(sys.path)

bl_info = { 
	"name"		: "LineMarch Render", 
	"category"	: "Render",
	"author"	: "Sofus Rose",
    "version"	: (0, 7),
    "blender"	: (2, 77, 0),
    "location"	: "Panel > Render",
}

class genVox :
	def fSphere(dims, f, inRad=10, outRad=50) :
		center = (dims[0]/2,dims[1]/2,dims[2]/2)
		
		for z in range(dims[2]):
			for y in range(dims[1]):
				for x in range(dims[0]):
					dx = abs(center[0] - x)
					dy = abs(center[1] - y)
					dz = abs(center[2] - z)
					
					cenDist = math.sqrt(dx*dx + dy*dy + dz*dz)
					
					if cenDist <= inRad:
						print(1, file=f)
						
					elif cenDist < outRad:
						print(1 - ( (cenDist-inRad) / (outRad-inRad) ), file=f)
						
					else:
						print(0, file=f)
		
	def sphere(dims, f, rad=50) :
		center = (dims[0]/2,dims[1]/2,dims[2]/2)
		
		for z in range(dims[2]):
			for y in range(dims[1]):
				for x in range(dims[0]):
					dx = abs(center[0] - x)
					dy = abs(center[1] - y)
					dz = abs(center[2] - z)
					
					cenDist = math.sqrt(dx*dx + dy*dy + dz*dz)
					
					if cenDist < rad:
						print(1, file=f)
						
					else:
						print(0, file=f)
						
	def ripple(dims, f, freq=10, amp=0.1) :
		center = (dims[0]/2,dims[1]/2,dims[2]/2)
		
		height = lambda x, y: freq * math.sin(amp * math.sqrt(x**2 + y**2))
		
		for z in range(dims[2]):
			for y in range(dims[1]):
				for x in range(dims[0]):
					dx = ((center[0] - x) / dims[0]) * 100
					dy = ((center[1] - y) / dims[1]) * 100
					dz = ((center[2] - z) / dims[2]) * 100
					
					#~ planeDist = math.sqrt(dx*dx + dy*dy)
					#~ height = math.sin(planeDist)
					#~ val = math.sin(10 * (dx**2+dy**2))/10
					 
					if dy < height(dx, dz) :
						print(1, file=f)
					else :
						print(0, file=f)
						
	def general(dims, f, funcIn) :
		center = (dims[0]/2,dims[1]/2,dims[2]/2) # z < 0.1 * math.sin(math.sqrt(10 * x**2 + y**2))
		
		#Variables that can be accessed from the expression
		global dimen
		dimen = dims
		
		#~ print(funcIn)
		exec('func = lambda x, y, z: ' + funcIn, globals()) #sys.modules seems to be passed in as well.
		#~ func = lambda x, y, z: exec(funcIn) #Careful...
		#Return true for density 1 and false for density 0.
		print(func(5, 6, 8))
		
		for z in range(dims[2]):
			for y in range(dims[1]):
				for x in range(dims[0]):
					dx = ((center[0] - x) / dims[0]) * 100
					dy = ((center[1] - y) / dims[1]) * 100
					dz = ((center[2] - z) / dims[2]) * 100
					
					if func(dx, dy, dz) :
						print(1, file=f)
					else :
						print(0, file=f)

def flip(vec) :
	#Convert from Left Handed XYZ (Hori, Depth, Vert) to Right Handed XZY (Hori, Depth, Vert)
	t = list(vec)
	t[1], t[2] = t[2], t[1]
	t[0] = -t[0] if t[0] != 0 else t[0]
	t[1] = -t[1] if t[1] != 0 else t[1] #Negate the axes because reasons...
	#~ t[2] = -t[2]
	return tuple(t)
	
def flipVox(vec) :
	#Convert from Left Handed XYZ (Hori, Depth, Vert) to Right Handed XZY (Hori, Depth, Vert)
	t = list(vec)
	t[1], t[2] = t[2], t[1]
	#~ t[0] = -t[0]
	#~ t[1] = -t[1] if t[1] != 0 else t[1] #Negate the axes because reasons...
	#~ t[2] = -t[2]
	return tuple(t)
	
def flipUp(vec) :
	#Flips the y and z coordinates of a 3D tuple.
	t = list(vec)
	t[1], t[2] = t[2], t[1]
	#~ t[0] = -t[0]
	#~ t[1] = -t[1] if t[1] != 0 else t[1] #Negate the axes because reasons...
	#~ t[2] = -t[2]
	return tuple(t)

def writeConstants(conf, fileName) : #write later.
	with open(fileName, 'w') as f :
		print("ALPH", "true" if conf["isAlpha"] else "false", file=f)
		print("RESO", *conf["resolution"], file=f)
		print("STAR", conf["frameStart"], file=f)
		print("STOP", conf["frameEnd"], file=f)
		print(file=f)
		
		print("PREF", conf["output"], file=f)
		print("FILE", "ren/out_", file=f)
		print("DIMS", *conf["voxDims"], file=f) #No need to specify at this point.
		print("VOXD", conf["voxDetail"], file=f) #No need to specify at this point.
		print("LOFR", int(conf["isNoise"]), file=f) #Only two options for now; boolean works fine.

def writeConfigs(conf, fileName) :
	with open(fileName, 'w') as f :
		#Basics
		print("STEP", conf["stepSize"], file=f)
		print("BRGB", *conf["backCol"], file=f)
		#~ print("ALPH", "true" if conf["isAlpha"] else "false", file=f) #
		#~ print("RESO", *conf["resolution"], file=f) #
		#~ print("STAR", conf["frameStart"], file=f) #
		#~ print("STOP", conf["frameEnd"], file=f) #
		print(file=f)
		
		#Other Attributes
		#~ print("PREF", conf["output"], file=f) #
		print("VOXN", conf["voxPath"], file=f) #Can't specify from Blender for now.
		#~ print("FILE", "ren/out", file=f) #
		#~ print("DIMS", *conf["voxDims"], file=f) #
		#~ print("LOFR", int(conf["isNoise"]), file=f) ##Only two options for now; boolean works fine.
		print(file=f)
		
		#Material Attributes - For now, only one voxel/noise object.
		print("MRGB", *conf["centers"][0]["matCol"], file=f)
		print("KATT", conf["centers"][0]["k"], file=f)
		print("DENS", conf["centers"][0]["denDiv"], file=f)
		print("PNOI", *conf["centers"][0]["noise"], file=f)
		print("CENT", *conf["centers"][0]["position"], file=f)
		print(file=f)
		
		#Camera Attributes
		print("EYEP", *conf["camera"]["position"], file=f)
		print("VDIR", *conf["camera"]["dir"], file=f)
		print("UVEC", *conf["camera"]["up"], file=f)
		print("FOVY", conf["camera"]["fov"], file=f)
		print(file=f)
		
		for light in conf["lights"] :
			print("LPOS", *light["position"], file=f)
			print("LCOL", *light["color"], file=f)
			print(file=f)
			
		print("Config Written: ", fileName)
	
def repRel(inp) :
	# Replace Blender's relative // with the corresponding absolute path.
	tmp = inp.find("//")
	if tmp != -1 :
		out = inp.replace("//", bpy.path.abspath("//"))
	else :
		out = inp
		
	return out
	
class renderOperator(bpy.types.Operator):
	bl_idname = 'lm.render'
	bl_label = 'Writes to Config Files.'
	
	def execute(self, context) :
		curFrame = copy.deepcopy(bpy.context.scene.frame_current)
		
		if context.scene.lmRen.singleFrame :
			startFrame = curFrame
			endFrame = curFrame
		else :
			startFrame = context.scene.frame_start
			endFrame = context.scene.frame_end
		
		bpy.context.scene.frame_set(startFrame)
		
		for i in range(startFrame, endFrame + 1) :
			print(i)
			conf = {}
			
			
			conf["output"] = repRel(context.scene.lmRen.outPref)
			
			scale = context.scene.lmRen.unitScale
			
			# Scenewide Attributes
			conf["stepSize"] = context.scene.lmRen.stepSize
			conf["unitScale"] = context.scene.lmRen.unitScale
			conf["backCol"] = tuple(context.scene.lmRen.backCol)
			conf["isAlpha"] = context.scene.lmRen.isAlpha
			conf["isNoise"] = context.scene.lmRen.isNoise
			
			conf["resolution"] = tuple(int(res * (context.scene.render.resolution_percentage / 100)) for res in (context.scene.render.resolution_x, context.scene.render.resolution_y))
			conf["frameStart"] = startFrame
			conf["frameEnd"] = endFrame
			#~ self.report({'INFO'}, 'Added Scene Attributes')
			
			conf["voxPath"] = repRel(context.scene.lmRen.voxPath)
			conf["voxDims"] = tuple(int(dim) for dim in context.scene.lmRen.voxDims)
			conf["voxDetail"] = context.scene.lmRen.voxDetail
			
			# Camera Attributes
			conf["camera"] = {}
			
			conf["camera"]["fov"] = math.degrees(bpy.data.cameras["Camera"].angle)
			conf["camera"]["position"] = flip(tuple(axis * conf["unitScale"] for axis in bpy.data.objects["Camera"].location))
			conf["camera"]["up"] = flipUp(tuple(bpy.data.objects["Camera"].matrix_world.to_quaternion() * Vector((0.0, 1.0, 0.0))))
			conf["camera"]["dir"] = flip(tuple(bpy.data.objects["Camera"].matrix_world.to_quaternion() * Vector((0.0, 0.0, -1.0))))
			#~ self.report({'INFO'}, 'Added Camera Attributes')
			
			# Per Object Attributes
			
			conf["lights"] = []
			conf["centers"] = []
			
			for obj in context.scene.objects :
				if obj.lmObj.isLight :
					#~ print(obj.location)
					position = flip(tuple(axis * conf["unitScale"] for axis in obj.location))
					color = tuple(chnl * obj.lmObj.lightStr for chnl in obj.lmObj.lightWidget) #Multiply lightWidget colors by lightStr.
					conf["lights"].append(
						{
							"position" 	: position,
							"color"		: color
						}
					)
					#~ self.report({'INFO'}, 'Added light')
				
				if obj.lmObj.isCenter :
					if not len(conf["centers"]) >= 1 :
						position = flip(tuple(axis * conf["unitScale"] for axis in obj.location))
						matCol = tuple(obj.lmObj.matColor)
						denDiv = obj.lmObj.denDiv
						k = obj.lmObj.denAtt
						
						noiAttr = tuple(obj.lmObj.noiAttr)
						
						conf["centers"].append(
							{
								"position" 	: position,
								"matCol"	: matCol,
								"denDiv"	: denDiv,
								"k"			: k,
								"noise"		: (int(noiAttr[0]), noiAttr[1], noiAttr[2], obj.lmObj.noiSeed)
							}
						)
						#~ self.report({'INFO'}, 'Added Center')
					else :
						self.report({'INFO'}, "Skipping nth Center")
					
			if i == startFrame: writeConstants(conf, repRel(context.scene.lmRen.outConst))
			writeConfigs(conf, conf["output"] + str(i) + ".txt")
			bpy.context.scene.frame_set(i+1)
			
		bpy.context.scene.frame_set(curFrame)
		self.report({'INFO'}, 'Wrote Configs')
		return {'FINISHED'}

class genRippleOperator(bpy.types.Operator) :
	bl_idname = 'lm.genripple'
	bl_label = 'Generates a voxel file, containing a ripple.'
	
	def execute(self, context) :
		genVox.ripple(
				tuple(int(dim) for dim in context.scene.lmRen.voxDims),
				open(repRel(context.scene.lmRen.voxPath), "w"),
				freq=context.scene.lmRen.voxFreq,
				amp=context.scene.lmRen.voxAmp
		)
		
		self.report({'INFO'}, 'Wrote Voxel File')
		return {'FINISHED'}
		
class genSphereOperator(bpy.types.Operator) :
	bl_idname = 'lm.gensphere'
	bl_label = 'Generates a voxel file, containing a sphere.'
	
	def execute(self, context) :
		genVox.fSphere(
				tuple(int(dim) for dim in context.scene.lmRen.voxDims),
				open(repRel(context.scene.lmRen.voxPath), "w"),
				inRad=context.scene.lmRen.voxInner,
				outRad=context.scene.lmRen.voxOuter
		)
		
		self.report({'INFO'}, 'Wrote Voxel File')
		return {'FINISHED'}
		
class genGeneralOperator(bpy.types.Operator) :
	bl_idname = 'lm.gengeneral'
	bl_label = 'Generates a voxel file, based on a user given height function.'
	
	def execute(self, context) :
		genVox.general(
				tuple(int(dim) for dim in context.scene.lmRen.voxDims),
				open(repRel(context.scene.lmRen.voxPath), "w"),
				context.scene.lmRen.voxFunc
		)
		
		self.report({'INFO'}, 'Wrote Voxel File')
		return {'FINISHED'}

class RenderProperties(bpy.types.PropertyGroup) :
	@classmethod
	def register(cls):

		bpy.types.Scene.lmRen = PointerProperty(
				name="Custom Props",
				description="Custom Properties for the Linemarch scene.",
				type=cls,
			)
		
		cls.stepSize = FloatProperty(
							name="Step Size",
							default=1.0,
							description="Affects the accuracy of the render, at the cost of performance."
						)
		
		cls.unitScale = FloatProperty(
							name="BU -> LM Factor",
							default=50,
							description="The multiplication factor to scale Blender Units to LightMarch units."
						)
						
		cls.backCol = FloatVectorProperty(
							name="Background", 
							subtype='COLOR', 
							default=(1.0,1.0,1.0),
							min=0.0, max = 1.0,
							description="Background LightMarch color. Useless if alpha is enabled."
						)
						
		cls.isAlpha = BoolProperty(
							name="Alpha?",
							description="Use alpha as the background.",
							default=True
						)
						
		cls.isNoise = BoolProperty(
							name="Generate Noise?",
							description="True generates noise, False loads vox.txt.",
							default=True
						)
						
		cls.singleFrame = BoolProperty(
							name="Single Frame?",
							description="Only use the current frame.",
							default=True
						)
		
		cls.outPref = StringProperty(
							name="Config Prefix",
							default="//configs/out",
							description="The prefix to output the config files to."
						)
						
		cls.outConst = StringProperty(
							name="Constants Output",
							default="//constants.txt",
							description="The prefix to output the constants file to. SHOULD BE NAMED constants.txt."
						)
						
		cls.voxPath = StringProperty(
							name="Voxel IO",
							default="//vox.txt",
							description="The prefix to load from and utput the voxel file to."
						)
						
		cls.voxDims = FloatVectorProperty(
							name="Voxel Dims", 
							default=(100,100,100),
							min=0.0,
							description="Dimensions of the generated/loaded voxel buffer."
						)
					
		# Sphere Generation
		cls.voxInner = FloatProperty(
							name="Inner Radius",
							default=30,
							description="The inner generated voxel sphere radius."
						)
						
		cls.voxOuter = FloatProperty(
							name="Outer Radius",
							default=50,
							description="The outer generated voxel sphere radius."
						)
						
		# Ripple Generation
		cls.voxFreq = FloatProperty(
							name="Frequency",
							default=10,
							description="The frequency of the generated ripple."
						)
						
		cls.voxAmp = FloatProperty(
							name="Amplitude",
							default=0.1,
							description="The amplitude of the generated ripple."
						)
						
		cls.voxDetail = FloatProperty(
							name="Voxel Detail",
							default=1,
							description="Shrinks the individual voxels for greater detail."
						)
			
		# Generalized Generation
		cls.voxFunc = StringProperty(
							name="Voxel Generation",
							default="x**2 + y**2 + z**2 <= 1",
							description="The prefix to load from and utput the voxel file to."
						)

	@classmethod
	def unregister(cls):
		del bpy.types.Scene.lmRen

class lmProperties(bpy.types.PropertyGroup):
	@classmethod
	def register(cls):

		bpy.types.Object.lmObj = PointerProperty(
			name="Custom Props",
			description="Custom Properties for Linemarch Empties",
			type=cls,
		)
		
		#Key Properties
		cls.isLight = BoolProperty(
							name="Point Light?",
							description="Use this object as a point light.",
							default=False
						)
		
		cls.isCenter = BoolProperty(
							name="Center?",
							description="Use this object as the center of the voxel grid. ONLY SPECIFY ONE.",
							default=False
						)

		#Light Properties     
		cls.lightWidget = FloatVectorProperty(
							name="Light Color", 
							subtype='COLOR', 
							default=(1.0,1.0,1.0),
							min=0.0, max = 1.0,
							description="Color of LightMarch Light"
						)
		
		cls.lightStr = FloatProperty(
							name="Light Strength",
							default=1.0,
							description="Strength of LightMarch Light"
						)
		
		#Material Properties
		cls.matColor = FloatVectorProperty(
							name="Material Color", 
							subtype='COLOR', 
							default=(1.0,1.0,1.0),
							min=0.0, max = 1.0,
							description="Color of LightMarch Material"
						)
						
		cls.noiAttr = FloatVectorProperty(
							name="Noise Attributes", 
							#subtype='COLOR', 
							default=(4.0,0.03,2.4),
							#min=0, max = 1.0,
							description="Color of LightMarch Material"
						)
						
		cls.noiSeed = IntProperty(
							name="Noise Seed",
							default=1,
							min=0,
							description="Strength of LightMarch Light"
						)
						
		#Density Properties
		cls.denDiv = FloatProperty(
							name="Density",
							default=8.0,
							description="Divisor for the density of the object."
						)
		
		cls.denAtt = FloatProperty(
							name="k",
							default=1.0,
							min=0, max=2,
							description="k factor for the volumetric absorption."
						)

	@classmethod
	def unregister(cls):
		del bpy.types.Object.lmObj
	
class UtilityPanel(bpy.types.Panel) :
	bl_label = "LineMarch Options"
	bl_idname = "OBJECT_PT_lmutil"

	bl_space_type = "PROPERTIES"
	bl_region_type = "WINDOW"
	bl_context = "object"
		
	def draw(self, context) :
		layout = self.layout
		split = layout.split()       
		
		col1 = split.column()
		
		row11 = col1.row()
		#row1.label("Light Options")
		row11.prop(context.active_object.lmObj, "lightWidget")
		
		row12 = col1.row()
		row12.prop(context.active_object.lmObj, "lightStr")
		
		
		col2 = split.column()
		
		row21 = col2.row()
		row21.prop(context.active_object.lmObj, "matColor")
		
		row22 = col2.row()
		row22.prop(context.active_object.lmObj, "denDiv")
		row22.prop(context.active_object.lmObj, "denAtt")
		
		
		layout.row().separator()
		layout.row().separator()
		
		split2 = layout.split()
		
		col3 = split2.column()
		
		row33 = col3.row()
		row33.prop(context.active_object.lmObj, "noiSeed")
		
		col4 = split2.column()
		
		row43 = col4.row()
		row43.prop(context.active_object.lmObj, "isLight")
		row43.prop(context.active_object.lmObj, "isCenter")
		
		row4 = layout.row()
		row4.prop(context.active_object.lmObj, "noiAttr")		

class RenderPanel(bpy.types.Panel) :
	bl_label = "LineMarch Options"
	bl_idname = "OBJECT_PT_lmren"

	bl_space_type = "PROPERTIES"
	bl_region_type = "WINDOW"
	bl_context = "render"
	
	def draw(self, context):
		layout = self.layout
		split = layout.split()
		
		col1 = split.column()
		
		row11 = col1.row()
		row11.prop(context.scene.lmRen, "stepSize")
		row12 = col1.row()
		row12.prop(context.scene.lmRen, "isNoise")
		
		col2 = split.column()
		
		row21 = col2.row()
		row21.prop(context.scene.lmRen, "backCol")
				
		row221 = col2.row()
		row221.prop(context.scene.lmRen, "singleFrame")
		row221.prop(context.scene.lmRen, "isAlpha")
		
		
		row3 = layout.row()
		row3.prop(context.scene.lmRen, "unitScale")
		
		row4 = layout.row()
		row4.prop(context.scene.lmRen, "outPref")
		
		row5 = layout.row()
		row5.prop(context.scene.lmRen, "outConst")
		
		row6 = layout.row()
		row6.operator("lm.render", text="Generate Config Files!")
		
		layout.row().separator()
		layout.row().separator()
		
		row7 = layout.row()
		row7.prop(context.scene.lmRen, "voxDims")
		
		row8 = layout.row()
		row8.prop(context.scene.lmRen, "voxPath")
		row8.prop(context.scene.lmRen, "voxDetail")
		
		row9 = layout.row()
	
		row9 = layout.row()
		row9.prop(context.scene.lmRen, "voxInner")
		row9.prop(context.scene.lmRen, "voxOuter")
		row9.operator("lm.gensphere", text="Generate Sphere!")
		
		row10 = layout.row()
		row10.prop(context.scene.lmRen, "voxFreq")
		row10.prop(context.scene.lmRen, "voxAmp")
		row10.operator("lm.genripple", text="Generate Ripple!")
		
		row11 = layout.row()
		row11.prop(context.scene.lmRen, "voxFunc")
		
		row12 = layout.row()
		row12.operator("lm.gengeneral", text="Generate Function!")
		
		
	
def register():
	bpy.utils.register_class(RenderProperties)
	bpy.utils.register_class(RenderPanel)
	bpy.utils.register_class(UtilityPanel)
	bpy.utils.register_class(renderOperator)
	bpy.utils.register_class(genRippleOperator)
	bpy.utils.register_class(genSphereOperator)
	bpy.utils.register_class(genGeneralOperator)
	bpy.utils.register_class(lmProperties)

def unregister():
	bpy.utils.unregister_class(RenderProperties)
	bpy.utils.unregister_class(renderOperator)
	bpy.utils.unregister_class(genRippleOperator)
	bpy.utils.unregister_class(genSphereOperator)
	bpy.utils.unregister_class(genGeneralOperator)
	bpy.utils.unregister_class(RenderPanel)
	bpy.utils.unregister_class(UtilityPanel)
	bpy.utils.unregister_class(lmProperties)
	
if __name__ == "__main__" :
	register()
	bpy.utils.register_module(__name__)
