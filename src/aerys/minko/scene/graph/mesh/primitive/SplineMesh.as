package aerys.minko.scene.graph.mesh.primitive
{
	import aerys.minko.scene.graph.mesh.Mesh;
	import aerys.minko.type.interpolation.IInterpolation;
	import aerys.minko.type.math.Vector4;
	import aerys.minko.type.stream.IndexStream;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.stream.VertexStreamList;
	
	public class SplineMesh extends Mesh
	{
		private static const TMP1 : Vector4 = new Vector4();
		private static const TMP2 : Vector4 = new Vector4();
		
		public function SplineMesh(path		: IInterpolation, 
									 width		: Number	= 0.05,
									 chunks		: uint		= 80)
		{
			var indexData 		: Vector.<uint> = new Vector.<uint>;
			var vertexData		: Vector.<Number> = new Vector.<Number>;
			
			path.position(0, TMP1);
			vertexData.push(
				/* xyz */ TMP1.x - width, TMP1.y - width, TMP1.z,	/* uv */ 0, 0,
				/* xyz */ TMP1.x, TMP1.y + width, TMP1.z,			/* uv */ 1, 0,
				/* xyz */ TMP1.x + width, TMP1.y - width, TMP1.z,	/* uv */ 0, 1
			);
			
			for (var chunk : uint = 0; chunk < chunks; ++chunk)
			{
				var t		: Number = (chunk + 1) / chunks;
				path.position(t, TMP2);
				
				vertexData.push(
					/* xyz */ TMP2.x - width, TMP2.y - width, TMP2.z,	/* uv */ 0, 0,
					/* xyz */ TMP2.x, TMP2.y + width, TMP2.z, 			/* uv */ 1, 0,
					/* xyz */ TMP2.x + width, TMP2.y - width, TMP2.z,	/* uv */ 0, 1
				);
				
				var j : uint = vertexData.length / 5 - 6;
				indexData.push(
					j, j + 1, j + 4,
					j, j + 4, j + 3,
					j + 1, j + 2, j + 4,
					j + 2, j + 5, j + 4,
					j, j + 5, j + 2,
					j, j + 3, j + 5
				);
				
				Vector4.copy(TMP2, TMP1);
			}
			
			var vertexStream 		: VertexStream 		= new VertexStream(vertexData);
			var vertexStreamList 	: VertexStreamList 	= new VertexStreamList(vertexStream);
			var indexStream 		: IndexStream 		= new IndexStream(indexData);
			
			super(vertexStreamList, indexStream);
		}
	}
}