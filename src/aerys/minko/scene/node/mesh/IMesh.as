package aerys.minko.scene.node.mesh
{
	import aerys.minko.scene.node.IScene;
	import aerys.minko.type.IVersionable;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.IndexStream;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.stream.VertexStreamList;

	/**
	 * The IMesh interface define the properties available in any mesh object.
	 *  
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public interface IMesh extends IScene, IVersionable
	{
		/**
		 * The VertexStream object containing the mesh geometry data. 
		 * @return 
		 * 
		 */
		function get vertexStream()	: IVertexStream;
		/**
		 * The IndexStream object containing the mesh indices. 
		 * @return 
		 * 
		 */
		function get indexStream()	: IndexStream;
	}
}
