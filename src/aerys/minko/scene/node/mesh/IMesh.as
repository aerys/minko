package aerys.minko.scene.node.mesh
{
	import aerys.common.IVersionnable;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.IndexStream;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.stream.VertexStreamList;

	public interface IMesh extends IScene, IVersionnable
	{
		function get vertexStreamList()	: VertexStreamList;
		function get indexStream()		: IndexStream;
	}
}
