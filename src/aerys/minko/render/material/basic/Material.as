package aerys.minko.render.material.basic
{
	import aerys.minko.render.effect.Effect;
	import aerys.minko.type.data.DataProvider;

	public class Material extends DataProvider
	{
		public function get effect() : Effect
		{
			return getProperty('effect') as Effect;
		}
		public function set effect(value : Effect) : void
		{
			setProperty('effect', value);
		}
		
		public function Material(effect : Effect, properties : Object = null)
		{
			super(properties);
			
			effect = effect;
		}
	}
}