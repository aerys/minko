package aerys.minko.scene.mesh.primitive
{
	import aerys.minko.scene.mesh.Mesh3D;
	import aerys.minko.type.stream.VertexStream3D;
	import aerys.minko.type.vertex.format.PackedVertex3DFormat;
	import aerys.minko.type.vertex.format.Vertex3DFormat;
	
	/**
	 * ...
	 * @author Promethe
	 */
	public class TriangleMesh3D extends Mesh3D
	{
		private static var _instance : TriangleMesh3D = null;
		
		public static function get triangleMesh() : TriangleMesh3D
		{
			if (!_instance)
				_instance = new TriangleMesh3D();
			
			return _instance;
		}
		
		
		public function TriangleMesh3D()
		{
			var vb : Vector.<Number> = Vector.<Number>([-0.5, 0.0, -0.5,
														0.5, 0.0, -0.5,
														0.0, 0.0, 0.5]);
			
			super(Vector.<VertexStream3D>([new VertexStream3D(vb, PackedVertex3DFormat.XYZ)]));
		}
		
	}

}