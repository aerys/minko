package aerys.minko.render.shader.binding
{
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.render.shader.accessor.IParameterAccessor;
	import aerys.minko.render.shader.compiler.Serializer;
	
	import flash.utils.Dictionary;
	
	public class ConstantParameterBinding implements IParameterBinding
	{
		private var _name				: String;
		private var _accessor			: IParameterAccessor;
		private var _offset				: uint;
		private var _size				: uint;
		private var _isVertexConstant	: Boolean;
		
		/**
		 * @inheritDoc
		 */		
		public function get name() : String
		{
			return _name;
		}
		
		public function ConstantParameterBinding(name				: String,
												 offset				: uint,
												 size				: uint,
												 isVertexConstant	: Boolean,
												 accessor			: IParameterAccessor = null)
		{
			_name				= name;
			_offset				= offset;
			_size				= size;
			_isVertexConstant	= isVertexConstant;
			_accessor			= accessor;
		}
		
		/**
		 * @inheritDoc
		 */		
		public function update(vsConstData		: Vector.<Number>, 
							   fsConstData		: Vector.<Number>, 
							   textures			: Vector.<ITextureResource>) : void
		{
			if (_accessor != null)
			{
				var data : Object = _accessor.getParameter();
				
				if (_isVertexConstant)
					Serializer.serializeKnownLength(data, vsConstData, _offset, _size);
				else
					Serializer.serializeKnownLength(data, fsConstData, _offset, _size);
			}
		}
		
		/**
		 * @inheritDoc
		 */		
		public function set(vsConstData 	: Vector.<Number>,
							fsConstData 	: Vector.<Number>,
							textures		: Vector.<ITextureResource>,
							value			: Object) : void
		{
			if (_isVertexConstant)
				Serializer.serializeKnownLength(value, vsConstData, _offset, _size);
			else
				Serializer.serializeKnownLength(value, fsConstData, _offset, _size);
		}

	}
}
