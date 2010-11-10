package aerys.minko.type.stream
{
	import aerys.minko.ns.minko;
	import aerys.minko.type.vertex.formats.IVertex3DFormat;
	import aerys.minko.type.vertex.formats.NativeFormat;
	import aerys.minko.type.vertex.formats.Vertex3DFormat;
	
	import flash.display3D.Context3D;
	import flash.display3D.Context3DVertexFormat;
	
	
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
	
		public function VertexStream3D(myData 		: Vector.<Number>,
									   myFormat		: IVertex3DFormat = null)
		{
			super();
			
			_data = myData.concat();
			_format = myFormat || DEFAULT_FORMAT;
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
		
		public static function fromVerticesAndUVs(myVertices : Vector.<Number>,
												  myUVs		 : Vector.<Number>) : VertexStream3D
		{
			var numVertices : int = myVertices.length / 3;
			var data : Vector.<Number> = new Vector.<Number>(numVertices * 5, true);
			
			for (var i : int = 0; i < numVertices; ++i)
			{
				data[int(i * 5)] = myVertices[int(i * 3)];
				data[int(i * 5 + 1)] = myVertices[int(i * 3 + 1)];
				data[int(i * 5 + 2)] = myVertices[int(i * 3 + 2)];
				data[int(i * 5 + 3)] = myUVs[int(i * 2)];
				data[int(i * 5 + 4)] = myUVs[int(i * 2 + 1)];
			}
			
			return new VertexStream3D(data, Vertex3DFormat.XYZ_UV);
		}
	}
}