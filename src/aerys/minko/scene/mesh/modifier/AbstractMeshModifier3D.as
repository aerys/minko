package aerys.minko.scene.mesh.modifier
{
	import aerys.minko.query.IScene3DQuery;
	import aerys.minko.scene.mesh.IMesh3D;
	import aerys.minko.type.stream.IndexStream3D;
	import aerys.minko.type.stream.VertexStream3D;
	import aerys.minko.type.stream.VertexStreamList3D;
	
	public class AbstractMeshModifier3D implements IMeshModifier3D
	{
		private var _target : IMesh3D	= null;
		
		public function AbstractMeshModifier3D(target : IMesh3D)
		{
			_target = target;
		}
		
		public function get target() : IMesh3D
		{
			return _target;
		}
		
		public function get version() : uint
		{
			return _target.version;
		}
				
		public function get vertexStreamList() : VertexStreamList3D
		{
			return _target.vertexStreamList;
		}
		
		public function get indexStream() : IndexStream3D
		{
			return _target.indexStream;
		}
		
		public function accept(query : IScene3DQuery) : void
		{
			query.query(_target);
		}
		
		public function get name() : String
		{
			return null;
		}
	}
}