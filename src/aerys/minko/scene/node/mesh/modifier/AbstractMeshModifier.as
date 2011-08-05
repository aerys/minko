package aerys.minko.scene.node.mesh.modifier
{
	import aerys.minko.scene.action.mesh.MeshAction;
	import aerys.minko.scene.node.AbstractScene;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.ISearchableScene;
	import aerys.minko.scene.node.mesh.IMesh;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.IndexStream;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.stream.VertexStreamList;
	
	import avmplus.getQualifiedClassName;
	
	import flash.net.getClassByAlias;
	
	/**
	 * The AbstractMeshModifier class provides basic support for mesh
	 * modifiers and can be extended in order to be specialized. 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class AbstractMeshModifier extends AbstractScene implements IMeshModifier
	{
		private var _target 			: IMesh				= null;
		
		protected var _indexStream		: IndexStream		= null;
		protected var _vertexStreamList	: VertexStreamList	= null;
		
		public function get target() : IMesh
		{
			return _target;
		}
		
		public function get version() : uint
		{
			return _target.version;
		}
				
		public function get vertexStream() : IVertexStream
		{
			return _vertexStreamList || (_target ? _target.vertexStream : null);
		}
		
		public function get indexStream() : IndexStream
		{
			return _indexStream || (_target ? _target.indexStream : null);
		}
		
		public function AbstractMeshModifier(target : IMesh, ...streams)
		{
			super();
			
			initialize(target, streams);
			
			//actions[0] = new MeshModifierAction();
			actions[0] = MeshAction.meshAction;
		}
		
		public function getDescendantByName(name : String) : IScene
		{
			if (_target)
			{
				if (_target.name == name)
					return _target;
				else if (_target is ISearchableScene)
					return (_target as ISearchableScene).getDescendantByName(name);
			}
			
			return null;
		}
		
		public function getDescendantsByType(type 			: Class,
											 descendants 	: Vector.<IScene> = null) : Vector.<IScene>
		{
			descendants ||= new Vector.<IScene>();
			
			if (_target)
			{
				if (getClassByAlias(getQualifiedClassName(_target)) == type)
					descendants.push(_target);
				if (_target is ISearchableScene)
					(_target as ISearchableScene).getDescendantsByType(type, descendants);
			}
			
			return descendants;
		}
	
		private function initialize(target : IMesh, streams : Array) : void
		{
			var numStreams : int = streams.length;
			
			_target = target;
			_target.parents.push(this);
			
			_vertexStreamList = target.vertexStream is VertexStreamList
								? (target.vertexStream as VertexStreamList).clone()
								: new VertexStreamList(_target.vertexStream);
			
			for (var i : int = 0; i < numStreams; ++i)
				_vertexStreamList.pushVertexStream(streams[i] as VertexStream);
		}
		
	}
}