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
	import aerys.minko.type.loader.AssetsLibrary;
	
	import flash.display.BitmapData;
	import flash.utils.Dictionary;
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
		
		private var _numTriangles	: uint;
		
		private var _enterFrame		: Signal;
		private var _renderingBegin	: Signal;
		private var _renderingEnd	: Signal;
		private var _exitFrame		: Signal;

		private var _assets			: AssetsLibrary;		
		
		public function get assets() : AssetsLibrary
		{
			return _assets;
		}

		public function get activeCamera() : AbstractCamera
		{
			return _camera;
		}
		
		public function get numPasses() : uint
		{
			return _renderingCtrl.numPasses;
		}
		
		public function get numEnabledPasses() : uint
		{
			return _renderingCtrl.numEnabledPasses;
		}
		
		public function get numTriangles() : uint
		{
			return _numTriangles;
		}
		
		public function get numDrawCalls() : uint
		{
			return _renderingCtrl.numDrawCalls;
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
		
		public function get renderingBegin() : Signal
		{
			return _renderingBegin;
		}
		
		public function get renderingEnd() : Signal
		{
			return _renderingEnd;
		}
		
		public function Scene(...children)
		{
			super(children);
		}
		
		override protected function initialize() : void
		{
			_bindings 					= new DataBindings(this);
			_assets						= new AssetsLibrary();
			this.properties 			= new DataProvider(DataProviderUsage.EXCLUSIVE);

			super.initialize();
		}
		
		override protected function initializeSignals() : void
		{
			super.initializeSignals();
			
			_enterFrame			= new Signal('Scene.enterFrame');
			_renderingBegin 	= new Signal('Scene.renderingBegin');
			_renderingEnd 		= new Signal('Scene.renderingEnd');
			_exitFrame 			= new Signal('Scene.exitFrame');
		}
		
		override protected function initializeSignalHandlers() : void
		{
			super.initializeSignalHandlers();
			
			added.add(addedHandler);
		}
		
		override protected function initializeContollers() : void
		{
			_renderingCtrl = new RenderingController();
			addController(_renderingCtrl);
			
			super.initializeContollers();
		}
		
		public function render(viewport : Viewport, destination : BitmapData = null) : void
		{
			_enterFrame.execute(this, viewport, destination, getTimer());
			if (viewport.ready && viewport.visible)
			{
				_renderingBegin.execute(this, viewport, destination, getTimer());
				_numTriangles = _renderingCtrl.render(viewport, destination);
				_renderingEnd.execute(this, viewport, destination, getTimer());
			}
			_exitFrame.execute(this, viewport, destination, getTimer());
		}
		
		private function addedHandler(child : ISceneNode, parent : Group) : void
		{
			throw new Error();
		}
	}
}