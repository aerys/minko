package aerys.minko.scene.node.mesh.geometry.primitive
{
	import aerys.minko.scene.node.mesh.geometry.Geometry;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.IndexStream;
	import aerys.minko.type.stream.StreamUsage;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.stream.format.VertexFormat;
	
	public class ConeGeometry extends Geometry
	{
		public function ConeGeometry(numSegments		: uint	= 10,
									 vertexStreamUsage	: uint	= StreamUsage.STATIC,
									 indexStreamUsage	: uint	= StreamUsage.STATIC)
		{
			super(
				getVertexStreams(numSegments, vertexStreamUsage),
				getIndexStream(numSegments, indexStreamUsage)
			);
		}
		
		private function getVertexStreams(numSegments 		: uint,
										  vertexStreamUsage : uint) : Vector.<IVertexStream>
		{
			var xyz : Vector.<Number>	= new <Number>[0., .5, 0.];
			
			for (var segmentId : uint = 0; segmentId < numSegments; ++segmentId)
			{
				var angle : Number = (Math.PI * 2.) * (segmentId / numSegments);
				
				xyz.push(
					Math.cos(angle),
					-.5,
					Math.sin(angle)
				);
			}
			
			xyz.push(0., -.5, 0.);
			
			return new <IVertexStream>[new VertexStream(vertexStreamUsage, VertexFormat.XYZ, xyz)];
		}
		
		private function getIndexStream(numSegments 		: uint,
										indexStreamUsage	: uint) : IndexStream
		{
			var indices : Vector.<uint> = new <uint>[];
			
			for (var segmentId : uint = 0; segmentId < numSegments - 1; ++segmentId)
			{
				indices.push(
					0, segmentId + 1, segmentId + 2,
					numSegments + 1, segmentId + 2, segmentId + 1
				);
			}
			// close the shape
			indices.push(
				0, segmentId + 1, 1,
				numSegments + 1, 1, segmentId + 1
			);
			
			return new IndexStream(indexStreamUsage, indices);
		}
	}
}