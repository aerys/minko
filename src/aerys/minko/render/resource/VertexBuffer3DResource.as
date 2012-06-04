package aerys.minko.render.resource
{
	import aerys.minko.ns.minko_stream;
	import aerys.minko.type.Signal;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.StreamUsage;
	import aerys.minko.type.stream.VertexStream;
	
	import flash.display3D.Context3D;
	import flash.display3D.VertexBuffer3D;

	/**
	 * VertexBuffer3DResource objects handle vertex buffers allocation
	 * and disposal using the Stage3D API.
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public final class VertexBuffer3DResource implements IResource
	{
		use namespace minko_stream;

		private var _stream			: VertexStream		= null;
		private var _update			: Boolean			= true;
		private var _lengthChanged	: Boolean			= true;
		private var _vertexBuffer	: VertexBuffer3D	= null;
		private var _numVertices	: uint				= 0;
		
		private var _disposed		: Boolean			= false;
		
		private var _uploaded		: Signal			= new Signal('VertexBuffer3DRessource.uploaded');
		
		public function get uploaded() : Signal
		{
			return _uploaded;
		}

		public function get numVertices() : uint
		{
			return _numVertices;
		}

		public function VertexBuffer3DResource(source : VertexStream)
		{
			_stream = source;
			_stream.changed.add(vertexStreamChangedHandler);
		}
		
		private function vertexStreamChangedHandler(vertexStream : VertexStream) : void
		{
			_update = true;
			_lengthChanged = vertexStream.length != _numVertices;
		}

		public function getVertexBuffer3D(context : Context3DResource) : VertexBuffer3D
		{
			var update	: Boolean	= _update;
			
			if (_disposed)
				throw new Error('Unable to render a disposed buffer.');

			if (_lengthChanged)
			{
				_lengthChanged = false;
				_numVertices = _stream.length
				
				if (_vertexBuffer)
					_vertexBuffer.dispose();

				_vertexBuffer = context.createVertexBuffer(
					_numVertices,
					_stream.format.dwordsPerVertex
				);
				
				update = true;
			}

			if (_vertexBuffer != null && update)
			{
				_vertexBuffer.uploadFromVector(_stream._data, 0, numVertices);
				_uploaded.execute(this);

				_update = false;
				_numVertices = numVertices;

				if (!(_stream.usage & StreamUsage.READ) || _stream._localDispose)
					_stream.disposeLocalData(false);
			}

			return _vertexBuffer;
		}

		public function dispose() : void
		{
			if (_vertexBuffer)
			{
				_vertexBuffer.dispose();
				_vertexBuffer = null;
			}
				
			_disposed = true;
			_stream = null;
			_update = false;
			_numVertices = 0;
		}
	}
}