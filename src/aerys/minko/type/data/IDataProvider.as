package aerys.minko.type.data
{
	import aerys.minko.type.Signal;

	public interface IDataProvider
	{
		function get dataDescriptor() : Object;
		
		/**
		 * The signal executed everytime one of the properties of
		 * the data provider is changed. Callback functions should
		 * accept the following arguments:
		 * <ul>
		 * <li>dataProvider : IDataProvider, the data provider that executed the signal.</li>
		 * <li>propertyName : String, the name of the property that changed. When more than
		 * one property changed (ie when the "unlock" method is called), the value is "null".</li>
		 * </ul>
		 * 
		 * @return 
		 * 
		 */
		function get changed() : Signal;
		
		/**
		 * Whether the data provider is locked or not. When the object is locked,
		 * the changed signal will not be executed when a property is changed but only
		 * when the object is unlocked with the "unlock" method. 
		 * @return 
		 * 
		 */
		function get locked() : Boolean;
		/**
		 * Lock the data provider. When it is locked, the data provider will not execute
		 * the "changed" signal. Locking the data provider prevents many unnecessary "changed"
		 * signals to be executed when several properties have to be updated at once.
		 * 
		 */
		function lock() : void;
		/**
		 * Unlock the data provider. The "changed" signal is executed if the data provider was
		 * locked.
		 * 
		 */
		function unlock() : void;
	}
}