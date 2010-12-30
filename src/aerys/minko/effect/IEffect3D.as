package aerys.minko.effect
{
	public interface IEffect3D
	{
		function getTechnique(name : String) : ITechnique3D;
		
		function begin() : void;
		function end() : void;
	}
}