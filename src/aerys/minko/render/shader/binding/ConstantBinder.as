package aerys.minko.render.shader.binding
{
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.render.shader.compiler.Serializer;
	
	import flash.utils.Dictionary;
	
	public class ConstantBinder implements IBinder
	{
		private var _bindingName	: String;
		private var _offset			: uint;
		private var _size			: uint;
		private var _isVertexShader	: Boolean;
		
		/**
		 * @inheritDoc
		 */		
		public function get bindingName() : String
		{
			return _bindingName;
		}
		
		public function ConstantBinder(name				: String,
									   offset			: uint,
									   size				: uint,
									   isVertexShader	: Boolean)
		{
			_bindingName	= name;
			_offset			= offset;
			_size			= size;
			_isVertexShader	= isVertexShader;
		}
		
		/**
		 * @inheritDoc
		 */		
		public function set(vsConstData 	: Vector.<Number>,
							fsConstData 	: Vector.<Number>,
							textures		: Vector.<ITextureResource>,
							value			: Object,
							dataStore		: Dictionary) : void
		{
			var constData : Vector.<Number> = _isVertexShader ? vsConstData : fsConstData;
			Serializer.serializeKnownLength(value, constData, _offset, _size);
		}
	}
}
