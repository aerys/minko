package aerys.minko.render.shader.node.leaf
{
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.IVertexNode;
	import aerys.minko.render.shader.node.operation.manipulation.Interpolate;
	import aerys.minko.type.stream.format.VertexComponent;

	public class Attribute extends AbstractLeaf implements IVertexNode
	{
		protected var _component	: VertexComponent;
		protected var _componentId	: uint;
		
		public function get interpolated()	: INode				{ return new Interpolate(this); }
		public function get component()		: VertexComponent	{ return _component; }
		public function get componentIndex()	: uint				{ return _componentId; }
		
		override public function get size()	: uint
		{
			return 4; // should be _component.dwords; 
		}
		
		public function Attribute(component		: VertexComponent, 
								  componentId	: uint = 0)
		{
			_component		= component;
			_componentId	= componentId;
		}
		
		override public function isSame(node : INode) : Boolean
		{
			var attrNode : Attribute = node as Attribute;
			return attrNode != null && _component == attrNode._component && _componentId == attrNode._componentId;
		}
		
		override public function toString() : String
		{
			return "Attribute\\n" + _component.toString() + ' - ' + _componentId.toString();
		}
	}
}
