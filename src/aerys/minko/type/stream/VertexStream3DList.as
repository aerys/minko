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
		
		private var _streams		: Vector.<VertexStream3D>;
		private var _streamVersions	: Vector.<int>;
		private var _format			: Vertex3DFormat;
		
		private var _version		: int;
		private var _dynamic		: Boolean;
		
		private var _boundingBox	: BoundingBox3D;
		private var _boundingSphere	: BoundingSphere3D;
		
		public function get version()	: uint { return _version; }
		public function get dynamic()	: Boolean { return _dynamic; }
		public function get length()	: int { return _streams.length ? _streams[0].length : 0; }
		
		public function VertexStream3DList(...streams)
		{
			_streams		= new Vector.<VertexStream3D>;
			_streamVersions	= new Vector.<int>;
			
			_format			= new Vertex3DFormat();
			_dynamic		= false;
			
			_version	= 0;
			
			for each (var stream:VertexStream3D in streams) 
				pushVertexStream(stream);
		}
		
		public function clone() : VertexStream3DList {
			var vertexStreamList:VertexStream3DList = new VertexStream3DList();
			for each (var stream:VertexStream3D in _streams)
				vertexStreamList.pushVertexStream(stream);
			return vertexStreamList;
		}
		
		public function pushVertexStream(vertexStream:VertexStream3D):void 
		{
			if (length && vertexStream.length != length)
				throw new Error('All VertexStream3D must have the same length');
			
			_streams.push(vertexStream);
			_streamVersions.push(vertexStream.version);
			
			_format.unionWith(vertexStream.format);
			
			_dynamic ||= vertexStream;
			
			++_version;
		}
		
		public function getComponentStream(vertexComponent:Vertex3DComponent):VertexStream3D
		{
			var streamLength:int = _streams.length;
			for (var i : int = 0; i < streamLength; ++i)
				if (_streams[i].format.hasComponent(vertexComponent))
					return _streams[i];
			
			return null;
		}
		
		public function deleteVertexByIndex(myIndex : int) : Boolean
		{
			if (myIndex > length)
				return false;
			
			for each (var stream:VertexStream3D in _streams)
				stream.deleteVertexByIndex(myIndex);
			
			return true;
		}
		
		private function updateBoundingVolumes() : void
		{
			var min				: Vector4			= new Vector4(Number.POSITIVE_INFINITY,
																  Number.POSITIVE_INFINITY,
																  Number.POSITIVE_INFINITY);
			
			var max				: Vector4			= new Vector4(Number.NEGATIVE_INFINITY,
																  Number.NEGATIVE_INFINITY,
																  Number.NEGATIVE_INFINITY);

			var xyzStream		: VertexStream3D	= getComponentStream(Vertex3DComponent.XYZ);
			var xyzOffset		: int				= xyzStream.format.getOffsetForComponent(Vertex3DComponent.XYZ);
			
			var dwords			: Vector.<Number>	= xyzStream._data;
			var dwordsPerVertex	: int				= xyzStream.format.dwordsPerVertex;
			var dwordsCount		: int				= dwords.length;
			
			for (var i : int = xyzOffset; i < dwordsCount; i += dwordsPerVertex)
			{
				var x : Number = dwords[i];
				var y : Number = dwords[int(i + 1)];
				var z : Number = dwords[int(i + 2)];
				
				min.x = x < min.x ? x : min.x;
				min.y = y < min.y ? y : min.y;
				min.z = z < min.z ? z : min.z;
				
				max.x = x > max.x ? x : max.x;
				max.y = y > max.y ? y : max.y;
				max.z = z > max.z ? z : max.z;
			}
			
			_boundingSphere = BoundingSphere3D.fromMinMax(min, max);
			_boundingBox = new BoundingBox3D(min, max);
		}
		
	}
}