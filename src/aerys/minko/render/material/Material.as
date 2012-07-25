package aerys.minko.render.material
{
	import aerys.minko.render.Effect;
	import aerys.minko.type.data.DataProvider;
	import aerys.minko.type.data.IDataProvider;
	import aerys.minko.type.enum.DataProviderUsage;

	public dynamic class Material extends DataProvider
	{
		public function get effect() : Effect
		{
			return getProperty('effect') as Effect;
		}
		public function set effect(value : Effect) : void
		{
			setProperty('effect', value);
		}
		
		public function Material(effect : Effect = null, properties : Object = null, name : String = null)
		{
			super(properties, name, DataProviderUsage.MANAGED);
			
			this.effect = effect;
		}
		
		override public function clone() : IDataProvider
		{
			var mat : Material	= new Material(effect, this, name);
			
			return mat;
		}
	}
}