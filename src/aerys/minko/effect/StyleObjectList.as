package aerys.minko.effect
{
	import aerys.common.Factory;

	public class StyleObjectList
	{
		protected var _styleName	: String;
		protected var _objects		: Vector.<IStyleObject>;
		protected var _dirty		: Boolean;
		protected var _hash			: String;
		
		public function get length() : uint
		{
			return _objects.length;
		}
		
		public function set length(v : uint) : void
		{
			if (v != _objects.length)
			{
				_objects.length = v;
				_dirty = true;
			}
		}
		
		public function get hash() : String
		{
			if (_dirty)
			{
				_hash = _styleName + "_";
				for each (var object : IStyleObject in _objects)
					_hash += object.hash;
			}
			return _hash;
		}
		
		public function getItem(v : uint) : IStyleObject
		{
			return _objects[v];
		}
		
		public function setItem(i : uint, value : IStyleObject) : void
		{
			_objects[i] = value;
		}
		
		public function StyleObjectList(styleName : String = null)
		{
			if (styleName != null)
			{
				_objects	= new Vector.<IStyleObject>();
				_dirty		= true;
				_styleName	= styleName;
			}
		}
		
		public function push(v : IStyleObject) : void
		{
			_objects.push(v);
			_dirty = true;
		}
		
		public function pop() : IStyleObject
		{
			return _objects.pop();
			_dirty = true;
		}
		
		public function clone() : StyleObjectList
		{
			var newStyleObjList : StyleObjectList = Factory.getFactory(StyleObjectList).create(true);
			newStyleObjList._styleName	= _styleName;
			newStyleObjList._objects	= _objects.concat();
			newStyleObjList._dirty		= _dirty;
			newStyleObjList._hash		= _hash;
			return newStyleObjList;
		}
		
	}
}