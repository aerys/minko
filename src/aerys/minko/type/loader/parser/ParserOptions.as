package aerys.minko.type.loader.parser
{
	import flash.net.URLRequest;
	
	import aerys.minko.render.Effect;
	import aerys.minko.render.material.Material;
	import aerys.minko.render.material.basic.BasicMaterial;
	import aerys.minko.render.material.basic.BasicProperties;
	import aerys.minko.type.loader.AssetsLibrary;
	import aerys.minko.type.loader.ILoader;
	import aerys.minko.type.loader.SceneLoader;
	import aerys.minko.type.loader.TextureLoader;
	
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
		private var _dependencyLoaderFunction	: Function;
		private var	_materialFunction			: Function;
        private var _loadSkin                   : Boolean;
		private var _skinningMethod				: uint;
		private var _flattenSkinning			: Boolean;
		private var _skinningNumFps				: uint;
		private var _mipmapTextures				: Boolean;
		private var _meshEffect					: Effect;
		private var _material					: Material;
		private var _vertexStreamUsage			: uint;
		private var _indexStreamUsage			: uint;
		private var _parser						: Class;
		private var _assets						: AssetsLibrary;
		private var _interpolateAnimations		: Boolean;
		
		public function get skinningMethod() : uint
		{
			return _skinningMethod;
		}

		public function set skinningMethod(value : uint) : void
		{
			_skinningMethod = value;
		}
		
		public function get skinningNumFps() : uint
		{
			return _skinningNumFps;
		}
		
		public function set skinningNumFps(value : uint) : void
		{
			_skinningNumFps = value;
		}
		
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
		
		public function get material() : Material
		{
			return _material;
		}
		
		public function set material(value : Material) : void
		{
			_material = value;
		}

		public function get mipmapTextures() : Boolean
		{
			return _mipmapTextures;
		}
		
		public function set mipmapTextures(value : Boolean) : void
		{
			_mipmapTextures = value;
		}
		
		public function get dependencyLoaderFunction() : Function
		{
			return _dependencyLoaderFunction;
		}
		
		public function set dependencyLoaderFunction(value : Function) : void
		{
			_dependencyLoaderFunction = value;
		}

		public function get materialFunction() : Function
		{
			return _materialFunction;
		}
		
		public function set materialFunction(value : Function) : void
		{
			_materialFunction = value;
		}
		
        public function get loadSkin() : Boolean
        {
            return _loadSkin;
        }

        public function set loadSkin(value : Boolean) : void
        {
            _loadSkin = value;
        }
		
		public function get flattenSkinning() : Boolean
		{
			return _flattenSkinning;
		}
		
		public function set flattenSkinning(value : Boolean) : void
		{
			_flattenSkinning = value;
		}
		
		public function get assets() : AssetsLibrary
		{
			return _assets || (_assets = new AssetsLibrary());
		}
		
		public function set assets(value : AssetsLibrary):void
		{
			_assets = value;
		}
		
		public function get interpolateAnimations():Boolean
		{
			return _interpolateAnimations;
		}
		
		public function set interpolateAnimations(value:Boolean):void
		{
			_interpolateAnimations = value;
		}

		public function clone() : ParserOptions
		{
			return new ParserOptions(
				_dependencyLoaderFunction,
				_materialFunction,
                _loadSkin,
				_skinningMethod,
				_flattenSkinning,
				_skinningNumFps,
				_mipmapTextures,
				_material,
				_vertexStreamUsage,
				_indexStreamUsage,
				_parser,
				_assets,
				_interpolateAnimations
			);
		}
		
		public function ParserOptions(dependencyLoaderFunction	: Function 		= null,
									  materialFunction			: Function		= null,
                                      loadSkin                  : Boolean 		= true,
									  skinningMethod			: uint			= 2,
									  flattenSkinning			: Boolean		= false,
									  skinningNumFps			: uint			= 0,
									  mipmapTextures			: Boolean 		= true,
									  material					: Material 		= null,
									  vertexStreamUsage			: uint 			= 0,
									  indexStreamUsage			: uint 			= 0,
									  parser					: Class			= null,
									  assets					: AssetsLibrary	= null,
									  interpolateAnimations		: Boolean		= false)
		{
			_dependencyLoaderFunction	= dependencyLoaderFunction 	|| defaultDependencyLoaderFunction;
			_materialFunction			= materialFunction			|| defaultMaterialFunction;
            _loadSkin                   = loadSkin;
			_skinningMethod				= skinningMethod;
			_flattenSkinning			= flattenSkinning;
			_skinningNumFps				= skinningNumFps;
			_mipmapTextures				= mipmapTextures;
			_material					= _material || new BasicMaterial();
			_vertexStreamUsage			= vertexStreamUsage;
			_indexStreamUsage			= indexStreamUsage;
			_parser						= parser;
			_assets						= assets;
			_interpolateAnimations		= interpolateAnimations;
		}
		
		private function defaultDependencyLoaderFunction(dependencyId	: String,
														 dependencyPath	: String,
														 isTexture		: Boolean,
														 options		: ParserOptions) : ILoader
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
		
		private function defaultMaterialFunction(materialName	: String,
												 material		: Material) : Material
		{
			return material ? material : _material; // default material if nothing
		}
    }
}
