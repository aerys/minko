package aerys.minko.render.ressource
{
	import aerys.minko.ns.minko_stream;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.stream.format.VertexComponent;
	
	import flash.display3D.Context3D;
	import flash.display3D.VertexBuffer3D;
	import flash.utils.Dictionary;

	public class VertexBufferRessource implements IRessource
	{
		use namespace minko_stream;
		
		private var _stream			: IVertexStream		= null;
		private var _streamVersion	: uint				= 0;
		private var _vertexBuffer	: VertexBuffer3D	= null;
		private var _numVertices	: uint				= 0;
		
		public function get numVertices() : uint	{ return _numVertices; }
		
		public function VertexBufferRessource(source : IVertexStream)
		{
			_stream = source;
		}
		
		public function getVertexBuffer3D(context : Context3D, component : VertexComponent) : VertexBuffer3D
		{
			var vstream		: VertexStream		= _stream.getSubStreamByComponent(component);
			
			if (!(_stream is VertexStream))
				return vstream.ressource.getVertexBuffer3D(context, component);
			
			var update		: Boolean			= vstream.version != _streamVersion;
			var numVertices	: uint				= vstream.length;
			
			if (numVertices && (!_vertexBuffer || numVertices != _numVertices))
			{
				if (_vertexBuffer)
					_vertexBuffer.dispose();
				
				_vertexBuffer = context.createVertexBuffer(vstream.length,
														   vstream.format.dwordsPerVertex);
				update = true;
			}
			
			if (_vertexBuffer && update)
			{
				_vertexBuffer.uploadFromVector(vstream._data, 0, numVertices);
				
				_streamVersion = vstream.version;
				_numVertices = numVertices;
				
				if (!vstream.dynamic)
					vstream.disposeLocalData();
			}
			
			return _vertexBuffer;
		}
		
		public function dispose() : void
		{
			_vertexBuffer.dispose();
		}
	}
}