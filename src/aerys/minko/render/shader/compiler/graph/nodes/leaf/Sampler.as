package aerys.minko.render.shader.compiler.graph.nodes.leaf
{
	import aerys.minko.render.resource.texture.TextureResource;
	import aerys.minko.render.shader.compiler.CRC32;
	
	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public class Sampler extends AbstractSampler
	{
		/**
		 * This vector is only used to compute the hash of the Sampler. 
		 */		
		private static const RESOURCES : Vector.<TextureResource> = new Vector.<TextureResource>();
		
		private var _textureResource : TextureResource;
		
		public function get textureResource() : TextureResource
		{
			return _textureResource;
		}
		
		public function Sampler(textureResource	: TextureResource,
								filter			: uint = 1, // SamplerFilter.LINEAR
								mipmap			: uint = 0, // SamplerMipmap.DISABLE
								wrapping		: uint = 1, // SamplerWrapping.REPEAT
								dimension		: uint = 0) // SamplerDimension.FLAT
		{
			_textureResource = textureResource;
			
			var textureResourceId : int = RESOURCES.indexOf(textureResource);
			if (textureResourceId === -1)
			{
				RESOURCES.push(textureResource);
				textureResourceId = RESOURCES.length - 1;
			}
			
			var hash : uint = CRC32.computeForString('Sampler' + textureResourceId);
			
			super(hash, filter, mipmap, wrapping, dimension);
		}
		
		override public function toString() : String
		{
			return 'Sampler';
		}
	}
}