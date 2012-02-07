package aerys.minko.type.data
{
	import aerys.minko.render.DrawCall;
	
	import flash.utils.Dictionary;

	public final class DataBinding
	{
		private var _drawCalls	: Vector.<DrawCall>	= null;
		private var _bindings	: Dictionary		= new Dictionary(true);
		
		public function DataBinding(drawCalls : Vector.<DrawCall>)
		{
			_drawCalls = drawCalls;
		}
		
		public function add(bindable : IBindable) : void
		{
			var dataDescriptor 	: Object 	= bindable.dataDescriptor;
			var numCalls 		: int 		= _drawCalls.length;
			
			for (var parameterName : String in dataDescriptor)
			{
				var key : String = dataDescriptor[parameterName];
				
				for (var callId : int = 0; callId < numCalls; ++callId)
				{
					var call : DrawCall = _drawCalls[callId];
					
					if (call.hasParameter(parameterName))
						addParameter(parameterName, bindable, key);
				}
			}
		}
		
		public function remove(bindable : IBindable) : void
		{
			var dataDescriptor 	: Object 	= bindable.dataDescriptor;
			var numCalls 		: int 		= _drawCalls.length;
			
			for (var parameterName : String in dataDescriptor)
			{
				var key : String = dataDescriptor[parameterName];
				
				for (var callId : int = 0; callId < numCalls; ++callId)
				{
					var call : DrawCall = _drawCalls[callId];
					
					if (call.hasParameter(parameterName))
						removeParameter(parameterName);
				}
			}
		}
		
		public function setParameter(name : String, value : Object) : void
		{
			var numCalls : int = _drawCalls.length;
			
			for (var callId : int = 0; callId < numCalls; ++callId)
				_drawCalls[callId].setParameter(name, value);
		}
		
		public function addParameter(parameter 	: String,
									 source		: IDataProvider,
									 key		: Object	= null) : void
		{
			var bindingTable : Object = _bindings[source] as Object;
			
			if (!bindingTable)
			{
				_bindings[source] = bindingTable = {};
				source.changed.add(parameterChangedHandler);
			}
//			trace("add", parameter);
			bindingTable[key] = parameter;
			
			setParameter(parameter, key ? source[key] : source);
		}
		
		public function removeParameter(parameter : String) : void
		{
			doUnbindParameter(parameter, null);
		}
		
		public function update() : void
		{
//			trace("update");
			for (var source : Object in _bindings)
			{
				var bindingTable 	: Object 	= _bindings[source];
				
				for (var key : String in _bindings)
				{
					doUnbindParameter(
						bindingTable[key],
						parameterIsRequired
					);
				}
			}
		}
		
		private function doUnbindParameter(parameter	 	: String,
										   checkFunction	: Function) : void
		{
			for (var source : Object in _bindings)
			{
				var bindingTable 	: Object 	= _bindings[source];
				var numKeys		 	: int		= 0;
				var numDeletedKeys	: int		= 0;
				
				for (var key : String in _bindings)
				{
					++numKeys;
					
					if (bindingTable[key] == parameter
						&& (checkFunction && !checkFunction.call(null, parameter)))
					{
						++numDeletedKeys;
						delete bindingTable[key];
					}
				}
				
				// no binding left => remove "changed" handler
				if (numKeys == numDeletedKeys)
				{
					(source as IDataProvider).changed.remove(
						parameterChangedHandler
					);
					
					trace("remove", parameter);
					
					delete _bindings[source];
				}
			}
		}
		
		private function parameterChangedHandler(source : IDataProvider, key : Object) : void
		{
			var bindingTable 	: Object = _bindings[source] as Object;
			var parameterName 	: String = bindingTable[key] as String;
			
			if (parameterName)
				setParameter(parameterName, key ? source[key] : source);
		}
		
		private function parameterIsRequired(parameter : String) : Boolean
		{
			var numCalls	: int	= _drawCalls.length;
			
			for (var callId : int = 0; callId < numCalls; ++callId)
				if (_drawCalls[callId].hasParameter(parameter))
					return true;
			
			return false;
		}
	}
}