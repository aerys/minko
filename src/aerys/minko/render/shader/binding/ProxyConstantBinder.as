package aerys.minko.render.shader.binding
{
	import aerys.minko.render.resource.texture.ITextureResource;
	
	public class ProxyConstantBinder implements IBinder
	{
		private var _evalExp		: EvalExp;
		private var _masterBinding	: IBinder;
		private var _bindingName	: String;
		
		public function get bindingName() : String
		{
			return _bindingName;
		}
		
		public function ProxyConstantBinder(bindingName		: String,
											masterBinding	: IBinder,
											evalExp			: EvalExp)
		{
			_bindingName	= bindingName;
			_evalExp		= evalExp;
		}
		
		public function set(vsConstData		: Vector.<Number>, 
							fsConstData		: Vector.<Number>, 
							textures		: Vector.<ITextureResource>, 
							value			: Object) : void
		{
			_evalExp.changeBindedConstant(_bindingName, value);
			
			var result : Vector.<Number> = _evalExp.result;
			if (result != null)
				_masterBinding.set(vsConstData, fsConstData, textures, result);
		}
	}
}