package aerys.minko.render.effect
{
	import aerys.minko.scene.node.mesh.Mesh;
	import aerys.minko.type.data.DataBinding;

	public class EffectInstance
	{
		private var _effect			: Effect			= null;
		private var _dataBinding	: DataBinding		= null;
		private var _watched		: Vector.<String>	= new <String>["test"];
		
		public function EffectInstance(effect : Effect, dataBinding : DataBinding)
		{
			_effect = effect;
			_dataBinding = dataBinding;
			
			initialize();
		}
		
		private function initialize() : void
		{
			_dataBinding.propertyChanged.add(propertyChangedHandler);
		}
		
		private function propertyChangedHandler(dataBinding		: DataBinding,
												propertyName	: String,
												oldValue		: Object,
												newValue		: Object) : void
		{
			if (_watched.indexOf(propertyName) >= 0)
			{
				_effect.fork(_dataBinding);
			}
		}
	}
}