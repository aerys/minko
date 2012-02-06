package aerys.minko.scene.node.mesh.primitive
{
	import aerys.minko.render.effect.Effect;
	import aerys.minko.scene.node.mesh.Mesh;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.VertexStream;

	/**
	 * The CubeMesh class represent a cubic mesh.
	 * @author Jean-Marc Le Roux
	 */
	public class CubeMesh extends Mesh
	{
		/**
		 * Creates a new CubeMesh object.
		 */
		public function CubeMesh(effect : Effect)
		{
			var xyz 	: Vector.<Number> = new <Number>[
				// top
				0.5, 0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5,
				0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, 0.5, -0.5,
				// bottom
				-0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5,
				-0.5, -0.5, 0.5, 0.5, -0.5, 0.5, -0.5, -0.5, -0.5,
				// back
				0.5, -0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5,
				-0.5, 0.5, 0.5, 0.5, -0.5, 0.5, -0.5, -0.5, 0.5,
				// front
				-0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, 0.5, -0.5,
				-0.5, -0.5, -0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5,
				// left
				-0.5, -0.5, -0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5,
				-0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5,
				// right
				0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5,
				0.5, 0.5, -0.5, 0.5, -0.5, -0.5, 0.5, -0.5, 0.5
			];

			var uv : Vector.<Number>	= new <Number>[
				// top
				1., 0., 0., 1., 1., 1.,
				1., 0., 0., 0., 0., 1.,
				// bottom
				0., 0., 1., 1., 1., 0.,
				0., 1., 1., 1., 0., 0.,
				// back
				0., 1., 1., 0., 0., 0.,
				1., 0., 0., 1., 1., 1.,
				// front
				0., 0., 0., 1., 1., 0.,
				0., 1., 1., 1., 1., 0.,
				// left
				1., 1., 0., 0., 0., 1.,
				0., 0., 1., 1., 1., 0.,
				// right
				1., 1., 1., 0., 0., 0.,
				0., 0., 0., 1., 1., 1.
			];

			super(effect, new <IVertexStream>[VertexStream.fromPositionsAndUVs(xyz, uv)]);
		}

	}
}