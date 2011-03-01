package aerys.minko.effect
{
	public interface IEffect3DStyle
	{
		function has(name : String) : Boolean;
		function get(name : String, readonly : Boolean = true) : *;
		function set(name : String, value : *) : IEffect3DStyle;
		function clear() : void
		function override(style : IEffect3DStyle = null) : IEffect3DStyle;
		function append(name : String, value : *) : IEffect3DStyle;
	}
}