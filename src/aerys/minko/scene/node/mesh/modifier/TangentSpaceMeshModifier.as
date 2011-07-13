package aerys.minko.scene.node.mesh.modifier
{
	import aerys.minko.ns.minko_stream;
	import aerys.minko.scene.node.mesh.IMesh;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.IndexStream;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.vertex.format.VertexComponent;
	import aerys.minko.type.vertex.format.VertexFormat;
	
	public class TangentSpaceMeshModifier extends AbstractMeshModifier
	{
		use namespace minko_stream;
		
		public function TangentSpaceMeshModifier(target : IMesh)
		{
			super(target, computeStream(target));
		}
		
		private function computeStream(target : IMesh) : VertexStream
		{
			var vertexStream	: IVertexStream		= target.vertexStream;
			var indexStream		: IndexStream		= target.indexStream;
			
			var withNormals		: Boolean			= vertexStream.getSubStreamByComponent(VertexComponent.NORMAL) == null;
			var f				: int				= withNormals ? 6 : 3;
			
			// (x, y, z) positions
			var xyzStream		: VertexStream		= vertexStream.getSubStreamByComponent(VertexComponent.XYZ);
			var xyzOffset		: int 				= xyzStream.format.getOffsetForComponent(VertexComponent.XYZ);
			var xyzSize			: int 				= xyzStream.format.dwordsPerVertex;
			var xyz				: Vector.<Number>	= xyzStream._data;
			
			// (u, v) texture coordinates
			var uvStream		: VertexStream		= vertexStream.getSubStreamByComponent(VertexComponent.UV);
			
			if (!uvStream)
				throw new Error("Missing vertex components u, v.");
			
			var uvOffset		: int 				= uvStream.format.getOffsetForComponent(VertexComponent.UV);
			var uvSize			: int 				= uvStream.format.dwordsPerVertex;
			var uv				: Vector.<Number>	= uvStream._data;
			
			var numVertices		: int				= xyz.length / xyzSize;
			var indices			: Vector.<uint>		= indexStream._indices;
			var numTriangles	: int				= indexStream.length / 3;
			
			var data			: Vector.<Number>	= new Vector.<Number>(f * numVertices);
			
			// normal
			var nx				: Number 			= 0.;
			var ny				: Number 			= 0.;
			var nz				: Number 			= 0.;
			
			// tangent
			var tx				: Number 			= 0.;
			var ty				: Number 			= 0.;
			var tz				: Number 			= 0.;
			
			var i				: int				= 0;
			var ii 				: int 				= 0;
			
			for (i = 0; i < numTriangles; ++i)
			{
				ii = i * 3;
				
				var i0		: int 		= indices[ii];
				var i1		: int 		= indices[int(ii + 1)];
				var i2		: int 		= indices[int(ii + 2)];
				
				var ii0		: int 		= xyzOffset + xyzSize * i0;
				var ii1		: int		= xyzOffset + xyzSize * i1;
				var ii2		: int 		= xyzOffset + xyzSize * i2;
				
				var x0		: Number 	= xyz[ii0];
				var y0		: Number 	= xyz[int(ii0 + 1)];
				var z0		: Number 	= xyz[int(ii0 + 2)];
				var u0		: Number	= uv[int(uvOffset + uvSize * i0)];
				var v0		: Number	= uv[int(uvOffset + uvSize * i0 + 1)];
				
				var x1		: Number 	= xyz[ii1];
				var y1		: Number 	= xyz[int(ii1 + 1)];
				var z1		: Number 	= xyz[int(ii1 + 2)];
				var u1		: Number	= uv[int(uvOffset + uvSize * i1)];
				var v1		: Number	= uv[int(uvOffset + uvSize * i1 + 1)];
				
				var x2		: Number 	= xyz[ii2];
				var y2		: Number 	= xyz[int(ii2 + 1)];
				var z2		: Number 	= xyz[int(ii2 + 2)];
				var u2		: Number	= uv[int(uvOffset + uvSize * i2)];
				var v2		: Number	= uv[int(uvOffset + uvSize * i2 + 1)];
			
				var v0v2	: Number 	= v0 - v2;
				var v1v2 	: Number 	= v1 - v2;
				var coef 	: Number 	= (u0 - u2) * v1v2 - (u1 - u2) * v0v2;
				
				if (coef == 0.)
					coef = 1.;
				else
					coef = 1. / coef;
				
				tx = coef * (v1v2 * (x0 - x2) - v0v2 * (x1 - x2));
				ty = coef * (v1v2 * (y0 - y2) - v0v2 * (y1 - y2));
				tz = coef * (v1v2 * (z0 - z2) - v0v2 * (z1 - z2));

				if (withNormals)
				{
					nx = (y0 - y2) * (z0 - z1) - (z0 - z2) * (y0 - y1);
					ny = (z0 - z2) * (x0 - x1) - (x0 - x2) * (z0 - z1);
					nz = (x0 - x2) * (y0 - y1) - (y0 - y2) * (x0 - x1);
				}
				
				ii = i0 * f;
				data[ii] += tx;
				data[int(ii + 1)] += ty;
				data[int(ii + 2)] += tz;
				if (withNormals)
				{
					data[int(ii + 3)] += nx;
					data[int(ii + 4)] += ny;
					data[int(ii + 5)] += nz;
				}

				ii = i1 * f;
				data[ii] += tx;
				data[int(ii + 1)] += ty;
				data[int(ii + 2)] += tz;
				if (withNormals)
				{
					data[int(ii + 3)] += nx;
					data[int(ii + 4)] += ny;
					data[int(ii + 5)] += nz;
				}
				
				ii = i2 * f;
				data[ii] += tx;
				data[int(ii + 1)] += ty;
				data[int(ii + 2)] += tz;
				if (withNormals)
				{
					data[int(ii + 3)] += nx;
					data[int(ii + 4)] += ny;
					data[int(ii + 5)] += nz;
				}
			}
			
			for (i = 0; i < numVertices; ++i)
			{
				ii = i * f;
				
				tx = data[ii];
				ty = data[int(ii + 1)];
				tz = data[int(ii + 2)];
				
				var mag	: Number = Math.sqrt(tx * tx + ty * ty + tz * tz);
				
				if (mag != 0.)
				{
					data[ii] /= mag;
					data[int(ii + 1)] /= mag;
					data[int(ii + 2)] /= mag;
				}
				
				if (withNormals)
				{					
					nx = data[int(ii + 3)];
					ny = data[int(ii + 4)];
					nz = data[int(ii + 5)];
					
					mag = Math.sqrt(nx * nx + ny * ny + nz * nz);
					
					if (mag != 0.)
					{
						data[int(ii + 3)] /= mag;
						data[int(ii + 4)] /= mag;
						data[int(ii + 5)] /= mag;
					}
				}
			}
			
			var format : VertexFormat = new VertexFormat(VertexComponent.TANGENT);
			
			if (withNormals)
				format.addComponent(VertexComponent.NORMAL);
			
			return new VertexStream(data, format, xyzStream.dynamic);
		}
	}
}