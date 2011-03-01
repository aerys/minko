package aerys.minko.scene.mesh.primitive
{
	import aerys.minko.scene.mesh.Mesh3D;
	import aerys.minko.type.stream.IndexStream3D;
	import aerys.minko.type.stream.VertexStream3D;
	import aerys.minko.type.stream.VertexStreamList3D;

	/**
	 * The CylinderMesh class represents a cylinder mesh.
	 */
	public class CylinderMesh3D extends Mesh3D
	{
		private static const DEFAULT_NUM_COLS	: uint	= 8;
		private static const DEFAULT_NUM_ROWS	: uint	= 2;
		
		private static var _instance	: CylinderMesh3D	= null;
		
		public static function get cylinderMesh() : CylinderMesh3D
		{
			if (!_instance)
				_instance = new CylinderMesh3D();
			
			return (_instance);
		}
		/**
		 * Creates a new CylinderMesh object.
		 * @param	myNumCols
		 * @param	myNumRows
		 */
		public function CylinderMesh3D(myNumCols	: uint	= DEFAULT_NUM_COLS,
									 myNumRows	: uint	= DEFAULT_NUM_ROWS)
		{
			var vb	: Vector.<Number>	= new Vector.<Number>();
			var ib	: Vector.<uint>		= new Vector.<uint>();
			var uv	: Vector.<Number>	= new Vector.<Number>();
			var ii  : int 				= 0;
			
			for (var i : uint = 0; i < myNumCols; ++i)
			{
				var ix : Number = i / (myNumCols - 1) * Math.PI * 2.0;
				
				for (var j : uint = 0; j < myNumRows; ++j)
				{
					var iy : Number = (j / (myNumRows - 1) - 0.5) * Math.PI;
					
					vb.push(Math.cos(ix), iy, Math.sin(ix));
					uv.push(i / (myNumCols - 1), j / (myNumRows - 1));
				}
			}
			
			for (var ik : int = 0 ; ik != myNumCols - 1; ++ik)
			{
				for (var jk : int = 0; jk != myNumRows - 1; jk++)
				{
					ib.push(ii, ii + myNumRows + 1, ii + 1,
							ii + myNumRows, ii + myNumRows + 1, ii++);
				}
				++ii;
			}
			
			super(new VertexStreamList3D([VertexStream3D.fromPositionsAndUVs(vb, uv)]), new IndexStream3D(ib));
		}
		
	}
}