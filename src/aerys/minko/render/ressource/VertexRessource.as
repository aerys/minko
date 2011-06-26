package aerys.minko.render.ressource
{
	import aerys.minko.ns.minko_stream;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.vertex.format.VertexComponent;
	import aerys.minko.type.vertex.format.VertexFormat;
	
	import flash.display3D.Context3D;
	import flash.display3D.VertexBuffer3D;
	import flash.utils.Dictionary;

	public class VertexRessource implements IRessource
	{
		use namespace minko_stream;
		
		private var _stream			: IVertexStream		= null;
		private var _vertexBuffers	: Array				= new Array();
				
		public function VertexRessource(source : IVertexStream)
		{
			_stream = source;
		}
		
		/*public function getNativeBuffer(context : Context3D, component : VertexComponent) : VertexBuffer3D
		{
			var stream			: VertexStream 		= _stream.getSubStreamByComponent(component);
			var buffer			: VertexBuffer3D	= _vertexBuffers[stream.id];
			var data 			: Vector.<Number> 	= stream._data;
			var format 			: VertexFormat 		= stream.format;
			var streamLength 	: int 				= stream._length;
			var dwordsPerVertex : int 				= stream.format.dwordsPerVertex;
			var currentLength 	: int 				= data ? data.length / dwordsPerVertex : streamLength;
			
			if ((!buffer && data.length) || streamLength != currentLength)
			{
				buffer = context.createVertexBuffer(length, format.dwordsPerVertex);
				_vertexBuffers[stream.id] = buffer;
				
				stream._length = currentLength;
				stream._update = true;
			}
			
			if (buffer && stream._update)
			{
				stream._update = false;
				buffer.uploadFromVector(data, 0, stream._length);
				trace("upload");
				
				if (!stream._dynamic)
					stream._data = null;
			}
			
			return buffer;
		}*/
		
		public function dispose() : Boolean
		{
			for each (var vb : VertexBuffer3D in _vertexBuffers)
				vb.dispose();
				
			return true;
		}
	}
}