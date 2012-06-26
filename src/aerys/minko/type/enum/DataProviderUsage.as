package aerys.minko.type.enum
{
	public class DataProviderUsage
	{
		/**
		 * Dataprovider with this type are exclusive: they must be used by a single mesh, or scene.
		 * They can not be shared between multiple databindings objects.
		 * 
		 * The user can only add and remove properties to them, but must not add or remove them
		 * from DataBindings.
		 * 
		 * Examples: transform provider and properties providers of each mesh...
		 */
		public static const EXCLUSIVE	: uint = 0;
		
		/**
		 * Dataproviders with this type can be shared between multiple instances of mesh, or scene.
		 * 
		 * They can be handled by the user (add or remove them to multiple databindings objects, 
		 * set and remove properties in them).
		 * 
		 * Examples: materials
		 */
		public static const SHARED		: uint = 1;
		
 		/**
		 * Dataproviders with this type are automatically managed, most likely by a controller.
		 * 
		 * They may be shared between meshes, but the user should not handle them manually at all.
		 * 
		 * Examples: all lighting related providers, skinning provider, HLSA provider...
		 */
		public static const MANAGED		: uint = 2;
	}
}