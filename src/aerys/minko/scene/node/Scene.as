package aerys.minko.scene.node
{
	import aerys.minko.render.Viewport;
	import aerys.minko.render.effect.Effect;
	import aerys.minko.scene.controller.scene.RenderingController;
	import aerys.minko.type.Signal;
	import aerys.minko.type.data.DataBindings;
	import aerys.minko.type.data.DataProvider;
	import aerys.minko.type.data.IDataProvider;
	
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
		private var _renderingCtrl	: RenderingController	= new RenderingController();
		
		private var _camera			: Camera				= null;
		private var _properties		: DataProvider			= null;
		private var _bindings		: DataBindings			= new DataBindings();
		
		private var _enterFrame		: Signal				= new Signal("Scene.enterFrame");
		private var _exitFrame		: Signal				= new Signal("Scene.exitFrame");

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
			super();
			
			this.properties = new DataProvider();
			
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