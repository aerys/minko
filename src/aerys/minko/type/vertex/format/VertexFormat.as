package aerys.minko.type.vertex.format
{
	public class VertexFormat
	{
		public static const XYZ			: VertexFormat 	= new VertexFormat(VertexComponent.XYZ);
		public static const XYZ_RGB		: VertexFormat 	= new VertexFormat(VertexComponent.XYZ,
			VertexComponent.RGB);
		public static const XYZ_UV		: VertexFormat 	= new VertexFormat(VertexComponent.XYZ,
			VertexComponent.UV);
		public static const XYZ_UV_ST	: VertexFormat	= new VertexFormat(VertexComponent.XYZ,
			VertexComponent.UV,
			VertexComponent.ST);
		
		protected var _components		: Object;
		
		private var _dwordsPerVertex	: int		= 0;
		private var _componentOffsets	: Object	= new Object();
		private var _fieldOffsets		: Object	= new Object();
		
		public function get components() 		: Object 	{ return _components; }
		public function get dwordsPerVertex()	: int		{ return _dwordsPerVertex; }
		
		public function VertexFormat(...components)
		{
			_components = new Object();
			
			if (components)
				initialize(components);
		}
		
		private function initialize(components : Array) : void
		{
			for each (var component : VertexComponent in components)
			addComponent(component);
		}
		
		public function addComponent(component : VertexComponent) : void
		{
			if (hasComponent(component))
				throw new Error('Component already present in this vertex format');
			
			_components[component.implodedFields] = component;
			_componentOffsets[component.implodedFields] = _dwordsPerVertex;
			
			for (var fieldName : String in component.offsets)
				_fieldOffsets[fieldName] = _dwordsPerVertex + component.offsets[fieldName];
			
			_dwordsPerVertex += component.dwords;
		}
		
		public function removeComponent(component : VertexComponent) : void
		{
			if (!hasComponent(component))
				throw new Error('No such component');
			
			var offset	: uint = _componentOffsets[component.implodedFields];
			var dwords	: uint = component.dwords;
			
			_components[component.implodedFields]		= undefined;
			_componentOffsets[component.implodedFields]	= undefined;
			
			for (var componentName : String in _componentOffsets)
			{
				var otherOffset : uint = _componentOffsets[componentName];
				if (otherOffset > offset)
					_componentOffsets[componentName] -= dwords;
			}
			
			for (var fieldName : String in component.offsets)
			{
				_fieldOffsets[fieldName] = undefined;
			}
			
			for (var otherFieldName : String in _fieldOffsets)
			{
				var otherFieldOffset : uint = _fieldOffsets[otherFieldName];
				if (otherFieldOffset > offset)
					_fieldOffsets[otherFieldName] -= dwords;
			}
			
			_dwordsPerVertex -= dwords;
		}
		
		/**
		 * Determine if this Vertex3DFormat has the specified component
		 */
		public function hasComponent(component : VertexComponent) : Boolean
		{
			return _components.hasOwnProperty(component.implodedFields);
		}
		
		/**
		 * Determine if this Vertex3DFormat is a subset of the Vertex3DFormat passed in attribute
		 */ 
		public function isSubsetOf(otherVertexFormat : VertexFormat) : Boolean
		{
			for (var implodedFields : String in _components)
				if (!otherVertexFormat._components.hasOwnProperty(implodedFields))
					return false;
			
			return true;
		}
		
		/**
		 * Add the components from the vertex format passed in attribute that we don't have in this one.
		 */
		public function unionWith(otherVertexFormat : VertexFormat) : void
		{
			for each (var component : VertexComponent in otherVertexFormat._components)
			addComponent(component);
		}
		
		public function intersectWith(otherVertexFormat : VertexFormat) : void
		{
			for each (var component : VertexComponent in _components)
			if (!otherVertexFormat.hasComponent(component))
				removeComponent(component);
		}
		
		public function getOffsetForComponent(component : VertexComponent) : int
		{
			return _componentOffsets[component.implodedFields];
		}
		
		public function getOffsetForField(fieldName : String) : int
		{ 
			return _fieldOffsets[fieldName];
		}
		
		public function clone() : VertexFormat
		{
			var clone : VertexFormat = new VertexFormat();
			clone._dwordsPerVertex = _dwordsPerVertex;
			
			var key : String;
			
			clone._componentOffsets = new Object();
			for (key in _componentOffsets) 
				clone._componentOffsets[key] = _componentOffsets[key];
			
			clone._components = new Object();
			for (key in _components) 
				clone._components[key] = _components[key];
			
			clone._fieldOffsets = new Object();
			for (key in _fieldOffsets) 
				clone._fieldOffsets[key] = _fieldOffsets[key];
			
			return clone;
		}
	}
}