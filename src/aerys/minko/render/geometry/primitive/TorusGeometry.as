package aerys.minko.render.geometry.primitive 
{
	import aerys.minko.render.geometry.Geometry;
	import aerys.minko.render.geometry.stream.IVertexStream;
	import aerys.minko.render.geometry.stream.IndexStream;
	import aerys.minko.render.geometry.stream.StreamUsage;
	import aerys.minko.render.geometry.stream.VertexStream;
	import aerys.minko.render.geometry.stream.format.VertexComponent;
	import aerys.minko.render.geometry.stream.format.VertexFormat;
	import aerys.minko.type.math.Vector4;
	
	public class TorusGeometry extends Geometry
	{
		private static const EPSILON	: Number	= 0.00001;
		private static const DTOR 		: Number	= 0.01745329252;
		
		private var _radius		: Number;
		private var _tube		: Number;
		private var _segmentsR	: uint;
		private var _segmentsT	: uint;
		private var _arc		: Number;
		
		public function TorusGeometry(radius				: Number	= 100.,
									  tube					: Number	= 40.,
									  segmentsR				: uint		= 8,
									  segmentsT				: uint		= 6,
									  arc					: Number	= Math.PI * 2.,
									  withUVs				: Boolean	= true,
									  withNormals			: Boolean	= true,
									  vertexStreamUsage		: uint		= StreamUsage.STATIC,
									  indexStreamUsage		: uint		= StreamUsage.STATIC)
		{
			_radius = radius;
			_tube = tube;
			_segmentsR = segmentsR;
			_segmentsT = segmentsT;
			_arc = arc;
			
			super(
				new <IVertexStream>[buildVertexStream(vertexStreamUsage, withUVs, withNormals)],
				buildIndexStream(indexStreamUsage)
			);
		}
		
		private function buildVertexStream(usage : uint, withUVs : Boolean, withNormals : Boolean) : VertexStream
		{
			var vertexData	: Vector.<Number>	= new <Number>[];
			
			for (var j : uint = 0; j <= _segmentsR; ++j)
			{
				for (var i : uint = 0; i <= _segmentsT; ++i)
				{
					var u 		: Number 	= i / _segmentsT * _arc;
					var v 		: Number 	= j / _segmentsR * _arc;
					var cosU	: Number	= Math.cos(u);
					var sinU	: Number	= Math.sin(u);
					var cosV	: Number	= Math.cos(v);
					var x		: Number	= (_radius + _tube * cosV) * cosU;
					var y		: Number	= (_radius + _tube * cosV) * sinU;
					var z		: Number	= _tube * Math.sin(v);
					
					vertexData.push(x, y, z);
					
					if (withUVs)
						vertexData.push(i / _segmentsT, 1 - j / _segmentsR);
					
					if (withNormals)
					{
						var normalX	: Number	= x - _radius * cosU;
						var normalY	: Number	= y - _radius * sinU;
						var normalZ	: Number	= z;
						var mag		: Number	= normalX * normalX + normalY * normalY
							+ normalZ * normalZ;
						
						normalX /= mag;
						normalY /= mag;
						normalZ /= mag;
						
						vertexData.push(normalX, normalY, normalZ);
					}
				}
			}
			
			var format : VertexFormat = new VertexFormat(VertexComponent.XYZ);
			
			if (withUVs)
				format.addComponent(VertexComponent.UV);
			
			if (withNormals)
				format.addComponent(VertexComponent.NORMAL);
			
			return new VertexStream(usage, format, vertexData);
		}
		
		private function buildIndexStream(usage : uint) : IndexStream
		{
			var indices : Vector.<uint> = new <uint>[];
			
			for (var j : uint = 1; j <= _segmentsR; ++j)
			{
				for (var i : uint = 1; i <= _segmentsT; ++i)
				{
					var a : uint = (_segmentsT + 1) * j + i - 1;
					var b : uint = (_segmentsT + 1) * (j - 1) + i - 1;
					var c : uint = (_segmentsT + 1) * (j - 1) + i;
					var d : uint = (_segmentsT + 1) * j + i;
					
					indices.push(a, c, b, a, d, c);
				}
			}
			
			return new IndexStream(usage, indices);
		}
	}
}