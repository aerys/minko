package aerys.minko.scene.node.mesh.geometry.primitive
{
	import aerys.minko.scene.node.mesh.geometry.Geometry;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.IndexStream;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.stream.format.VertexFormat;

	public class QuadGeometry extends Geometry
	{
		private static var _instance	: QuadGeometry	= null;
		
		public static function get quadGeometry() : QuadGeometry
		{
			return _instance || (_instance = new QuadGeometry());
		}
		
		public function QuadGeometry(doubleSided	: Boolean 	= false,
									 numColumns 	: uint 		= 1,
									 numRows 		: uint 		= 0,
									 streamsUsage	: uint		= 0)
		{
			var vertices 	: Vector.<Number> 	= new Vector.<Number>();
			var indices 	: Vector.<uint> 	= new Vector.<uint>();

			numRows ||= numColumns;

			for (var y : int = 0; y <= numRows; y++)
			{
				for (var x : int = 0; x <= numColumns; x++)
				{
					vertices.push(
						x / numColumns - .5, y / numRows - .5, 0.,
						x / numColumns, 1. - y / numRows
					);
				}
			}

			for (y = 0; y < numRows; y++)
			{
				for (x = 0; x < numColumns; x++)
				{
					indices.push(
						x + (numColumns + 1) * y,
						x + 1 + y * (numColumns + 1),
						(y + 1) * (numColumns + 1) + x,
						x + 1 + y * (numColumns + 1),
						(y + 1) * (numColumns + 1) + x + 1,
						(y + 1) * (numColumns + 1) + x
					);
				}
			}

			if (doubleSided)
				indices = indices.concat(indices.concat().reverse());
			
			var vstream : VertexStream = new VertexStream(
				streamsUsage,
				VertexFormat.XYZ_UV,
				vertices
			);
			
			super(
				new <IVertexStream>[vstream],
				new IndexStream(streamsUsage, indices)
			);
		}

	}
}