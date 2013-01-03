package aerys.minko.scene.controller.light
{
	import aerys.minko.scene.data.LightDataProvider;
	import aerys.minko.scene.node.Mesh;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.scene.node.light.AbstractLight;
	import aerys.minko.scene.node.light.DirectionalLight;
	import aerys.minko.type.enum.ShadowMappingType;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	
	/**
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public final class DirectionalLightController extends LightShadowController
	{
		private static const SCREEN_TO_UV	: Matrix4x4	= new Matrix4x4(
			.5,		.0,		.0,		.0,
			.0, 	-.5,	.0,		.0,
			.0,		.0,		1.,		.0,
			.5, 	.5,		.0,		1.
		);
		
		private var _worldPosition		: Vector4;
		private var _worldDirection		: Vector4;
		private var _worldToScreen		: Matrix4x4;
		private var _worldToUV			: Matrix4x4;
		private var _projection			: Matrix4x4;
		
		public function DirectionalLightController()
		{
			super(DirectionalLight, ShadowMappingType.MATRIX);
			
			initialize();
		}
		
		private function initialize() : void
		{
			_worldPosition = new Vector4();
			_worldDirection = new Vector4();
			_worldToScreen = new Matrix4x4();
			_worldToUV = new Matrix4x4();
			_projection = new Matrix4x4();
		}

		override protected function targetAddedHandler(ctrl		: LightController,
													  target	: AbstractLight) : void
		{
			super.targetAddedHandler(ctrl, target);
			
			lightData.setLightProperty('worldPosition', _worldPosition);
			lightData.setLightProperty('worldDirection', _worldDirection);
			lightData.setLightProperty('worldToScreen', _worldToScreen);
			lightData.setLightProperty('worldToUV', _worldToUV);
			lightData.setLightProperty('projection', _projection);
		}
		
		override protected function lightAddedToScene(scene : Scene) : void
		{
			super.lightAddedToScene(scene);
			
			updateProjectionMatrix();
			lightLocalToWorldChangedHandler(light, light.getLocalToWorldTransform());
			light.localToWorldTransformChanged.add(lightLocalToWorldChangedHandler);
		}
		
		override protected function lightRemovedFromScene(scene : Scene) : void
		{
			super.lightRemovedFromScene(scene);
			
			light.localToWorldTransformChanged.remove(lightLocalToWorldChangedHandler);
		}
		
		override protected function lightDataChangedHandler(lightData		: LightDataProvider,
															propertyName	: String) : void
		{
			super.lightDataChangedHandler(lightData, propertyName);
			
			propertyName = LightDataProvider.getPropertyName(propertyName);
			
			if (propertyName == 'shadowWidth' || propertyName == 'shadowZFar')
				updateProjectionMatrix();
		}
		
		private function lightLocalToWorldChangedHandler(light 			: AbstractLight,
														 localToWorld 	: Matrix4x4) : void
		{
			// compute position
			localToWorld.getTranslation(_worldPosition);
			
			// compute direction
			localToWorld.deltaTransformVector(Vector4.Z_AXIS, _worldDirection);
			_worldDirection.normalize();
			
			// update world to screen/uv
			_worldToScreen.lock().copyFrom(localToWorld).invert().append(_projection).unlock();
			_worldToUV.lock().copyFrom(_worldToScreen).append(SCREEN_TO_UV).unlock();
		}
		
		private function updateProjectionMatrix() : void
		{
			var zFar 	: Number 	= lightData.getLightProperty('shadowZFar');
			var width	: Number	= lightData.getLightProperty('shadowWidth');
			
			_projection.initialize(
				2. / width, 	0., 			0.,			0.,
				0., 			2. / width, 	0.,			0.,
				0., 			0., 			2. / zFar, 	0.,
				0., 			0., 			0.,			1.
			);
			
			_worldToScreen.lock()
				.copyFrom(light.getWorldToLocalTransform())
				.append(_projection)
				.unlock();
			
			_worldToUV.lock()
				.copyFrom(_worldToScreen)
				.append(SCREEN_TO_UV)
				.unlock();
		}
	}
}