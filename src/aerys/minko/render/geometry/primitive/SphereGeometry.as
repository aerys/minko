package aerys.minko.render.geometry.primitive
{
	import aerys.minko.render.geometry.Geometry;
	import aerys.minko.render.geometry.stream.IVertexStream;
	import aerys.minko.render.geometry.stream.IndexStream;
	import aerys.minko.render.geometry.stream.StreamUsage;
	import aerys.minko.render.geometry.stream.VertexStream;
	import aerys.minko.render.geometry.stream.VertexStreamList;
	import aerys.minko.render.geometry.stream.format.VertexComponent;
	import aerys.minko.render.geometry.stream.format.VertexFormat;

	public class SphereGeometry extends Geometry
	{
		private static var _instance	: SphereGeometry	= null;
		
		public static function get sphereGeometry() : SphereGeometry
		{
			return _instance || (_instance = new SphereGeometry());
		}
		
		public static const DEFAULT_NUM_PARALLELS	: uint 	= 10;
		public static const DEFAULT_NUM_MERIDIANS	: uint 	= 10;

		public function SphereGeometry(numParallels 		: uint 		= DEFAULT_NUM_PARALLELS,
									   numMeridians 		: uint 		= 0,
									   withNormals			: Boolean	= true,
									   vertexStreamUsage	: uint		= 6,
									   indexStreamsUsage	: uint		= 6)
		{
			numMeridians ||= numParallels;

			super(
				new <IVertexStream>[buildVertexStream(numParallels, numMeridians, withNormals, vertexStreamUsage)],
				buildIndexStream(numParallels, numMeridians, indexStreamsUsage)
			);
		}
		
		private function buildVertexStream(numParallels 		: uint,
										   numMeridians			: uint,
										   withNormals 			: Boolean,
										   vertexStreamUsage 	: uint) : IVertexStream
		{
			var numVertices	: int				= (numParallels - 2) * (numMeridians + 1) + 2;
			var vertices	: Vector.<Number>	= new Vector.<Number>(numVertices * 3, true);
			var normals		: Vector.<Number>	= withNormals ? new Vector.<Number>(numVertices * 3, true) : null;
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
					
					var x : Number 	= Math.sin(theta) * Math.cos(phi) * .5;
					var y : Number	= Math.cos(theta) * .5;
					var z : Number	= -Math.sin(theta) * Math.sin(phi) * .5;
					
					vertices[c] = x;
					vertices[int(c + 1)] = y;
					vertices[int(c + 2)] = z;
					
					uv[k] = 1. - i / numMeridians;
					uv[int(k + 1)] = j / (numParallels - 1);
					
					if (withNormals)
					{
						normals[c] = x * 2;
						normals[int(c + 1)] = y * 2;
						normals[int(c + 2)] = z * 2;
					}
				}
			}
			
			// top
			vertices[c] = 0.;
			vertices[int(c + 1)] = .5;
			vertices[int(c + 2)] = 0.;
			
			uv[k] = .5;
			uv[int(k + 1)] = 0.;
			
			if (withNormals)
			{
				normals[c] = 0.;
				normals[int(c + 1)] = 1.;
				normals[int(c + 2)] = 0.;
			}
			
			// bottom
			vertices[int(c + 3)] = 0.;
			vertices[int(c + 4)]= -.5;
			vertices[int(c + 5)] = 0.;
			
			uv[int(k + 2)] = .5;
			uv[int(k + 3)] = 1.;
			
			if (withNormals)
			{
				normals[int(c + 3)] = 0.;
				normals[int(c + 4)]= -1.;
				normals[int(c + 5)] = 0.;
			}
			
			var xyzUvStream 	: VertexStream 		= VertexStream.fromPositionsAndUVs(vertices, uv, vertexStreamUsage);
			var list			: VertexStreamList	= new VertexStreamList(xyzUvStream);
			
			if (withNormals)
			{
				list.pushVertexStream(
					new VertexStream(
						vertexStreamUsage,
						new VertexFormat(VertexComponent.NORMAL),
						normals
					)
				);
			}
			
			return list;
		}
		
		private function buildIndexStream(numParallels		: uint,
										  numMeridians		: uint,
										  indexStreamUsage	: uint) : IndexStream
		{
			var numFaces	: uint			= (numParallels - 2) * (numMeridians) * 2;
			var	indices		: Vector.<uint>	= new Vector.<uint>(numFaces * 3, true);
			
			numMeridians++;
			for (var c : uint = 0, j : uint = 0; j < numParallels - 3; j++)
			{
				for (var i : uint = 0; i < numMeridians - 1; i++)
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
			
			return new IndexStream(indexStreamUsage, indices);
		}

	}
}