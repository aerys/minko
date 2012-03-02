package aerys.minko.render
{
	import aerys.minko.ns.minko_render;
	import aerys.minko.render.resource.IndexBuffer3DResource;
	import aerys.minko.render.resource.VertexBuffer3DResource;
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.render.shader.binding.IBinder;
	import aerys.minko.type.data.DataBindings;
	import aerys.minko.type.enum.Blending;
	import aerys.minko.type.enum.ColorMask;
	import aerys.minko.type.enum.TriangleCulling;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.IndexStream;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.stream.format.VertexComponent;
	import aerys.minko.type.stream.format.VertexFormat;
	
	import flash.display3D.Context3D;
	import flash.display3D.Context3DProgramType;
	import flash.utils.Dictionary;
	
	public final class DrawCall
	{
		use namespace minko_render;
		
		private static const PROGRAM_TYPE_VERTEX	: String		= Context3DProgramType.VERTEX;
		private static const PROGRAM_TYPE_FRAGMENT	: String		= Context3DProgramType.FRAGMENT;
		
		private var _cpuConstants		: Dictionary						= new Dictionary();
		private var _bindings			: Object							= null;
		private var _bindingsNames		: Vector.<String>					= new <String>[];
		
		// states
		private var _indexBuffer		: IndexBuffer3DResource				= null;
		private var _firstIndex			: int								= 0;
		
		private var _vertexBuffers		: Vector.<VertexBuffer3DResource>	= new Vector.<VertexBuffer3DResource>(8, true);
		private var _numVertexBuffers	: int								= 0;
		private var _vsConstants		: Vector.<Number>					= null;
		private var _offsets			: Vector.<int>						= new Vector.<int>(8, true);
		private var _formats			: Vector.<String>					= new Vector.<String>(8, true);
		private var _vsInputComponents	: Vector.<VertexComponent>			= null;
		private var _vsInputIndices		: Vector.<uint>						= null;
		
		private var _fsConstants		: Vector.<Number>					= null;
		private var _fsTextures			: Vector.<ITextureResource>			= new Vector.<ITextureResource>(8, true);
		
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
		
		public function get numParameters() : uint
		{
			return _bindingsNames.length;
		}
		
		public function DrawCall(vsConstants 			: Vector.<Number>,
								 fsConstants 			: Vector.<Number>,
								 fsTextures				: Vector.<ITextureResource>,
								 vertexInputComponents	: Vector.<VertexComponent>,
								 vertexInputIndices		: Vector.<uint>,
								 bindings				: Object)
		{
			_vsConstants		= vsConstants.concat();
			_fsConstants		= fsConstants.concat();
			_fsTextures			= fsTextures.concat();
			_vsInputComponents	= vertexInputComponents;
			_vsInputIndices		= vertexInputIndices;
			_bindings			= bindings;
			
			initialize();
		}
		
		private function initialize() : void
		{
			triangleCulling	= TriangleCulling.FRONT;
			blending		= Blending.NORMAL;
			colorMask		= ColorMask.RGBA;
				
			for (var bindingName : String in _bindings)
				_bindingsNames.push(bindingName);
		}
		
		public function clone() : DrawCall
		{
			var clone : DrawCall	= new DrawCall(
				_vsConstants,
				_fsConstants,
				_fsTextures,
				_vsInputComponents,
				_vsInputIndices,
				_bindings
			);
			
			clone._vertexBuffers = _vertexBuffers.concat();
			clone._formats = _formats.concat();
			clone._offsets = _offsets.concat();
			clone._numVertexBuffers = _numVertexBuffers;
			clone._indexBuffer = _indexBuffer;
			
			clone.triangleCulling = triangleCulling;
			clone.blending = blending;
			clone.colorMask = colorMask;
			
			return clone;
		}
		
		public function setStreams(vertexStreams 	: Vector.<IVertexStream>,
								   indexStream		: IndexStream) : void
		{
			_numVertexBuffers	= _vsInputComponents.length;
			_indexBuffer		= indexStream.resource;
			
			for (var i : int = 0; i < _numVertexBuffers; ++i)
			{
				var component	: VertexComponent	= _vsInputComponents[i];
				var index		: uint				= _vsInputIndices[i];
				
				if (component)
				{
					var stream 	: VertexStream	= vertexStreams[index].getStreamByComponent(component);
					if (stream == null)
						throw new Error('Missing vertex component: ' + component.toString() + '.');
					
					var format 	: VertexFormat 	= stream.format;
					
					_vertexBuffers[i] = stream.resource;
					_formats[i] = component.nativeFormatString;
					_offsets[i] = format.getOffsetForComponent(component);
				}
			}
		}
		
		public function apply(context 	: Context3D,
							  previous	: DrawCall) : uint
		{
			if (!_enabled)
				return 0;
			
			context.setColorMask(_colorMaskR, _colorMaskG, _colorMaskB, _colorMaskA);
			/*context.setBlendFactors(_blendingSource, _blendingDest);
			context.setCulling(_triangleCullingStr);*/
			
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
			var maxBuffers	: int	= previous ? previous._numVertexBuffers : 8;
			var i 			: int 	= 0;
			
			for (i = 0; i < numTextures; ++i)
				context.setTextureAt(i, _fsTextures[i].getNativeTexture(context));
			
			while (i < maxTextures)
				context.setTextureAt(i++, null);
			
			for (i = 0; i < _numVertexBuffers; ++i)
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
		
		public function getParameterName(index : uint) : String
		{
			return _bindingsNames[index];
		}
		
		public function hasParameter(name : String) : Boolean
		{
			return _bindings[name] != null;
		}
		
		public function setBindings(meshBindings	: DataBindings,
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
		
		private function parameterChangedHandler(dataBindings	: DataBindings,
												 property		: String,
												 oldValue		: Object,
												 newValue		: Object) : void
		{
			newValue && setParameter(property, newValue);
		}
	}
}