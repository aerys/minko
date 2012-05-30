package aerys.minko.render.shader.compiler.graph.nodes.leaf
{
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.render.resource.texture.TextureResource;
	import aerys.minko.render.shader.compiler.CRC32;
	import aerys.minko.render.shader.compiler.graph.nodes.AbstractNode;
	
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
		private static const RESOURCES : Vector.<ITextureResource> = new Vector.<ITextureResource>();
		
		private var _textureResource : ITextureResource;
		
		public function get textureResource() : ITextureResource
		{
			return _textureResource;
		}
		
		public function Sampler(textureResource	: ITextureResource,
								filter			: uint = 1, // SamplerFilter.LINEAR
								mipmap			: uint = 0, // SamplerMipmap.DISABLE
								wrapping		: uint = 1, // SamplerWrapping.REPEAT
								dimension		: uint = 0) // SamplerDimension.FLAT
		{
			_textureResource = textureResource;
			
			super(filter, mipmap, wrapping, dimension);
		}
		
		override protected function computeHash() : uint
		{
			var textureResourceId : int = RESOURCES.indexOf(_textureResource);
			
			if (textureResourceId === -1)
			{
				RESOURCES.push(textureResource);
				textureResourceId = RESOURCES.length - 1;
			}
			
			return CRC32.computeForString('Sampler' + textureResourceId)
		}
		
		override public function toString() : String
		{
			return 'Sampler';
		}
		
		override public function clone() : AbstractNode
		{
			return new Sampler(_textureResource, filter, mipmap, wrapping, dimension);
		}

	}
}