package aerys.minko.type.stream
{
	import aerys.minko.ns.minko_stream;
	import aerys.minko.render.resource.VertexBufferResource;
	import aerys.minko.type.IVersionable;
	import aerys.minko.type.stream.format.VertexComponent;
	import aerys.minko.type.stream.format.VertexComponentType;
	import aerys.minko.type.stream.format.VertexFormat;
	
	import flash.utils.ByteArray;
	
	public final class VertexStream implements IVersionable, IVertexStream
	{
		use namespace minko_stream;
		
		public static const DEFAULT_FORMAT	: VertexFormat	= VertexFormat.XYZ_UV;
		
		minko_stream var _data		: Vector.<Number>		= null;
		
		private var _dynamic		: Boolean				= false;
		private var _version		: uint					= 0;
		
		private var _format			: VertexFormat			= null;
		private var _ressource		: VertexBufferResource		= null;
		private var _length			: uint					= 0;
		
		public function get format()	: VertexFormat		{ return _format; }
		public function get version()	: uint				{ return _version; }
		public function get dynamic()	: Boolean			{ return _dynamic; }
		public function get ressource()	: VertexBufferResource	{ return _ressource; }
		public function get length()	: uint				{ return _length; }
		
		public function VertexStream(data 		: Vector.<Number>	= null,
									 format		: VertexFormat 		= null,
									 dynamic	: Boolean			= false)
		{
			super();
			
			initialize(data, format, dynamic);
		}
		
		private function initialize(data 	: Vector.<Number>	= null,
									format	: VertexFormat 		= null,
									dynamic	: Boolean			= false) : void
		{
			_ressource = new VertexBufferResource(this);
			_format = format || DEFAULT_FORMAT;
			
			if (data && data.length && data.length % _format.dwordsPerVertex)
				throw new Error("Incompatible vertex format: the data length does not match.");
			
			_data = data ? data.concat() : new Vector.<Number>();
			_dynamic = dynamic;
			
			invalidate();
		}
		
		public function deleteVertexByIndex(index : uint) : Boolean
		{
			if (index > length)
				return false;
			
			_data.splice(index, _format.dwordsPerVertex);
			
			invalidate();
			
			return true;
		}
		
		public function getSubStreamByComponent(vertexComponent : VertexComponent) : VertexStream
		{
			if (_format.components.indexOf(vertexComponent) != -1)
				return this;
			
			return null;
		}
		
		public function getSubStreamById(id : int) : VertexStream
		{
			return this;
		}
		
		public function push(...data) : void
		{
			var dataLength : int = data.length;
			
			if (dataLength % _format.dwordsPerVertex)
				throw new Error("Invalid data length.");
			
			for (var i : int = 0; i < dataLength; i++)
				_data.push(data[i]);
			
			invalidate();
		}
		
		public function pushData(data : Vector.<Number>) : void
		{
			var dataLength : int = data.length;
			
			if (dataLength % _format.dwordsPerVertex)
				throw new Error("Invalid data length.");
			
			for (var i : int = 0; i < dataLength; i++)
				_data.push(data[i]);
			
			invalidate();
		}
		
		public function disposeLocalData() : void
		{
			if (length != ressource.numVertices)
				throw new Error("Unable to dispose local data: "
								+ "some vertices have not been uploaded.");
			
			_data = null;
			_dynamic = false;
		}
		
		minko_stream function invalidate() : void
		{
			_length = _data.length / _format.dwordsPerVertex;
			++_version;
		}
		
		public static function fromPositionsAndUVs(positions 	: Vector.<Number>,
												   uvs		 	: Vector.<Number> 	= null,
												   dynamic		: Boolean			= false) : VertexStream
		{
			var numVertices : int 				= positions.length / 3;
			var stride 		: int 				= uvs ? 5 : 3;
			var data 		: Vector.<Number> 	= new Vector.<Number>(numVertices * stride, true);
			
			for (var i : int = 0; i < numVertices; ++i)
			{
				var offset : int = i * stride;
				
				data[offset] = positions[int(i * 3)];
				data[int(offset + 1)] = positions[int(i * 3 + 1)];
				data[int(offset + 2)] = positions[int(i * 3 + 2)];
				
				if (uvs)
				{
					data[int(offset + 3)] = uvs[int(i * 2)];
					data[int(offset + 4)] = uvs[int(i * 2 + 1)];
				}
			}
			
			return new VertexStream(data,
									uvs ? VertexFormat.XYZ_UV : VertexFormat.XYZ,
									dynamic);
		}
		
		public static function extractSubStream(source			: IVertexStream,
										 		vertexFormat 	: VertexFormat	= null) : VertexStream
		{
			vertexFormat ||= source.format;
			
			var newVertexStreamData			: Vector.<Number>			= new Vector.<Number>();
			
			var components					: Vector.<VertexComponent>	= vertexFormat.componentList;
			var numComponents				: uint						= components.length;
			var componentOffsets			: Vector.<uint>				= new Vector.<uint>(numComponents, true);
			var componentSizes				: Vector.<uint>				= new Vector.<uint>(numComponents, true);
			var componentDwordsPerVertex	: Vector.<uint>				= new Vector.<uint>(numComponents, true);
			var componentDatas				: Vector.<Vector.<Number>>	= new Vector.<Vector.<Number>>(numComponents, true);
			
			var totalVertices				: int						= 0;
			var totalIndices				: int						= 0;
			
			// cache get offsets, sizes, and buffers for each components
			for (var k : int = 0; k < numComponents; ++k)
			{
				var vertexComponent	: VertexComponent	= components[k];
				var subVertexStream	: VertexStream		= source.getSubStreamByComponent(vertexComponent);
				var subvertexFormat	: VertexFormat		= subVertexStream.format;
				
				componentOffsets[k]			= subvertexFormat.getOffsetForComponent(vertexComponent);
				componentDwordsPerVertex[k]	= subvertexFormat.dwordsPerVertex;
				componentSizes[k]			= vertexComponent.dwords;
				componentDatas[k]			= subVertexStream._data;
			}
			
			// push vertex data into the new buffer.
			var numVertices : uint 	= source.length;
			
			for (var vertexId : uint = 0; vertexId < numVertices; ++vertexId)
			{
				for (var componentId : int = 0; componentId < numComponents; ++componentId)
				{
					var vertexData		: Vector.<Number>	= componentDatas[componentId];
					var componentSize	: uint				= componentSizes[componentId];
					var componentOffset	: uint				= componentOffsets[componentId]
															  + vertexId * componentDwordsPerVertex[componentId];
					var componentLimit	: uint				= componentSize + componentOffset;
					
					for (var n : int = componentOffset; n < componentLimit; ++n, ++totalVertices)
						newVertexStreamData[totalVertices] = vertexData[n];
				}
			}
			
			// avoid copying data vectors
			var newVertexStream		: VertexStream	= new VertexStream(null, vertexFormat);
			
			newVertexStream._data = newVertexStreamData;
			newVertexStream.invalidate();
			
			return newVertexStream;
		}

		public static function fromByteArray(bytes 		: ByteArray,
											 count		: int,
											 formatIn	: VertexFormat,
											 formatOut	: VertexFormat	= null,
											 dynamic	: Boolean		= false,
											 reader 	: Function 		= null,
											 dwordSize	: uint			= 4) : VertexStream
		{
			formatOut ||= formatIn;
			reader ||= bytes.readFloat;
			
			var dataLength		: int						= 0;
			var data			: Vector.<Number>			= null;
			var stream			: VertexStream				= new VertexStream(null, formatOut, dynamic);
			var start			: int						= bytes.position;
			var componentsOut	: Vector.<VertexComponent>	= formatOut.components;
			var numComponents	: int						= componentsOut.length;
			var nativeFormats	: Vector.<int>				= new Vector.<int>(numComponents, true);
			
			for (var k : int = 0; k < numComponents; k++)
				nativeFormats[k] = componentsOut[k].nativeFormat;
			
			data = new Vector.<Number>(formatOut.dwordsPerVertex * count, true);
			for (var vertexId : int = 0; vertexId < count; ++vertexId)
			{
				for (var componentId : int = 0; componentId < numComponents; ++componentId)
				{
					bytes.position = start + formatIn.dwordsPerVertex * vertexId * dwordSize
									+ formatIn.getOffsetForComponent(componentsOut[componentId]) * dwordSize;
					
					switch (nativeFormats[componentId])
					{
						case VertexComponentType.FLOAT_4 :
							data[int(dataLength++)] = reader();
						case VertexComponentType.FLOAT_3 :
							data[int(dataLength++)] = reader();
						case VertexComponentType.FLOAT_2 :
							data[int(dataLength++)] = reader();
						case VertexComponentType.FLOAT_1 :
							data[int(dataLength++)] = reader();
							break ;
					}
				}
			}
			// make sure the ByteArray position is at the end of the buffer
			bytes.position = start + formatIn.dwordsPerVertex * count * dwordSize;
			
			stream._data = data;
			stream.invalidate();

			return stream;
		}
		
	}
}