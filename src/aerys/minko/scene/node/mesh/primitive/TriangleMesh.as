package aerys.minko.scene.node.mesh.primitive
{
	import aerys.minko.render.effect.Effect;
	import aerys.minko.scene.node.mesh.Mesh;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.stream.format.VertexFormat;

	public class TriangleMesh extends Mesh
	{
		public function TriangleMesh(effect			: Effect,
									 streamsUsage 	: uint = 0)
		{
			var vertices : Vector.<Number> = new Vector.<Number>();

			vertices.push(
				-0.5, 	0.0,	-0.5, 	0.,	0.,
				0.5, 	0.0, 	-0.5, 	0., 1.,
				0.0, 	0.0, 	0.5, 	1., 0.
			);

			super(
				effect,
				new <IVertexStream>[
					new VertexStream(
						streamsUsage,
						VertexFormat.XYZ_UV,
						vertices
					)
				]
			);
		}

	}

}