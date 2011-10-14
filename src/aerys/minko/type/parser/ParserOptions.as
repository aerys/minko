package aerys.minko.type.parser
{
	import aerys.minko.scene.node.IScene;
	
	import flash.utils.Dictionary;

	public final class ParserOptions
	{
		private var _loadTextures					: Boolean	= false;
		
		private var _loadMeshes						: Boolean	= true;
		private var _loadSkins						: Boolean	= true;
		private var _mergeMeshes					: Boolean	= false;
		
		private var _keepStreamsDynamic				: Boolean	= true;
		
		private var _loadDependencyFunction			: Function	= null;
		private var _replaceDependencyPathFunction	: Function	= function(path : String) : String { return path; };
		private var _replaceNodeFunction			: Function	= function(node : IScene) : IScene { return node; };
		
		public function get loadTextures()					: Boolean	{ return _loadTextures;						}
		
		public function get loadMeshes()					: Boolean	{ return _loadMeshes;						}
		public function get loadSkins()						: Boolean	{ return _loadSkins;						}
		public function get mergeMeshes()					: Boolean	{ return _mergeMeshes;						}
		
		public function get keepStreamsDynamic()			: Boolean	{ return _keepStreamsDynamic;				}
		
		public function get loadDependencyFunction()		: Function	{ return _loadDependencyFunction;			}
		public function get replaceNodeFunction()			: Function	{ return _replaceNodeFunction;				}
		public function get replaceDependencyPathFunction()	: Function	{ return _replaceDependencyPathFunction;	}
		
		public function set loadTextures(value : Boolean) : void
		{
			_loadTextures = value;
		}

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
		
		public function set keepStreamsDynamic(value:Boolean):void
		{
			_keepStreamsDynamic = value;
		}
		
		/**
		 * @param value The prototype of this function must be function(path : String) : IScene
		 */		
		public function set loadDependencyFunction(value:Function):void
		{
			_loadDependencyFunction = value;
		}
		
		/**
		 * @param value The prototype of this function must be function(path : String) : String
		 */
		public function set replaceDependencyPathFunction(value:Function):void
		{
			_replaceDependencyPathFunction = value;
		}

		/**
		 * @param value The prototype of this function must be function(node : IScene) : IScene
		 */		
		public function set replaceNodeFunction(value:Function):void
		{
			_replaceNodeFunction = value;
		}
	}
}