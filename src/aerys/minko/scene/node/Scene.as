package aerys.minko.scene.node
{
	import aerys.minko.render.Viewport;
	import aerys.minko.scene.controller.scene.SceneRenderingController;
	import aerys.minko.type.Signal;
	import aerys.minko.type.data.DataBindings;
	
	import flash.display.BitmapData;

	/**
	 * Scene objects are the root of any 3D scene.
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public final class Scene extends Group
	{
		private static const TIME_OFFSET		: Number				= new Date().time;
		
		private var _renderingController	: SceneRenderingController	= new SceneRenderingController();
		
		private var _camera		: Camera			= null;
		private var _bindings	: DataBindings		= new DataBindings();
		
		private var _enterFrame	: Signal			= new Signal("Scene.enterFrame");
		private var _exitFrame	: Signal			= new Signal("Scene.exitFrame");

		public function get bindings() : DataBindings
		{
			return _bindings;
		}
		
		/**
		 * The signal executed when the viewport is about to start rendering a frame.
		 * Callback functions for this signal should accept the following arguments:
		 * <ul>
		 * <li>viewport : Viewport, the viewport who starts rendering the frame</li>
		 * </ul>
		 * @return 
		 * 
		 */
		public function get enterFrame() : Signal
		{
			return _enterFrame;
		}
		
		/**
		 * The signal executed when the viewport is done rendering a frame.
		 * Callback functions for this signal should accept the following arguments:
		 * <ul>
		 * <li>viewport : Viewport, the viewport who just finished rendering the frame</li>
		 * </ul>
		 * @return 
		 * 
		 */
		public function get exitFrame() : Signal
		{
			return _exitFrame;
		}
		
		public function Scene(...children)
		{
			super();
			
			addController(_renderingController);
			
			initializeChildren(children);
		}
		
		public function render(viewport : Viewport, target : BitmapData = null) : void
		{
			_enterFrame.execute(this, viewport, target, new Date().time);
			_exitFrame.execute(this, viewport, target, new Date().time);
		}
	}
}