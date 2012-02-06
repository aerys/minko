package aerys.minko.render.shader.compiler.graph.nodes.leaf
{
	import aerys.minko.render.shader.compiler.CRC32;
	import aerys.minko.render.shader.compiler.graph.nodes.INode;
	import aerys.minko.type.stream.format.VertexComponent;
	
	public class Attribute implements INode
	{
		private var _component		: VertexComponent;
		private var _componentId	: uint;
		private var _hash			: uint;
		
		public function get component() : VertexComponent
		{
			return _component;
		}
		
		public function get componentId() : uint
		{
			return _componentId;
		}
		
		public function get hash() : uint
		{
			return _hash;
		}
		
		public function get size() : uint
		{
			return 4;
		}
		
		public function Attribute(component		: VertexComponent,
								  componentId	: uint = 0)
		{
			_component		= component;
			_componentId	= componentId;
			_hash			= CRC32.computeForString('Attribute' + component.fields.join());
		}
		
		public function toString() : String
		{
			return 'Attribute';
		}
		
	}
}