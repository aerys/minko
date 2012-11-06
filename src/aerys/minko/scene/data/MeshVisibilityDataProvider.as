package aerys.minko.scene.data
{
	import aerys.minko.type.Signal;
	import aerys.minko.type.binding.IDataProvider;
	import aerys.minko.type.enum.DataProviderUsage;
	
	public final class MeshVisibilityDataProvider implements IDataProvider
	{
		private static const DATA_DESCRIPTOR	: Object	= {
			'visible'			: 'visible',
			'frustumCulling'	: 'frustumCulling',
			'insideFrustum'		: 'insideFrustum'
		};
		
		private var _visible			: Boolean	= true;
		private var _frustumCulling		: uint		= 0;
		private var _insideFrustum		: Boolean	= true;
		
		private var _changed			: Signal	= new Signal('MeshVisibilityDataProvider.changed');
		private var _propertyChanged	: Signal	= new Signal('MeshVisibilityDataProvider.propertyChanged');
		
		public function get usage() : uint
		{
			return DataProviderUsage.EXCLUSIVE;
		}
		
		public function get visible() : Boolean
		{
			return _visible;
		}
		public function set visible(value : Boolean) : void
		{
			if (_visible != value)
			{
				_visible = value;
				_changed.execute(this, 'visible');
			}
		}
		
		public function get frustumCulling() : uint
		{
			return _frustumCulling;
		}
		public function set frustumCulling(value : uint) : void
		{
			if (_frustumCulling != value)
			{
				_frustumCulling = value;
				_changed.execute(this, 'frustumCulling');
			}
		}
		
		public function get insideFrustum() : Boolean
		{
			return _insideFrustum;
		}
		public function set insideFrustum(value : Boolean) : void
		{
			if (_insideFrustum != value)
			{
				_insideFrustum = value;
				_changed.execute(this, 'insideFrustum');
			}
		}
		
		public function get propertyChanged() : Signal
		{
			return _propertyChanged;
		}
		
		public function get changed() : Signal
		{
			return _changed;
		}
		
		public function get dataDescriptor() : Object
		{
			return DATA_DESCRIPTOR;
		}
		
		public function clone() : IDataProvider
		{
			var clone : MeshVisibilityDataProvider = new MeshVisibilityDataProvider();
			
			clone.visible = visible;
			clone.frustumCulling = frustumCulling;
			clone.insideFrustum = insideFrustum;
			
			return clone;
		}
	}
}