package aerys.minko.scene.data
{
	import aerys.minko.type.enum.DataProviderUsage;
	import aerys.minko.type.binding.DataProvider;

	public dynamic class LightDataProvider extends DataProvider
	{
		public function LightDataProvider()
		{
			super(null, 'LightingDataProvider', DataProviderUsage.MANAGED);
		}
	}
}
