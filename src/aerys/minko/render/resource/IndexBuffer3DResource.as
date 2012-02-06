package aerys.minko.render.resource
{
	import aerys.minko.ns.minko_stream;
	import aerys.minko.type.stream.IndexStream;
	import aerys.minko.type.stream.StreamUsage;
	
	import flash.display3D.Context3D;
	import flash.display3D.IndexBuffer3D;
	import flash.events.Event;

	public final class IndexBuffer3DResource implements IResource
	{
		use namespace minko_stream;

		private var _stream			: IndexStream	= null;
		private var _update			: Boolean		= true;
		private var _indexBuffer	: IndexBuffer3D	= null;
		private var _numIndices		: uint			= 0;

		public function get numIndices()	: uint	{ return _numIndices; }

		public function IndexBuffer3DResource(source : IndexStream)
		{
			_stream = source;
			
			initialize();
		}
		
		private function initialize() : void
		{
			_stream.changed.add(indexStreamChangedHandler);
		}
		
		private function indexStreamChangedHandler(stream : IndexStream, property : String) : void
		{
			_update = true;
		}

		public function getIndexBuffer3D(context : Context3D) : IndexBuffer3D
		{
			var update : Boolean	= _update;

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

				_update = false;
				_numIndices = _stream.length;

				if (!(_stream.usage & StreamUsage.READ))
					_stream.disposeLocalData();
			}

			return _indexBuffer;
		}

		public function dispose() : void
		{
			if (_indexBuffer)
			{
				_indexBuffer.dispose();
				_indexBuffer = null;
			}
		}
	}
}
