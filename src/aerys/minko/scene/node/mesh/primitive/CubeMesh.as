package aerys.minko.scene.node.mesh.primitive
{
	import aerys.minko.scene.node.mesh.Mesh;
	import aerys.minko.type.stream.VertexStream;

	/**
	 * The CubeMesh class represent a cubic mesh.
	 * @author Jean-Marc Le Roux
	 */
	public class CubeMesh extends Mesh
	{
		private static var _instance	: CubeMesh	= null;
		
		/**
		 * A unit sized CubeMesh object. Use this property instead of "new CubeMesh()" in order
		 * to avoid pointless memory consumption.
		 */
		public static function get cubeMesh() : CubeMesh
		{
			return _instance || (_instance = new CubeMesh());
		}
		/**
		 * Creates a new CubeMesh object.
		 */
		public function CubeMesh()
		{
			var vb 	: Vector.<Number> = new Vector.<Number>();
				
			vb.push(
				// top
				0.5, 0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5,
				0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, 0.5, -0.5,
				// bottom
				-0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5,
				-0.5, -0.5, 0.5, 0.5, -0.5, 0.5, -0.5, -0.5, -0.5,
				// back
				0.5, -0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
				-0.5, 0.5, 0.5, 0.5, -0.5, 0.5, -0.5, -0.5, 0.5,
				// front
				-0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, 0.5, -0.5,
				-0.5, -0.5, -0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5,
				// left
				-0.5, -0.5, -0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5,
				-0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5,
				// right
				0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5,
				0.5, 0.5, -0.5, 0.5, -0.5, -0.5, 0.5, -0.5, 0.5
			);
			
			var uv : Vector.<Number>	= new Vector.<Number>();
			
			uv.push(
				// top
				1., 0., 0., 1., 1., 1.,
				1., 0., 0., 0., 0., 1.,
				// bottom
				0., 0., 1., 1., 1., 0.,
				0., 1., 1., 1., 0., 0.,
				// back
				0., 1., 1., 0., 0., 0.,
				1., 0., 0., 1., 1., 1.,
				// front
				0., 0., 0., 1., 1., 0.,
				0., 1., 1., 1., 1., 0.,														   
				// left
				1., 1., 0., 0., 0., 1.,
				0., 0., 1., 1., 1., 0.,
				// right
				1., 1., 1., 0., 0., 0.,
				0., 0., 0., 1., 1., 1.
			);
			
			super(VertexStream.fromPositionsAndUVs(vb, uv));
		}
		
	}
}