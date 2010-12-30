package aerys.minko.effect
{
	public interface ITechnique3D
	{
		function get name() : String;
		function get passes() : Vector.<IPass3D>;
	}
}