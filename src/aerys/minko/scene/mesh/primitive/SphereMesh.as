package aerys.minko.scene.mesh.primitive
{
	import aerys.minko.scene.mesh.Mesh3D;
	import aerys.minko.type.stream.IndexStream3D;
	import aerys.minko.type.stream.VertexStream3D;

	public class SphereMesh extends Mesh3D
	{
		private static const DEFAULT_NUM_PARALLELS	: uint 	= 10;
		private static const DEFAULT_NUM_MERIDIANS	: uint 	= 10;
		
		private static var _instance	: SphereMesh	= null;
		
		public static function get sphereMesh() : SphereMesh
		{
			return _instance || (_instance = new SphereMesh());
		}
		
		public function SphereMesh(myParallels : uint = DEFAULT_NUM_PARALLELS,
								   myMeridians : uint = 0)
		{
			myMeridians ||= myParallels;
			
			var numVertices	: int				= (myParallels - 2) * (myMeridians + 1) + 2;
			var numFaces	: int				= (myParallels - 2) * (myMeridians) * 2;
			var	indices		: Vector.<uint>		= new Vector.<uint>(numFaces * 3, true);
			var vertices	: Vector.<Number>	= new Vector.<Number>(numVertices * 3, true);
			var uv			: Vector.<Number>	= new Vector.<Number>(numVertices * 2, true);
			var phi			: Number			= 0.;
			var theta		: Number			= 0.;
			var c			: int 				= 0;
			var k			: int				= 0;
			var j			: int				= 0;
			var i			: int				= 0;
			
			for (j = 1; j < myParallels - 1; j++)
			{
				for (i = 0; i < myMeridians + 1; i++, c += 3, k += 2)
				{
					theta = j / (myParallels - 1) * Math.PI;
					phi = i / myMeridians * Math.PI * 2;
					
					vertices[c] = Math.sin(theta) * Math.cos(phi) * .5;
					vertices[int(c + 1)] = Math.cos(theta) * .5;
					vertices[int(c + 2)] = -Math.sin(theta) * Math.sin(phi) * .5;
					
					uv[k] = 1. - i / myMeridians;
					uv[int(k + 1)] = j / (myParallels - 1);
				}
			}
			
			vertices[c] = 0.; vertices[int(c + 1)] = .5; vertices[int(c + 2)] = 0.;
			uv[k] = .5; uv[int(k + 1)] = 0.;
			
			vertices[int(c + 3)] = 0.; vertices[int(c + 4)]= -.5; vertices[int(c + 5)] = 0.;
			uv[int(k + 2)] = .5; uv[int(k + 3)] = 1.;
			
			myMeridians++;
			for (c = 0,j = 0; j < myParallels - 3; j++)
			{
				for (i = 0; i < myMeridians - 1; i++)
				{
					indices[int(c++)] = j * myMeridians + i;
					indices[int(c++)] = j * myMeridians + i + 1;
					indices[int(c++)] = (j + 1) * myMeridians + i + 1;

					indices[int(c++)] = j * myMeridians + i;
					indices[int(c++)] = (j + 1) * myMeridians + i + 1;
					indices[int(c++)] = (j + 1) * myMeridians + i;
				}
			}
			
			for (i = 0; i < myMeridians - 1; i++)
			{
				indices[int(c++)] = (myParallels - 2) * myMeridians;
				indices[int(c++)] = i + 1;
				indices[int(c++)] = i;

				indices[int(c++)] = (myParallels - 2) * myMeridians + 1;
				indices[int(c++)] = (myParallels - 3) * myMeridians + i;
				indices[int(c++)] = (myParallels - 3) * myMeridians + i + 1;
			}
			
			super(VertexStream3D.fromPositionsAndUVs(vertices, uv), new IndexStream3D(indices));
		}
		
	}
}