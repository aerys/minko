package aerys.minko.type.parser
{
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.group.LoaderGroup;
	
	import flash.display.Loader;
	import flash.net.URLRequest;
	import flash.utils.Dictionary;

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
		private var _loadTextures					: Boolean	= false;
		
		private var _loadMeshes						: Boolean	= true;
		private var _loadSkins						: Boolean	= true;
		private var _mergeMeshes					: Boolean	= false;
		
		private var _keepStreamsDynamic				: Boolean	= true;
		
		private var _loadFunction					: Function	= load;
		private var _rewritePathFunction			: Function	= rewritePath;
		private var _replaceNodeFunction			: Function	= replaceNode;
		
		/**
		 * If set to true, the LoaderGroup will load embed and/or external textures. 
		 * @return 
		 * 
		 */
		public function get loadTextures()			: Boolean	{ return _loadTextures; }
		
		/**
		 * If set to true, the LoaderGroup will load meshes. 
		 * @return 
		 * 
		 */
		public function get loadMeshes()			: Boolean	{ return _loadMeshes; }
		/**
		 * If set to true, the LoaderGroup will load skins and animations. 
		 * @return 
		 * 
		 */
		public function get loadSkins()				: Boolean	{ return _loadSkins; }
		/**
		 * If set to true, meshes will be merged whenever possible. 
		 * @return 
		 * 
		 */
		public function get mergeMeshes()			: Boolean	{ return _mergeMeshes; }
		
		public function get keepStreamsDynamic()	: Boolean	{ return _keepStreamsDynamic; }
		
		/**
		 * The function to call when external items such as textures must be loaded.
		 * This function should have the following prototype:
		 * 
		 * <pre>
		 * function(request : URLRequest, options : ParserOptions = null) : IScene
		 * </pre>
		 * 
		 * The default value is the LoaderGroup.load method.
		 *  
		 * @return 
		 * 
		 */
		public function get loadFunction()			: Function	{ return _loadFunction; }
		/**
		 * A function to call on every loaded node in order to replace it before inserting it
		 * in the loaded scene graph. This function should have the following prototype:
		 * 
		 * <pre>
		 * function(node : IScene) : IScene
		 * </pre>
		 *
		 * The default value is the idendity function: it will return the node unchanged.
		 * 
		 * @return 
		 * 
		 */
		public function get replaceNodeFunction()	: Function	{ return _replaceNodeFunction; }
		/**
		 * A function that will rewrite the path of the external loaded items such as textures.
		 * This function should have the following prototype:
		 * 
		 * <pre>
		 * function(path : String) : String
		 * </pre> 
		 * 
		 * The default value is the idendity function: it will return the path unchanged.
		 * 
		 * @return 
		 * 
		 */
		public function get rewritePathFunction()	: Function	{ return _rewritePathFunction; }
		
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
		public function set loadFunction(value:Function):void
		{
			_loadFunction = value;
		}
		
		/**
		 * @param value The prototype of this function must be function(path : String) : String
		 */
		public function set rewritePathFunction(value:Function):void
		{
			_rewritePathFunction = value;
		}

		/**
		 * @param value The prototype of this function must be function(node : IScene) : IScene
		 */		
		public function set replaceNodeFunction(value:Function):void
		{
			_replaceNodeFunction = value;
		}
		
		protected function load(request : URLRequest, options : ParserOptions = null) : LoaderGroup
		{
			return LoaderGroup.load(request, options);
		}
		
		protected function rewritePath(path : String) : String
		{
			return path;
		}
		
		protected function replaceNode(node : IScene) : IScene
		{
			return node;
		}
	}
}