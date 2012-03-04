package aerys.minko.scene.node.mesh.geometry.primitive
{
	import aerys.minko.scene.node.mesh.geometry.Geometry;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.stream.format.VertexFormat;

	public class TriangleGeometry extends Geometry
	{
		private static var _instance	: TriangleGeometry	= null;
		
		public static function get triangleGeometry() : TriangleGeometry
		{
			return _instance || (_instance = new TriangleGeometry());
		}
		
		public function TriangleGeometry(streamsUsage 	: uint = 0)
		{
			var vertices : Vector.<Number> = new Vector.<Number>();

			vertices.push(
				-0.5, 	0.0,	-0.5, 	0.,	0.,
				0.5, 	0.0, 	-0.5, 	0., 1.,
				0.0, 	0.0, 	0.5, 	1., 0.
			);

			super(
				new <IVertexStream>[
					new VertexStream(
						streamsUsage,
						VertexFormat.XYZ_UV,
						vertices
					)
				]
			);
		}

	}

}