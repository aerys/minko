package aerys.minko.render
{
	import aerys.minko.ns.minko_render;
	import aerys.minko.render.resource.IndexBuffer3DResource;
	import aerys.minko.render.resource.Program3DResource;
	import aerys.minko.render.resource.VertexBuffer3DResource;
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.render.resource.texture.TextureResource;
	import aerys.minko.render.shader.binding.IBinder;
	import aerys.minko.scene.node.mesh.geometry.Geometry;
	import aerys.minko.type.Signal;
	import aerys.minko.type.data.DataBindings;
	import aerys.minko.type.enum.Blending;
	import aerys.minko.type.enum.ColorMask;
	import aerys.minko.type.enum.TriangleCulling;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.StreamUsage;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.stream.format.VertexComponent;
	import aerys.minko.type.stream.format.VertexFormat;
	
	import flash.display3D.Context3D;
	import flash.display3D.Context3DProgramType;
	import flash.utils.Dictionary;
	import flash.utils.getQualifiedClassName;
	
	/**
	 * DrawCall objects contain all the shader constants and buffer settings required
	 * to perform drawing operations using the Stage3D API.
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public final class DrawCall
	{
		use namespace minko_render;
		
		private static const PROGRAM_TYPE_VERTEX	: String			= Context3DProgramType.VERTEX;
		private static const PROGRAM_TYPE_FRAGMENT	: String			= Context3DProgramType.FRAGMENT;
		private static const NUM_TEXTURES			: uint				= 8;
		private static const NUM_VERTEX_BUFFERS		: uint				= 8;
		
		private static const TMP_VECTOR4			: Vector4			= new Vector4();
		private static const TMP_NUMBERS			: Vector.<Number>	= new Vector.<Number>(0xffff, true);
		private static const TMP_INTS				: Vector.<int>		= new Vector.<int>(0xffff, true);
		
		private var _bindings			: Object							= null;
		
		private var _vsInputComponents	: Vector.<VertexComponent>			= null;
		private var _vsInputIndices		: Vector.<uint>						= null;
		private var _cpuConstants		: Dictionary						= null;
		private var _vsConstants		: Vector.<Number>					= null;
		private var _fsConstants		: Vector.<Number>					= null;
		private var _fsTextures			: Vector.<ITextureResource>			= new Vector.<ITextureResource>(NUM_TEXTURES, true);
		
		// states
		private var _indexBuffer		: IndexBuffer3DResource				= null;
		private var _firstIndex			: int								= 0;
		
		private var _vertexBuffers		: Vector.<VertexBuffer3DResource>	= new Vector.<VertexBuffer3DResource>(NUM_VERTEX_BUFFERS, true);
		private var _numVertexComponents: uint								= 0;
		private var _offsets			: Vector.<int>						= new Vector.<int>(8, true);
		private var _formats			: Vector.<String>					= new Vector.<String>(8, true);
		
		private var _blending			: uint								= 0;
		private var _blendingSource		: String							= null;
		private var _blendingDest		: String							= null;
		
		private var _triangleCulling	: uint								= 0;
		private var _triangleCullingStr	: String							= null;
		
		private var _colorMask			: uint								= 0;
		private var _colorMaskR			: Boolean							= true;
		private var _colorMaskG			: Boolean							= true;
		private var _colorMaskB			: Boolean							= true;
		private var _colorMaskA			: Boolean							= true;
		
		private var _enabled			: Boolean							= true;
		
		private var _depth				: Number							= 0.;
		private var _invalidDepth		: Boolean							= false;
		private var _localToWorld		: Matrix4x4							= null;
		private var _worldToScreen		: Matrix4x4							= null;
		
		public function get vertexComponents() : Vector.<VertexComponent>
		{
			return _vsInputComponents;
		}
		
		public function get blending() : uint
		{
			return _blending;
		}
		public function set blending(value : uint) : void
		{
			_blending = value;
			_blendingSource = Blending.STRINGS[int(value & 0xffff)];
			_blendingDest	= Blending.STRINGS[int(value >>> 16)]
		}
		
		public function get triangleCulling() : uint
		{
			return _triangleCulling;
		}
		public function set triangleCulling(value : uint) : void
		{
			_triangleCulling = value;
			_triangleCullingStr = TriangleCulling.STRINGS[value];
		}
		
		public function get colorMask() : uint
		{
			return _colorMask;
		}
		public function set colorMask(value : uint) : void
		{
			_colorMask = value;
			_colorMaskR = (value & ColorMask.RED) != 0;
			_colorMaskG = (value & ColorMask.GREEN) != 0;
			_colorMaskB = (value & ColorMask.BLUE) != 0;
			_colorMaskA = (value & ColorMask.ALPHA) != 0;
		}

		public function get enabled() : Boolean
		{
			return _enabled;
		}
		public function set enabled(value : Boolean) : void
		{
			_enabled = value;
		}
		
		public function get depth() : Number
		{
			if (_invalidDepth)
			{
				_invalidDepth = false;
				
				if (_localToWorld != null && _worldToScreen != null)
				{
					_localToWorld.transformVector(Vector4.ZERO, TMP_VECTOR4);
					_depth = _worldToScreen.transformVector(TMP_VECTOR4, TMP_VECTOR4).z;
				}
			}
			
			return _depth;
		}
		
		public function configure(program		: Program3DResource,
								  geometry		: Geometry,
								  meshBindings	: DataBindings,
								  sceneBindings	: DataBindings,
								  computeDepth	: Boolean) : void
		{
			if (_bindings != null)
				unsetBindings(meshBindings, sceneBindings);
		
			_invalidDepth = computeDepth;
			
			setProgram(program);
			updateGeometry(geometry);
			setGeometry(geometry);
			setBindings(meshBindings, sceneBindings, computeDepth);
		}
		
		private function unsetBindings(meshBindings		: DataBindings,
									   sceneBindings	: DataBindings) : void
		{
			for (var parameter : String in _bindings)
			{
				meshBindings.getPropertyChangedSignal(parameter).remove(
					parameterChangedHandler
				);
				
				sceneBindings.getPropertyChangedSignal(parameter).remove(
					parameterChangedHandler
				);
			}
			
			var worldToScreenSignal	: Signal = sceneBindings.getPropertyChangedSignal('worldToScreen');
			var localToWorldSignal	: Signal = meshBindings.getPropertyChangedSignal('localToWorld');
			
			if (worldToScreenSignal.hasCallback(transformChangedHandler))
				worldToScreenSignal.remove(transformChangedHandler);

			if (worldToScreenSignal.hasCallback(transformChangedHandler))
				localToWorldSignal.remove(transformChangedHandler);
		}
		
		private function setProgram(program : Program3DResource) : void
		{
			_cpuConstants		= new Dictionary();
			_vsConstants		= program._vsConstants.concat();
			_fsConstants		= program._fsConstants.concat();
			_fsTextures			= program._fsTextures.concat();
			_vsInputComponents	= program._vertexInputComponents;
			_vsInputIndices		= program._vertexInputIndices;
			_bindings			= program._bindings;
			
			triangleCulling		= TriangleCulling.FRONT;
			blending			= Blending.NORMAL;
			colorMask			= ColorMask.RGBA;
		}
		
		 /**
		  * Ask geometry to compute additional vertex data if needed for this drawcall.
		  */
		public function updateGeometry(geometry : Geometry) : void
		{
			var vertexFormat : VertexFormat	= geometry.format;
			
			if (_vsInputComponents.indexOf(VertexComponent.TANGENT) >= 0
				&& !vertexFormat.hasComponent(VertexComponent.TANGENT))
			{
				geometry.computeTangentSpace(StreamUsage.DYNAMIC);
			}
			else if (_vsInputComponents.indexOf(VertexComponent.NORMAL) >= 0
				&& !vertexFormat.hasComponent(VertexComponent.NORMAL))
			{
				geometry.computeNormals(StreamUsage.DYNAMIC);
			}
		}
		
		/**
		 * Obtain a reference to each buffer and offset that apply() may possibly need.
		 * 
		 */		
		public function setGeometry(geometry : Geometry, frame : uint = 0) : void
		{
			_numVertexComponents = _vsInputComponents.length;
			_indexBuffer		 = geometry.indexStream.resource;
			
			for (var i : uint = 0; i < _numVertexComponents; ++i)
			{
				var component : VertexComponent	= _vsInputComponents[i];
				var index	  : uint			= _vsInputIndices[i];
				
				if (component)
				{
					var vertexStream	: IVertexStream = geometry.getVertexStream(index + frame);
					var stream	 		: VertexStream	= vertexStream.getStreamByComponent(component);
					
					if (stream == null)
					{
						throw new Error(
							'Missing vertex component: \'' + component.toString() + '\'.'
						);
					}
					
					_vertexBuffers[i]	= stream.resource;
					_formats[i]			= component.nativeFormatString;
					_offsets[i]			= stream.format.getOffsetForComponent(component);
				}
			}
		}
		
		/**
		 * @fixme There is a bug here
		 * @fixme We splitted properties between scene and mesh
		 * @fixme it should be done on the compiler also to avoid this ugly hack
		 */		
		private function setBindings(meshBindings	: DataBindings,
									 sceneBindings	: DataBindings,
									 computeDepth	: Boolean) : void
		{
			for (var parameter : String in _bindings)
			{
				meshBindings.getPropertyChangedSignal(parameter).add(
					parameterChangedHandler
				);
				
				sceneBindings.getPropertyChangedSignal(parameter).add(
					parameterChangedHandler
				);
				
				if (meshBindings.propertyExists(parameter))
					setParameter(parameter, meshBindings.getProperty(parameter));
				else if (sceneBindings.propertyExists(parameter))
					setParameter(parameter, sceneBindings.getProperty(parameter));
			}
			
			if (computeDepth)
			{
				sceneBindings.getPropertyChangedSignal('worldToScreen').add(
					transformChangedHandler
				);
				if (sceneBindings.propertyExists('worldToScreen'))
					_worldToScreen = sceneBindings.getProperty('worldToScreen') as Matrix4x4;
				
				meshBindings.getPropertyChangedSignal('localToWorld').add(
					transformChangedHandler
				);
				if (meshBindings.propertyExists('localToWorld'))
					_localToWorld = meshBindings.getProperty('localToWorld') as Matrix4x4;
			}
		}

		public function apply(context : Context3D, previous : DrawCall) : uint
		{
			if (!_enabled)
				return 0;
			
			context.setColorMask(_colorMaskR, _colorMaskG, _colorMaskB, _colorMaskA);
			
			// setup shader constants
			context.setProgramConstantsFromVector(PROGRAM_TYPE_VERTEX, 0, _vsConstants);
			context.setProgramConstantsFromVector(PROGRAM_TYPE_FRAGMENT, 0, _fsConstants);
			
			var numTextures	: uint	= _fsTextures.length;
			var maxTextures	: uint	= previous ? previous._fsTextures.length : NUM_TEXTURES;
			var maxBuffers	: uint	= previous ? previous._numVertexComponents : NUM_VERTEX_BUFFERS;
			var i 			: uint 	= 0;

			// setup textures
			for (i = 0; i < numTextures; ++i)
				context.setTextureAt(i, _fsTextures[i].getNativeTexture(context));
			
			while (i < maxTextures)
				context.setTextureAt(i++, null);

			// setup buffers
			for (i = 0; i < _numVertexComponents; ++i)
				context.setVertexBufferAt(
					i,
					_vertexBuffers[i].getVertexBuffer3D(context),
					_offsets[i],
					_formats[i]
				);
			
			while (i < maxBuffers)
				context.setVertexBufferAt(i++, null);
			
			// draw triangles
			context.drawTriangles(_indexBuffer.getIndexBuffer3D(context));
			
			return _indexBuffer.numIndices / 3;
		}
		
		public function setParameter(name : String, value : Object) : void
		{
			var binding : IBinder = _bindings[name] as IBinder;
			
			if (binding != null)
				binding.set(_cpuConstants, _vsConstants, _fsConstants, _fsTextures, value);
		}
		
		private function parameterChangedHandler(dataBindings	: DataBindings,
												 property		: String,
												 newValue		: Object) : void
		{
			newValue && setParameter(property, newValue);
		}
		
		private function transformChangedHandler(bindings 	: DataBindings,
												 property 	: String,
												 value 		: Matrix4x4) : void
		{
			if (property == "worldToScreen")
				_worldToScreen = value;
			else if (property == "localToWorld")
				_localToWorld = value;
			
			_invalidDepth = true;
		}
		
		public static function sort(drawCalls : Vector.<DrawCall>, numDrawCalls : uint) : void
		{
			var n 		: int 		= numDrawCalls;
			var i		: int 		= 0;
			var j		: int 		= 0;
			var k		: int 		= 0;
			var t		: int		= 0;
			var call 	: DrawCall	= drawCalls[0];
			var anmin	: Number 	= -call.depth;
			var nmax	: int  		= 0;
			var p		: Number	= 0.;
			var sorted	: Boolean	= true;
			
			for (i = 0; i < n; ++i)
			{
				call = drawCalls[i];
				p = -call.depth;
				
				TMP_INTS[i] = 0;
				TMP_NUMBERS[i] = p;
				if (p < anmin)
					anmin = p;
				else if (p > Number(TMP_NUMBERS[nmax]))
					nmax = i;
			}
			
			if (anmin == Number(TMP_NUMBERS[nmax]))
				return ;
			
			var m		: int 	= Math.ceil(n * .125);
			var nmove	: int 	= 0;
			var c1		: Number = (m - 1) / (Number(TMP_NUMBERS[nmax]) - anmin);
			
			for (i = 0; i < n; ++i)
			{
				k = int(c1 * (Number(TMP_NUMBERS[i]) - anmin));
				TMP_INTS[k] = int(TMP_INTS[k]) + 1;
			}
			
			for (k = 1; k < m; ++k)
				TMP_INTS[k] = int(TMP_INTS[k]) + int(TMP_INTS[int(k - 1)]);
			
			var hold		: Number 		= Number(TMP_NUMBERS[nmax]);
			var holdState 	: DrawCall 	= drawCalls[nmax] as DrawCall;
			
			TMP_NUMBERS[nmax] = Number(TMP_NUMBERS[0]);
			TMP_NUMBERS[0] = hold;
			drawCalls[nmax] = drawCalls[0];
			drawCalls[0] = holdState;
			
			var flash		: Number			= 0.;
			var flashState	: DrawCall	= null;
			
			j = 0;
			k = int(m - 1);
			i = int(n - 1);
			
			while (nmove < i)
			{
				while (j > int(TMP_INTS[k]) - 1)
				{
					++j;
					k = int(c1 * (Number(TMP_NUMBERS[j]) - anmin));
				}
				
				flash = Number(TMP_NUMBERS[j]);
				flashState = drawCalls[j] as DrawCall;
				
				while (!(j == int(TMP_INTS[k])))
				{
					k = int(c1 * (flash - anmin));
					
					t = int(TMP_INTS[k]) - 1;
					hold = Number(TMP_NUMBERS[t]);
					holdState = drawCalls[t] as DrawCall;
					
					TMP_NUMBERS[t] = flash;
					drawCalls[t] = flashState;
					
					flash = hold;
					flashState = holdState;
					
					TMP_INTS[k] = int(TMP_INTS[k]) - 1;
					++nmove;
				}
			}
			
			for (j = 1; j < n; ++j)
			{
				hold = Number(TMP_NUMBERS[j]);
				holdState = drawCalls[j];
				
				i = int(j - 1);
				while (i >= 0 && Number(TMP_NUMBERS[i]) > hold)
				{
					// not trivial
					TMP_NUMBERS[int(i + 1)] = Number(TMP_NUMBERS[i]);
					drawCalls[int(i + 1)] = drawCalls[i];
					
					--i;
				}
				
				TMP_NUMBERS[int(i + 1)] = hold;
				drawCalls[int(i + 1)] = holdState;
			}
		}
	}
}