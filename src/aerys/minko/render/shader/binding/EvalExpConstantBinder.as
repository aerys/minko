package aerys.minko.render.shader.binding
{
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.render.shader.compiler.Serializer;
	
	import flash.utils.Dictionary;
	
	public class EvalExpConstantBinder implements IBinder
	{
		private var _bindingName	: String;
		private var _size			: uint;
		private var _evalExp		: EvalExp;
		private var _masterBinding	: IBinder;
		
		public function get bindingName() : String
		{
			return _bindingName;
		}
		
		public function EvalExpConstantBinder(bindingName		: String,
											size			: uint,
											masterBinding	: IBinder,
											evalExp			: EvalExp)
		{
			_bindingName	= bindingName;
			_masterBinding	= masterBinding;
			_evalExp		= evalExp;
			_size			= size;
		}
		
		public function set(vsConstData		: Vector.<Number>, 
							fsConstData		: Vector.<Number>, 
							textures		: Vector.<ITextureResource>, 
							value			: Object,
							dataStore		: Dictionary) : void
		{
			if (!dataStore[_bindingName])
				dataStore[_bindingName] = new Vector.<Number>();
			
			Serializer.serializeKnownLength(value, dataStore[_bindingName], 0, _size);
			
			var result : Vector.<Number> = _evalExp.compute(dataStore);
			if (result != null)
				_masterBinding.set(vsConstData, fsConstData, textures, result, dataStore);
		}
	}
}
