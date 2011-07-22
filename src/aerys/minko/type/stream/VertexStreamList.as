package aerys.minko.type.stream
{
	import aerys.minko.ns.minko;
	import aerys.minko.render.ressource.VertexRessource;
	import aerys.minko.type.IVersionnable;
	import aerys.minko.type.vertex.format.VertexComponent;
	import aerys.minko.type.vertex.format.VertexFormat;
	
	public class VertexStreamList implements IVersionnable, IVertexStream
	{
		use namespace minko;
		
		private var _streams		: Vector.<VertexStream>	= new Vector.<VertexStream>();
		private var _streamVersions	: Vector.<int>			= new Vector.<int>();
		private var _format			: VertexFormat			= new VertexFormat();
			
		private var _version		: int					= 0;
		private var _dynamic		: Boolean				= false;
		
		private var _ressource		: VertexRessource		= null;
		
		public function get version()		: uint 				{ return _version; }
		public function get dynamic()		: Boolean 			{ return _dynamic; }
		public function get format()		: VertexFormat		{ return _format; }
		public function get streamCount()	: uint				{ return _streams.length; }
		public function get length()		: uint				{ return _streams.length ? _streams[0].length : 0; }
		public function get ressource()		: VertexRessource	{ return _ressource; }
		
		public function VertexStreamList(...streams)
		{
			initialize(streams);
		}
		
		private function initialize(streams : Array) : void
		{
			_ressource = new VertexRessource(this);
			
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
		
		public function pushVertexStream(vertexStream : VertexStream, force : Boolean = false) : void 
		{
			if (length && vertexStream.length != length)
				throw new Error('All VertexStream must have the same length');
			
			_streams.push(vertexStream);
			_streamVersions.push(vertexStream.version);
			
			_format.unionWith(vertexStream.format, force);
			
			_dynamic ||= vertexStream;
			
			++_version;
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