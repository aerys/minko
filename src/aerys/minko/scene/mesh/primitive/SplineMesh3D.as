package aerys.minko.scene.mesh.primitive
{
	import aerys.minko.scene.mesh.Mesh3D;
	import aerys.minko.type.interpolation.AbstractSegment;
	import aerys.minko.type.interpolation.IInterpolation;
	import aerys.minko.type.math.Vector4;
	import aerys.minko.type.stream.IndexStream3D;
	import aerys.minko.type.stream.VertexStream3D;
	import aerys.minko.type.stream.VertexStream3DList;
	import aerys.minko.type.vertex.format.PackedVertex3DFormat;
	
	public class SplineMesh3D extends Mesh3D
	{
		private static const TMP1 : Vector4 = new Vector4();
		private static const TMP2 : Vector4 = new Vector4();
		
		public function SplineMesh3D(path		: IInterpolation, 
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
			
			var vertexStream : VertexStream3D = new VertexStream3D(vertexData, PackedVertex3DFormat.XYZ_UV);
			var vertexStreamList : VertexStream3DList = new VertexStream3DList(vertexStream);
			var indexStream : IndexStream3D = new IndexStream3D(indexData);
			super(vertexStreamList, indexStream);
		}
	}
}