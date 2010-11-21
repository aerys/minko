package aerys.minko.scene.mesh.primitive
{
	import aerys.minko.scene.mesh.Mesh3D;
	import aerys.minko.type.stream.VertexStream3D;
	import aerys.minko.type.vertex.format.Vertex3DFormat;
	
	/**
	 * ...
	 * @author Promethe
	 */
	public class TriangleMesh extends Mesh3D
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
			var vb : Vector.<Number> = Vector.<Number>([-0.5, 0.0, -0.5,
														0.5, 0.0, -0.5,
														0.0, 0.0, 0.5]);
			super(new VertexStream3D(vb, Vertex3DFormat.XYZ));
		}
		
	}

}