package aerys.minko.scene.material
{
	import aerys.minko.render.ressource.IRessource3D;
	import aerys.minko.scene.interfaces.IScene3DRessource;
	import aerys.minko.scene.group.Group3D;
	
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
		
		
		public function get ressource() : IRessource3D
		{
			return null;
		}
		
		/**
		 * FIXME Je doute que ce truc marche.
		 */
		public function gotoFrame(frameId : uint) : void
		{
			if (_lastFrame != frameId)
			{
				var t : int = getTimer();
				
				if (t - _time > (1000. / _framerate))
				{
					_lastFrame = frameId;
					nextFrame();
					
					_time = t;
				}
			}
		}
		
		public function nextFrame() : void
		{
			_frame = (_frame + 1) % numChildren;
			++_version;
		}
	}
}