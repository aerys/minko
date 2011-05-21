package aerys.minko.type.stream
{
	import aerys.common.IVersionnable;
	import aerys.minko.ns.minko_stream;
	import aerys.minko.type.vertex.format.NativeFormat;
	import aerys.minko.type.vertex.format.VertexComponent;
	import aerys.minko.type.vertex.format.VertexFormat;
	
	import flash.display3D.Context3D;
	import flash.display3D.VertexBuffer3D;
	import flash.utils.ByteArray;
	
	
	public final class VertexStream implements IVersionnable, IVertexStream
	{
		use namespace minko_stream;
		
		public static const DEFAULT_FORMAT	: VertexFormat	= VertexFormat.XYZ_UV;
		
		minko_stream var _data			: Vector.<Number>		= null;
		minko_stream var _update		: Boolean				= true;
		minko_stream var _dynamic		: Boolean				= false;
		minko_stream var _nativeBuffer	: VertexBuffer3D		= null;
		minko_stream var _version		: uint					= 0;
		
		private var _format			: VertexFormat			= null;
		private var _length			: int					= 0;
		
		public function get length() 	: uint				{ return _data ? _data.length / _format.dwordsPerVertex : _length; }
		public function get format()	: VertexFormat		{ return _format; }
		public function get version()	: uint				{ return _version; }
		public function get dynamic()	: Boolean			{ return _dynamic; }
		
		public function VertexStream(data 		: Vector.<Number>,
									 format		: VertexFormat 	= null,
									 dynamic	: Boolean		= false)
		{
			super();
			
			_format = format || DEFAULT_FORMAT;
			
			if (data.length % _format.dwordsPerVertex)
				throw new Error("Incompatible vertex format: the data length does not match.");
			
			_data = data ? data.concat() : null;
			_dynamic = dynamic;
		}
		
		minko_stream function getNativeVertexBuffer3D(context : Context3D) : VertexBuffer3D
		{
			var currentLength : int = _data ? _data.length / _format.dwordsPerVertex : _length;
			
			if ((!_nativeBuffer && _data.length) || _length != currentLength)
			{
				_nativeBuffer = context.createVertexBuffer(length, format.dwordsPerVertex);
				_length = currentLength;
				_update = true;
			}
			
			if (_nativeBuffer && _update)
			{
				_update = false;
				_nativeBuffer.uploadFromVector(_data, 0, _length);
				
				if (!_dynamic)
					_data = null;
			}
			
			return _nativeBuffer;
		}
		
		public function deleteVertexByIndex(index : uint) : Boolean
		{
			if (index > length)
				return false;
			
			_data.splice(index, _format.dwordsPerVertex);
			_update = true;
			
			return true;
		}
		
		public function getVertexStreamByComponent(vertexComponent : VertexComponent) : VertexStream
		{
			if (vertexComponent in _format.components)
				return this;
			
			return null;
		}
		
		public function push(...data) : void
		{
			var dataLength : int = data.length;
			
			if (dataLength % _format.dwordsPerVertex)
				throw new Error("Invalid data length.");
			
			for (var i : int = 0; i < dataLength; i++)
				_data.push(data[i]);
			
			++_version;
			_update = true;
		}
		
		public function pushData(data : Vector.<Number>) : void
		{
			var dataLength : int = data.length;
			
			if (dataLength % _format.dwordsPerVertex)
				throw new Error("Invalid data length.");
			
			for (var i : int = 0; i < dataLength; i++)
				_data.push(data[i]);
			
			++_version;
			_update = true;
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
			
			tmp = new Vector.<Number>(format.dwordsPerVertex * count,
				true);
			
			for (var j : int = 0; j < count; ++j)
			{
				for (var i : int = 0; i < numFormats; ++i)
				{
					switch (nativeFormats[i])
					{
						case NativeFormat.FLOAT_4 :
							tmp[int(length++)] = data.readFloat();
						case NativeFormat.FLOAT_3 :
							tmp[int(length++)] = data.readFloat();
						case NativeFormat.FLOAT_2 :
							tmp[int(length++)] = data.readFloat();
						case NativeFormat.FLOAT_1 :
							tmp[int(length++)] = data.readFloat();
							break ;
					}
				}
			}
			
			return stream;
		}
		
	}
}