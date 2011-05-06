package aerys.minko.type
{
	public class Animation
	{
		private var _name		: String	= null;
		private var _numFrames	: int		= 0;
		private var _offset		: int		= 0;
		private var _loop		: Boolean	= false;
		
		public function get name() 		: String	{ return _name; }
		public function get numFrames()	: int		{ return _numFrames; }
		public function get offset()	: int		{ return _offset; }
		public function get loop()		: Boolean	{ return _loop; }
		
		public function Animation(name		: String,
									numFrames	: int,
									offset		: int,
									loop		: Boolean	= false)
		{
			_name = name;
			_numFrames = numFrames;
			_offset = offset;
			_loop = loop;
		}
	}
}