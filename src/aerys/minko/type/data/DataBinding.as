package aerys.minko.type.data
{
	import aerys.minko.render.DrawCall;
	
	import flash.utils.Dictionary;

	public final class DataBinding
	{
		private static const NO_KEY	: String		= "__no_key__";
		
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
			
			for (var parameter : String in dataDescriptor)
			{
				var key : String = dataDescriptor[parameter];
				
				for (var callId : int = 0; callId < numCalls; ++callId)
				{
					var call : DrawCall = _drawCalls[callId];
					
					if (call.hasParameter(parameter))
						addParameter(parameter, bindable, key);
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
		
		public function setParameter(parameter : String, value : Object) : void
		{
			var numCalls : int = _drawCalls.length;
			
			for (var callId : int = 0; callId < numCalls; ++callId)
				_drawCalls[callId].setParameter(parameter, value);
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
			
			if (key === null)
				key = NO_KEY;
			bindingTable[key] = parameter;
			
			setParameter(parameter, key !== NO_KEY ? source[key] : source);
		}
		
		public function removeParameter(parameter : String) : void
		{
			for (var source : Object in _bindings)
			{
				var bindingTable 	: Object 	= _bindings[source];
				var numKeys		 	: int		= 0;
				var numDeletedKeys	: int		= 0;
				
				for (var key : String in bindingTable)
				{
					++numKeys;
					
					if (bindingTable[key] == parameter)
					{
						++numDeletedKeys;
						delete bindingTable[key];
					}
				}
				
				if (numKeys == numDeletedKeys)
				{
					(source as IDataProvider).changed.remove(
						parameterChangedHandler
					);
					
					delete _bindings[source];
				}
			}
		}
		
		public function clear() : void
		{
			for (var source : Object in _bindings)
			{
				var bindingTable 	: Object 	= _bindings[source];
				
				for (var key : String in bindingTable)
					removeParameter(bindingTable[key]);
			}
		}
		
		private function parameterChangedHandler(source : IDataProvider, key : Object) : void
		{
			var bindingTable 	: Object = _bindings[source] as Object;
			var parameterName 	: String = bindingTable[key] as String;
			
			if (parameterName)
				setParameter(parameterName, key ? source[key] : source);
		}
	}
}