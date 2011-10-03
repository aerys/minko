package aerys.minko.scene.node.texture
{
	import flash.display.MovieClip;

	/**
	 * The MovieClipTexture enables using MovieClip objects as texture data.
	 *
	 * <p>
	 * MovieClipTexture objects makes it easy to use animated materials and
	 * provides an interface similar to the MovieClip class with methods such
	 * as "gotoAndPlay", "play" or "gotoAndStop".
	 * </p>
	 *
	 * <p>
	 * The source MovieClip frames are uploaded to the graphics hardware when
	 * required. Because each MovieClip only has a limited number of frames,
	 * each frame will be uploaded once and only once. Thus, modifying the
	 * MovieClip during or after upload will give unpredicted results.
	 * </p>
	 *
	 * @author Jean-Marc Le Roux
	 *
	 */
	public class MovieClipTexture extends AnimatedTexture
	{
		private var _source			: MovieClip			= null;
		private var _currentFrame	: int				= 1;
		private var _playing		: Boolean			= true;

		private var _frames			: Vector.<Boolean>	= new Vector.<Boolean>();

		public function get isPlaying() : Boolean	{ return _playing; }
		public function get source()	: MovieClip	{ return _source; }

		public function set source(value : MovieClip) : void
		{
			_source = value;

			if (_source)
			{
				_source.gotoAndStop(0);
				_frames.length = 0;
			}
		}

		public function MovieClipTexture(source : MovieClip = null)
		{
			super();

			if (source)
			{
				_source = source;
				_source.gotoAndStop(0);
			}
		}

		override public function nextFrame() : void
		{
			if (_source && _playing)
			{
				if (_currentFrame >= _frames.length)
				{
					_frames[int(_currentFrame - 1)] = true;

					var mat : BitmapTexture = BitmapTexture.fromDisplayObject(_source);

					mat.name = _source.currentFrameLabel;

					addChild(mat);
					_source.gotoAndStop(_currentFrame++);

					if (numChildren == _source.totalFrames)
						_source = null;
				}
			}

			super.nextFrame();
		}

		public function stop() : void
		{
			_playing = false;
		}

		public function play() : void
		{
			_playing = true;
		}

		public function gotoAndPlay(frame : Object) : void
		{
			_source.gotoAndStop(frame);
			_playing = true;
		}

		public function gotoAndStop(frame : Object) : void
		{
			_source.gotoAndStop(frame);
			_playing = false;
		}
	}
}