package aerys.minko.type.parser
{
	public final class ParserOptions
	{
		private var _loadTextures				: Boolean	= true;
		private var _textureFilenameFunction	: Function	= null;
		private var _textureFunction			: Function	= null;

//		private var _sceneFunction				: Function	= null;

		private var _loadMeshes					: Boolean	= true;
		private var _mergeMeshes				: Boolean	= false;

		private var _loadSkins					: Boolean	= true;

		public function get loadTextures()				: Boolean	{ return _loadTextures; }
		public function get textureFilenameFunction()	: Function	{ return _textureFilenameFunction; }
		public function get textureFunction()			: Function	{ return _textureFunction; }
//		public function get sceneFunction()				: Function	{ return _sceneFunction; }

		public function get loadMeshes()				: Boolean	{ return _loadMeshes; }
		public function get mergeMeshes()				: Boolean	{ return _mergeMeshes; }

		public function get loadSkins()					: Boolean	{ return _loadSkins; }

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

		/*public function set sceneFunction(value : Function) : void
		{
			_sceneFunction = value;
		}*/

		public function set loadMeshes(value : Boolean) : void
		{
			_loadMeshes = value;
		}

		public function set mergeMeshes(value : Boolean) : void
		{
			_mergeMeshes = value;
		}

		public function set loadSkins(value : Boolean) : void
		{
			_loadSkins = value;
		}
	}
}