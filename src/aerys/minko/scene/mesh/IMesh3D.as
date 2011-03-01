package aerys.minko.scene.mesh
{
	import aerys.common.IVersionnable;
	import aerys.minko.scene.IScene3D;
	import aerys.minko.type.stream.IndexStream3D;
	import aerys.minko.type.stream.VertexStream3D;
	import aerys.minko.type.stream.VertexStreamList3D;

	public interface IMesh3D extends IScene3D, IVersionnable
	{
		function get vertexStreamList()	: VertexStreamList3D;
		function get indexStream()		: IndexStream3D;
	}
}
