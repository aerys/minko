package aerys.minko.render.shader.compiler.graph.nodes.vertex
{
	import aerys.minko.render.shader.compiler.CRC32;
	import aerys.minko.render.shader.compiler.graph.nodes.AbstractNode;
	import aerys.minko.render.shader.compiler.register.Components;
	
	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public class VariadicExtract extends AbstractNode
	{
		private var _isMatrix : Boolean;
		
		public function get index() : AbstractNode
		{
			return getArgumentAt(0);
		}
		
		public function get constant() : AbstractNode
		{
			return getArgumentAt(1);
		}
		
		public function get isMatrix() : Boolean
		{
			return _isMatrix;
		}
		
		public function get indexComponentSelect() : uint
		{
			return getComponentAt(0) & 0xff;
		}
		
		public function set indexComponentSelect(v : uint) : void
		{
			setComponentAt(0, Components.createFromParts(v));
		}
		
		public function set index(v : AbstractNode) : void
		{
			setArgumentAt(0, v);
		}
		
		public function set constant(v : AbstractNode) : void
		{
			setArgumentAt(1, v);
		}
		
		public function VariadicExtract(index		: AbstractNode, 
										constant	: AbstractNode,
										isMatrix	: Boolean)
		{
			_isMatrix = isMatrix;
			
			var arguments : Vector.<AbstractNode>	= new <AbstractNode>[index, constant];
			var components: Vector.<uint>			= new <uint>[
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
		
		override public function clone() : AbstractNode
		{
			return new VariadicExtract(index, constant, _isMatrix);
		}
	}
}