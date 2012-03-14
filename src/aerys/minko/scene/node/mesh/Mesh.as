package aerys.minko.scene.node.mesh
{
	import aerys.minko.render.effect.Effect;
	import aerys.minko.render.effect.basic.BasicShader;
	import aerys.minko.scene.controller.mesh.RenderingController;
	import aerys.minko.scene.node.AbstractSceneNode;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.scene.node.mesh.geometry.Geometry;
	import aerys.minko.type.Signal;
	import aerys.minko.type.bounding.BoundingBox;
	import aerys.minko.type.bounding.BoundingSphere;
	import aerys.minko.type.data.DataBindings;

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
	public final class Mesh extends AbstractSceneNode
	{
		public static const DEFAULT_EFFECT		: Effect			= new Effect(
			new BasicShader()
		);
		
		private static const EXCLUDED_BINDINGS	: Vector.<String>	= new <String>[
			"local to world",
			"world to local"
		];
		
		private var _geometry			: Geometry			= null;
		private var _effect				: Effect			= null;
		private var _bindings			: DataBindings		= new DataBindings();
		
		private var _boundingSphere		: BoundingSphere	= null;
		private var _boundingBox		: BoundingBox		= null;
		private var _visible			: Boolean			= true;
		
		private var _frustumCulling		: uint				= 0;
		
		private var _effectChanged		: Signal			= new Signal();
		private var _visibilityChanged	: Signal			= new Signal();
		
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
		
		public function get boundingSphere() : BoundingSphere
		{
			return _boundingSphere;
		}
		
		public function get boundingBox() : BoundingBox
		{
			return _boundingBox;
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
		
		public function get visibilityChanged() : Signal
		{
			return _visibilityChanged;
		}
		
		public function get effectChanged() : Signal
		{
			return _effectChanged;
		}
		
		public function Mesh(geometry	: Geometry	= null,
							 properties	: Object	= null,
							 effect		: Effect	= null)
		{
			super();

			initialize(properties);

			_geometry = geometry;
			this.effect = effect || DEFAULT_EFFECT;
		}
		
		private function initialize(properties : Object) : void
		{
			if (properties)
				_bindings.setProperties(properties);
			
			addController(RenderingController.renderingController);
		}
		
		override public function clone() : ISceneNode
		{
			var cloned : Mesh = new Mesh();
			
			cloned.copyFrom(this, true);
			
			return cloned;
		}
		
		override protected function addedToSceneHandler(child : ISceneNode, scene : Scene) : void
		{
			super.addedToSceneHandler(child, scene);
	
			if (child === this)
			{
				_bindings.addProperty("local to world", localToWorld);
				_bindings.addProperty("world to local", worldToLocal);
			}
		}
		
		override protected function removedFromSceneHandler(child : ISceneNode, scene : Scene) : void
		{
			super.removedFromSceneHandler(child, scene);
			
			if (child === this)
			{
				_bindings.removeProperty("local to world");
				_bindings.removeProperty("world to local");
			}
		}
		
		protected function copyFrom(source 			: Mesh,
									withBindings 	: Boolean) : void
		{
			var numControllers : uint = source.numControllers;
			
			// remove all controllers
			while (this.numControllers)
				removeController(getController(this.numControllers - 1));
			
			for (var controllerId : uint = 0; controllerId < numControllers; ++controllerId)
				addController(source.getController(controllerId));
			
			name = source.name;
			_geometry = source._geometry;
			
			if (withBindings)
				_bindings = source._bindings.clone(EXCLUDED_BINDINGS);
			
			transform.copyFrom(source.transform);
			
			effect = source._effect;
			
		}
	}
}