package aerys.minko.render.shader.binding
{
	import aerys.minko.render.resource.texture.ITextureResource;
	
	import flash.utils.Dictionary;

	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public interface IBinder
	{
		/**
		 * Name of the parameter binding.
		 * Must be an unique identifier of the parameter.
		 */		
		function get bindingName() : String;
		
		/**
		 * Set the monitored shader constant or texture.
		 * 
		 * @param vsConstData Vertex shader constant data.
		 * @param fsConstData Fragment shader constant data.
		 * @param textures Fragment shader textures samplers.
		 * @param value The value to be set.
		 */		
		function set(cpuConstants	: Dictionary,
					 vsConstants 	: Vector.<Number>,
					 fsConstants 	: Vector.<Number>,
					 fsTextures		: Vector.<ITextureResource>,
					 value			: Object) : void;
	}
}
