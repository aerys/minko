package aerys.minko.scene.node
{
	import aerys.minko.ns.minko_scene;
	import aerys.minko.render.Effect;
	import aerys.minko.render.Viewport;
	import aerys.minko.scene.controller.scene.RenderingController;
	import aerys.minko.scene.node.camera.AbstractCamera;
	import aerys.minko.type.Signal;
	import aerys.minko.type.binding.DataBindings;
	import aerys.minko.type.binding.DataProvider;
	import aerys.minko.type.enum.DataProviderUsage;
	
	import flash.display.BitmapData;
	import flash.utils.getTimer;

	/**
	 * Scene objects are the root of any 3D scene.
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public final class Scene extends Group
	{
		use namespace minko_scene;
		
		minko_scene var _camera		: AbstractCamera;
		
		private var _renderingCtrl	: RenderingController;
		
		private var _properties		: DataProvider;
		private var _bindings		: DataBindings;
		
		private var _enterFrame		: Signal;
		private var _exitFrame		: Signal;

		public function get activeCamera() : AbstractCamera
		{
			return _camera;
		}
		
		public function get numPasses() : uint
		{
			return _renderingCtrl.numPasses;
		}
		
		public function get numTriangles() : uint
		{
			return _renderingCtrl.numTriangles;
		}
		
		public function get postProcessingEffect() : Effect
		{
			return _renderingCtrl.postProcessingEffect;
		}
		public function set postProcessingEffect(value : Effect) : void
		{
			_renderingCtrl.postProcessingEffect = value;
		}
		
		public function get postProcessingProperties() : DataProvider
		{
			return _renderingCtrl.postProcessingProperties;
		}
		
		public function get properties() : DataProvider
		{
			return _properties;
		}
		public function set properties(value : DataProvider) : void
		{
			if (_properties != value)
			{
				if (_properties)
					_bindings.removeProvider(_properties);
				
				_properties = value;
				
				if (value)
					_bindings.addProvider(value);
			}
		}
		
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
			_enterFrame = new Signal('Scene.enterFrame');
			_exitFrame = new Signal('Scene.exitFrame');

			super();
			
			initialize(children);
		}
		
		private function initialize(children : Array) : void
		{
			_bindings = new DataBindings(this);
			this.properties = new DataProvider(DataProviderUsage.EXCLUSIVE);
			
			_renderingCtrl = new RenderingController();
			addController(_renderingCtrl);
			
			initializeChildren(children);
		}
		
		public function render(viewport : Viewport, destination : BitmapData = null) : void
		{
			_enterFrame.execute(this, viewport, destination, getTimer());
			_exitFrame.execute(this, viewport, destination, getTimer());
		}
		
		override protected function addedHandler(child : ISceneNode, parent : Group) : void
		{
			throw new Error();
		}
	}
}