package aerys.minko.scene.node.mesh.geometry.primitive
{
	import aerys.minko.render.effect.Effect;
	import aerys.minko.scene.node.mesh.geometry.Geometry;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.StreamUsage;
	import aerys.minko.type.stream.VertexStream;

	/**
	 * The CubeGeometry class represents the 3D geometry of a cube.
	 * 
	 * @author Jean-Marc Le Roux
	 */
	public class CubeGeometry extends Geometry
	{
		private static var _instance	: CubeGeometry	= null;
		
		public static function get cubeGeometry() : CubeGeometry
		{
			return _instance || (_instance = new CubeGeometry());
		}
		
		/**
		 * Creates a new CubeMesh object.
		 */
		public function CubeGeometry()
		{
			var xyz 	: Vector.<Number> = new <Number>[
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
			];

			var uv : Vector.<Number>	= new <Number>[
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
			];

			super(
				new <IVertexStream>[VertexStream.fromPositionsAndUVs(xyz, uv, StreamUsage.DYNAMIC)],
				null
			);
		}

	}
}