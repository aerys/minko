package aerys.minko.scene.graph.texture
{
	import aerys.minko.scene.visitor.ISceneVisitor;
	import aerys.minko.scene.visitor.rendering.RenderingVisitor;
	import aerys.minko.scene.graph.IScene;
	import aerys.minko.scene.graph.group.Group;
	
	import flash.display.BitmapData;
	import flash.utils.getTimer;
	
	public class AnimatedTexture extends Group implements ITexture
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
		
		public function AnimatedTexture(framerate : Number = DEFAULT_FRAMERATE)
		{
			super();
			
			_framerate = framerate;
		}
		
		override public function visited(query : ISceneVisitor) : void
		{
			var q : RenderingVisitor = query as RenderingVisitor;
			
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
				query.visit(rawChildren[_frame]);
		}
		
		protected function nextFrame(query : RenderingVisitor) : void
		{
			_frame = (_frame + 1) % numChildren;
			++_version;
		}
	}
}