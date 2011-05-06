package aerys.minko.scene.graph.texture
{
	import aerys.common.IVersionnable;
	import aerys.minko.scene.graph.IScene;
	import aerys.minko.scene.graph.ISceneRessource;
	
	/**
	 * <p>Common interface for all the materials.</p>
	 * <p>Materials use the "decorator" design pattern: each material is defined as a decorator
	 * of another underlaying material. This way, one can easily stack materials
	 * and effects.</p>
	 *
	 * @author Jean-Marc Le Roux
	 *
	 */
	public interface ITexture extends IScene, IVersionnable, ISceneRessource
	{
	}
}