package aerys.minko.scene.node.mesh
{
	import aerys.minko.render.effect.Effect;
	import aerys.minko.render.effect.basic.BasicShader;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.node.AbstractSceneNode;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.scene.node.mesh.geometry.Geometry;
	import aerys.minko.type.Signal;
	import aerys.minko.type.bounding.BoundingBox;
	import aerys.minko.type.bounding.BoundingSphere;
	import aerys.minko.type.data.DataBindings;
	import aerys.minko.type.data.DataProvider;

	/**
	 * Mesh objects are a visible instance of a Geometry rendered using a specific
	 * Effect with specific rendering properties.
	 * 
	 * <p>
	 * Those rendering properties are stored in a DataBindings object so they can
	 * be directly used by the shaders in the rendering API.
	 * </p>
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class Mesh extends AbstractSceneNode
	{
		public static const DEFAULT_EFFECT	: Effect	= new Effect(
			new BasicShader()
		);
		
		private var _geometry			: Geometry			= null;
		private var _effect				: Effect			= null;
		private var _properties			: DataProvider		= null;
		private var _bindings			: DataBindings		= new DataBindings();
		
		private var _visible			: Boolean			= true;
		
		private var _frustumCulling		: uint				= 0;
		
		private var _frame				: uint				= 0;
		
		private var _cloned				: Signal 			= new Signal('Mesh.clones');
		private var _effectChanged		: Signal			= new Signal('Mesh.effectChanged');
		private var _visibilityChanged	: Signal			= new Signal('Mesh.visibilityChanged');
		private var _frameChanged		: Signal			= new Signal('Mesh.frameChanged');
		private var _geometryChanged	: Signal			= new Signal('Mesh.geometryChanged');
		
		/**
		 * A DataProvider object already bound to the Mesh bindings.
		 * 
		 * <pre>
		 * // animate the "diffuseColor" property
		 * mesh.addController(
		 *   new AnimationController(
		 * 	  new <ITimeline>[new ColorTimeline(
		 * 	    "dataProvider.diffuseColor",
		 * 	    5000,
		 * 	    new <uint>[0xffffffff, 0xffffff00, 0xffffffff]
		 *    )]
		 *   )
		 * );
		 * </pre>
		 *  
		 * @return 
		 * 
		 */
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
		
		/**
		 * The rendering properties provided to the shaders to customize
		 * how the mesh will appear on screen.
		 * 
		 * @return 
		 * 
		 */
		public function get bindings() : DataBindings
		{
			return _bindings;
		}
		
		/**
		 * The Effect used for rendering. 
		 * @return 
		 * 
		 */
		public function get effect() : Effect
		{
			return _effect;
		}
		
		public function set effect(value : Effect) : void
		{
			if (_effect == value)
				return ;
			
			if (value == null)
				throw new Error();
			
			var oldEffect : Effect = _effect;
			
			_effect = value;
			
			_effectChanged.execute(this, oldEffect, value);
		}
		
		/**
		 * The Geometry of the mesh. 
		 * @return 
		 * 
		 */
		public function get geometry() : Geometry
		{
			return _geometry;
		}
		
		public function set geometry(value : Geometry) : void
		{
			if (_geometry != value)
			{
				var oldGeometry	: Geometry = _geometry;
				
				_geometry = value;
				_geometryChanged.execute(this, oldGeometry, value);
			}
		}
		
		/**
		 * Whether the mesh is visible or not.
		 *  
		 * @return 
		 * 
		 */
		public function get visible() : Boolean
		{
			return _visible;
		}
		
		public function set visible(value : Boolean) : void
		{
			var oldVisible : Boolean = _visible;
			
			_visible = value;
			
			if (oldVisible != value)
				_visibilityChanged.execute(this, value);
		}
		
		public function get frustumCulling() : uint
		{
			return _frustumCulling;
		}
		public function set frustumCulling(value : uint) : void
		{
			_frustumCulling = value;
		}
		
		public function get cloned() : Signal
		{
			return _cloned;
		}
		
		public function get visibilityChanged() : Signal
		{
			return _visibilityChanged;
		}
		
		public function get effectChanged() : Signal
		{
			return _effectChanged;
		}
		
		public function get frameChanged() : Signal
		{
			return _frameChanged;
		}
		
		public function get geometryChanged() : Signal
		{
			return _geometryChanged;
		}
		
		public function get frame() : uint
		{
			return _frame;
		}
		public function set frame(value : uint) : void
		{
			if (_frame != value)
			{
				var oldFrame : uint = _frame;
				
				_frame = value;
				_frameChanged.execute(this, oldFrame, value);
			}
		}
		
		public function Mesh(geometry	: Geometry	= null,
							 properties	: Object	= null,
							 effect		: Effect	= null,
							 ...controllers)
		{
			super();

			initialize(geometry, properties, effect, controllers);
		}
		
		private function initialize(geometry	: Geometry,
									properties	: Object,
									effect		: Effect,
									controllers	: Array) : void
		{
			this.properties = new DataProvider(properties);
			
			_geometry = geometry;
			this.effect = effect || DEFAULT_EFFECT;
			
			while (controllers && !(controllers[0] is AbstractController))
					controllers = controllers[0];
			
			if (controllers)
				for each (var ctrl : AbstractController in controllers)
					addController(ctrl);
		}
		
		override public function clone(cloneControllers : Boolean = false) : ISceneNode
		{
			var clone : Mesh = new Mesh();
			
			clone.copyFrom(this, true, cloneControllers);
			
			return clone;
		}
		
		override protected function addedToSceneHandler(child : ISceneNode, scene : Scene) : void
		{
			super.addedToSceneHandler(child, scene);
	
			if (child === this)
				_bindings.addProvider(transformData);
		}
		
		override protected function removedFromSceneHandler(child : ISceneNode, scene : Scene) : void
		{
			super.removedFromSceneHandler(child, scene);
			
			if (child === this)
				_bindings.removeProvider(transformData);
		}
		
		protected function copyFrom(source 				: Mesh,
									withBindings 		: Boolean,
									cloneControllers 	: Boolean) : void
		{
			var numControllers : uint = source.numControllers;
			
			name = source.name;
			_geometry = source._geometry;
			properties = source._properties.clone();
			
			copyControllersFrom(source, this, cloneControllers);
			transform.copyFrom(source.transform);
			
			effect = source._effect;
			
			source.cloned.execute(this, source);
		}
	}
}