package aerys.minko.scene.node.mesh.primitive
{
	import aerys.minko.scene.node.mesh.Mesh;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.stream.VertexStreamList;
	import aerys.minko.type.vertex.format.VertexFormat;
	
	/**
	 * ...
	 * @author Promethe
	 */
	public class TriangleMesh extends Mesh
	{
		private static var _instance : TriangleMesh = null;
		
		public static function get triangleMesh() : TriangleMesh
		{
			if (!_instance)
				_instance = new TriangleMesh();
			
			return _instance;
		}
		
		
		public function TriangleMesh()
		{
			var vb : Vector.<Number> = Vector.<Number>([-0.5, 0.0, -0.5, 0., 0.,
														0.5, 0.0, -0.5, 0., 1.,
														0.0, 0.0, 0.5, 1., 0.]);
			
			var vertexStreamList : VertexStreamList = new VertexStreamList(
				new VertexStream(vb, VertexFormat.XYZ_UV)
			);
			super(vertexStreamList);
		}
		
	}

}