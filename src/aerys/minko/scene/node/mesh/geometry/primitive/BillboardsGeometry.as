package aerys.minko.scene.node.mesh.geometry.primitive
{
	import aerys.minko.render.effect.Effect;
	import aerys.minko.scene.node.mesh.geometry.Geometry;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.IndexStream;
	import aerys.minko.type.stream.StreamUsage;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.stream.format.VertexComponent;
	import aerys.minko.type.stream.format.VertexFormat;
	
	public class BillboardsGeometry extends Geometry
	{
		public static const PARTICLE_FORMAT	: VertexFormat	= new VertexFormat(
			VertexComponent.XY,
			VertexComponent.ID
		);
		
		public function BillboardsGeometry(numParticles : uint)
		{
			super();
			
			initialize(numParticles);
		}
		
		private function initialize(numParticles : uint) : void
		{
			var vertices	: Vector.<Number>	= new <Number>[];
			var indices		: Vector.<uint>		= new <uint>[];
			
			for (var particleId : int = 0; particleId < numParticles; ++particleId)
			{
				vertices.push(
					-0.5, 0.5, particleId,
					0.5, 0.5, particleId,
					0.5, -0.5, particleId,
					-0.5, -0.5, particleId
				);
				
				indices.push(
					particleId * 4, particleId * 4 + 2, particleId * 4 + 1,
					particleId * 4, particleId * 4 + 3, particleId * 4 + 2
				);
			}
			
			setVertexStream(
				new VertexStream(
					StreamUsage.STATIC,
					PARTICLE_FORMAT,
					vertices
				),
				0
			);
			
			indexStream = new IndexStream(StreamUsage.STATIC, indices);
		}
	}
}