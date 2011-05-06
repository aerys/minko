package aerys.minko.type.stream
{
	import aerys.common.IVersionnable;
	import aerys.minko.ns.minko;
	import aerys.minko.type.bounding.BoundingBox;
	import aerys.minko.type.bounding.BoundingSphere;
	import aerys.minko.type.math.Vector4;
	import aerys.minko.type.vertex.format.VertexComponent;
	import aerys.minko.type.vertex.format.VertexFormat;
	
	public class VertexStreamList implements IVersionnable
	{
		use namespace minko;
		
		private var _streams		: Vector.<VertexStream>	= new Vector.<VertexStream>();
		private var _streamVersions	: Vector.<int>				= new Vector.<int>();
		private var _format			: VertexFormat			= new VertexFormat();
		
		private var _version		: int						= 0;
		private var _dynamic		: Boolean					= false;
		
		public function get version()	: uint 		{ return _version; }
		public function get dynamic()	: Boolean 	{ return _dynamic; }
		public function get length()	: int		{ return _streams.length ? _streams[0].length : 0; }
		
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
			var vertexStreamList:VertexStreamList = new VertexStreamList();
			
			for each (var stream:VertexStream in _streams)
				vertexStreamList.pushVertexStream(stream);
				
			return vertexStreamList;
		}
		
		public function pushVertexStream(vertexStream : VertexStream) : void 
		{
			if (length && vertexStream.length != length)
				throw new Error('All VertexStream3D must have the same length');
			
			_streams.push(vertexStream);
			_streamVersions.push(vertexStream.version);
			
			_format.unionWith(vertexStream.format);
			
			_dynamic ||= vertexStream;
			
			++_version;
		}
		
		public function getComponentStream(vertexComponent : VertexComponent) : VertexStream
		{
			var streamLength:int = _streams.length;
			for (var i : int = 0; i < streamLength; ++i)
				if (_streams[i].format.hasComponent(vertexComponent))
					return _streams[i];
			
			return null;
		}
		
		public function deleteVertexByIndex(index : int) : Boolean
		{
			if (index > length)
				return false;
			
			for each (var stream:VertexStream in _streams)
				stream.deleteVertexByIndex(index);
			
			return true;
		}
		
	}
}