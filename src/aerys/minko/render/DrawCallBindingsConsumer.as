package aerys.minko.render
{
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.render.shader.binding.IBinder;
	
	import flash.utils.Dictionary;
	import aerys.minko.type.binding.IDataBindingsConsumer;

	internal final class DrawCallBindingsConsumer implements IDataBindingsConsumer
	{
		private var _enabled		: Boolean;
		
		private var _bindings		: Object;
		
		private var _cpuConstants	: Dictionary;
		private var _vsConstants	: Vector.<Number>;
		private var _fsConstants	: Vector.<Number>;
		private var _fsTextures		: Vector.<ITextureResource>;
		
		private var _changes		: Object;
		
		public function get enabled() : Boolean
		{
			return _enabled;
		}
		public function set enabled(value : Boolean) : void
		{
			_enabled = value;
			
			if (value)
				for (var bindingName : String in _changes)
				{
					setProperty(bindingName, _changes[bindingName]);
					delete _changes[bindingName];
				}
		}

		public function DrawCallBindingsConsumer(bindings		: Object,
												 cpuConstants	: Dictionary,
												 vsConstants	: Vector.<Number>,
												 fsConstants	: Vector.<Number>,
												 fsTextures		: Vector.<ITextureResource>)
		{
			_bindings = bindings;
			_cpuConstants = cpuConstants;
			_vsConstants = vsConstants;
			_fsConstants = fsConstants;
			_fsTextures = fsTextures;
			
			initialize();
		}
		
		private function initialize() : void
		{
			_enabled = true;
			_changes = {};
		}
		
		public function setProperty(propertyName : String, value : Object) : void
		{
			if (!value)
				return;
			
			if (!_enabled)
			{
				_changes[propertyName] = value;
				
				return ;
			}
			
			var binding : IBinder = _bindings[propertyName] as IBinder;
			
			if (binding != null)
				binding.set(_cpuConstants, _vsConstants, _fsConstants, _fsTextures, value);
		}
	}
}