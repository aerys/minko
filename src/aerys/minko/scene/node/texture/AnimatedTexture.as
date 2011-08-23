package aerys.minko.scene.node.texture
{
	import aerys.minko.render.resource.TextureResource;
	import aerys.minko.scene.action.texture.AnimatedTextureAction;
	import aerys.minko.scene.node.group.Group;
	
	public class AnimatedTexture extends Group implements ITexture
	{
		public static const DEFAULT_FRAMERATE	: Number	= 30.;
		
		private var _version		: uint		= 0;
		
		private var _frame			: int		= 0;
		private var _framerate		: Number	= 0.;
		
		public function get version()		: uint		{ return _version; }
		public function get framerate() 	: Number	{ return _framerate; }
		public function get currentFrame()	: int		{ return _frame;}
		
		/**
		 * @todo implement 
		 * @return 
		 * 
		 */
		public function get styleProperty()	: int		{ return (rawChildren[_frame] as ITexture).styleProperty; }
		
		public function set framerate(value : Number) : void
		{
			_framerate = value;
		}
		
		public function AnimatedTexture(framerate : Number = DEFAULT_FRAMERATE)
		{
			super();
			
			_framerate = framerate;
			
			actions.length = 0;
			actions[0] = new AnimatedTextureAction();
		}
		
		/**
		 * @todo implement
		 */
		public function get resource() : TextureResource
		{
			throw new Error('Implement me');
		}
		
		public function nextFrame() : void
		{
			_frame = (_frame + 1) % numChildren;
			++_version;
		}
	}
}