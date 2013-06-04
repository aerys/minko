package aerys.minko.scene.node
{
	import aerys.minko.ns.minko_scene;
	import aerys.minko.render.geometry.Geometry;
	import aerys.minko.render.material.Material;
	import aerys.minko.render.material.basic.BasicMaterial;
	import aerys.minko.scene.controller.mesh.MeshController;
	import aerys.minko.scene.controller.mesh.MeshVisibilityController;
	import aerys.minko.type.Signal;
	import aerys.minko.type.binding.DataBindings;
	import aerys.minko.type.binding.DataProvider;
	import aerys.minko.type.enum.DataProviderUsage;
	import aerys.minko.type.enum.FrustumCulling;
	import aerys.minko.type.math.Ray;

	use namespace minko_scene;
	
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
	public class Mesh extends AbstractVisibleSceneNode implements ITaggable
	{
		private var _ctrl				: MeshController;
		private var _properties			: DataProvider;
		private var _material			: Material;
		private var _bindings			: DataBindings;
		
		private var _visibility			: MeshVisibilityController;
		
		private var _cloned				: Signal;
		
		private var _tag				: uint = 1;
		
		/**
		 * A DataProvider object already bound to the Mesh bindings.
		 * 
		 * <pre>
		 * // set the "diffuseColor" property to 0x0000ffff
		 * mesh.properties.diffuseColor = 0x0000ffff;
		 * 
		 * // animate the "diffuseColor" property
		 * mesh.addController(
		 *   new AnimationController(
		 * 	  new &lt;ITimeline&gt;[new ColorTimeline(
		 * 	    "dataProvider.diffuseColor",
		 * 	    5000,
		 * 	    new &lt;uint&gt;[0xffffffff, 0xffffff00, 0xffffffff]
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
		
		public function get material() : Material
		{
			return _material;
		}

		public function set material(value : Material) : void
		{
			if (_material != value)
			{
				if (_material)
					_bindings.removeProvider(_material);

				_material = value;
				
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
		 * The Geometry of the mesh. 
		 * @return 
		 * 
		 */
		public function get geometry() : Geometry
		{
			return _ctrl.geometry;
		}
		
		public function set geometry(value : Geometry) : void
		{
			_ctrl.geometry = value;
		}
				
		public function get tag() : uint
		{
			return _tag;
		}
		
		public function set tag(value : uint) : void
		{
			if (_tag != value)
			{
				_tag = value;
				_properties.setProperty('tag', value);
			}
		}

        /**
         * Whether the mesh in inside the camera frustum or not. 
         * @return 
         * 
         */
        public function get insideFrustum() : Boolean
        {
            return _visibility.insideFrustum;
        }
        
        override public function get computedVisibility() : Boolean
        {
            return scene
				? super.computedVisibility && _visibility.computedVisibility
				: super.computedVisibility;
        }
        
		public function get frustumCulling() : uint
		{
			return _visibility.frustumCulling;
		}

		public function set frustumCulling(value : uint) : void
		{
			_visibility.frustumCulling = value;
		}
		
		public function get cloned() : Signal
		{
			return _cloned;
		}
		
		public function get frame() : uint
		{
			return _ctrl.frame;
		}
		public function set frame(value : uint) : void
		{
			_ctrl.frame = value;
		}
		
		public function Mesh(geometry	: Geometry	= null,
							 material	: Material	= null,
							 name		: String	= null,
							 tag		: uint		= 1)
		{
			super();

			initializeMesh(geometry, material, name, tag);
		}
		
		override protected function initialize() : void
		{
			_bindings = new DataBindings(this);
			this.properties = new DataProvider(
				properties,
				'meshProperties',
				DataProviderUsage.EXCLUSIVE
			);
			
			super.initialize();
		}
		
		override protected function initializeSignals() : void
		{
			super.initializeSignals();
			
			_cloned = new Signal('Mesh.cloned');
			added.add(addedToSceneHandler);
		}

		private function addedToSceneHandler(child : ISceneNode, ancestor : Group) : void
		{
			added.remove(addedToSceneHandler);
			removed.add(removedFromSceneHandler);

			if (_material && !_bindings.hasProvider(_material))
				_bindings.addProvider(_material);

			if (!_bindings.hasProvider(_properties))
				_bindings.addProvider(_properties);
		}

		private function removedFromSceneHandler(child : ISceneNode, ancestor : Group) : void
		{
			removed.remove(removedFromSceneHandler);
			added.add(addedToSceneHandler);

			if (_material && _bindings.hasProvider(_material))
				_bindings.removeProvider(_material);

			_bindings.removeProvider(_properties);
		}

		override protected function initializeContollers() : void
		{
			super.initializeContollers();

			_ctrl = new MeshController();
			addController(_ctrl);
			
			_visibility = new MeshVisibilityController();
			_visibility.frustumCulling = FrustumCulling.ENABLED;
			addController(_visibility);
		}
		
		protected function initializeMesh(geometry	: Geometry,
										  material	: Material,
										  name		: String,
										  tag		: uint) : void
		{
			if (name)
				this.name = name;
			
			this.geometry	= geometry;
			this.material	= material;
			_tag			= tag;
		}
		
		public function cast(ray            : Ray,
                             maxDistance    : Number    = Number.POSITIVE_INFINITY,
                             tag            : uint      = 1) : Number
		{
			if (!(_tag & tag))
				return -1;
			
			if(_ctrl.geometry.boundingBox == null) {
				return -1;
			}
			
			return _ctrl.geometry.boundingBox.testRay(
				ray,
				getWorldToLocalTransform(),
				maxDistance
			);
		}

		override minko_scene function cloneNode() : AbstractSceneNode
		{
			var clone : Mesh = new Mesh();
			
			clone.name 		= name;
			clone.geometry 	= _ctrl.geometry;
			
			clone.properties = DataProvider(_properties.clone());
			clone._bindings.copySharedProvidersFrom(_bindings);
			
			clone.transform.copyFrom(transform);
			clone.material = _material;
			
			this.cloned.execute(this, clone);
			
			return clone;
		}
	}
}
