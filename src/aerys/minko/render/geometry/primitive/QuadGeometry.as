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
									 indexStreamUsage	: uint		= 3,
									 width 				: Number 	= 1,
									 height 			: Number 	= 1)
		{
			numRows ||= numColumns;
			
			super(
				new <IVertexStream>[buildVertexStream(
					doubleSided, numColumns, numRows, vertexStreamUsage, width, height
				)],
				buildIndexStream(doubleSided, numColumns, numRows, indexStreamUsage)
			);
		}
		
		protected function buildVertexStream(doubleSided			: Boolean,
											 numColumns			: uint,
											 numRows			: uint,
											 vertexStreamUsage	: uint,
											 width 				: Number = 1,
											 height 			: Number = 1) : VertexStream
		{
			var vertices 	: Vector.<Number> 	= new <Number>[];
			
			for (var y : int = 0; y <= numRows; y++)
			{
				for (var x : int = 0; x <= numColumns; x++)
				{
					vertices.push(
						(x / numColumns - 0.5) * width, (y / numRows - 0.5) * height, 0.,
						x / numColumns, 1. - y / numRows
					);
				}
			}
			
			if (doubleSided)
				vertices = vertices.concat(vertices);
			
			return new VertexStream(vertexStreamUsage, VertexFormat.XYZ_UV, vertices);
		}
		
		protected function buildIndexStream(doubleSided			: Boolean,
											numColumns			: uint,
											numRows				: uint,
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
			{
				var numIndices : int = indices.length - 1;
				while (numIndices >= 0)
					indices.push((numRows + 1) * (numColumns + 1) + indices[uint(numIndices--)]);
			}
			
			return new IndexStream(indexStreamUsage, indices);
		}
	}
}