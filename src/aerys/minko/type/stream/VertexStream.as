package aerys.minko.type.stream
{
	import aerys.minko.ns.minko_stream;
	import aerys.minko.render.ressource.VertexBufferRessource;
	import aerys.minko.type.IVersionnable;
	import aerys.minko.type.stream.format.VertexComponent;
	import aerys.minko.type.stream.format.VertexComponentType;
	import aerys.minko.type.stream.format.VertexFormat;
	
	import flash.display3D.Context3D;
	import flash.display3D.VertexBuffer3D;
	import flash.events.ProgressEvent;
	import flash.utils.ByteArray;
	
	
	public final class VertexStream implements IVersionnable, IVertexStream
	{
		use namespace minko_stream;
		
		public static const DEFAULT_FORMAT	: VertexFormat	= VertexFormat.XYZ_UV;
		
		minko_stream var _data		: Vector.<Number>		= null;
		
		private var _dynamic		: Boolean				= false;
		private var _version		: uint					= 0;
		
		private var _format			: VertexFormat			= null;
		private var _ressource		: VertexBufferRessource		= null;
		private var _length			: uint					= 0;
		
		public function get format()	: VertexFormat		{ return _format; }
		public function get version()	: uint				{ return _version; }
		public function get dynamic()	: Boolean			{ return _dynamic; }
		public function get ressource()	: VertexBufferRessource	{ return _ressource; }
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
			_ressource = new VertexBufferRessource(this);
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
		
		public static function fromPositionsAndUVs(positions : Vector.<Number>,
												   uvs		 : Vector.<Number> = null) : VertexStream
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
			
			return new VertexStream(data, uvs ? VertexFormat.XYZ_UV : VertexFormat.XYZ);
		}
		
		public static function fromByteArray(data 	: ByteArray,
											 count	: int,
											 format	: VertexFormat) : VertexStream
		{
			var numFormats		: int				= format.components.length;
			var nativeFormats	: Vector.<int>		= new Vector.<int>(numFormats, true);
			var length			: int				= 0;
			var tmp				: Vector.<Number>	= null;
			var stream			: VertexStream		= new VertexStream(null, format);
			
			for (var k : int = 0; k < numFormats; k++)
				nativeFormats[k] = format.components[k].nativeFormat;
			
			stream._data = tmp;
			
			tmp = new Vector.<Number>(format.dwordsPerVertex * count, true);
			
			for (var j : int = 0; j < count; ++j)
			{
				for (var i : int = 0; i < numFormats; ++i)
				{
					switch (nativeFormats[i])
					{
						case VertexComponentType.FLOAT_4 :
							tmp[int(length++)] = data.readFloat();
						case VertexComponentType.FLOAT_3 :
							tmp[int(length++)] = data.readFloat();
						case VertexComponentType.FLOAT_2 :
							tmp[int(length++)] = data.readFloat();
						case VertexComponentType.FLOAT_1 :
							tmp[int(length++)] = data.readFloat();
							break ;
					}
				}
			}
			
			return stream;
		}
		
	}
}