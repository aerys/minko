package aerys.minko.scene.node.mesh
{
	import aerys.minko.ns.minko_stream;
	import aerys.minko.scene.action.mesh.MeshAction;
	import aerys.minko.scene.action.mesh.PopMeshSkinAction;
	import aerys.minko.scene.action.mesh.PushMeshSkinAction;
	import aerys.minko.scene.node.AbstractScene;
	import aerys.minko.scene.node.group.IGroup;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.IndexStream;
	import aerys.minko.type.stream.format.VertexComponent;
	import aerys.minko.type.stream.format.VertexFormat;

	use namespace minko_stream;

	public class SkinnedMesh extends AbstractScene implements IMesh
	{
		private var _name					: String;

		private var _mesh					: IMesh;
		private var _skeletonRootName		: String;
		private var _skeletonReference		: IGroup;
		private var _bindShapeMatrix		: Matrix4x4;
		private var _jointNames				: Vector.<String>;
		private var _inverseBindMatrices	: Vector.<Matrix4x4>;
		private var _maxInfluences			: uint;

		public function set skeletonReference(v : IGroup) : void { _skeletonReference = v; }

		/**
		 * IMesh implementation
		 */
		
		public function get version()				: uint					{ return _mesh.version; }
		public function get vertexStream()			: IVertexStream			{ return _mesh.vertexStream; }
		public function get indexStream()			: IndexStream			{ return _mesh.indexStream; }

		public function get maxInfluences()			: uint					{ return _maxInfluences; }
		public function get mesh()					: IMesh					{ return _mesh; }
		public function get skeletonRootName()		: String				{ return _skeletonRootName; }
		public function get skeletonReference()		: IGroup				{ return _skeletonReference; }
		public function get bindShapeMatrix()		: Matrix4x4				{ return _bindShapeMatrix; }

		public function get jointNames()			: Vector.<String>		{ return _jointNames; }
		public function get inverseBindMatrices()	: Vector.<Matrix4x4>	{ return _inverseBindMatrices; }

		public function SkinnedMesh(mesh				: IMesh,
									skeletonReference	: IGroup,
									skeletonRootName	: String,
									bindShapeMatrix		: Matrix4x4,
									jointNames			: Vector.<String>,
									inverseBindMatrices	: Vector.<Matrix4x4>)
		{
			super();

			_name					= 'SkinnedMesh';

			_skeletonReference		= skeletonReference;
			_skeletonRootName		= skeletonRootName;
			_mesh					= mesh;
			_bindShapeMatrix		= bindShapeMatrix;
			_jointNames				= jointNames;
			_inverseBindMatrices	= inverseBindMatrices;

			_maxInfluences			= getMaxInfluencesFromVertexFormat(_mesh.vertexStream.format);

			actions.push(
				new PushMeshSkinAction(),
				MeshAction.meshAction,
				PopMeshSkinAction.popMeshSkinAction)
				;
		}

		private function getMaxInfluencesFromVertexFormat(vertexFormat : VertexFormat) : uint
		{
			var maxInfluences	: uint = VertexComponent.BONES.length;
			for (var i : uint = 0; i < maxInfluences; ++i)
				if (!vertexFormat.hasComponent(VertexComponent.BONES[i]))
					break;
			return i;
		}

	}
}
