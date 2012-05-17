package aerys.minko.type.stream.format
{
	import flash.utils.Dictionary;

	public final class VertexFormat
	{
		public static const XYZ			: VertexFormat 	= new VertexFormat(VertexComponent.XYZ);
		public static const XYZ_RGB		: VertexFormat 	= new VertexFormat(VertexComponent.XYZ,
																		   VertexComponent.RGB);
		public static const XYZ_UV		: VertexFormat 	= new VertexFormat(VertexComponent.XYZ,
																		   VertexComponent.UV);
		public static const XYZ_UV_ST	: VertexFormat	= new VertexFormat(VertexComponent.XYZ,
																		   VertexComponent.UV,
																		   VertexComponent.ST);

		private var _dwordsPerVertex	: int;
		private var _components			: Vector.<VertexComponent>;
		private var _componentOffsets	: Dictionary;
		private var _fieldOffsets		: Object;

		public function get components() : Vector.<VertexComponent>
		{
			return _components;
		}
		public function get dwordsPerVertex() : int
		{
			return _dwordsPerVertex;
		}

		public function VertexFormat(...components)
		{
			_dwordsPerVertex	= 0
			_components			= new Vector.<VertexComponent>();
			_componentOffsets	= new Dictionary();
			_fieldOffsets		= new Object()

			if (components)
				initialize(components);
		}

		private function initialize(components : Array) : void
		{
			for each (var component : VertexComponent in components)
				addComponent(component);
		}

		public function addComponent(component : VertexComponent, force : Boolean = false) : void
		{
			if (hasComponent(component))
			{
				if (force)
					return;

				throw new Error('Component already present in this vertex format: ' + component.toString());
			}

			_components.push(component);
			_componentOffsets[component] = _dwordsPerVertex;

			for (var fieldName : String in component.offsets)
				_fieldOffsets[fieldName] = _dwordsPerVertex + component.offsets[fieldName];

			_dwordsPerVertex += component.dwords;
		}

		public function removeComponent(component : VertexComponent) : void
		{
			if (!hasComponent(component))
				throw new Error('No such component');

			var offset	: uint = _componentOffsets[component];
			var dwords	: uint = component.dwords;

			_components.splice(_components.indexOf(component), 1);
			delete _componentOffsets[component]

			for (var otherComponent : Object in _componentOffsets)
			{
				var otherOffset : uint = _componentOffsets[otherComponent];
				if (otherOffset > offset)
					_componentOffsets[otherComponent] -= dwords;
			}

			for (var fieldName : String in component.offsets)
			{
				delete _fieldOffsets[fieldName];
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
		 * Determine if this VertexFormat has the specified component
		 */
		public function hasComponent(component : VertexComponent) : Boolean
		{
			return _components.indexOf(component) != -1;
		}

		/**
		 * Determine if this VertexFormat is a subset of the VertexFormat passed in attribute
		 */
		public function isSubsetOf(otherVertexFormat : VertexFormat) : Boolean
		{
			for each (var component : VertexComponent in _components)
				if (otherVertexFormat._components.indexOf(component) == -1)
					return false;

			return true;
		}

		/**
		 * Add the components from the vertex format passed in attribute that we don't have in this one.
		 */
		public function unionWith(otherVertexFormat : VertexFormat, force : Boolean = false) : void
		{
			for each (var component : VertexComponent in otherVertexFormat._components)
				addComponent(component, force);
		}

		public function intersectWith(otherVertexFormat : VertexFormat) : void
		{
			for each (var component : VertexComponent in _components)
				if (!otherVertexFormat.hasComponent(component))
					removeComponent(component);
		}

		public function getOffsetForComponent(component : VertexComponent) : int
		{
			return _componentOffsets[component];
		}

		public function getOffsetForField(fieldName : String) : int
		{
			return _fieldOffsets[fieldName];
		}

		public function clone() : VertexFormat
		{
			var clone	: VertexFormat 	= new VertexFormat();
			var key 	: Object		= null;

			clone._dwordsPerVertex	= _dwordsPerVertex;
			clone._components		= _components.slice()
			clone._componentOffsets	= new Dictionary();
			clone._fieldOffsets		= new Object();

			for (key in _componentOffsets)
				clone._componentOffsets[key] = _componentOffsets[key];

			for (key in _fieldOffsets)
				clone._fieldOffsets[key] = _fieldOffsets[key];

			return clone;
		}

		public function equals(vertexFormat : VertexFormat) : Boolean
		{
			var numComponents1	: int	= _components.length;
			var numComponents2	: int	= vertexFormat._components.length;

			for (var i : int = 0; i < numComponents1 && i < numComponents2; ++i)
				if (_components[i] != vertexFormat.components[i])
					return false;

			return i == numComponents1 && i == numComponents2;
		}

		public function toString() : String
		{
			return _components.join('|');
		}
	}
}