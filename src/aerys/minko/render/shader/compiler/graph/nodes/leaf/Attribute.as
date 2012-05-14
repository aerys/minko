package aerys.minko.render.shader.compiler.graph.nodes.leaf
{
	import aerys.minko.render.shader.compiler.CRC32;
	import aerys.minko.render.shader.compiler.graph.nodes.AbstractNode;
	import aerys.minko.render.shader.compiler.graph.nodes.AbstractNode;
	import aerys.minko.type.stream.format.VertexComponent;
	
	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public class Attribute extends AbstractNode
	{
		private var _component		: VertexComponent;
		private var _componentId	: uint;
		
		public function get component() : VertexComponent
		{
			return _component;
		}
		
		public function get componentId() : uint
		{
			return _componentId;
		}
		
		public function Attribute(component		: VertexComponent,
								  componentId	: uint = 0)
		{
			super(new <AbstractNode>[], new <uint>[]);
			
			_component		= component;
			_componentId	= componentId;
		}
		
		override protected function computeHash() : uint
		{
			return CRC32.computeForString('Attribute' + _component.fields.join());
		}
		
		override protected function computeSize() : uint
		{
			return 4;
		}
		
		override public function toString() : String
		{
			return 'Attribute ' + _component.fields + '(' + _componentId + ')';
		}
		
		override public function clone() : AbstractNode
		{
			return new Attribute(_component, _componentId);
		}
		
	}
}