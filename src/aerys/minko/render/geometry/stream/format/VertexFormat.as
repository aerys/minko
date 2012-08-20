package aerys.minko.render.geometry.stream.format
{
	import flash.utils.Dictionary;

	public final class VertexFormat
	{
		public static const XYZ				: VertexFormat 	= new VertexFormat(
			VertexComponent.XYZ
		);
		public static const XYZ_RGB			: VertexFormat 	= new VertexFormat(
			VertexComponent.XYZ,
			VertexComponent.RGB
		);
		public static const XYZ_UV			: VertexFormat 	= new VertexFormat(
			VertexComponent.XYZ,
			VertexComponent.UV
		);
		public static const XYZ_UV_NORMAL	: VertexFormat 	= new VertexFormat(
			VertexComponent.XYZ,
			VertexComponent.UV,
			VertexComponent.NORMAL
		);
		public static const XYZ_UV_ST		: VertexFormat	= new VertexFormat(
			VertexComponent.XYZ,
			VertexComponent.UV,
			VertexComponent.ST
		);

		private var _bytesPerVertex		: uint;
		private var _components			: Vector.<VertexComponent>;
		private var _componentOffsets	: Dictionary;
		private var _propertyOffsets	: Object;

		public function get numBytesPerVertex() : uint
		{
			return _bytesPerVertex;
		}
		
		public function get numComponents() : uint
		{
			return _components.length;
		}

		public function VertexFormat(...components)
		{
			initialize(components);
		}

		private function initialize(components : Array) : void
		{
			_bytesPerVertex		= 0
			_components			= new <VertexComponent>[];
			_componentOffsets	= new Dictionary();
			_propertyOffsets	= {};

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
			_componentOffsets[component] = _bytesPerVertex;

			var numProperties : uint = component.numProperties;
			for (var propertyId : uint = 0; propertyId < numProperties; ++propertyId)
			{
				var propertyName : String = component.getProperty(propertyId);
				
				_propertyOffsets[propertyName] = _bytesPerVertex + component.getPropertyBytesOffset(propertyName);
			}

			_bytesPerVertex += component.numBytes;
		}

		public function removeComponent(component : VertexComponent) : void
		{
			if (!hasComponent(component))
				throw new Error('No such component');

			var offset		: uint = _componentOffsets[component];
			var numBytes	: uint = component.numBytes;

			_components.splice(_components.indexOf(component), 1);
			delete _componentOffsets[component]

			for (var otherComponent : Object in _componentOffsets)
			{
				var otherOffset : uint = _componentOffsets[otherComponent];
				
				if (otherOffset > offset)
					_componentOffsets[otherComponent] -= numBytes;
			}

			var numProperties : uint = component.numProperties;
			for (var propertyId : uint = 0; propertyId < numProperties; ++propertyId)
				delete _propertyOffsets[component.getProperty(propertyId)];

			for (var otherFieldName : String in _propertyOffsets)
			{
				var otherFieldOffset : uint = _propertyOffsets[otherFieldName];
				
				if (otherFieldOffset > offset)
					_propertyOffsets[otherFieldName] -= numBytes;
			}

			_bytesPerVertex -= numBytes;
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

		public function getBytesOffsetForComponent(component : VertexComponent, index : uint = 0) : uint
		{
			return _componentOffsets[component] + index * _bytesPerVertex;
		}
		
		public function getOffsetForComponent(component : VertexComponent, index : uint = 0) : uint
		{
			return (_componentOffsets[component] >>> 2) + index * (_bytesPerVertex >>> 2);
		}

		public function getBytesOffsetForProperty(property : String) : uint
		{
			return _propertyOffsets[property];
		}
		
		public function getOffsetForProperty(property : String, index : uint = 0) : uint
		{
			return (_propertyOffsets[property] >>> 2) + index * (_bytesPerVertex >>> 2);
		}

		public function clone() : VertexFormat
		{
			var clone	: VertexFormat 	= new VertexFormat();
			var key 	: Object		= null;

			clone._bytesPerVertex	= _bytesPerVertex;
			clone._components		= _components.slice()
			clone._componentOffsets	= new Dictionary();
			clone._propertyOffsets	= {};

			for (key in _componentOffsets)
				clone._componentOffsets[key] = _componentOffsets[key];

			for (key in _propertyOffsets)
				clone._propertyOffsets[key] = _propertyOffsets[key];

			return clone;
		}

		public function equals(vertexFormat : VertexFormat) : Boolean
		{
			var numComponents1	: int	= _components.length;
			var numComponents2	: int	= vertexFormat._components.length;

			for (var i : int = 0; i < numComponents1 && i < numComponents2; ++i)
				if (_components[i] != vertexFormat._components[i])
					return false;

			return i == numComponents1 && i == numComponents2;
		}

		public function toString() : String
		{
			return _components.join('|');
		}
		
		public function getComponent(index : uint) : VertexComponent
		{
			return _components[index];
		}
	}
}