package aerys.minko.scene.controller.light
{
	import aerys.minko.scene.data.LightDataProvider;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.scene.node.light.AbstractLight;
	import aerys.minko.scene.node.light.DirectionalLight;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	
	/**
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public final class DirectionalLightController extends LightController
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
			super(DirectionalLight);
			
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

		override protected function lightAddedHandler(ctrl		: LightController,
													  target	: AbstractLight) : void
		{
			super.lightAddedHandler(ctrl, target);
			
			lightData.setLightProperty('worldPosition', _worldPosition);
			lightData.setLightProperty('worldDirection', _worldDirection);
			lightData.setLightProperty('worldToScreen', _worldToScreen);
			lightData.setLightProperty('worldToUV', _worldToUV);
			lightData.setLightProperty('projection', _projection);
		}
		
		override protected function lightAddedToSceneHandler(light 	: AbstractLight,
															 scene	: Scene) : void
		{
			super.lightAddedToSceneHandler(light, scene);
			
			updateProjectionMatrix();
			lightLocalToWorldChangedHandler(light.localToWorld);
			light.localToWorld.changed.add(lightLocalToWorldChangedHandler);
		}
		
		override protected function lightRemovedFromSceneHandler(light	: AbstractLight,
																 scene	: Scene) : void
		{
			super.lightRemovedFromSceneHandler(light, scene);
			
			light.localToWorld.changed.remove(lightLocalToWorldChangedHandler);
		}
		
		override protected function lightDataChangedHandler(lightData		: LightDataProvider,
															propertyName	: String) : void
		{
			super.lightDataChangedHandler(lightData, propertyName);
			
			propertyName = LightDataProvider.getPropertyName(propertyName);
			
			if (propertyName == 'shadowWidth' || propertyName == 'shadowZFar')
				updateProjectionMatrix();
		}
		
		private function lightLocalToWorldChangedHandler(localToWorld : Matrix4x4) : void
		{
			// compute position
			localToWorld.getTranslation(_worldPosition);
			
			// compute direction
			_worldDirection	= localToWorld.deltaTransformVector(Vector4.Z_AXIS, _worldDirection);
			_worldDirection.normalize();
			
			// update world to screen/uv
			_worldToScreen.lock().copyFrom(light.worldToLocal).append(_projection).unlock();
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
			
			_worldToScreen.lock().copyFrom(light.worldToLocal).append(_projection).unlock();
			_worldToUV.lock().copyFrom(_worldToScreen).append(SCREEN_TO_UV).unlock();
		}
	}
}