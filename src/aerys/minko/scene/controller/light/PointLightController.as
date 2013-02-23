package aerys.minko.scene.controller.light
{
	import aerys.minko.scene.data.LightDataProvider;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.scene.node.light.AbstractLight;
	import aerys.minko.scene.node.light.PointLight;
	import aerys.minko.type.enum.ShadowMappingType;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	
	/**
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public final class PointLightController extends LightShadowController
	{
		private var _worldPosition	: Vector4;
		private var _projection		: Matrix4x4;
		
		public function PointLightController()
		{
			super(
				PointLight, 
				ShadowMappingType.PCF
				| ShadowMappingType.DUAL_PARABOLOID
				| ShadowMappingType.VARIANCE
				| ShadowMappingType.EXPONENTIAL
			);
			
			initialize();
		}

		private function initialize() : void
		{
			_worldPosition = new Vector4();
			_projection = new Matrix4x4();
		}
		
		override protected function targetAddedHandler(ctrl	: LightController,
													  light	: AbstractLight) : void
		{
			super.targetAddedHandler(ctrl, light);
			
			lightData.setLightProperty('worldPosition', _worldPosition);
			lightData.setLightProperty('projection', _projection);
		}
		
		override protected function lightAddedToScene(scene : Scene) : void
		{
			super.lightAddedToScene(scene);

            light.localToWorld(Vector4.ZERO, _worldPosition);
			updateProjectionMatrix();
            
            light.localToWorldTransformChanged.add(lightLocalToWorldTransformChangedHandler);
		}
		
        protected function lightLocalToWorldTransformChangedHandler(light         : AbstractLight,
                                                                    localToWorld  : Matrix4x4) : void
		{
			localToWorld.getTranslation(_worldPosition);
		}
		
		override protected function lightDataChangedHandler(lightData		: LightDataProvider,
															propertyName	: String,
															bindingName		: String,
															value			: Object) : void
		{
			super.lightDataChangedHandler(lightData, propertyName, bindingName, propertyName);
			
			propertyName = LightDataProvider.getPropertyName(propertyName);
			
			if (propertyName == 'shadowZNear' || propertyName == 'shadowZFar')
				updateProjectionMatrix();
		}
		
		private function updateProjectionMatrix() : void
		{
			var zNear	: Number	= lightData.getLightProperty('shadowZNear');
			var zFar	: Number	= lightData.getLightProperty('shadowZFar');
			var fd		: Number	= 1. / Math.tan(Math.PI / 4);
			var m33		: Number	= 1. / (zFar - zNear);
			var m43		: Number	= -zNear / (zFar - zNear);
			
			_projection.initialize(
				fd,		0.,		0., 	0.,
				0., 	fd, 	0.,		0.,
				0., 	0., 	m33, 	1.,
				0., 	0., 	m43, 	0.
			);
		}
	}
}