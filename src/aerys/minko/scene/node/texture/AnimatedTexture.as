package aerys.minko.scene.node.texture
{
	import aerys.minko.render.ressource.IRessource;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.group.Group;
	import aerys.minko.scene.visitor.ISceneVisitor;
	import aerys.minko.scene.visitor.rendering.RenderingVisitor;
	
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
		
		private var _current	: IScene	= null;
		
		public function get version()		: uint		{ return _version; }
		public function get framerate() 	: Number	{ return _framerate; }
		/**
		 * @todo implement 
		 * @return 
		 * 
		 */
		public function get styleProperty()	: String	{ throw new Error(); }
		
		public function set framerate(value : Number) : void
		{
			_framerate = value;
		}
		
		public function AnimatedTexture(framerate : Number = DEFAULT_FRAMERATE)
		{
			super();
			
			_framerate = framerate;
		}
		
		/**
		 * @todo implement
		 */
		public function get ressource() : IRessource
		{
			throw new Error('Implement me');
		}
		
//		override public function visited(query : ISceneVisitor) : void
//		{
//			var q : RenderingVisitor = query as RenderingVisitor;
//			
//			if (q && _lastFrame != q.frameId)
//			{
//				var t : int = getTimer();
//				
//				if (t - _time > (1000. / _framerate))
//				{
//					_lastFrame = q.frameId;
//					nextFrame(q);
//					
//					_time = t;
//				}
//			}
//			
//			if (_frame < numChildren)
//			{
//				_current = rawChildren[_frame];
//				query.query(_current);
//			}
//		}
		
		public function nextFrame() : void
		{
			_frame = (_frame + 1) % numChildren;
			++_version;
		}
	}
}