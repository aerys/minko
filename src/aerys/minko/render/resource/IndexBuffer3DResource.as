package aerys.minko.render.resource
{
	import flash.display3D.IndexBuffer3D;
	
	import aerys.minko.ns.minko_stream;
	import aerys.minko.render.geometry.stream.IndexStream;
	import aerys.minko.render.geometry.stream.StreamUsage;
	import aerys.minko.type.Signal;

	/**
	 * IndexBuffer3DResource objects handle index buffers allocation
	 * and disposal using the Stage3D API.
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public final class IndexBuffer3DResource implements IResource
	{
		use namespace minko_stream;

		private var _stream			: IndexStream	= null;
		private var _update			: Boolean		= true;
		private var _lengthChanged	: Boolean		= true;
		private var _indexBuffer	: IndexBuffer3D	= null;
		private var _numIndices		: uint			= 0;
		
		private var _disposed		: Boolean		= false;
		
		private var _contextLost	: Signal		= new Signal("IndexBuffer3DResource.contextLost");

		public function get contextLost():Signal
		{
			return _contextLost;
		}

		public function get numIndices() : uint
		{
			return _numIndices;
		}

		public function IndexBuffer3DResource(source : IndexStream)
		{
			_stream = source;
			_stream.changed.add(indexStreamChangedHandler);
		}
		
		private function indexStreamChangedHandler(stream : IndexStream) : void
		{
			_update = true;
			_lengthChanged = stream.length != _numIndices;
		}
		
		private function contextLostHandler(context : Context3DResource) : void
		{
			if (_disposed)
				return;
			
			if ((_stream.usage & StreamUsage.READ) && _stream._data != null)
				_indexBuffer = null;
			_contextLost.execute(this);
		}

		public function getIndexBuffer3D(context : Context3DResource) : IndexBuffer3D
		{
			if (!context.contextChanged.hasCallback(contextLostHandler))
				context.contextChanged.add(contextLostHandler);
			
			var update : Boolean	= _update;
			
			if (_disposed)
				throw new Error('Unable to render a disposed buffer.');

			if (_indexBuffer == null || _lengthChanged)
			{
				_lengthChanged = false;
				if (_indexBuffer)
					_indexBuffer.dispose();
				update = true;
				_indexBuffer = context.createIndexBuffer(_stream.length);
			}

			if (update)
			{
				_indexBuffer.uploadFromByteArray(_stream._data, 0, 0, _stream.length);

				_update = false;
				_numIndices = _stream.length;

				if (!(_stream.usage & StreamUsage.READ) || _stream._localDispose)
					_stream.disposeLocalData(false);
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
			
			_disposed = true;
			_stream = null;
			_numIndices = 0;
		}
	}
}
