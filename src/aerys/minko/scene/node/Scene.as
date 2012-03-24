package aerys.minko.scene.node
{
	import aerys.minko.ns.minko_scene;
	import aerys.minko.scene.controller.scene.RenderingController;
	import aerys.minko.scene.controller.scene.TickController;
	import aerys.minko.type.data.DataBindings;

	/**
	 * Scene objects are the root of any 3D scene.
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public dynamic class Scene extends Group
	{
		private var _tickController			: TickController		= new TickController();
		private var _renderingController	: RenderingController	= new RenderingController();
		
		private var _camera		: Camera			= null;
		private var _bindings	: DataBindings		= new DataBindings();

		public function get bindings() : DataBindings
		{
			return _bindings;
		}
		
		public function get tickController() : TickController
		{
			return _tickController;
		}
		
		public function get renderingController() : RenderingController
		{
			return _renderingController;
		}
		
		public function Scene(...children)
		{
			super();
			
			addController(_tickController);
			addController(_renderingController);
			
			initializeChildren(children);
		}
		
		minko_scene function update() : void
		{
			_tickController.update();
		}
	}
}