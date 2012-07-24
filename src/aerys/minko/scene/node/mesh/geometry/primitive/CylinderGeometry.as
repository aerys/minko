package aerys.minko.scene.node.mesh.geometry.primitive
{
	import aerys.minko.render.Effect;
	import aerys.minko.scene.node.mesh.geometry.Geometry;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.IndexStream;
	import aerys.minko.type.stream.StreamUsage;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.stream.format.VertexFormat;

	/**
	 * The CylinderGeometry class represents the 3D geometry of a cylinder.
	 */
	public class CylinderGeometry extends Geometry
	{
		private static const DEFAULT_NUM_COLS	: uint	= 8;
		private static const DEFAULT_NUM_ROWS	: uint	= 2;
		
		private static var _instance	: CylinderGeometry	= null;
		
		public static function get cylinderGeometry() : CylinderGeometry
		{
			return _instance || (_instance = new CylinderGeometry());
		}

		/**
		 * Creates a new CylinderMesh object.
		 */
		public function CylinderGeometry(numCols		: uint		= DEFAULT_NUM_COLS,
										 numRows		: uint		= DEFAULT_NUM_ROWS,
										 generateUVs	: Boolean	= true,
										 streamsUsage	: uint		= StreamUsage.STATIC)
		{
			var xyzUv	: Vector.<Number>	= new <Number>[];
			var indices	: Vector.<uint>		= new <uint>[];
			var ii  	: int 				= 0;

			for (var i : uint = 0; i < numCols; ++i)
			{
				var ix : Number = i / (numCols - 1) * Math.PI * 2.0;

				for (var j : uint = 0; j < numRows; ++j)
				{
					var iy : Number = j / (numRows - 1) - 0.5;

					xyzUv.push(
						0.5 * Math.cos(ix),
						iy,
						0.5 * Math.sin(ix)
					);
					
					if (generateUVs)
						xyzUv.push(i / (numCols - 1), 1. - j / (numRows - 1));
				}
			}

			for (var ik : int = 0 ; ik != numCols - 1; ++ik)
			{
				for (var jk : int = 0; jk != numRows - 1; jk++)
				{
					indices.push(
						ii, ii + numRows + 1, ii + 1,
						ii + numRows, ii + numRows + 1, ii++
					);
				}
				++ii;
			}
			
			var format : VertexFormat = generateUVs ? VertexFormat.XYZ_UV : VertexFormat.XYZ;

			super(
				new <IVertexStream>[new VertexStream(streamsUsage, format, xyzUv)],
				new IndexStream(streamsUsage, indices)
			);
		}

	}
}