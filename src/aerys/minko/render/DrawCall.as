package aerys.minko.render
{
	import aerys.minko.ns.minko_render;
	import aerys.minko.render.geometry.Geometry;
	import aerys.minko.render.geometry.stream.IVertexStream;
	import aerys.minko.render.geometry.stream.StreamUsage;
	import aerys.minko.render.geometry.stream.VertexStream;
	import aerys.minko.render.geometry.stream.format.VertexComponent;
	import aerys.minko.render.geometry.stream.format.VertexFormat;
	import aerys.minko.render.resource.Context3DResource;
	import aerys.minko.render.resource.IndexBuffer3DResource;
	import aerys.minko.render.resource.Program3DResource;
	import aerys.minko.render.resource.VertexBuffer3DResource;
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.render.shader.binding.IBinder;
	import aerys.minko.type.binding.DataBindings;
	import aerys.minko.type.enum.Blending;
	import aerys.minko.type.enum.ColorMask;
	import aerys.minko.type.enum.TriangleCulling;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	
	import flash.display3D.Context3DProgramType;
	import flash.utils.Dictionary;
	
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
		private var _numTriangles		: int								= -1;
		
		private var _vertexBuffers		: Vector.<VertexBuffer3DResource>	= new Vector.<VertexBuffer3DResource>(NUM_VERTEX_BUFFERS, true);
		private var _numVertexComponents: uint								= 0;
		private var _offsets			: Vector.<int>						= new Vector.<int>(8, true);
		private var _formats			: Vector.<String>					= new Vector.<String>(8, true);
		
		private var _blending			: uint								= 0;
		private var _blendingSource		: String							= null;
		private var _blendingDest		: String							= null;
		
		private var _triangleCulling	: uint								= 0;
		private var _triangleCullingStr	: String							= null;
		
		private var _enabled			: Boolean							= true;
		
		private var _depth				: Number							= 0.;
        private var _center				: Vector4							= null;
		private var _invalidDepth		: Boolean							= false;
		private var _localToWorld		: Matrix4x4							= null;
		private var _worldToScreen		: Matrix4x4							= null;
        
		private var _bindingsConsumer	: DrawCallBindingsConsumer;
		
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

		public function get enabled() : Boolean
		{
			return _enabled;
		}
		public function set enabled(value : Boolean) : void
		{
			_enabled = value;
			if (_bindingsConsumer)
				_bindingsConsumer.enabled = value;
		}
		
		public function get depth() : Number
		{
			if (_invalidDepth && _enabled)
			{
				_invalidDepth = false;
				
				if (_localToWorld != null && _worldToScreen != null)
				{
					var worldSpacePosition 	: Vector4	= _localToWorld.transformVector(
						_center, TMP_VECTOR4
					);
					var screenSpacePosition : Vector4 	= _worldToScreen.transformVector(
						worldSpacePosition, TMP_VECTOR4
					);
					
					_depth = screenSpacePosition.z / screenSpacePosition.w;
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
			_invalidDepth = computeDepth;
			
			setProgram(program);
			setGeometry(geometry);
			setBindings(meshBindings, sceneBindings, computeDepth);
		}
		
		public function unsetBindings(meshBindings	: DataBindings,
									  sceneBindings	: DataBindings) : void
		{
			if (_bindingsConsumer != null)
			{
				meshBindings.removeConsumer(_bindingsConsumer);
				sceneBindings.removeConsumer(_bindingsConsumer);
			}
			
			if (sceneBindings.hasCallback('worldToScreen', transformChangedHandler))
				sceneBindings.removeCallback('worldToScreen', transformChangedHandler);
			
			if (meshBindings.hasCallback('localToWorld', transformChangedHandler))
				meshBindings.removeCallback('localToWorld', transformChangedHandler);
            
            _bindingsConsumer = null;
		}
		
		private function setProgram(program : Program3DResource) : void
		{
			_cpuConstants		= new Dictionary();
			_vsConstants		= program._vsConstants.slice();
			_fsConstants		= program._fsConstants.slice();
			_fsTextures			= program._fsTextures.slice();
			_vsInputComponents	= program._vertexInputComponents;
			_vsInputIndices		= program._vertexInputIndices;
			_bindings			= program._bindings;
			
			_bindingsConsumer	= new DrawCallBindingsConsumer(
				_bindings,
				_cpuConstants,
				_vsConstants,
				_fsConstants,
				_fsTextures
			);
			_bindingsConsumer.enabled = _enabled;
			
			triangleCulling		= TriangleCulling.FRONT;
			blending			= Blending.OPAQUE;
		}
		
		 /**
		  * Ask geometry to compute additional vertex data if needed for this drawcall.
		  */
		public function updateGeometry(geometry : Geometry) : void
		{
			var vertexFormat    : VertexFormat	= geometry.format;
            var hasNormals      : Boolean       = vertexFormat.hasComponent(VertexComponent.NORMAL);
			
			if (_vsInputComponents.indexOf(VertexComponent.TANGENT) >= 0
				&& !vertexFormat.hasComponent(VertexComponent.TANGENT))
			{
				geometry.computeTangentSpace(!hasNormals);
			}
			else if (_vsInputComponents.indexOf(VertexComponent.NORMAL) >= 0 && !hasNormals)
			{
				geometry.computeNormals();
			}
		}
		
		/**
		 * Obtain a reference to each buffer and offset that apply() may possibly need.
		 * 
		 */		
		public function setGeometry(geometry : Geometry, frame : uint = 0) : void
		{
			if (!_vsInputComponents)
				return ;
			
			updateGeometry(geometry);
			
			_center = geometry.boundingSphere
				? geometry.boundingSphere.center
				: Vector4.ZERO;
			
			_numVertexComponents = _vsInputComponents.length;
			_indexBuffer		 = geometry.indexStream.resource;
			_firstIndex			 = geometry.firstIndex;
			_numTriangles		 = geometry.numTriangles;
			
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
			meshBindings.addConsumer(_bindingsConsumer);
			sceneBindings.addConsumer(_bindingsConsumer);
			
			if (computeDepth)
			{

                if (sceneBindings.propertyExists('worldToScreen'))
                    _worldToScreen = sceneBindings.getProperty('worldToScreen');
				sceneBindings.addCallback('worldToScreen', transformChangedHandler);
                
                if (meshBindings.propertyExists('localToWorld'))
                    _localToWorld = meshBindings.getProperty('localToWorld');
				meshBindings.addCallback('localToWorld', transformChangedHandler);
				
				_invalidDepth = true;
			}
		}

		public function apply(context : Context3DResource, previous : DrawCall) : uint
		{
			context.setProgramConstantsFromVector(PROGRAM_TYPE_VERTEX, 0, _vsConstants)
				   .setProgramConstantsFromVector(PROGRAM_TYPE_FRAGMENT, 0, _fsConstants);
			
			var numTextures	: uint	= _fsTextures.length;
			var maxTextures	: uint	= previous ? previous._fsTextures.length : NUM_TEXTURES;
			var maxBuffers	: uint	= previous ? previous._numVertexComponents : NUM_VERTEX_BUFFERS;
			var i 			: uint 	= 0;

			// setup textures
			for (i = 0; i < numTextures; ++i)
			{
				context.setTextureAt(
					i,
					(_fsTextures[i] as ITextureResource).getTexture(context)
				);
			}
			
			while (i < maxTextures)
				context.setTextureAt(i++, null);

			// setup buffers
			for (i = 0; i < _numVertexComponents; ++i)
			{
				context.setVertexBufferAt(
					i,
					(_vertexBuffers[i] as VertexBuffer3DResource).getVertexBuffer3D(context),
					_offsets[i],
					_formats[i]
				);
			}
			
			while (i < maxBuffers)
				context.setVertexBufferAt(i++, null);
			
			// draw triangles
			context.drawTriangles(
				_indexBuffer.getIndexBuffer3D(context),
				_firstIndex,
				_numTriangles
			);
			
			return _numTriangles == -1 ? _indexBuffer.numIndices / 3 : _numTriangles;
		}
		
		public function setParameter(name : String, value : Object) : void
		{
			_bindingsConsumer.setProperty(name, value);
		}
		
		private function transformChangedHandler(bindings 	: DataBindings,
												 property 	: String,
												 oldValue	: Matrix4x4,
												 newValue	: Matrix4x4) : void
		{
			if (property == 'worldToScreen')
				_worldToScreen = newValue;
			else if (property == 'localToWorld')
				_localToWorld = newValue;
			
			_invalidDepth = true;
		}
	}
}
