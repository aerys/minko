package aerys.minko.scene
{
	import aerys.minko.ns.minko;
	import aerys.minko.scene.node.IScene;

	import flash.net.getClassByAlias;
	import flash.utils.Proxy;
	import flash.utils.flash_proxy;

	public class SceneIterator extends Proxy
	{
		use namespace minko;

		minko var _items	: Vector.<IScene>	= new Vector.<IScene>();

		public function get length() : int	{ return _items.length; }

		public function SceneIterator()
		{
			super();
		}

		protected function getItemByName(name : String) : IScene
		{
			var numItems : int = _items.length;

			for (var itemIndex : int = 0; itemIndex < numItems; ++itemIndex)
				if (_items[itemIndex].name == name)
					return _items[itemIndex];

			return null;
		}

		protected function addItemAt(item : IScene, index : int) : void
		{
			if (!item)
				throw new Error("Parameter child must be non-null.");

			var numItems : int = _items.length;

			if (index < numItems)
				for (var i : int = numItems; i > index; --i)
					_items[i] = _items[int(i - 1)];
			else
				index = numItems;

			_items[index] = item;
		}

		override flash_proxy function getProperty(name : *) : *
		{
			var index : int = parseInt(name);

			return index == name ? _items[index] : getItemByName(name as String);
		}

		override flash_proxy function setProperty(name : *, value : *) : void
		{
			var index	: int 	= parseInt(name);

			if (index != name)
			{
				var numItems	: int	= _items.length;

				for (var itemIndex : int = 0; itemIndex < numItems; ++itemIndex)
					_items[itemIndex][name] = value;
			}
		}

		override flash_proxy function getDescendants(name : *) : *
		{
			//return getDescendantByName(name);
		}

		override flash_proxy function nextNameIndex(index : int) : int
		{
			return index < _items.length ? index + 1 : 0;
		}

		override flash_proxy function nextName(index : int) : String
		{
			return String(index - 1);
		}

		override flash_proxy function nextValue(index : int) : *
		{
			return _items[int(index - 1)];
		}

		override flash_proxy function callProperty(name : *, ...parameters) : *
		{
			var numItems	: int	= _items.length;

			for (var itemIndex : int = 0; itemIndex < numItems; ++itemIndex)
				_items[itemIndex][name].apply(null, parameters);
		}

		public function filterByType(type : Class) : SceneIterator
		{
			var numItems		: int				= _items.length;
			var result			: SceneIterator		= new SceneIterator();
			var resultItems		: Vector.<IScene>	= result._items;
			var numResultItems	: int				= 0;

			for (var itemIndex : int = 0; itemIndex < numItems; ++itemIndex)
			{
				var item : IScene	= _items[itemIndex];

				if (item is type)
					resultItems[int(numResultItems++)] = item;
			}

			return result;
		}

		public function toString() : String
		{
			var numItems 	: int 		= _items.length;
			var str			: String	= "";

			for (var itemIndex : int = 0; itemIndex < numItems; ++itemIndex)
				str += _items[itemIndex];

			return str;
		}
	}
}