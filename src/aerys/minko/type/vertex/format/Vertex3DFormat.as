package aerys.minko.type.vertex.format
{
	public class Vertex3DFormat
	{
		protected var _components : Object;
		
		public function get components():Object { return _components; }
		
		public function Vertex3DFormat(components : Array = null)
		{
			_components = new Object();
			
			if (components)
				for each (var component:Vertex3DComponent in components)
					_components[component.implodedFields] = component;
		}
		
		/**
		 * Determine if this Vertex3DFormat has the specified component
		 */
		public function hasComponent(component:Vertex3DComponent):Boolean {
			return _components.hasOwnProperty(component.implodedFields);
		}
		
		/**
		 * Determine if this Vertex3DFormat is a subset of the Vertex3DFormat passed in attribute
		 */ 
		public function isSubsetOf(otherVertexFormat:Vertex3DFormat):Boolean
		{
			for (var implodedFields:String in _components)
				if (!otherVertexFormat._components.hasOwnProperty(implodedFields))
					return false;
			return true;
		}
		
		/**
		 * Add the components from the vertex format passed in attribute that we don't have in this one.
		 */
		public function unionWith(otherVertexFormat:Vertex3DFormat):void {
			for (var implodedFields:String in otherVertexFormat._components)
				_components[implodedFields] = otherVertexFormat._components[implodedFields];
		}
	}
}