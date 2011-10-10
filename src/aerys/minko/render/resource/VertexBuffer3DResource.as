package aerys.minko.render.resource
{
	import aerys.minko.ns.minko_stream;
	import aerys.minko.type.stream.VertexStream;

	import flash.display3D.Context3D;
	import flash.display3D.VertexBuffer3D;

	public class VertexBuffer3DResource implements IResource
	{
		use namespace minko_stream;

		private var _stream			: VertexStream		= null;
		private var _streamVersion	: uint				= 0;
		private var _vertexBuffer	: VertexBuffer3D	= null;
		private var _numVertices	: uint				= 0;

		public function get numVertices() : uint	{ return _numVertices; }

		public function VertexBuffer3DResource(source : VertexStream)
		{
			_stream = source;
		}

		public function getVertexBuffer3D(context : Context3D) : VertexBuffer3D
		{
			var update		: Boolean			= _stream.version != _streamVersion;
			var numVertices	: uint				= _stream.length;

			if (numVertices && (!_vertexBuffer || numVertices != _numVertices))
			{
				if (_vertexBuffer)
					_vertexBuffer.dispose();

				_vertexBuffer = context.createVertexBuffer(_stream.length,
														   _stream.format.dwordsPerVertex);
				update = true;
			}

			if (_vertexBuffer && update)
			{
				_vertexBuffer.uploadFromVector(_stream._data, 0, numVertices);

				_streamVersion = _stream.version;
				_numVertices = numVertices;

				if (!_stream.dynamic)
					_stream.disposeLocalData();
			}

			return _vertexBuffer;
		}

		public function dispose() : void
		{
			_vertexBuffer.dispose();
		}
	}
}