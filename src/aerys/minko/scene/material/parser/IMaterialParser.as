package aerys.minko.scene.material.parser
{
	import aerys.minko.scene.material.IMaterial3D;
	
	import flash.utils.ByteArray;
	
	/**
	 * Define the common interface for a material parser.
	 * 
	 * @author Promethe
	 * 
	 */
	public interface IMaterialParser
	{
		/**
		 * 
		 * @return The IMaterialDecorator object resulting from the parsing.
		 * 
		 */
		function get material() : IMaterial3D;
		
		function parse(myData : ByteArray) : void;
	}
}