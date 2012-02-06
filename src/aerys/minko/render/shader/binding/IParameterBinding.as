package aerys.minko.render.shader.binding
{
	import aerys.minko.render.resource.texture.ITextureResource;

	public interface IParameterBinding
	{
		/**
		 * Name of the parameter binding.
		 * Must be an unique identifier of the parameter.
		 */		
		function get name() : String;
		
		/**
		 * Updates the monitored shader constant or texture by reading from the provided data (style, transform, world).
		 * 
		 * @param vsConstData Vertex shader constant data.
		 * @param fsConstData Fragment shader constant data.
		 * @param textures Fragment shader textures samplers.
		 * @param styleData Style information for the current mesh.
		 * @param transformData Transform information for the current mesh.
		 * @param worldData World information for the current scene.
		 */		
		function update(vsConstData 	: Vector.<Number>,
						fsConstData 	: Vector.<Number>,
						textures		: Vector.<ITextureResource>) : void;
		
		/**
		 * Manually set the monitored shader constant or texture.
		 * 
		 * @param vsConstData Vertex shader constant data.
		 * @param fsConstData Fragment shader constant data.
		 * @param textures Fragment shader textures samplers.
		 * @param value The value to be set.
		 */		
		function set(vsConstData 	: Vector.<Number>,
					 fsConstData 	: Vector.<Number>,
					 textures		: Vector.<ITextureResource>,
					 value			: Object) : void;
	}
}
