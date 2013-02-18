package aerys.minko.type.xpath
{
	import aerys.minko.scene.node.ISceneNode;

	public interface IPredicate
	{
		function filterProperty(lexer			: XPathLexer,
								propertyName	: String,
								selection		: Vector.<ISceneNode>) : void;
	}
}