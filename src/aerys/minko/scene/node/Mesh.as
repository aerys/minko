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
		public static const DEFAULT_MATERIAL	: Material	= new BasicMaterial();
		
		private var _geometry			: Geometry;
		private var _properties			: DataProvider;
		private var _material			: Material;
		private var _bindings			: DataBindings;
		
		private var _visibility			: MeshVisibilityController;
		
		private var _frame				: uint;
		
		private var _cloned				: Signal;
		private var _materialChanged	: Signal;
		private var _frameChanged		: Signal;
		private var _geometryChanged	: Signal;
		
		private var _tag				: uint = 1;
		private var _tagChanged			: Signal;
		
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
				
				var oldMaterial : Material = _material;
				
				_material = value;
				
				if (value)
					_bindings.addProvider(value);
				
				_materialChanged.execute(this, oldMaterial, value);
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
			return _geometry;
		}
		
		public function set geometry(value : Geometry) : void
		{
			if (_geometry != value)
			{
				var oldGeometry	: Geometry = _geometry;
				
				if (oldGeometry)
					oldGeometry.changed.remove(geometryChangedHandler);
				
				_geometry = value;
				
				if (value)
					_geometry.changed.add(geometryChangedHandler);
				
				_geometryChanged.execute(this, oldGeometry, value);
			}
		}
				
		public function get tag():uint
		{
			return _tag;
		}
		
		public function set tag(value:uint):void
		{
			if (_tag != value)
			{
				var oldTag	: uint	= _tag;
				_tag				= value;
				
				_properties.setProperty('tag', value);
				_tagChanged.execute(this, oldTag, value);
			}
		}
		
		public function get tagChanged():Signal
		{
			return _tagChanged;
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
			var visibility : Boolean = super.computedVisibility && _material && _material.effect;
			
            return scene ? visibility && _visibility.computedVisibility : visibility;
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
		
		public function get materialChanged() : Signal
		{
			return _materialChanged;
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
		
		override protected function initializeSignals():void
		{
			super.initializeSignals();
			
			_cloned = new Signal('Mesh.cloned');
			_materialChanged = new Signal('Mesh.materialChanged');
			_frameChanged = new Signal('Mesh.frameChanged');
			_geometryChanged = new Signal('Mesh.geometryChanged');
			_tagChanged = new Signal('Mesh.tagChanged');
		}
		
		override protected function initializeContollers():void
		{
			super.initializeContollers();
			
			addController(new MeshController());
			
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
			this.material	= material || DEFAULT_MATERIAL;
			_tag			= tag;
		}
		
		public function cast(ray : Ray, maxDistance : Number = Number.POSITIVE_INFINITY, tag : uint = 1) : Number
		{
			if (!(_tag & tag))
			{
				trace(tag);
				return -1;
			}
			
			return _geometry.boundingBox.testRay(
				ray,
				getWorldToLocalTransform(),
				maxDistance
			);
		}
        
		override minko_scene function cloneNode() : AbstractSceneNode
		{
			var clone : Mesh = new Mesh();
			
			clone.name 		= name;
			clone.geometry 	= _geometry;
			
			clone.properties = DataProvider(_properties.clone());
			clone._bindings.copySharedProvidersFrom(_bindings);
			
			clone.transform.copyFrom(transform);
			clone.material = _material;
			
			this.cloned.execute(this, clone);
			
			return clone;
		}
		
		private function geometryChangedHandler(geometry : Geometry) : void
		{
			_geometryChanged.execute(this, _geometry, _geometry);
		}
	}
}
