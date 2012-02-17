package aerys.minko.scene.node.mesh.primitive
{
	import aerys.minko.render.effect.Effect;
	import aerys.minko.scene.node.mesh.Mesh;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.IndexStream;
	import aerys.minko.type.stream.VertexStream;

	/**
	 * The CylinderMesh class represents a cylinder mesh.
	 */
	public class CylinderMesh extends Mesh
	{
		private static const DEFAULT_NUM_COLS	: uint	= 8;
		private static const DEFAULT_NUM_ROWS	: uint	= 2;

		/**
		 * Creates a new CylinderMesh object.
		 * @param	myNumCols
		 * @param	myNumRows
		 */
		public function CylinderMesh(effect			: Effect,
									 numCols		: uint	= DEFAULT_NUM_COLS,
									 numRows		: uint	= DEFAULT_NUM_ROWS,
									 streamsUsage	: uint	= 0)
		{
			var vb	: Vector.<Number>	= new Vector.<Number>();
			var ib	: Vector.<uint>		= new Vector.<uint>();
			var uv	: Vector.<Number>	= new Vector.<Number>();
			var ii  : int 				= 0;

			for (var i : uint = 0; i < numCols; ++i)
			{
				var ix : Number = i / (numCols - 1) * Math.PI * 2.0;

				for (var j : uint = 0; j < numRows; ++j)
				{
					var iy : Number = j / (numRows - 1) - 0.5;

					vb.push(0.5 * Math.cos(ix), iy, 0.5 * Math.sin(ix));
					uv.push(i / (numCols - 1), 1. - j / (numRows - 1));
				}
			}

			for (var ik : int = 0 ; ik != numCols - 1; ++ik)
			{
				for (var jk : int = 0; jk != numRows - 1; jk++)
				{
					ib.push(ii, ii + numRows + 1, ii + 1,
							ii + numRows, ii + numRows + 1, ii++);
				}
				++ii;
			}

			super(
				effect,
				new <IVertexStream>[
					VertexStream.fromPositionsAndUVs(vb, uv, streamsUsage)
				],
				new IndexStream(streamsUsage, ib)
			);
		}

	}
}