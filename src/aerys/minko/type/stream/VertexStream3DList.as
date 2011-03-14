package aerys.minko.type.stream
{
	import aerys.common.IVersionnable;
	import aerys.minko.ns.minko;
	import aerys.minko.type.bounding.BoundingBox3D;
	import aerys.minko.type.bounding.BoundingSphere3D;
	import aerys.minko.type.math.Vector4;
	import aerys.minko.type.vertex.format.Vertex3DComponent;
	import aerys.minko.type.vertex.format.Vertex3DFormat;
	
	public class VertexStream3DList implements IVersionnable
	{
		use namespace minko;
		
		private var _streams		: Vector.<VertexStream3D>	= new Vector.<VertexStream3D>();
		private var _streamVersions	: Vector.<int>				= new Vector.<int>();
		private var _format			: Vertex3DFormat			= new Vertex3DFormat();
		
		private var _version		: int						= 0;
		private var _dynamic		: Boolean					= false;
		
		public function get version()	: uint 		{ return _version; }
		public function get dynamic()	: Boolean 	{ return _dynamic; }
		public function get length()	: int		{ return _streams.length ? _streams[0].length : 0; }
		
		public function VertexStream3DList(...streams)
		{
			initialize(streams);
		}
		
		private function initialize(streams : Array) : void
		{
			for each (var stream : VertexStream3D in streams) 
				pushVertexStream(stream);
		}
		
		public function clone() : VertexStream3DList
		{
			var vertexStreamList:VertexStream3DList = new VertexStream3DList();
			
			for each (var stream:VertexStream3D in _streams)
				vertexStreamList.pushVertexStream(stream);
				
			return vertexStreamList;
		}
		
		public function pushVertexStream(vertexStream : VertexStream3D) : void 
		{
			if (length && vertexStream.length != length)
				throw new Error('All VertexStream3D must have the same length');
			
			_streams.push(vertexStream);
			_streamVersions.push(vertexStream.version);
			
			_format.unionWith(vertexStream.format);
			
			_dynamic ||= vertexStream;
			
			++_version;
		}
		
		public function getComponentStream(vertexComponent : Vertex3DComponent) : VertexStream3D
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
			
			for each (var stream:VertexStream3D in _streams)
				stream.deleteVertexByIndex(index);
			
			return true;
		}
		
	}
}