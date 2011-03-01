package aerys.minko.scene.mesh
{
	import aerys.minko.ns.minko;
	import aerys.minko.query.RenderingQuery;
	import aerys.minko.scene.AbstractScene3D;
	import aerys.minko.type.stream.IndexStream3D;
	import aerys.minko.type.stream.VertexStream3D;
	import aerys.minko.type.stream.VertexStreamList3D;
	import aerys.minko.type.vertex.format.Vertex3DComponent;
	
	public class Mesh3D extends AbstractScene3D implements IMesh3D
	{
		use namespace minko;
		
		private static var _id : uint = 0;
		
		protected var _vertexStreamList	: VertexStreamList3D	= null;
		protected var _indexStream		: IndexStream3D			= null;
		
		public function get version() : uint
		{
			return _vertexStreamList.version + _indexStream.version;
		}
		
		public function get vertexStreamList() : VertexStreamList3D
		{
			return _vertexStreamList;
		}
		
		public function set vertexStreamList(value : VertexStreamList3D) : void
		{
			_vertexStreamList = value;
		}

		public function get indexStream() : IndexStream3D
		{
			return _indexStream;
		}

		public function Mesh3D(vertexStreamList	: VertexStreamList3D	= null,
							   indexBuffer		: IndexStream3D 		= null)
		{
			if (null == vertexStreamList.getComponentStream(Vertex3DComponent.XYZ))
				throw new Error('VertexStreamList must contain vertex position component (Vertex3DComponent.XYZ)');
			
			_vertexStreamList = vertexStreamList;
			_indexStream = indexBuffer || IndexStream3D.dummy(vertexStreamList.length, vertexStreamList.dynamic);
			
			name = "Mesh_" + ++_id;
		}
		
		override protected function acceptRenderingQuery(query : RenderingQuery) : void
		{
			query.draw(vertexStreamList, indexStream);
		}
		
	}
}