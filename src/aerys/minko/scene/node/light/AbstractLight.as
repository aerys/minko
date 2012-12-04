package aerys.minko.scene.node.light
{
	import aerys.minko.ns.minko_lighting;
	import aerys.minko.scene.controller.light.LightController;
	import aerys.minko.scene.data.LightDataProvider;
	import aerys.minko.scene.node.AbstractSceneNode;
	
	use namespace minko_lighting;
	
	public class AbstractLight extends AbstractSceneNode
	{
		private var _ctrl		: LightController;
		private var _type		: uint;
		
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
