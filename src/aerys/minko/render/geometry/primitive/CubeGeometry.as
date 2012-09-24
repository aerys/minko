package aerys.minko.render.geometry.primitive
{
	import aerys.minko.render.geometry.Geometry;
	import aerys.minko.render.geometry.stream.IVertexStream;
	import aerys.minko.render.geometry.stream.IndexStream;
	import aerys.minko.render.geometry.stream.StreamUsage;
	import aerys.minko.render.geometry.stream.VertexStream;
	import aerys.minko.render.geometry.stream.format.VertexFormat;

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
			return _instance || (_instance = new CubeGeometry(StreamUsage.DYNAMIC));
		}
		
		/**
		 * Creates a new CubeMesh object.
		 */
		public function CubeGeometry(vertexStreamUsage 	: uint 	= 3,
									 indexStreamUsage	: uint	= 3)
		{
			var data : Vector.<Number> = new <Number>[
				// top
				0.5, 0.5, 0.5, 1., 0., -0.5, 0.5, -0.5, 0., 1., 0.5, 0.5, -0.5, 1., 1.,
				0.5, 0.5, 0.5, 1., 0., -0.5, 0.5, 0.5, 0., 0., -0.5, 0.5, -0.5, 0., 1.,
				// bottom
				-0.5, -0.5, -0.5, 0., 0., 0.5, -0.5, 0.5, 1., 1., 0.5, -0.5, -0.5, 1., 0.,
				-0.5, -0.5, 0.5, 0., 1., 0.5, -0.5, 0.5, 1., 1., -0.5, -0.5, -0.5, 0., 0.,
				// back
				0.5, -0.5, 0.5, 0., 1., -0.5, 0.5, 0.5, 1., 0., 0.5, 0.5, 0.5, 0., 0.,
				-0.5, 0.5, 0.5, 1., 0., 0.5, -0.5, 0.5, 0., 1., -0.5, -0.5, 0.5, 1., 1.,
				// front
				-0.5, 0.5, -0.5, 0., 0., -0.5, -0.5, -0.5, 0., 1., 0.5, 0.5, -0.5, 1., 0.,
				-0.5, -0.5, -0.5, 0., 1., 0.5, -0.5, -0.5, 1., 1., 0.5, 0.5, -0.5, 1., 0.,
				// left
				-0.5, -0.5, -0.5, 1., 1., -0.5, 0.5, 0.5, 0., 0., -0.5, -0.5, 0.5, 0., 1.,
				-0.5, 0.5, 0.5, 0., 0., -0.5, -0.5, -0.5, 1., 1., -0.5, 0.5, -0.5, 1., 0.,
				// right
				0.5, -0.5, 0.5, 1., 1., 0.5, 0.5, 0.5, 1., 0., 0.5, 0.5, -0.5, 0., 0.,
				0.5, 0.5, -0.5, 0., 0., 0.5, -0.5, -0.5, 0., 1., 0.5, -0.5, 0.5, 1., 1.
			];
			
			var vertexStream : VertexStream = VertexStream.fromVector(
				vertexStreamUsage, VertexFormat.XYZ_UV, data
			);
			
			super(
				new <IVertexStream>[vertexStream],
				new IndexStream(indexStreamUsage, IndexStream.dummyData(vertexStream.numVertices, 0))
			);
		}
	}
}