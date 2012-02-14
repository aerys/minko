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
		
		public function add(bindable : IDataProvider) : DataBinding
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
			
			return this;
		}
		
		public function remove(bindable : IDataProvider) : DataBinding
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
			
			return this;
		}
		
		public function setParameter(parameter : String, value : Object) : DataBinding
		{
			var numCalls : int = _drawCalls.length;
			
			for (var callId : int = 0; callId < numCalls; ++callId)
				_drawCalls[callId].setParameter(parameter, value);
			
			return this;
		}
		
		public function addParameter(parameter 	: String,
									 source		: IDataProvider,
									 key		: Object	= null) : DataBinding
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
			
			return this;
		}
		
		public function removeParameter(parameter : String) : DataBinding
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
			
			return this;
		}
		
		public function clear() : DataBinding
		{
			for (var source : Object in _bindings)
			{
				var bindingTable 	: Object 	= _bindings[source];
				
				for (var key : String in bindingTable)
					removeParameter(bindingTable[key]);
			}
			
			return this;
		}
		
		private function parameterChangedHandler(source : IDataProvider, key : Object) : void
		{
			key ||= NO_KEY;
			
			var bindingTable 	: Object = _bindings[source] as Object;
			var parameterName 	: String = bindingTable[key] as String;
			
			if (parameterName)
				setParameter(parameterName, key !== NO_KEY ? source[key] : source);
		}
	}
}