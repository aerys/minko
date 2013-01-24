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
		
		private var _enterFrame			: Signal;
		private var _renderingBegin		: Signal;
		private var _renderingEnd		: Signal;
		private var _exitFrame			: Signal;
		private var _layerNameChanged	: Signal; // function(scene : Scene, oldName : String, newName : String);

		private var _layers			: Vector.<String>	= new Vector.<String>(32, true);
		private var _nameToLayer	: Dictionary		= new Dictionary();
		
		public function getLayerName(layer : uint) : String
		{
			return _layers[layer];
		}
		
		public function setLayerName(layer : uint, name : String) : void
		{
			var oldName	: String = null;
			if (_layers[layer])
				oldName = _layers[oldName];
			
			_layers[layer] = name;
			_nameToLayer[name] = layer;
			
			_layerNameChanged.execute(this, oldName, name);
		}
		
		public function getLayerByName(name : String) : uint
		{
			return _nameToLayer[name];
		}
		
		public function belongsToLayer(leaf : ITaggable, layerName : String) : Boolean
		{
			var layer : uint = getLayerByName(layerName);
			
			return (leaf.tag & layer) != 0;
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
			_bindings = new DataBindings(this);
			this.properties = new DataProvider(DataProviderUsage.EXCLUSIVE);
			
			super.initialize();
		}
		
		override protected function initializeSignals() : void
		{
			super.initializeSignals();
			
			_enterFrame = new Signal('Scene.enterFrame');
			_renderingBegin = new Signal('Scene.renderingBegin');
			_renderingEnd = new Signal('Scene.renderingEnd');
			_exitFrame = new Signal('Scene.exitFrame');
			_layerNameChanged = new Signal('Scene.layerNameChanged');
			_layers[0] = 'Default';
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