package aerys.minko.render.visitor
{
	public interface IScene3DVisitorModifier extends IScene3DVisitor
	{
		function get target() : IScene3DVisitor;
		
		function set target(value : IScene3DVisitor) : void;
	}
}