package aerys.minko.scene.mesh.modifier
{
	import aerys.minko.query.IScene3DQuery;
	import aerys.minko.query.RenderingQuery;
	import aerys.minko.scene.mesh.IMesh3D;
	import aerys.minko.type.triangle.Triangle3DIterator;
	import aerys.minko.type.triangle.Triangle3DReference;
	import aerys.minko.type.vertex.Vertex3DReference;
	
	public class NormalMesh extends AbstractMeshModifier3D
	{
		private var _version	: uint	= 0;
		
		override public function get version() : uint
		{
			return _version;
		}
		
		public function NormalMesh(target : IMesh3D)
		{
			super(target);
		}
		
		private function initializeNormals() : void
		{
			var triangles	: Triangle3DIterator	= new Triangle3DIterator(vertexStream, indexStream);
			var numVertices	: int					= vertexStream.length;
			var normals		: Vector.<Number>		= new Vector.<Number>(numVertices * 3.);
			var count		: Vector.<int>			= new Vector.<int>(numVertices);
			
			for each (var triangle : Triangle3DReference in triangles)
			{
				var v0	: Vertex3DReference	= triangle.v0;
				var v1	: Vertex3DReference	= triangle.v1;
				var v2	: Vertex3DReference	= triangle.v2;
				var i0	: int				= triangle.i0;
				var i1	: int				= triangle.i1;
				var i2	: int				= triangle.i2;
				
				normals[int(i0 * 3)] += v0.x;
				normals[int(i0 * 3 + 1)] += v0.y;
				normals[int(i0 * 3 + 2)] += v0.z;
				count[i0]++;
				
				normals[int(i1 * 3)] += v1.x;
				normals[int(i1 * 3 + 1)] += v1.y;
				normals[int(i1 * 3 + 2)] += v1.z;
				count[i1]++;
				
				normals[int(i2 * 3)] += v2.x;
				normals[int(i2 * 3 + 1)] += v2.y;
				normals[int(i2 * 3 + 2)] += v2.z;
				count[i2]++;
			}
			
			for (var i : int = 0; i < numVertices; +i)
			{
				normals[int(i * 3)] /= count[i];
				normals[int(i * 3 + 1)] /= count[i];
				normals[int(i * 3 + 2)] /= count[i];
			}
		}
		
		override public function accept(query : IScene3DQuery) : void
		{
			var q : RenderingQuery = query as RenderingQuery;
			
			if (q && _version != target.version)
			{
				_version = target.version;
			}
		}
	}
}