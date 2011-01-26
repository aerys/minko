package aerys.minko.effect
{
	public interface IEffect3DStyle
	{
		function get(name : String) : *;
		function set(name : String, value : *) : IEffect3DStyle;
		function override(style : IEffect3DStyle = null) : IEffect3DStyle;
	}
}