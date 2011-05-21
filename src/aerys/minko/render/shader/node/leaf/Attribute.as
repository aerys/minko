package aerys.minko.render.shader.node.leaf
{
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.IVertexNode;
	import aerys.minko.render.shader.node.operation.manipulation.Interpolate;
	import aerys.minko.type.vertex.format.VertexComponent;

	public class Attribute extends AbstractLeaf implements IVertexNode
	{
		protected var _component : VertexComponent;
		
		public function get interpolated() : INode
		{
			return new Interpolate(this);
		}
		
		public function get vertexComponent() : VertexComponent
		{
			return _component;
		}
		
		override public function get size() : uint 
		{
			return 4;//_component.dwords;
		}
		
		public function Attribute(component : VertexComponent)
		{
			_component = component;
		}
		
		override public function isSame(node : INode) : Boolean
		{
			var attrNode : Attribute = node as Attribute;
			return attrNode != null && _component.isSameAs(attrNode._component);
		}
		
		override public function toString() : String
		{
			return "Attribute\\n" + _component.implodedFields;
		}
	}
}
