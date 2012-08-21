package aerys.minko.render.geometry.primitive
{
	import aerys.minko.render.geometry.Geometry;
	import aerys.minko.render.geometry.stream.IVertexStream;
	import aerys.minko.render.geometry.stream.IndexStream;
	import aerys.minko.render.geometry.stream.StreamUsage;
	import aerys.minko.render.geometry.stream.VertexStream;
	import aerys.minko.render.geometry.stream.format.VertexFormat;

	public class QuadGeometry extends Geometry
	{
		private static var _instance			: QuadGeometry	= null;
		private static var _instanceDoubleSided	: QuadGeometry	= null;
		
		public static function get quadGeometry() : QuadGeometry
		{
			return _instance || (_instance = new QuadGeometry());
		}
		
		public static function get doubleSidedQuadGeometry() : QuadGeometry
		{
			return _instanceDoubleSided || (_instanceDoubleSided = new QuadGeometry(true));
		}
		
		public function QuadGeometry(doubleSided		: Boolean 	= false,
									 numColumns 		: uint 		= 1,
									 numRows 			: uint 		= 0,
									 vertexStreamUsage	: uint		= 3,
									 indexStreamUsage	: uint		= 3)
		{
			numRows ||= numColumns;
			
			super(
				new <IVertexStream>[buildVertexStream(numColumns, numRows, vertexStreamUsage)],
				buildIndexStream(doubleSided, numColumns, numRows, indexStreamUsage)
			);
		}
		
		private function buildVertexStream(numColumns			: uint,
										   numRows				: uint,
										   vertexStreamUsage	: uint) : VertexStream
		{
			var vertices 	: Vector.<Number> 	= new <Number>[];
			
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
			
			return VertexStream.fromVector(vertexStreamUsage, VertexFormat.XYZ_UV, vertices);
		}

		private function buildIndexStream(doubleSided		: Boolean,
										  numColumns		: uint,
										  numRows			: uint,
										  indexStreamUsage	: uint) : IndexStream
		{
			var indices	: Vector.<uint>	= new <uint>[];
			
			for (var y : uint = 0; y < numRows; y++)
			{
				for (var x : uint = 0; x < numColumns; x++)
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
			
			return IndexStream.fromVector(indexStreamUsage, indices);
		}
	}
}