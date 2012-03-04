package aerys.minko.scene.node.mesh
{
	import aerys.minko.ns.minko_stream;
	import aerys.minko.render.effect.Effect;
	import aerys.minko.render.effect.basic.BasicShader;
	import aerys.minko.scene.controller.RenderingController;
	import aerys.minko.scene.node.AbstractSceneNode;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.scene.node.mesh.geometry.Geometry;
	import aerys.minko.type.Signal;
	import aerys.minko.type.bounding.BoundingBox;
	import aerys.minko.type.bounding.BoundingSphere;
	import aerys.minko.type.data.DataBindings;

	public class Mesh extends AbstractSceneNode
	{
		use namespace minko_stream;
		
		public static const DEFAULT_EFFECT	: Effect	= new Effect(
			new BasicShader()
		);
		
		private var _geometry			: Geometry			= null;
		private var _effect				: Effect			= null;
		private var _bindings			: DataBindings		= new DataBindings();
		
		private var _boundingSphere		: BoundingSphere	= null;
		private var _boundingBox		: BoundingBox		= null;
		private var _visible			: Boolean			= true;
		
		private var _effectChanged		: Signal			= new Signal();
		private var _visibilityChanged	: Signal			= new Signal();
		
		public function get bindings() : DataBindings
		{
			return _bindings;
		}
		
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
		
		public function get visibilityChanged() : Signal
		{
			return _visibilityChanged;
		}
		
		public function get effectChanged() : Signal
		{
			return _effectChanged;
		}
		
		public function Mesh(geometry		: Geometry	= null,
							 properties		: Object	= null,
							 effect			: Effect	= null)
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
			
			var ctrl : RenderingController	= new RenderingController();
			
//			ctrl.drawCallCreated.add(drawCallCreatedHandler);
			
			addController(ctrl);
		}
		
		public function clone(properties	: Object	= null,
							  withBindings 	: Boolean 	= true) : Mesh
		{
			var clone : Mesh = new Mesh();
			
			clone.copyFrom(this, withBindings);
			clone.bindings.setProperties(properties);
			
			return clone;
		}
		
		override protected function addedToSceneHandler(child : ISceneNode, scene : Scene) : void
		{
			super.addedToSceneHandler(child, scene);
	
			if (child == this)
				_bindings.addProperty("local to world", parent.localToWorld);
		}
		
		override protected function removedFromSceneHandler(child : ISceneNode, scene : Scene) : void
		{
			super.removedFromSceneHandler(child, scene);
			
			if (child == this)
				_bindings.removeProperty("local to world");
		}
		
		protected function copyFrom(source 			: Mesh,
									withBindings 	: Boolean) : void
		{
			name = source.name;
			_geometry = source._geometry;
			
			if (withBindings)
				_bindings = source._bindings.clone();
			
			effect = source._effect;
		}
		
		
	}
}