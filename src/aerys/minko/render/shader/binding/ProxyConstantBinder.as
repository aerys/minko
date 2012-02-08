package aerys.minko.render.shader.binding
{
	import aerys.minko.render.resource.texture.ITextureResource;
	
	import flash.utils.Dictionary;
	
	public class ProxyConstantBinder implements IBinder
	{
		private var _bindingName	: String;
		private var _binders		: Vector.<IBinder>;
		private var _numBinders		: uint;
		
		public function get bindingName() : String
		{
			return _bindingName;
		}
		
		public function ProxyConstantBinder(bindingName	: String)
		{
			_bindingName	= bindingName;
			_binders		= new Vector.<IBinder>();
		}
		
		public function addBinder(binder : IBinder) : void
		{
			if (binder.bindingName != _bindingName)
				throw new Error('Invalid binder.');
			
			_binders.push(binder);
			++_numBinders;
		}
		
		public function set(vsConstData	: Vector.<Number>, 
							fsConstData	: Vector.<Number>, 
							textures	: Vector.<ITextureResource>, 
							value		: Object, 
							dataStore	: Dictionary) : void
		{
			for (var binderId : uint = 0; binderId < _numBinders; ++binderId)
				IBinder(_binders[binderId]).set(vsConstData, fsConstData, textures, value, dataStore);
		}
	}
}