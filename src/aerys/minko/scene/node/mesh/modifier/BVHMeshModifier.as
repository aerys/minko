package aerys.minko.scene.node.mesh.modifier
{
	import aerys.minko.ns.minko_stream;
	import aerys.minko.render.renderer.state.FrustumCulling;
	import aerys.minko.scene.action.BoundingVolumeAction;
	import aerys.minko.scene.node.mesh.IMesh;
	import aerys.minko.scene.visitor.ISceneVisitor;
	import aerys.minko.type.bounding.BoundingBox;
	import aerys.minko.type.bounding.BoundingSphere;
	import aerys.minko.type.bounding.IBoundingVolume;
	import aerys.minko.type.math.Vector4;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.vertex.format.VertexComponent;
	
	public class BVHMeshModifier extends AbstractMeshModifier implements IBoundingVolume
	{
		use namespace minko_stream;
		
		private var _version		: uint				= 0;
		
		private var _boundingBox	: BoundingBox		= null;
		private var _boundingSphere	: BoundingSphere	= null;
		private var _frustumCulling	: uint				= 0;
		
		override public function get version() : uint	{ return super.version + _version; }
		
		public function get boundingBox()		: BoundingBox		{ return _boundingBox; }
		public function get boundingSphere()	: BoundingSphere	{ return _boundingSphere; }
		public function get frustumCulling()	: uint				{ return _frustumCulling; }
		
		public function set frustumCulling(value : uint) : void
		{
			_frustumCulling = value;
			++_version;
		}
		
		public function BVHMeshModifier(target : IMesh)
		{
			super(target);
			
			_frustumCulling = FrustumCulling.ENABLED;
			actions[0] = new BoundingVolumeAction();
			
			initialize();
		}
		
		private function initialize() : void
		{
			//if (target.version != _version || !_boundingBox)
			{
				var min				: Vector4			= new Vector4(Number.POSITIVE_INFINITY,
															 		  Number.POSITIVE_INFINITY,
																	  Number.POSITIVE_INFINITY);
				
				var max				: Vector4			= new Vector4(Number.NEGATIVE_INFINITY,
																	  Number.NEGATIVE_INFINITY,
																	  Number.NEGATIVE_INFINITY);
				
				var xyzStream		: VertexStream		= vertexStreamList.getVertexStreamByComponent(VertexComponent.XYZ);
				var xyzOffset		: int				= xyzStream.format.getOffsetForComponent(VertexComponent.XYZ);
				
				var xyz				: Vector.<Number>	= xyzStream._data;
				var dwordsPerVertex	: int				= xyzStream.format.dwordsPerVertex;
				var dwordsCount		: int				= xyz.length;
				
				for (var i : int = xyzOffset; i < dwordsCount; i += dwordsPerVertex)
				{
					var x : Number = xyz[i];
					var y : Number = xyz[int(i + 1)];
					var z : Number = xyz[int(i + 2)];
					
					min.x = x < min.x ? x : min.x;
					min.y = y < min.y ? y : min.y;
					min.z = z < min.z ? z : min.z;
					
					max.x = x > max.x ? x : max.x;
					max.y = y > max.y ? y : max.y;
					max.z = z > max.z ? z : max.z;
				}
				
				_boundingSphere = BoundingSphere.fromMinMax(min, max);
				_boundingBox = new BoundingBox(min, max);
				
				_version = target.version;
			}
		}
		
	}
}