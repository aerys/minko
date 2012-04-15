package aerys.minko.render.shader.binding
{
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.render.shader.compiler.Serializer;
	
	import flash.utils.Dictionary;
	
	/**
	 * @private
	 * @author Romain Gilliotte
	 * 
	 */
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
		
		public function EvalExpConstantBinder(bindingName	: String,
											  size			: uint,
											  masterBinding	: IBinder,
											  evalExp		: EvalExp)
		{
			_bindingName	= bindingName;
			_masterBinding	= masterBinding;
			_evalExp		= evalExp;
			_size			= size;
		}
		
		public function set(cpuConstants	: Dictionary,
							vsConstants		: Vector.<Number>, 
							fsConstants		: Vector.<Number>,
							fsTextures		: Vector.<ITextureResource>, 
							value			: Object) : void
		{
			if (!cpuConstants[_bindingName])
				cpuConstants[_bindingName] = new Vector.<Number>();
			
			Serializer.serializeKnownLength(value, cpuConstants[_bindingName], 0, _size);
			
			var result : Vector.<Number> = _evalExp.compute(cpuConstants);
			if (result != null)
				_masterBinding.set(cpuConstants, vsConstants, fsConstants, fsTextures, result);
		}
	}
}
