package aerys.minko.effect
{
	public interface IEffect3DStyle
	{
		function get(name : String, defaultValue : * = null) : *;
		function set(name : String, value : *) : IEffect3DStyle;
		function clear() : void
		function override(style : IEffect3DStyle = null) : IEffect3DStyle;
		function append(name : String, value : *) : IEffect3DStyle;
	}
}