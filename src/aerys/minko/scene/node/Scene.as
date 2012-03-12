package aerys.minko.scene.node
{
	import aerys.minko.ns.minko_scene;
	import aerys.minko.render.RenderingList;
	import aerys.minko.scene.controller.SceneController;
	import aerys.minko.type.data.DataBindings;

	/**
	 * Scene objects are the root of any 3D scene.
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public dynamic class Scene extends Group
	{
		private var _controller	: SceneController	= new SceneController();
		
		private var _list		: RenderingList		= new RenderingList();
		private var _camera		: Camera			= null;
		private var _bindings	: DataBindings		= new DataBindings();

		/**
		 * The RenderingList containing all the shaders/draw calls corresponding to
		 * the content of the scene.
		 *  
		 * @return 
		 * 
		 */
		public function get renderingList() : RenderingList
		{
			return _list;
		}
		
		public function get bindings() : DataBindings
		{
			return _bindings;
		}
		
		public function get sceneController() : SceneController
		{
			return _controller;
		}
		
		public function Scene(...children)
		{
			super();
			
			addController(_controller);
			
			initializeChildren(children);
		}
		
		minko_scene function update() : void
		{
			_controller.update();
		}
	}
}