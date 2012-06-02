package aerys.minko.type.loader.parser
{
	import aerys.minko.render.effect.Effect;
	import aerys.minko.scene.node.mesh.Mesh;
	import aerys.minko.type.loader.ILoader;
	import aerys.minko.type.loader.SceneLoader;
	import aerys.minko.type.loader.TextureLoader;
	
	import flash.net.URLRequest;
	
	/**
	 * ParserOptions objects provide properties and function references
	 * to customize how a LoaderGroup object will load and interpret 
	 * content.
	 *  
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public final class ParserOptions
	{
		private var _loadDependencies			: Boolean	= false;
		private var _dependencyLoaderClosure	: Function	= defaultDependencyLoaderClosure;
        private var _loadSkin                   : Boolean   = false;
		private var _mipmapTextures				: Boolean	= true;
		private var _meshEffect					: Effect	= null;
		private var _vertexStreamUsage			: uint		= 0;
		private var _indexStreamUsage			: uint		= 0;
		private var _parser						: Class		= null;
		
		public function get parser() : Class
		{
			return _parser;
		}
		
		public function set parser(value : Class) : void
		{
			_parser = value;
		}
		
		public function get indexStreamUsage() : uint
		{
			return _indexStreamUsage;
		}
		
		public function set indexStreamUsage(value : uint) : void
		{
			_indexStreamUsage = value;
		}
		
		public function get vertexStreamUsage() : uint
		{
			return _vertexStreamUsage;
		}
		
		public function set vertexStreamUsage(value : uint) : void
		{
			_vertexStreamUsage = value;
		}
		
		public function get effect() : Effect
		{
			return _meshEffect;
		}
		
		public function set effect(value : Effect) : void
		{
			_meshEffect = value;
		}
		
		public function get mipmapTextures() : Boolean
		{
			return _mipmapTextures;
		}
		
		public function set mipmapTextures(value : Boolean) : void
		{
			_mipmapTextures = value;
		}
		
		public function get dependencyLoaderClosure() : Function
		{
			return _dependencyLoaderClosure;
		}
		
		public function set dependencyLoaderClosure(value : Function) : void
		{
			_dependencyLoaderClosure = value;
		}
		
		public function get loadDependencies() : Boolean
		{
			return _loadDependencies;
		}
		
		public function set loadDependencies(value : Boolean) : void
		{
			_loadDependencies = value;
		}

        public function get loadSkin() : Boolean
        {
            return _loadSkin;
        }

        public function set loadSkin(value : Boolean) : void
        {
            _loadSkin = value;
        }

		public function clone() : ParserOptions
		{
			return new ParserOptions(
				_loadDependencies,
				_dependencyLoaderClosure,
                _loadSkin,
				_mipmapTextures,
				_meshEffect,
				_vertexStreamUsage,
				_indexStreamUsage,
				_parser
			);
		}
		
		public function ParserOptions(loadDependencies			: Boolean 	= false,
									  dependencyLoaderClosure	: Function 	= null,
                                      loadSkin                  : Boolean 	= true,
									  mipmapTextures			: Boolean 	= true,
									  meshEffect				: Effect 	= null,
									  vertexStreamUsage			: uint 		= 0,
									  indexStreamUsage			: uint 		= 0,
									  parser					: Class 	= null)
		{
			_loadDependencies			= loadDependencies;
			_dependencyLoaderClosure	= dependencyLoaderClosure || _dependencyLoaderClosure;
            _loadSkin                   = loadSkin;
			_mipmapTextures				= mipmapTextures;
			_meshEffect					= meshEffect || Mesh.DEFAULT_EFFECT;
			_vertexStreamUsage			= vertexStreamUsage;
			_indexStreamUsage			= indexStreamUsage;
			_parser						= parser;
		}
		
		private function defaultDependencyLoaderClosure(dependencyPath	: String,
														isTexture		: Boolean,
														options			: ParserOptions) : ILoader
		{
			var loader : ILoader;
			
			if (isTexture)
			{
				loader = new TextureLoader(true);
				loader.load(new URLRequest(dependencyPath));
			}
			else
			{
				loader = new SceneLoader(options);
				loader.load(new URLRequest(dependencyPath));
			}
			
			return loader;
		}

    }
}
