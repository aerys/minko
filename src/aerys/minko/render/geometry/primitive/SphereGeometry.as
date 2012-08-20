package aerys.minko.render.geometry.primitive
{
	import aerys.minko.render.geometry.Geometry;
	import aerys.minko.render.geometry.stream.IVertexStream;
	import aerys.minko.render.geometry.stream.IndexStream;
	import aerys.minko.render.geometry.stream.VertexStream;
	import aerys.minko.render.geometry.stream.format.VertexFormat;
	
	import flash.utils.ByteArray;
	import flash.utils.Endian;

	public class SphereGeometry extends Geometry
	{
		private static var _instance	: SphereGeometry	= null;
		
		public static function get sphereGeometry() : SphereGeometry
		{
			return _instance || (_instance = new SphereGeometry());
		}
		
		public static const DEFAULT_NUM_PARALLELS	: uint 	= 10;
		public static const DEFAULT_NUM_MERIDIANS	: uint 	= 10;
		
		private var _numParallels	: uint;
		private var _numMeridians	: uint;
		
		public function get numParallels() : uint
		{
			return _numParallels;
		}
		
		public function get numMeridians() : uint
		{
			return _numMeridians;
		}

		public function SphereGeometry(numParallels 		: uint 		= DEFAULT_NUM_PARALLELS,
									   numMeridians 		: uint 		= 0,
									   withNormals			: Boolean	= true,
									   vertexStreamUsage	: uint		= 3,
									   indexStreamsUsage	: uint		= 3)
		{
			_numParallels = numParallels;
			_numMeridians = numMeridians || numParallels;

			super(
				new <IVertexStream>[buildVertexStream(withNormals, vertexStreamUsage)],
				buildIndexStream(indexStreamsUsage)
			);
		}
		
		private function buildVertexStream(withNormals 			: Boolean,
										   vertexStreamUsage 	: uint) : IVertexStream
		{
			var numVertices	: int		= (numParallels - 2) * (numMeridians + 1) + 2;
			var data		: ByteArray	= new ByteArray();
			var c			: uint 		= 0;
			var k			: uint		= 0;
			var j			: uint		= 0;
			var i			: uint		= 0;
			
			data.endian = Endian.LITTLE_ENDIAN;
			
			for (j = 1; j < _numParallels - 1; j++)
			{
				for (i = 0; i < _numMeridians + 1; i++, c += 3, k += 2)
				{
					var theta 	: Number	= j / (_numParallels - 1) * Math.PI;
					var phi 	: Number	= i / _numMeridians * Math.PI * 2;
					
					var x : Number 	= Math.sin(theta) * Math.cos(phi) * .5;
					var y : Number	= Math.cos(theta) * .5;
					var z : Number	= -Math.sin(theta) * Math.sin(phi) * .5;
					
					// xyz
					data.writeFloat(x);
					data.writeFloat(y);
					data.writeFloat(z);
					
					// uv
					data.writeFloat(1. - i / _numMeridians);
					data.writeFloat(j / (_numParallels - 1));
					
					if (withNormals)
					{
						data.writeFloat(x * 2);
						data.writeFloat(y * 2);
						data.writeFloat(z * 2);
					}
				}
			}
			
			// top
			data.writeFloat(0.);
			data.writeFloat(.5);
			data.writeFloat(0.);
			data.writeFloat(.5);
			data.writeFloat(0.);
			if (withNormals)
			{
				data.writeFloat(0.);
				data.writeFloat(1.);
				data.writeFloat(0.);
			}
			
			// bottom
			data.writeFloat(0.);
			data.writeFloat(-.5);
			data.writeFloat(0.);
			data.writeFloat(.5);
			data.writeFloat(1.);
			if (withNormals)
			{
				data.writeFloat(0.);
				data.writeFloat(-1.);
				data.writeFloat(0.);
			}
			
			data.position = 0;
			
			return new VertexStream(
				vertexStreamUsage,
				withNormals ? VertexFormat.XYZ_UV_NORMAL : VertexFormat.XYZ_UV,
				data
			);
		}
		
		private function buildIndexStream(indexStreamUsage : uint) : IndexStream
		{
			var numFaces	: uint			= (_numParallels - 2) * (_numMeridians) * 2;
			var	indices		: Vector.<uint>	= new Vector.<uint>(numFaces * 3, true);
			
			_numMeridians++;
			for (var c : uint = 0, j : uint = 0; j < _numParallels - 3; j++)
			{
				for (var i : uint = 0; i < _numMeridians - 1; i++)
				{
					indices[uint(c++)] = j * _numMeridians + i;
					indices[uint(c++)] = j * _numMeridians + i + 1;
					indices[uint(c++)] = (j + 1) * _numMeridians + i + 1;
					
					indices[uint(c++)] = j * _numMeridians + i;
					indices[uint(c++)] = (j + 1) * _numMeridians + i + 1;
					indices[uint(c++)] = (j + 1) * _numMeridians + i;
				}
			}
			
			for (i = 0; i < numMeridians - 1; i++)
			{
				indices[uint(c++)] = (_numParallels - 2) * _numMeridians;
				indices[uint(c++)] = i + 1;
				indices[uint(c++)] = i;
				
				indices[uint(c++)] = (_numParallels - 2) * _numMeridians + 1;
				indices[uint(c++)] = (_numParallels - 3) * _numMeridians + i;
				indices[uint(c++)] = (_numParallels - 3) * _numMeridians + i + 1;
			}
			
			return new IndexStream(indexStreamUsage, indices);
		}
	}
}