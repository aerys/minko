package aerys.minko.scene.node.light
{
	import aerys.minko.ns.minko_lighting;
	import aerys.minko.scene.controller.light.LightController;
	import aerys.minko.scene.data.LightDataProvider;
	import aerys.minko.scene.node.AbstractSceneNode;
	
	use namespace minko_lighting;
	
	public class AbstractLight extends AbstractSceneNode
	{
		private var _ctrl	: LightController;
		private var _type	: uint;
		
		protected function get lightData() : LightDataProvider
		{
			return _ctrl.lightData;
		}
		
		public function get color() : uint
		{
			return lightData.getLightProperty('color') as uint;
		}
		public function set color(v : uint)	: void
		{
			lightData.setLightProperty('color', v);
		}
		
		public function get emissionMask() : uint
		{
			return lightData.getLightProperty('emissionMask') as uint;
		}
		public function set emissionMask(v : uint) : void
		{
			lightData.setLightProperty('emissionMask', v);
		}
		
		public function get enabled() : Boolean
		{
			return lightData.getLightProperty('enabled') as Boolean; 
		}
		public function set enabled(v : Boolean) : void
		{
			lightData.setLightProperty('enabled', v);
		}
		
		public function get type() : uint
		{
			return _type;
		}
		
		public function get lightBleedingBias() : Number
		{
			return lightData.getLightProperty('lightBleedingBias');
		}
		public function set lightBleedingBias(value : Number) : void
		{
			lightData.setLightProperty('lightBleedingBias', value);
		}
		
		public function get lightBleedingInterpolation() : uint
		{
			return lightData.getLightProperty('lightBleedingInterpolation');
		}
		public function set lightBleedingInterpolation(value : uint) : void
		{
			lightData.setLightProperty('lightBleedingInterpolation', value);
		}
		
		public function get exponentialFactor() : uint
		{
			return lightData.getLightProperty('exponentialFactor');
		}
		public function set exponentialFactor(value : uint) : void
		{
			lightData.setLightProperty('exponentialFactor', value);
		}
		
		public function AbstractLight(controller		: LightController,
									  type				: uint,
									  color				: uint,
									  emissionMask		: uint)
		{
			super();
			
			_ctrl = controller;
			_type = type;
			
			addController(controller);
			
			this.color			= color;
			this.emissionMask	= emissionMask;
		}
	}
}
