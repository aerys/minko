package aerys.minko.scene.mesh.primitive
{
	import aerys.minko.scene.mesh.Mesh3D;
	import aerys.minko.type.stream.IndexStream3D;
	import aerys.minko.type.stream.VertexStream3D;
	import aerys.minko.type.stream.VertexStream3DList;

	public class SphereMesh3D extends Mesh3D
	{
		private static const DEFAULT_NUM_PARALLELS	: uint 	= 10;
		private static const DEFAULT_NUM_MERIDIANS	: uint 	= 10;
		
		private static var _instance	: SphereMesh3D	= null;
		
		public static function get sphereMesh() : SphereMesh3D
		{
			return _instance || (_instance = new SphereMesh3D());
		}
		
		public function SphereMesh3D(numParallels : uint = DEFAULT_NUM_PARALLELS,
								   	 numMeridians : uint = 0)
		{
			numMeridians ||= numParallels;
			
			var numVertices	: int				= (numParallels - 2) * (numMeridians + 1) + 2;
			var numFaces	: int				= (numParallels - 2) * (numMeridians) * 2;
			var	indices		: Vector.<uint>		= new Vector.<uint>(numFaces * 3, true);
			var vertices	: Vector.<Number>	= new Vector.<Number>(numVertices * 3, true);
			var uv			: Vector.<Number>	= new Vector.<Number>(numVertices * 2, true);
			var phi			: Number			= 0.;
			var theta		: Number			= 0.;
			var c			: int 				= 0;
			var k			: int				= 0;
			var j			: int				= 0;
			var i			: int				= 0;
			
			for (j = 1; j < numParallels - 1; j++)
			{
				for (i = 0; i < numMeridians + 1; i++, c += 3, k += 2)
				{
					theta = j / (numParallels - 1) * Math.PI;
					phi = i / numMeridians * Math.PI * 2;
					
					vertices[c] = Math.sin(theta) * Math.cos(phi) * .5;
					vertices[int(c + 1)] = Math.cos(theta) * .5;
					vertices[int(c + 2)] = -Math.sin(theta) * Math.sin(phi) * .5;
					
					uv[k] = 1. - i / numMeridians;
					uv[int(k + 1)] = j / (numParallels - 1);
				}
			}
			
			vertices[c] = 0.; vertices[int(c + 1)] = .5; vertices[int(c + 2)] = 0.;
			uv[k] = .5; uv[int(k + 1)] = 0.;
			
			vertices[int(c + 3)] = 0.; vertices[int(c + 4)]= -.5; vertices[int(c + 5)] = 0.;
			uv[int(k + 2)] = .5; uv[int(k + 3)] = 1.;
			
			numMeridians++;
			for (c = 0,j = 0; j < numParallels - 3; j++)
			{
				for (i = 0; i < numMeridians - 1; i++)
				{
					indices[int(c++)] = j * numMeridians + i;
					indices[int(c++)] = j * numMeridians + i + 1;
					indices[int(c++)] = (j + 1) * numMeridians + i + 1;

					indices[int(c++)] = j * numMeridians + i;
					indices[int(c++)] = (j + 1) * numMeridians + i + 1;
					indices[int(c++)] = (j + 1) * numMeridians + i;
				}
			}
			
			for (i = 0; i < numMeridians - 1; i++)
			{
				indices[int(c++)] = (numParallels - 2) * numMeridians;
				indices[int(c++)] = i + 1;
				indices[int(c++)] = i;

				indices[int(c++)] = (numParallels - 2) * numMeridians + 1;
				indices[int(c++)] = (numParallels - 3) * numMeridians + i;
				indices[int(c++)] = (numParallels - 3) * numMeridians + i + 1;
			}
			
			
			super(
				new VertexStream3DList(VertexStream3D.fromPositionsAndUVs(vertices, uv)),
				new IndexStream3D(indices)
			);
		}
		
	}
}