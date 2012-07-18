package aerys.minko.type.loader
{
	import aerys.minko.type.Signal;
	
	import flash.net.URLRequest;
	import flash.utils.ByteArray;

	public interface ILoader
	{
		/**
		 * The signal executed when new data is received.
		 * 
		 * <p>
		 * Callbacks for this signal must accept the following arguments:
		 * </p>
		 * <ul>
		 * <li>loader : ILoader, the ILoader that executed the signal</li>
		 * <li>percent : Number, the progress of the loading operation as a floatting
		 * point value between 0.0 and 1.0</li>
		 * </ul>
		 *  
		 * @return 
		 * 
		 */
		function get progress()		: Signal;
		/**
		 * The signal executed when an error happens during loading.
		 * 
		 * <p>
		 * Callbacks for this signal must accept the following arguments:
		 * </p>
		 * <ul>
		 * <li>loader : ILoader, the ILoader causing the error</li>
		 * <li>errorId : int, the ID of the error</li>
		 * <li>message : String, the error message</li>
		 * </ul>
		 *  
		 * @return 
		 * 
		 */
		function get error()		: Signal;
		/**
		 * The signal executed when the loading has been successfully completed.
		 * 
		 * <p>
		 * Callbacks for this signal must accept the following arguments:
		 * </p>
		 * <ul>
		 * <li>loader : ILoader, the ILoader that executed the signal</li>
		 * <li>object : Object, the result of the loading operation</li>
		 * </ul>
		 *  
		 * @return 
		 * 
		 */
		function get complete()		: Signal;
		
		function get isComplete()	: Boolean;
		
		function load(urlRequest : URLRequest) : void;
		function loadClass(classObject : Class) : void;
		function loadBytes(data : ByteArray) : void;
	}
}
