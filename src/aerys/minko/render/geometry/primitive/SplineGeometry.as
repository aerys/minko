package aerys.minko.render.geometry.primitive
{
	import aerys.minko.render.geometry.Geometry;
	import aerys.minko.render.geometry.stream.IVertexStream;
	import aerys.minko.render.geometry.stream.IndexStream;
	import aerys.minko.render.geometry.stream.StreamUsage;
	import aerys.minko.render.geometry.stream.VertexStream;
	import aerys.minko.render.geometry.stream.format.VertexFormat;
	import aerys.minko.type.interpolation.IInterpolation;
	import aerys.minko.type.math.Vector4;
	
	public class SplineGeometry extends Geometry
	{
		private static const TMP1	: Vector4	= new Vector4();
		private static const TMP2	: Vector4	= new Vector4();
		
		public function SplineGeometry(path					: IInterpolation,
									   width				: Number			= 0.05,
									   numChunks			: uint				= 80,
									   vertexStreamUsage	: uint				= 3, // StreamUsage.DYNAMIC
									   indexStreamUsage		: uint 				= 3)
		{
			var indexData 		: Vector.<uint>		= new Vector.<uint>;
			var vertexData		: Vector.<Number>	= new Vector.<Number>;
		
			var stepTexCoordU	: Number	= numChunks > 0 ? 1.0 / numChunks : 0.0;
			var texCoordU		: Number	= 0.0;
			
			path.position(0, TMP1);
			vertexData.push(
				/* xyz */ TMP1.x - width,	TMP1.y - width, TMP1.z,	/* uv */ texCoordU, 0.0,
				/* xyz */ TMP1.x,			TMP1.y + width, TMP1.z,	/* uv */ texCoordU, 1.0,
				/* xyz */ TMP1.x + width,	TMP1.y - width, TMP1.z,	/* uv */ texCoordU, 0.0
			);
			
			for (var chunk:uint = 0; chunk < numChunks; ++chunk)
			{
				var t	: Number = (chunk + 1) / numChunks;
				
				path.position(t, TMP2);

				texCoordU	+= stepTexCoordU;
								
				vertexData.push(
					/* xyz */ TMP2.x - width,	TMP2.y - width, TMP2.z,	/* uv */ texCoordU, 0.0,
					/* xyz */ TMP2.x,			TMP2.y + width, TMP2.z, /* uv */ texCoordU, 1.0,
					/* xyz */ TMP2.x + width,	TMP2.y - width, TMP2.z,	/* uv */ texCoordU, 0.0
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
				
				TMP1.copyFrom(TMP2);
			}
			
			var vertexStream : VertexStream = VertexStream.fromVector(vertexStreamUsage, VertexFormat.XYZ_UV, vertexData); 
			super(
				new <IVertexStream>[vertexStream as IVertexStream],
				IndexStream.fromVector(indexStreamUsage, indexData)
			);
		}
	}
}