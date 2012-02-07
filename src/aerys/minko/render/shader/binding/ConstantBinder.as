package aerys.minko.render.shader.binding
{
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.render.shader.compiler.Serializer;
	
	public class ConstantBinder implements IBinder
	{
		private var _name			: String;
		private var _offset			: uint;
		private var _size			: uint;
		private var _isVertexShader	: Boolean;
		
		/**
		 * @inheritDoc
		 */		
		public function get bindingName() : String
		{
			return _name;
		}
		
		public function ConstantBinder(name				: String,
									   offset			: uint,
									   size				: uint,
									   isVertexShader	: Boolean)
		{
			_name	= name;
			_offset	= offset;
			_size	= size;
			_isVertexShader = isVertexShader;
		}
		
		/**
		 * @inheritDoc
		 */		
		public function set(vsConstData 	: Vector.<Number>,
							fsConstData 	: Vector.<Number>,
							textures		: Vector.<ITextureResource>,
							value			: Object) : void
		{
			var constData : Vector.<Number> = _isVertexShader ? vsConstData : fsConstData;
			Serializer.serializeKnownLength(value, constData, _offset, _size);
		}
	}
}
