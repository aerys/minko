package aerys.minko.type.parser
{
	public final class ParserOptions
	{
		private var _loadTextures				: Boolean	= true;
		private var _textureFilenameFunction	: Function	= null;
		private var _textureFunction			: Function	= null;
		private var _sceneFunction				: Function	= null;
		
		public function get loadTextures()				: Boolean	{ return _loadTextures; }
		public function get textureFilenameFunction()	: Function	{ return _textureFilenameFunction; }
		public function get textureFunction()			: Function	{ return _textureFunction; }
		public function get sceneFunction()				: Function	{ return _sceneFunction; }
		
		public function set loadTextures(value : Boolean) : void
		{
			_loadTextures = value;
		}
		
		public function set textureFilenameFunction(value : Function) : void
		{
			_textureFilenameFunction = value;
		}
		
		public function set textureFunction(value : Function) : void
		{
			_textureFunction = value;
		}
		
		public function set sceneFunction(value : Function) : void
		{
			_sceneFunction = value;
		}
	}
}