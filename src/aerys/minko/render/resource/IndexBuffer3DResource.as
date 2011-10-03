package aerys.minko.render.resource
{
	import aerys.minko.ns.minko_stream;
	import aerys.minko.type.stream.IndexStream;

	import flash.display3D.Context3D;
	import flash.display3D.IndexBuffer3D;

	public final class IndexBuffer3DResource implements IResource
	{
		use namespace minko_stream;

		private var _stream			: IndexStream	= null;
		private var _streamVersion	: uint			= 0;
		private var _indexBuffer	: IndexBuffer3D	= null;
		private var _numIndices		: uint			= 0;

		public function get numIndices()	: uint	{ return _numIndices; }

		public function IndexBuffer3DResource(source : IndexStream)
		{
			_stream = source;
		}

		public function getIndexBuffer3D(context : Context3D) : IndexBuffer3D
		{
			var update : Boolean	= _stream.version != _streamVersion;

			if (_stream.length == 0)
				return null;

			if (!_indexBuffer || _stream.length != _numIndices)
			{
				if (_indexBuffer)
					_indexBuffer.dispose();
				update = true;
				_indexBuffer = context.createIndexBuffer(_stream.length);
			}

			if (update)
			{
				_indexBuffer.uploadFromVector(_stream._data, 0, _stream.length);

				_streamVersion = _stream.version;
				_numIndices = _stream.length;

				if (!_stream.dynamic)
					_stream.disposeLocalData();
			}

			return _indexBuffer;
		}

		public function dispose() : void
		{
			_indexBuffer.dispose();
			_indexBuffer = null;
		}
	}
}
