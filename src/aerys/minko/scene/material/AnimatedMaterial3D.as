package aerys.minko.scene.material
{
	import aerys.minko.query.IScene3DQuery;
	import aerys.minko.query.rendering.RenderingQuery;
	import aerys.minko.scene.IScene3D;
	import aerys.minko.scene.group.Group3D;
	
	import flash.display.BitmapData;
	import flash.utils.getTimer;
	
	public class AnimatedMaterial3D extends Group3D implements IMaterial3D
	{
		public static const DEFAULT_FRAMERATE	: Number	= 30.;
		
		private var _version	: uint		= 0;
		
		private var _frame		: int		= 0;
		private var _lastFrame	: uint		= uint(-1);
		private var _framerate	: Number	= 0.;
		
		private var _time		: int		= 0;
		
		public function get version()	: uint		{ return _version; }
		public function get framerate() : Number	{ return _framerate; }
		
		public function set framerate(value : Number) : void
		{
			_framerate = value;
		}
		
		public function AnimatedMaterial3D(framerate : Number = DEFAULT_FRAMERATE)
		{
			super();
			
			_framerate = framerate;
		}
		
		override public function accept(query : IScene3DQuery) : void
		{
			var q : RenderingQuery = query as RenderingQuery;
			
			if (q && _lastFrame != q.frameId)
			{
				var t : int = getTimer();
				
				if (t - _time > (1000. / _framerate))
				{
					_lastFrame = q.frameId;
					nextFrame(q);
					
					_time = t;
				}
			}
			
			if (_frame < numChildren)
				query.query(rawChildren[_frame]);
		}
		
		protected function nextFrame(query : RenderingQuery) : void
		{
			_frame = (_frame + 1) % numChildren;
			++_version;
		}
	}
}