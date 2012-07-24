package aerys.minko.scene.node.debug
{
	import aerys.minko.render.effect.Effect;
	import aerys.minko.render.effect.basic.BasicShader;
	import aerys.minko.scene.controller.debug.OrientationAxisController;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.mesh.Mesh;
	import aerys.minko.scene.node.mesh.geometry.Geometry;
	import aerys.minko.scene.node.mesh.geometry.primitive.ConeGeometry;
	import aerys.minko.scene.node.mesh.geometry.primitive.CylinderGeometry;
	import aerys.minko.type.enum.Blending;
	import aerys.minko.type.enum.DepthTest;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.StreamUsage;
	import aerys.minko.type.stream.VertexStream;

	public class OrientationAxis extends Group
	{
		public function OrientationAxis()
		{
			super();
			
			initialize();
		}
		
		private function initialize() : void
		{
			var coneGeometry 		: Geometry 	= new ConeGeometry(10, StreamUsage.DYNAMIC, StreamUsage.DYNAMIC);
			var cylinderGeometry	: Geometry	= new CylinderGeometry(10, 2, false, StreamUsage.DYNAMIC);
			var effect				: Effect	= new Effect(
				new BasicShader(null, Number.NEGATIVE_INFINITY)
			);
			
			coneGeometry.applyTransform(
				new Matrix4x4()
					.appendScale(.1, .5, .1)
					.appendTranslation(0, .75, 0)
			);
			cylinderGeometry.applyTransform(
				new Matrix4x4()
					.appendScale(0.04, 1, 0.04)
			);
			
			var arrowGeometry : Geometry = new Geometry(
				new <IVertexStream>[
					VertexStream.concat(
						new <IVertexStream>[
							cylinderGeometry.getVertexStream(),
							coneGeometry.getVertexStream()
						],
						StreamUsage.STATIC
					)
				],
				cylinderGeometry.indexStream.clone().concat(
					coneGeometry.indexStream,
					0,
					0,
					cylinderGeometry.getVertexStream().length
				)
			);
			
			var xAxis : Mesh = new Mesh(
				arrowGeometry,
				{
					diffuseColor 	: 0x00CC00aa,
					depthTest		: DepthTest.ALWAYS,
					blending		: Blending.ALPHA
				}
			);

			xAxis.transform
				.appendTranslation(0., .5, 0.)
				.appendRotation(-Math.PI / 2, Vector4.Z_AXIS);
			addChild(xAxis);
			
			var yAxis : Mesh = new Mesh(
				arrowGeometry,
				{
					diffuseColor 	: 0xCC0000aa,
					depthTest		: DepthTest.ALWAYS,
					blending		: Blending.ALPHA
				}
			);

			yAxis.transform.appendTranslation(0., .5, 0.);
			addChild(yAxis);
			
			var zAxis : Mesh = new Mesh(
				arrowGeometry,
				{
					diffuseColor 	: 0x0000CCaa,
					depthTest		: DepthTest.ALWAYS,
					blending		: Blending.ALPHA
				}
			);
			
			zAxis.transform
				.appendTranslation(0., .5, 0.)
				.appendRotation(Math.PI / 2, Vector4.X_AXIS);
			addChild(zAxis);
			
			addController(new OrientationAxisController());
		}
	}
}