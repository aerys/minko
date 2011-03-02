package aerys.minko.scene.mesh.primitive
{
	import aerys.minko.scene.mesh.Mesh3D;
	import aerys.minko.type.stream.IndexStream3D;
	import aerys.minko.type.stream.VertexStream3D;
	import aerys.minko.type.stream.VertexStream3DList;
	
	public class QuadMesh3D extends Mesh3D
	{
		static private var _instance	: QuadMesh3D	= null;
		static private var _instance2	: QuadMesh3D	= null;
		
		public static function get quadMesh() : QuadMesh3D
		{
			return _instance || (_instance = new QuadMesh3D());
		}
		
		public static function get doubleSidedQuadMesh() : QuadMesh3D
		{
			return _instance2 || (_instance2 = new QuadMesh3D(true));
		}
		
		public function QuadMesh3D(doubleSided	: Boolean 	= false,
								 width 			: uint 		= 1,
								 height 		: uint 		= 0)
		{
			var vertices : Vector.<Number> = new Vector.<Number>();
			var indices : Vector.<uint> = new Vector.<uint>();
			
			height ||= width;
			
			for (var y : int = 0; y <= height; y++)
			{
				for (var x : int = 0; x <= width; x++)
				{
					// x, y, z
					vertices.push(x / width - .5, y / height - .5, 0.);
					// u, v
					vertices.push(x / width, 1. - y / height);
				}
			}
			
			for (y = 0; y < height; y++)
			{
				for (x = 0; x < width; x++)
				{
					indices.push(x + (width + 1) * y);
					indices.push(x + 1 + y * (width + 1));
					indices.push((y + 1) * (width + 1) + x);
					
					indices.push(x + 1 + y * (width + 1));
					indices.push((y + 1) * (width + 1) + x + 1);
					indices.push((y + 1) * (width + 1) + x);
				}
			}
			
			if (doubleSided)
				indices = indices.concat(indices.concat().reverse());
			
			var vertexStreamList:VertexStream3DList = new VertexStream3DList([new VertexStream3D(vertices)]);
			super(vertexStreamList, new IndexStream3D(indices));
		}

	}
}