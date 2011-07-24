package aerys.minko.scene.node.mesh
{
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.action.mesh.SkinnedMeshAction;
	import aerys.minko.scene.action.mesh.MeshAction;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.group.IGroup;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.IndexStream;
	import aerys.minko.type.vertex.format.VertexComponent;
	import aerys.minko.type.vertex.format.VertexFormat;
	
	public class SkinnedMesh implements IScene, IMesh
	{
		private static const _ACTIONS : Vector.<IAction> = new Vector.<IAction>();
		
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
		 * IScene implementation 
		 */		
		public function get actions()		: Vector.<IAction>	{ return _ACTIONS; }
		public function get name()			: String			{ return _name; }
		public function set name(v : String): void				{ _name = v; }
		
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
		
		public function SkinnedMesh(mesh				: Mesh,
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
			
			_maxInfluences = getMaxInfluencesFromVertexFormat(_mesh.vertexStream.format);
			
			_ACTIONS.length = 2;
			_ACTIONS[0] = SkinnedMeshAction.instance;
			_ACTIONS[1] = MeshAction.meshAction;
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
