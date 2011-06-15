package aerys.minko.scene.node.mesh.modifier
{
	import aerys.minko.ns.minko;
	import aerys.minko.ns.minko_stream;
	import aerys.minko.scene.node.mesh.IMesh;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.vertex.format.NativeFormat;
	import aerys.minko.type.vertex.format.VertexComponent;
	import aerys.minko.type.vertex.format.VertexFormat;
	
	import flash.geom.Vector3D;
	
	public class TangentSpaceMeshModifier extends AbstractMeshModifier
	{
		use namespace minko_stream;
		
		private static const VERTEX_FORMAT		: VertexFormat		= new VertexFormat(VertexComponent.NORMAL,
																					   VertexComponent.TANGENT,
																					   VertexComponent.BITANGENT);
		
		public function TangentSpaceMeshModifier(target : IMesh)
		{
			super(target);
			
			_vertexStreamList = target.vertexStreamList.clone();
			initialize();
		}
		private function initialize() : void
		{
			// (x, y, z) positions
			var xyzStream		: VertexStream		= vertexStreamList.getVertexStreamByComponent(VertexComponent.XYZ);
			var xyzOffset		: int 				= xyzStream.format.getOffsetForComponent(VertexComponent.XYZ);
			var xyzSize			: int 				= xyzStream.format.dwordsPerVertex;
			var xyz				: Vector.<Number>	= xyzStream._data;
			
			// (u, v) texture coordinates
			var uvStream		: VertexStream		= vertexStreamList.getVertexStreamByComponent(VertexComponent.UV);
			var uvOffset		: int 				= uvStream.format.getOffsetForComponent(VertexComponent.UV);
			var uvSize			: int 				= uvStream.format.dwordsPerVertex;
			var uv				: Vector.<Number>	= uvStream._data;
			
			var numVertices		: int				= xyz.length / xyzSize;
			var indices			: Vector.<uint>		= indexStream._indices;
			var numTriangles	: int				= indexStream.length / 3;
			
			var data			: Vector.<Number>	= new Vector.<Number>(9 * numVertices);
			
			// normal
			var nx				: Number 			= 0.;
			var ny				: Number 			= 0.;
			var nz				: Number 			= 0.;
			
			// tangent
			var tx				: Number 			= 0.;
			var ty				: Number 			= 0.;
			var tz				: Number 			= 0.;
			
			// binormal
			var bx				: Number 			= 0.;
			var by				: Number 			= 0.;
			var bz				: Number 			= 0.;
			
			var i				: int				= 0;
			var ii 				: int 				= 0;
			
			for (i = 0; i < numTriangles; ++i)
			{
				ii = i * 3;
				
				var i0	: int 		= indices[ii];
				var i1	: int 		= indices[int(ii + 1)];
				var i2	: int 		= indices[int(ii + 2)];
				
				var ii0	: int 		= xyzOffset + xyzSize * i0;
				var ii1	: int		= xyzOffset + xyzSize * i1;
				var ii2	: int 		= xyzOffset + xyzSize * i2;
				
				var x0	: Number 	= xyz[ii0];
				var y0	: Number 	= xyz[int(ii0 + 1)];
				var z0	: Number 	= xyz[int(ii0 + 2)];
				var u0	: Number	= uv[int(uvOffset + uvSize * i0)];
				var v0	: Number	= uv[int(uvOffset + uvSize * i0 + 1)];
				
				var x1	: Number 	= xyz[ii1];
				var y1	: Number 	= xyz[int(ii1 + 1)];
				var z1	: Number 	= xyz[int(ii1 + 2)];
				var u1	: Number	= uv[int(uvOffset + uvSize * i1)];
				var v1	: Number	= uv[int(uvOffset + uvSize * i1 + 1)];
				
				var x2	: Number 	= xyz[ii2];
				var y2	: Number 	= xyz[int(ii2 + 1)];
				var z2	: Number 	= xyz[int(ii2 + 2)];
				var u2	: Number	= uv[int(uvOffset + uvSize * i2)];
				var v2	: Number	= uv[int(uvOffset + uvSize * i2 + 1)];
				
				nx = (y0 - y2) * (z0 - z1) - (z0 - z2) * (y0 - y1);
				ny = (z0 - z2) * (x0 - x1) - (x0 - x2) * (z0 - z1);
				nz = (x0 - x2) * (y0 - y1) - (y0 - y2) * (x0 - x1);
				
				var c2c1_T : Number = u0 - u2;
				var c2c1_B : Number = v0 - v2;
				var c3c1_T : Number = u1 - u2;
				var c3c1_B : Number = v1 - v2;
				var coef : Number = c2c1_T * c3c1_B - c3c1_T * c2c1_B;
				
				if (coef == 0.)
					coef = 1.;
				else
					coef = 1. / coef;
				
				tx = coef * (c3c1_B * (x0 - x2) - c2c1_B * (x1 - x2));
				ty = coef * (c3c1_B * (y0 - y2) - c2c1_B * (y1 - y2));
				tz = coef * (c3c1_B * (z0 - z2) - c2c1_B * (z1 - z2));
				
				bx = ny * tz - nz * ty;
				by = nz * tx - nx * tz;
				bz = nx * ty - ny * tx;
				
				ii = i0 * 9;
				data[ii] += nx;
				data[int(ii + 1)] += ny;
				data[int(ii + 2)] += nz;
				data[int(ii + 3)] += tx;
				data[int(ii + 4)] += ty;
				data[int(ii + 5)] += tz;
				data[int(ii + 6)] += bx;
				data[int(ii + 7)] += by;
				data[int(ii + 8)] += bz;

				ii = i1 * 9;
				data[ii] += nx;
				data[int(ii + 1)] += ny;
				data[int(ii + 2)] += nz;
				data[int(ii + 3)] += tx;
				data[int(ii + 4)] += ty;
				data[int(ii + 5)] += tz;
				data[int(ii + 6)] += bx;
				data[int(ii + 7)] += by;
				data[int(ii + 8)] += bz;
				
				ii = i2 * 9;
				data[ii] += nx;
				data[int(ii + 1)] += ny;
				data[int(ii + 2)] += nz;
				data[int(ii + 3)] += tx;
				data[int(ii + 4)] += ty;
				data[int(ii + 5)] += tz;
				data[int(ii + 6)] += bx;
				data[int(ii + 7)] += by;
				data[int(ii + 8)] += bz;
			}
			
			for (i = 0; i < numVertices; ++i)
			{
				ii = i * 9;
				
				nx = data[ii];
				ny = data[int(ii + 1)];
				nz = data[int(ii + 2)];
				
				tx = data[int(ii + 3)];
				ty = data[int(ii + 4)];
				tz = data[int(ii + 5)];
				
				bx = data[int(ii + 6)];
				by = data[int(ii + 7)];
				bz = data[int(ii + 8)];
				
				var normalMag		: Number = Math.sqrt(nx * nx + ny * ny + nz * nz);
				var tangentMag		: Number = Math.sqrt(tx * tx + ty * ty + tz * tz);
				var bitangentMag	: Number = Math.sqrt(bx * bx + by * by + bz * bz);
				
				if (normalMag != 0.)
				{
					data[ii] /= normalMag;
					data[int(ii + 1)] /= normalMag;
					data[int(ii + 2)] /= normalMag;
				}
				
				if (tangentMag != 0.)
				{
					data[int(ii + 3)] /= tangentMag;
					data[int(ii + 4)] /= tangentMag;
					data[int(ii + 5)] /= tangentMag;
				}
				
				if (bitangentMag != 0.)
				{
					data[int(ii + 6)] /= bitangentMag;
					data[int(ii + 7)] /= bitangentMag;
					data[int(ii + 8)] /= bitangentMag;
				}
			}
			
			var stream : VertexStream = new VertexStream(data,
														 VERTEX_FORMAT,
														 xyzStream.dynamic);
			
			_vertexStreamList.pushVertexStream(stream);
		}
	}
}