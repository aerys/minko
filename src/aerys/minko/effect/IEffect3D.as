package aerys.minko.effect
{
	public interface IEffect3D
	{
		function getTechnique(name : String) : ITechnique3D;
		
		function set currentTechnique(value : ITechnique3D) : void;
		function get currentTechnique() : ITechnique3D;
		
		function begin() : void;
		function end() : void;
	}
}