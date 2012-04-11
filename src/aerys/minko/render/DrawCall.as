package aerys.minko.render
{
	import aerys.minko.ns.minko_render;
	import aerys.minko.render.resource.IndexBuffer3DResource;
	import aerys.minko.render.resource.Program3DResource;
	import aerys.minko.render.resource.VertexBuffer3DResource;
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.render.shader.binding.IBinder;
	import aerys.minko.scene.node.mesh.geometry.Geometry;
	import aerys.minko.type.data.DataBindings;
	import aerys.minko.type.enum.Blending;
	import aerys.minko.type.enum.ColorMask;
	import aerys.minko.type.enum.TriangleCulling;
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
		
		private static const PROGRAM_TYPE_VERTEX	: String		= Context3DProgramType.VERTEX;
		private static const PROGRAM_TYPE_FRAGMENT	: String		= Context3DProgramType.FRAGMENT;
		
		private var _bindings			: Object							= null;
		
		private var _vsInputComponents	: Vector.<VertexComponent>			= null;
		private var _vsInputIndices		: Vector.<uint>						= null;
		private var _cpuConstants		: Dictionary						= null;
		private var _vsConstants		: Vector.<Number>					= null;
		private var _fsConstants		: Vector.<Number>					= null;
		private var _fsTextures			: Vector.<ITextureResource>			= new Vector.<ITextureResource>(8, true);
		
		// states
		private var _indexBuffer		: IndexBuffer3DResource				= null;
		private var _firstIndex			: int								= 0;
		
		private var _vertexBuffers		: Vector.<VertexBuffer3DResource>	= new Vector.<VertexBuffer3DResource>(8, true);
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
		
		public function DrawCall()
		{
		}
		
		public function configure(program		: Program3DResource,
								  geometry		: Geometry,
								  meshBindings	: DataBindings,
								  sceneBindings	: DataBindings) : void
		{
			if (_bindings != null)
				unsetBindings(meshBindings, sceneBindings);
			
			setProgram(program);
			updateGeometry(geometry);
			setGeometry(geometry);
			setBindings(meshBindings, sceneBindings);
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
									 sceneBindings	: DataBindings) : void
		{
			for (var parameter : String in _bindings)
			{
				meshBindings.getPropertyChangedSignal(parameter).add(
					parameterChangedHandler
				);
				
				sceneBindings.getPropertyChangedSignal(parameter).add(
					parameterChangedHandler
				);
				
				var value : Object = meshBindings.getProperty(parameter)
					|| sceneBindings.getProperty(parameter);
				
				if (value)
					setParameter(parameter, value);
			}
		}

		public function apply(context 	: Context3D,
							  previous	: DrawCall) : uint
		{
			if (!_enabled)
				return 0;
			
			context.setColorMask(_colorMaskR, _colorMaskG, _colorMaskB, _colorMaskA);
			
			context.setProgramConstantsFromVector(
				PROGRAM_TYPE_VERTEX,
				0,
				_vsConstants
			);
			
			context.setProgramConstantsFromVector(
				PROGRAM_TYPE_FRAGMENT,
				0,
				_fsConstants
			);
			
			var numTextures	: int	= _fsTextures.length;
			var maxTextures	: int	= previous ? previous._fsTextures.length : 8;
			var maxBuffers	: int	= previous ? previous._numVertexComponents : 8;
			var i 			: int 	= 0;
			
			for (i = 0; i < numTextures; ++i)
				context.setTextureAt(i, _fsTextures[i].getNativeTexture(context));
			
			while (i < maxTextures)
				context.setTextureAt(i++, null);
			
			for (i = 0; i < _numVertexComponents; ++i)
				context.setVertexBufferAt(
					i,
					_vertexBuffers[i].getVertexBuffer3D(context),
					_offsets[i],
					_formats[i]
				);
			
			while (i < maxBuffers)
				context.setVertexBufferAt(i++, null);
			
			context.drawTriangles(_indexBuffer.getIndexBuffer3D(context));
			
			return _indexBuffer.numIndices / 3;
		}
		
		public function setParameter(name : String, value : Object) : void
		{
			var binding : IBinder = _bindings[name] as IBinder;
			
			if (binding)
				binding.set(_cpuConstants, _vsConstants, _fsConstants, _fsTextures, value);
		}
		
		private function parameterChangedHandler(dataBindings	: DataBindings,
												 property		: String,
												 newValue		: Object) : void
		{
			newValue && setParameter(property, newValue);
		}
	}
}