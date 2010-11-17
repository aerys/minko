package aerys.minko.type.stream
{
	import aerys.minko.ns.minko;
	import aerys.minko.type.vertex.formats.IVertex3DFormat;
	import aerys.minko.type.vertex.formats.NativeFormat;
	import aerys.minko.type.vertex.formats.Vertex3DFormat;
	
	import flash.display3D.Context3D;
	import flash.display3D.Context3DVertexFormat;
	import flash.utils.ByteArray;
	
	
	public final class VertexStream3D
	{
		use namespace minko;
		
		public static const DEFAULT_FORMAT	: IVertex3DFormat			= Vertex3DFormat.XYZ_UV;
		
		minko var _data				: Vector.<Number>					= null;
		minko var _update			: Boolean							= true;
		
		minko var _nativeBuffer		: flash.display3D.VertexBuffer3D	= null;
		
		private var _format			: IVertex3DFormat					= null;
		
		public function get length() : int				{ return _data.length / _format.dwordsPerVertex; }
		public function get format() : IVertex3DFormat	{ return _format; }
	
		public function VertexStream3D(data 	: Vector.<Number>,
									   format	: IVertex3DFormat 	= null)
		{
			super();

			_format = format || DEFAULT_FORMAT;
			
			if (data.length % _format.dwordsPerVertex)
				throw new Error("Incompatible vertex format: the data length does not match");
			
			_data = data ? data.concat() : null;
		}
		
		public function deleteVertexByIndex(myIndex : int) : Boolean
		{
			var max : int = length - 1;
			
			if (myIndex > length)
				return false;
			
			_data.splice(myIndex, _format.dwordsPerVertex);
			
			_update = true;
			
			return true;
		}
		
		public function prepareContext(myContext 	: Context3D,
									   myFormat 	: IVertex3DFormat = null) : void
		{
			var numVertices : int 				= length;
			var format 		: IVertex3DFormat 	= myFormat || _format;

			if (!_nativeBuffer)
			{
				_nativeBuffer = myContext.createVertexBuffer(numVertices,
															 _format.dwordsPerVertex);
			}
			
			if (_update)
			{
				_update = false;
				_nativeBuffer.upload(_data, 0, numVertices);
			}
			
			var formats 	: Vector.<int> 	= format.nativeFormats;
			var numFormats 	: int 			= formats.length;
			var offset 		: int 			= 0;
			
			// set input vertex streams
			for (var i : int = 0; i < numFormats; ++i)
			{
				var nativeFormat : int = formats[i];
				
				myContext.setVertexStream(i,
										  _nativeBuffer,
										  offset,
										  NativeFormat.STRINGS[nativeFormat]);
				
				offset += NativeFormat.NB_DWORDS[nativeFormat];
			}
			
			// disable the other streams
			while (i < 8)
				myContext.setVertexStream(i++, null, 0, Context3DVertexFormat.DISABLED);
		}
		
		public static function fromPositionsAndUVs(positions : Vector.<Number>,
												   uvs		 : Vector.<Number>) : VertexStream3D
		{
			var numVertices : int = positions.length / 3;
			var data : Vector.<Number> = new Vector.<Number>(numVertices * 5, true);
			
			for (var i : int = 0; i < numVertices; ++i)
			{
				data[int(i * 5)] = positions[int(i * 3)];
				data[int(i * 5 + 1)] = positions[int(i * 3 + 1)];
				data[int(i * 5 + 2)] = positions[int(i * 3 + 2)];
				data[int(i * 5 + 3)] = uvs[int(i * 2)];
				data[int(i * 5 + 4)] = uvs[int(i * 2 + 1)];
			}
			
			return new VertexStream3D(data, Vertex3DFormat.XYZ_UV);
		}
		
		public static function fromByteArray(data 	: ByteArray,
											 count	: int,
											 format	: IVertex3DFormat) : VertexStream3D
		{
			var nativeFormats	: Vector.<int>		= format.nativeFormats;
			var numFormats		: int				= nativeFormats.length;
			var length			: int				= 0;
			var tmp				: Vector.<Number>	= null;
			var stream			: VertexStream3D	= new VertexStream3D(null, format);

			stream._data = tmp;

			tmp = new Vector.<Number>(format.dwordsPerVertex * count,
									  true);
			
			for (var j : int = 0; j < count; ++j)
			{
				for (var i : int = 0; i < numFormats; ++i)
				{
					switch (nativeFormats[i])
					{
						case NativeFormat.RGBA :
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