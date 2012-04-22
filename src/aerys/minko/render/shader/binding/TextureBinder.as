package aerys.minko.render.shader.binding
{
	import aerys.minko.render.resource.texture.ITextureResource;
	
	import flash.utils.Dictionary;
	
	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
	public class TextureBinder implements IBinder
	{
		private var _name		: String;
		private var _samplerId	: uint;
		
		/**
		 * @inheritDoc
		 */		
		public function get bindingName() : String
		{
			return _name;
		}
		
		public function TextureBinder(name		: String,
									  samplerId	: uint)
		{
			_name		= name;
			_samplerId	= samplerId;
		}
		
		/**
		 * @inheritDoc
		 */		
		public function set(cpuConstants	: Dictionary,
							vsConstData 	: Vector.<Number>,
							fsConstData 	: Vector.<Number>,
							fsTextures		: Vector.<ITextureResource>,
							value			: Object) : void
		{
			fsTextures[_samplerId] = ITextureResource(value);
		}
	}
}
