package aerys.minko.scene.node.mesh.primitive
{
	import aerys.minko.scene.node.mesh.Mesh;
	import aerys.minko.type.stream.IndexStream;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.stream.VertexStreamList;
	
	public class QuadMesh extends Mesh
	{
		static private var _instance	: QuadMesh	= null;
		static private var _instance2	: QuadMesh	= null;
		
		public static function get quadMesh() : QuadMesh
		{
			return _instance || (_instance = new QuadMesh());
		}
		
		public static function get doubleSidedQuadMesh() : QuadMesh
		{
			return _instance2 || (_instance2 = new QuadMesh(true));
		}
		
		public function QuadMesh(doubleSided	: Boolean 	= false,
								 width 			: uint 		= 1,
								 height 		: uint 		= 0)
		{
			var vertices 	: Vector.<Number> 	= new Vector.<Number>();
			var indices 	: Vector.<uint> 	= new Vector.<uint>();
			
			height ||= width;
			
			for (var y : int = 0; y <= height; y++)
				for (var x : int = 0; x <= width; x++)
					vertices.push(x / width - .5, y / height - .5, 0.,
								  x / width, 1. - y / height);
			
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
			
			super(new VertexStream(vertices), new IndexStream(indices));
		}

	}
}