package aerys.minko.scene.mesh.modifier
{
	import aerys.minko.ns.minko;
	import aerys.minko.query.IScene3DQuery;
	import aerys.minko.query.RenderingQuery;
	import aerys.minko.scene.mesh.IMesh3D;
	import aerys.minko.type.stream.VertexStream3D;
	import aerys.minko.type.stream.VertexStream3DList;
	import aerys.minko.type.vertex.format.NativeFormat;
	import aerys.minko.type.vertex.format.PackedVertex3DFormat;
	import aerys.minko.type.vertex.format.Vertex3DComponent;
	
	public class NormalMesh3D extends AbstractMeshModifier3D
	{
		use namespace minko;
		
		public static const VERTEX_FORMAT	: PackedVertex3DFormat	=
			new PackedVertex3DFormat(new Vertex3DComponent(['nx', 'ny', 'nz'], NativeFormat.FLOAT_3));
		
		protected var _version	: uint	= 0;
		protected var _vertexStreamList:VertexStream3DList;
		
		override public function get version() : uint { return _version; }
		override public function get vertexStreamList():VertexStream3DList { return _vertexStreamList; }
		
		public function NormalMesh3D(target : IMesh3D)
		{
			super(target);
			
			_vertexStreamList = target.vertexStreamList.clone();
			_vertexStreamList.pushVertexStream(computeNormals());
		}
		
		private function computeNormals() : VertexStream3D
		{
			var normalsFormat	: PackedVertex3DFormat	= VERTEX_FORMAT;
			
			var vertexStream	: VertexStream3D		= vertexStreamList.getComponentStream(Vertex3DComponent.XYZ);
			var vertexOffset	: int 					= vertexStream.format.getOffsetForComponent(Vertex3DComponent.XYZ);
			var vertexLength	: int 					= vertexStream.format.dwordsPerVertex;
			
			var vertices		: Vector.<Number>		= vertexStream._data;
			var numVertices		: int					= vertices.length / vertexLength;
			var indices			: Vector.<uint>			= indexStream._indices;
			var numTriangles	: int					= indexStream.length / 3;
			
			// on stocke les normales de tous les triangles ici
			var normals			: Vector.<Number>		= new Vector.<Number>(3 * numVertices);
			
			var i				: int					= 0;
			
			for (i = 0; i < numTriangles; ++i) {

				var i0:int = indices[int(3 * i)];
				var i1:int = indices[int(3 * i + 1)];
				var i2:int = indices[int(3 * i + 2)];
				
				var ii0:int = vertexOffset + vertexLength * i0;
				var ii1:int = vertexOffset + vertexLength * i1;
				var ii2:int = vertexOffset + vertexLength * i2;
				
				var x0:Number = vertices[ii0];
				var y0:Number = vertices[int(ii0 + 1)];
				var z0:Number = vertices[int(ii0 + 2)];
				
				var x1:Number = vertices[ii1];
				var y1:Number = vertices[int(ii1 + 1)];
				var z1:Number = vertices[int(ii1 + 2)];
				
				var x2:Number = vertices[ii2];
				var y2:Number = vertices[int(ii2 + 1)];
				var z2:Number = vertices[int(ii2 + 2)];
				
				var nx:Number = (y0 - y2) * (z0 - z1) - (z0 - z2) * (y0 - y1);
				var ny:Number = (z0 - z2) * (x0 - x1) - (x0 - x2) * (z0 - z1);
				var nz:Number = (x0 - x2) * (y0 - y1) - (y0 - y2) * (x0 - x1);
				
				normals[int(i0 * 3)] += nx;
				normals[int(i0 * 3 + 1)] += ny;
				normals[int(i0 * 3 + 2)] += nz;
				
				normals[int(i1 * 3)] += nx;
				normals[int(i1 * 3 + 1)] += ny;
				normals[int(i1 * 3 + 2)] += nz;
				
				normals[int(i2 * 3)] += nx;
				normals[int(i2 * 3 + 1)] += ny;
				normals[int(i2 * 3 + 2)] += nz;
			}
			
			
			for (i = 0; i < numVertices; ++i)
			{
				var x:Number = normals[int(i * 3)];
				var y:Number = normals[int(i * 3 + 1)];
				var z:Number = normals[int(i * 3 + 2)];
				
				var mag:Number = Math.sqrt(x * x + y * y + z * z);
				
				if (mag != 0.) 
				{
					normals[int(i * 3)] /= mag;
					normals[int(i * 3 + 1)] /= mag;
					normals[int(i * 3 + 2)] /= mag;
				}
			}
			
			return new VertexStream3D(normals, normalsFormat, vertexStreamList.dynamic);
		}
		
		override public function accept(query : IScene3DQuery) : void
		{
			var q : RenderingQuery = query as RenderingQuery;
			
			q && q.draw(vertexStreamList, indexStream);
//			if (q && _version != target.version)
//			{
//				_version = target.version;
//			}
		}
	}
}