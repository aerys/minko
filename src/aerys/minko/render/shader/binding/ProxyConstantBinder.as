package aerys.minko.render.shader.binding
{
	import aerys.minko.render.resource.texture.ITextureResource;
	
	import flash.utils.Dictionary;
	
	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
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
		
		public function set(cpuConstants	: Dictionary,
							vsConstants		: Vector.<Number>, 
							fsConstants		: Vector.<Number>,
							fsTextures		: Vector.<ITextureResource>, 
							value			: Object) : void
		{
			for (var binderId : uint = 0; binderId < _numBinders; ++binderId)
				_binders[binderId].set(cpuConstants, vsConstants, fsConstants, fsTextures, value);
		}
	}
}