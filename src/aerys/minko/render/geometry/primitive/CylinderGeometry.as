package aerys.minko.render.geometry.primitive
{
	import aerys.minko.render.Effect;
	import aerys.minko.render.geometry.Geometry;
	import aerys.minko.render.geometry.stream.IVertexStream;
	import aerys.minko.render.geometry.stream.IndexStream;
	import aerys.minko.render.geometry.stream.StreamUsage;
	import aerys.minko.render.geometry.stream.VertexStream;
	import aerys.minko.render.geometry.stream.format.VertexFormat;

	/**
	 * The CylinderGeometry class represents the 3D geometry of a cylinder.
	 */
	public class CylinderGeometry extends Geometry
	{
		private static const DEFAULT_NUM_COLS		: uint	= 8;
		private static const DEFAULT_NUM_ROWS		: uint	= 2;
		
		private static var _instance	: CylinderGeometry	= null;
		
		public static function get cylinderGeometry() : CylinderGeometry
		{
			return _instance || (_instance = new CylinderGeometry());
		}

		/**
		 * Creates a new CylinderMesh object.
		 */
		public function CylinderGeometry(numColumns			: uint		= DEFAULT_NUM_COLS,
										 numRows			: uint		= DEFAULT_NUM_ROWS,
										 generateUVs		: Boolean	= true,
										 vertexStreamUsage	: uint		= 3,
										 indexStreamUsage	: uint		= 3)
		{
			

			super(
				new <IVertexStream>[buildVertexStream(numColumns, numRows, generateUVs, vertexStreamUsage)],
				buildIndexStream(numColumns, numRows, indexStreamUsage)
			);
		}
		
		private function buildVertexStream(numColumns			: uint,
										   numRows				: uint,
										   generateUVs			: Boolean,
										   vertexStreamUsage	: uint) : VertexStream
		{
			var xyzUv	: Vector.<Number>	= new <Number>[];
			
			for (var i : uint = 0; i < numColumns; ++i)
			{
				var ix : Number = i / (numColumns - 1) * Math.PI * 2.0;
				
				for (var j : uint = 0; j < numRows; ++j)
				{
					var iy : Number = j / (numRows - 1) - 0.5;
					
					xyzUv.push(0.5 * Math.cos(ix), iy, 0.5 * Math.sin(ix));
					
					if (generateUVs)
						xyzUv.push(i / (numColumns - 1), 1. - j / (numRows - 1));
				}
			}
			
			return new VertexStream(
				vertexStreamUsage,
				generateUVs ? VertexFormat.XYZ_UV : VertexFormat.XYZ,
				xyzUv
			);
		}

		private function buildIndexStream(numColumns 		: uint,
										  numRows			: uint,
										  indexStreamUsage	: uint) : IndexStream
		{
			var indices	: Vector.<uint>		= new <uint>[];
			var ii  	: uint 				= 0;
			
			for (var ik : uint = 0 ; ik != numColumns - 1; ++ik)
			{
				for (var jk : uint = 0; jk != numRows - 1; jk++)
				{
					indices.push(
						ii, ii + numRows + 1, ii + 1,
						ii + numRows, ii + numRows + 1, ii++
					);
				}
				++ii;
			}
			
			return new IndexStream(indexStreamUsage, indices);
		}
		
	}
}