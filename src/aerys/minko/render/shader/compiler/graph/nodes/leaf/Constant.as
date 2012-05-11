package aerys.minko.render.shader.compiler.graph.nodes.leaf
{
	import aerys.minko.render.shader.compiler.CRC32;
	import aerys.minko.render.shader.compiler.Serializer;
	import aerys.minko.render.shader.compiler.graph.nodes.AbstractNode;


	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public class Constant extends AbstractNode
	{
		private var _value : Vector.<Number>
		
		public function get value() : Vector.<Number>
		{
			return _value;
		}
		
		public function set value(v : Vector.<Number>) : void
		{
			_value = v;
		}
		
		public function Constant(value : Vector.<Number>)
		{
			super(new <AbstractNode>[], new <uint>[]);
			
			_value = value;
		}
		
		override protected function computeSize() : uint
		{
			return _value.length;
		}
		
		override protected function computeHash() : uint
		{
			return CRC32.computeForNumberVector(_value);
		}
		
		override public function toString() : String
		{
			return 'Constant';
		}
		
		override public function clone() : AbstractNode
		{
			return new Constant(_value.slice());
		}

	}
}
