package aerys.minko.type.stream
{
	import aerys.minko.ns.minko_stream;
	import aerys.minko.type.IVersionable;
	import aerys.minko.type.stream.format.VertexComponent;
	import aerys.minko.type.stream.format.VertexFormat;

	public final class VertexStreamList implements IVersionable, IVertexStream
	{
		use namespace minko_stream;

		private var _streams		: Vector.<VertexStream>	= new Vector.<VertexStream>();
		private var _streamVersions	: Vector.<int>			= new Vector.<int>();
		private var _format			: VertexFormat			= new VertexFormat();
		private var _version		: int					= 0;
		private var _dynamic		: Boolean				= false;

		public function get version()		: uint 				{ return _version; }
		public function get dynamic()		: Boolean 			{ return _dynamic; }
		public function get format()		: VertexFormat		{ return _format; }
		public function get numStreams()	: uint				{ return _streams.length; }

		public function get length()	: uint
		{
			return _streams.length ? _streams[0].length : 0;
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
			vertexStreamList._streamVersions = _streamVersions.concat();
			vertexStreamList._format = _format.clone();
			vertexStreamList._version = _version;
			vertexStreamList._dynamic = _dynamic;

			return vertexStreamList;
		}

		public function pushVertexStream(vertexStream : IVertexStream, force : Boolean = false) : void
		{
			if (length && vertexStream.length != length)
				throw new Error('All VertexStream must have the same total number of vertices.');

			_dynamic ||= vertexStream;
			_format.unionWith(vertexStream.format, force);
			++_version;

			if (vertexStream is VertexStream)
			{
				_streams.push(vertexStream);
				_streamVersions.push(vertexStream.version);
			}
			else if (vertexStream is VertexStreamList)
			{
				var vsList			: VertexStreamList		= VertexStreamList(vertexStream);
				var streamsCount	: uint					= vsList._streams.length;

				for (var streamId : uint = 0; streamId < streamsCount; ++streamId)
				{
					_streams.push(vsList._streams[streamId]);
					_streamVersions.push(vsList._streamVersions[streamId]);
				}
			}
			else
				throw new Error('Unknown VertexStream type.');
		}

		public function getSubStreamById(id : int) : VertexStream
		{
			return _streams[id];
		}

		public function getSubStreamByComponent(vertexComponent : VertexComponent) : VertexStream
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

	}
}