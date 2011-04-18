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
		
		protected var _components : Object;
		
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
			_components[component.implodedFields] = component;
			_componentOffsets[component.implodedFields] = _dwordsPerVertex;
			
			for (var fieldName : String in component.offsets)
			{
				_fieldOffsets[fieldName] = _dwordsPerVertex + component.offsets[fieldName];
			}
			
			_dwordsPerVertex += component.dwords;
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
			for (var implodedFields:String in otherVertexFormat._components)
				_components[implodedFields] = otherVertexFormat._components[implodedFields];
		}
		
		public function getOffsetForComponent(component : VertexComponent) : int
		{
			return _componentOffsets[component.implodedFields];
		}
		
		public function getOffsetForField(fieldName:String) : int
		{ 
			return _fieldOffsets[fieldName];
		}
	}
}