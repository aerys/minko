package aerys.minko.scene.data
{
	import aerys.minko.type.data.DataProvider;
	
	public final class ViewportDataProvider extends DataProvider
	{
		private static const DATA_DESCRIPTOR	: Object	=
		{
			viewportWidth	: 'width',
			viewportHeight	: 'height'
		};
		
		private var _width	: Number	= 0.;
		private var _height	: Number	= 0.;
		
		public function get width() : Number
		{
			return _width;
		}
		public function set width(value : Number) : void
		{
			if (_width != value)
			{
				_width = value;
				changed.execute(this, 'width');
			}
		}
		
		public function get height() : Number
		{
			return _height;
		}
		public function set height(value : Number) : void
		{
			if (_height != value)
			{
				_height = value;
				changed.execute(this, 'height');
			}
		}
		
		override public function get dataDescriptor() : Object
		{
			return DATA_DESCRIPTOR;
		}
		
		public function ViewportDataProvider(data : Object = null)
		{
			super(data);
		}
	}
}