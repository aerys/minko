package aerys.minko.scene.mesh.primitive
{
	import aerys.minko.scene.mesh.Mesh3D;
	import aerys.minko.type.stream.IndexStream3D;
	import aerys.minko.type.stream.VertexStream3D;
	
	public class SquareMesh extends Mesh3D
	{
		static private var _instance	: SquareMesh	= null;
		
		public static function get squareMesh() : SquareMesh
		{
			if (!_instance)
				_instance = new SquareMesh();
			
			return _instance;
		}
		
		public function SquareMesh(myDoubleSided	: Boolean 	= false,
								   myWidth 			: uint 		= 1,
								   myHeight 		: uint 		= 0)
		{
			var vertices : Vector.<Number> = new Vector.<Number>();
			var indices : Vector.<uint> = new Vector.<uint>();
			
			myHeight ||= myWidth;
			
			for (var y : int = 0; y <= myHeight; y++)
			{
				for (var x : int = 0; x <= myWidth; x++)
				{
					// x, y, z
					vertices.push(x / myWidth - .5, y / myHeight - .5, 0.);
					// u, v
					vertices.push(1. - x / myWidth, 1. - y / myHeight);
				}
			}
			
			for (y = 0; y < myHeight; y++)
			{
				for (x = 0; x < myWidth; x++)
				{
					indices.push(x + (myWidth + 1) * y);
					indices.push((y + 1) * (myWidth + 1) + x);
					indices.push(x + 1 + y * (myWidth + 1));
					
					indices.push(x + 1 + y * (myWidth + 1));
					indices.push((y + 1) * (myWidth + 1) + x);
					indices.push((y + 1) * (myWidth + 1) + x + 1);
				}
			}
			
			if (myDoubleSided)
				indices = indices.concat(indices.concat().reverse());
			
			super(new VertexStream3D(vertices), new IndexStream3D(indices));
		}

	}
}