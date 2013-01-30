package aerys.minko.render
{
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.type.binding.DataBindings;
	import aerys.minko.type.binding.Signature;
	
	public class DataBindingsProxy
	{
		private var _target 		: DataBindings;
		private var _signature		: Signature;
		private var _signatureFlags	: uint;
		
		protected function get target() : DataBindings
		{
			return _target;
		}
		
		protected function get signature() : Signature
		{
			return _signature;
		}
		
		protected function get signatureFlags() : uint
		{
			return _signatureFlags;
		}
		
		public function DataBindingsProxy(target 			: DataBindings,
										  signature			: Signature,
										  signatureFlags	: uint)
		{
			_target = target;
			_signature = signature;
			_signatureFlags = signatureFlags;
		}
		
		public function propertyExists(propertyName : String) : Boolean
		{
			var value 	: Boolean	= _target.propertyExists(propertyName);
			
			_signature.update(
				propertyName,
				value,
				Signature.OPERATION_EXISTS | _signatureFlags
			);
			
			return value;
		}
		
		public function getProperty(propertyName	: String,
									defaultValue	: Object	= null) : *
		{
			if (!_target.propertyExists(propertyName))
			{
				if (defaultValue === null)
				{
					throw new Error(
						'The property \'' + propertyName
						+ '\' does not exist and no default value was provided.'
					);
				}
				
				_signature.update(
					propertyName,
					false,
					Signature.OPERATION_EXISTS | _signatureFlags
				);
				
				return defaultValue;
			}
			
			var value : Object = _target.getProperty(propertyName);
			
			_signature.update(
				propertyName,
				value,
				Signature.OPERATION_GET | _signatureFlags
			);
			
			return value;
		}
	}
}