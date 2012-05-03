package aerys.minko.render.shader.compiler.graph.nodes.vertex
{
	import aerys.minko.render.shader.compiler.CRC32;
	import aerys.minko.render.shader.compiler.graph.nodes.ANode;
	import aerys.minko.render.shader.compiler.register.Components;
	
	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public class VariadicExtract extends ANode
	{
		private static const INDEX		: uint = 0;
		private static const CONSTANT	: uint = 1;
		
		
		private var _isMatrix : Boolean;
		
		public function get index() : ANode
		{
			return getArgumentAt(INDEX);
		}
		
		public function get constant() : ANode
		{
			return getArgumentAt(CONSTANT);
		}
		
		public function get isMatrix() : Boolean
		{
			return _isMatrix;
		}
		
		public function get indexComponentSelect() : uint
		{
			return getComponentAt(INDEX) & 0xff;
		}
		
		public function set indexComponentSelect(v : uint) : void
		{
			setComponentAt(INDEX, Components.createFromParts(v));
		}
		
		public function set index(v : ANode) : void
		{
			setArgumentAt(INDEX, v);
		}
		
		public function set constant(v : ANode) : void
		{
			setArgumentAt(CONSTANT, v);
		}
		
		public function VariadicExtract(index		: ANode, 
										constant	: ANode,
										isMatrix	: Boolean)
		{
			_isMatrix				= isMatrix;
			
			var arguments : Vector.<ANode>	= new <ANode>[index, constant];
			var components: Vector.<uint>	= new <uint>[
				Components.createFromParts(0), 
				Components.createFromParts(0, 1, 2, 3)
			];
			
			arguments.fixed = components.fixed = true;
			
			super(arguments, components);
		}
		
		override protected function computeSize() : uint
		{
			return _isMatrix ? 16 : 4;
		}
		
		override protected function computeHash() : uint
		{
			return CRC32.computeForString(
				'VariadicExtract'
				+ index.hash.toString(16)
				+ constant.hash.toString(16)
				+ indexComponentSelect.toString()
				+ isMatrix.toString())
		}
		
		override public function toString() : String
		{
			return 'VariadicExtract';
		}
	}
}