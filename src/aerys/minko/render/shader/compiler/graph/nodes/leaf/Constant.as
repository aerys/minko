package aerys.minko.render.shader.compiler.graph.nodes.leaf
{
	import aerys.minko.render.shader.compiler.CRC32;
	import aerys.minko.render.shader.compiler.Serializer;
	import aerys.minko.render.shader.compiler.graph.nodes.ANode;
	import aerys.minko.render.shader.compiler.graph.nodes.ANode;

	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public class Constant extends ANode
	{
		private var _value	: Vector.<Number>
		
		public function get value() : Vector.<Number>
		{
			return _value;
		}
		
		public function Constant(value : Vector.<Number>)
		{
			super(new <ANode>[], new <uint>[]);
			
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
	}
}
