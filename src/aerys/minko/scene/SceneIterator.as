package aerys.minko.scene
{
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.type.binding.DataBindings;
	import aerys.minko.type.xpath.SceneIteratorPredicate;
	import aerys.minko.type.xpath.XPathEvaluator;
	
	import flash.utils.Dictionary;
	import flash.utils.Proxy;
	import flash.utils.describeType;
	import flash.utils.flash_proxy;
	import flash.utils.getQualifiedClassName;
	
	public dynamic class SceneIterator extends Proxy
	{
		private var _evaluator		: XPathEvaluator;
		
		public function get length() : uint
		{
			return !_evaluator ? 0 : _evaluator.selection.length;
		}
		
		public function SceneIterator(path 		: String,
									  selection : Vector.<ISceneNode>,
									  modifier	: String	= "")
		{
			super();
			_evaluator = new XPathEvaluator(path, selection, modifier, new SceneIteratorPredicate());
		}
		
		public function toString() : String
		{
			return !_evaluator ? "" : _evaluator.selection.toString();
		}
		
		override flash_proxy function getProperty(name : *) : *
		{
			var index : int = parseInt(name);
			
			if (index == name)
				return index < _evaluator.selection.length ? _evaluator.selection[index] : null;
			else
			{
				throw new Error(
					'Unable to get a property on a set of objects. '
					+ 'You must use the [] operator to fetch one of the objects.'
				);
			}
		}
		
		override flash_proxy function nextNameIndex(index : int) : int
		{
			return index < _evaluator.selection.length ? index + 1 : 0;
		}
		
		override flash_proxy function nextName(index : int) : String
		{
			return String(index - 1);
		}
		
		override flash_proxy function nextValue(index : int) : *
		{
			return _evaluator.selection[int(index - 1)];
		}
	}
}