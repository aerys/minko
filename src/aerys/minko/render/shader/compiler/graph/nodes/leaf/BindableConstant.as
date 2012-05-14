package aerys.minko.render.shader.compiler.graph.nodes.leaf
{
	import aerys.minko.render.shader.compiler.CRC32;
	import aerys.minko.render.shader.compiler.graph.nodes.AbstractNode;

	
	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public class BindableConstant extends AbstractNode
	{
		public static const COMPUTABLE_CONSTANT_PREFIX : String = 'computableConstant';
		
		private var _bindingName	: String;
		private var _size			: uint;
		
		public function get bindingName() : String
		{
			return _bindingName;
		}
		
		public function BindableConstant(bindingName : String, size : uint)
		{
			super(new <AbstractNode>[], new <uint>[]);
			
			if (size == 0)
				throw new Error('Cannot create a zero-sized parameter');
			
			if (size > 4 && size % 4 != 0)
				throw new Error('Size of a parameter must be either less or equal than 4, or a multiple of 4');
			
			_size			= size;
			_bindingName	= bindingName;
		}
		
		override protected function computeHash() : uint
		{
			return CRC32.computeForString('BindableConstant_' + _bindingName + '_' + _size);	
		}
		
		override protected function computeSize() : uint
		{
			return _size;
		}
		
		override public function toString() : String
		{
			return 'BindableConstant_' + bindingName + '_' + size;
		}
		
		override public function clone() : AbstractNode
		{
			return new BindableConstant(_bindingName, _size);
		}

	}
}
