package aerys.minko.render.shader.binding
{
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.render.resource.texture.TextureResource;
	import aerys.minko.render.shader.accessor.IParameterAccessor;
	
	import flash.utils.Dictionary;
	
	public class TextureParameterBinding implements IParameterBinding
	{
		private var _name		: String;
		private var _accessor 	: IParameterAccessor;
		private var _samplerId	: uint;
		
		/**
		 * @inheritDoc
		 */		
		public function get name() : String
		{
			return _name;
		}
		
		public function TextureParameterBinding(name		: String,
												samplerId	: uint,
												accessor 	: IParameterAccessor = null)
		{
			_name		= name;
			_accessor	= accessor;
			_samplerId	= samplerId;
		}
		
		/**
		 * @inheritDoc
		 */		
		public function update(vsConstData		: Vector.<Number>, 
							   fsConstData		: Vector.<Number>, 
							   textures			: Vector.<ITextureResource>) : void
		{
			if (_accessor != null)
				textures[_samplerId] = ITextureResource(_accessor.getParameter());
		}
		
		/**
		 * @inheritDoc
		 */		
		public function set(vsConstData 	: Vector.<Number>,
							fsConstData 	: Vector.<Number>,
							textures		: Vector.<ITextureResource>,
							value			: Object) : void
		{
			textures[_samplerId] = TextureResource(value);
		}
	}
}
