package aerys.minko.type.stream
{
	import aerys.minko.ns.minko_stream;
	import aerys.minko.type.Signal;
	import aerys.minko.type.stream.format.VertexComponent;
	import aerys.minko.type.stream.format.VertexFormat;

	public final class VertexStreamList implements IVertexStream
	{
		use namespace minko_stream;

		private var _streams		: Vector.<VertexStream>	= new Vector.<VertexStream>();
		private var _format			: VertexFormat			= new VertexFormat();
		private var _usage			: uint					= 0;
		
		private var _changed		: Signal				= new Signal('VertexStreamList.changed');
		private var _boundsChanged	: Signal				= new Signal('VertexStream.boundsChanged');

		public function get usage()	: uint
		{
			return _usage;
		}
		
		public function get format() : VertexFormat
		{
			return _format;
		}
		
		public function get numStreams() : uint
		{
			return _streams.length;
		}

		public function get length() : uint
		{
			return _streams.length ? _streams[0].length : 0;
		}
		
		public function get changed() : Signal
		{
			return _changed;
		}
		
		public function get boundsChanged() : Signal
		{
			return _boundsChanged;
		}
		
		public function VertexStreamList(...streams)
		{
			initialize(streams);
		}

		private function initialize(streams : Array) : void
		{
			for each (var stream : VertexStream in streams)
				pushVertexStream(stream);
		}

		public function clone() : VertexStreamList
		{
			var vertexStreamList : VertexStreamList = new VertexStreamList();

			vertexStreamList._streams = _streams.concat();
			vertexStreamList._format = _format.clone();

			return vertexStreamList;
		}

		public function pushVertexStream(vertexStream : VertexStream, force : Boolean = false) : void
		{
			if (length && vertexStream.length != length)
				throw new Error('All streams must have the same total number of vertices.');

			_usage |= vertexStream.usage;
			_format.unionWith(vertexStream.format, force);
			_streams.push(vertexStream);
			
			vertexStream.changed.add(subStreamChangedHandler);
			vertexStream.boundsChanged.add(subStreamBoundsChangedHandler);
		}
		
		public function getSubStreamById(id : int) : VertexStream
		{
			return _streams[id];
		}

		public function getStreamByComponent(vertexComponent : VertexComponent) : VertexStream
		{
			var streamLength	: int = _streams.length;

			for (var i : int = streamLength - 1; i >= 0; --i)
				if (_streams[i].format.hasComponent(vertexComponent))
					return _streams[i];

			return null;
		}

		public function getVertexStream(id : int = 0) : VertexStream
		{
			return id < _streams.length ? _streams[id] : null;
		}

		public function deleteVertexByIndex(index : uint) : Boolean
		{
			if (index > length)
				return false;

			for each (var stream : VertexStream in _streams)
				stream.deleteVertexByIndex(index);

			return true;
		}
		
		public function disposeLocalData(waitForUpload : Boolean = true) : void
		{
			for (var i : int = _streams.length - 1; i >= 0; --i)
				_streams[i].disposeLocalData(waitForUpload);
		}
		
		public function dispose() : void
		{
			for (var i : int = _streams.length - 1; i >= 0; --i)
				_streams[i].dispose();
		}
		
		private function subStreamChangedHandler(subStream : VertexStream) : void
		{
			_changed.execute(this);
		}
		
		private function subStreamBoundsChangedHandler(subStream : VertexStream) : void
		{
			_boundsChanged.execute(this);
		}
	}
}