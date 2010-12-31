package aerys.minko.effect
{
	public interface IEffect3DTechnique
	{
		function get name() : String;
		function get passes() : Vector.<IEffect3DPass>;
	}
}