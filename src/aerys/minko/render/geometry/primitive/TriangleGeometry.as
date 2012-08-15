package aerys.minko.render.geometry.primitive
{
	import aerys.minko.render.geometry.Geometry;
	import aerys.minko.render.geometry.stream.IVertexStream;
	import aerys.minko.render.geometry.stream.IndexStream;
	import aerys.minko.render.geometry.stream.VertexStream;
	import aerys.minko.render.geometry.stream.format.VertexFormat;

	public class TriangleGeometry extends Geometry
	{
		private static var _instance	: TriangleGeometry	= null;
		
		public static function get triangleGeometry() : TriangleGeometry
		{
			return _instance || (_instance = new TriangleGeometry());
		}
		
		public function TriangleGeometry(vertexStreamUsage 	: uint 	= 6,
										 indexStreamUsage	: uint	= 6)
		{
			var vertices : Vector.<Number> = new <Number>[
				-0.5, 	0.0,	-0.5, 	0.,	0.,
				0.5, 	0.0, 	-0.5, 	0., 1.,
				0.0, 	0.0, 	0.5, 	1., 0.
			];

			super(
				new <IVertexStream>[
					new VertexStream(vertexStreamUsage, VertexFormat.XYZ_UV, vertices)
				],
				new IndexStream(indexStreamUsage, IndexStream.dummyData(3))
			);
		}

	}

}