package aerys.minko.scene.material
{
	import aerys.common.IVersionnable;
	import aerys.minko.scene.interfaces.IScene3D;
	import aerys.minko.scene.interfaces.IScene3DRessource;
	
	/**
	 * <p>Common interface for all the materials.</p>
	 * <p>Materials use the "decorator" design pattern: each material is defined as a decorator
	 * of another underlaying material. This way, one can easily stack materials
	 * and effects.</p>
	 *
	 * @author Jean-Marc Le Roux
	 *
	 */
	public interface IMaterial3D extends IScene3D, IVersionnable, IScene3DRessource
	{
	}
}