package aerys.minko.scene.node.mesh.modifier
{
	import aerys.minko.ns.minko;
	import aerys.minko.scene.visitor.ISceneVisitor;
	import aerys.minko.scene.node.mesh.IMesh;
	import aerys.minko.type.bounding.BoundingBox;
	import aerys.minko.type.bounding.BoundingSphere;
	import aerys.minko.type.math.Vector4;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.vertex.format.VertexComponent;
	
	public class BVHMesh extends AbstractMeshModifier
	{
		use namespace minko;
		
		private var _version		: uint				= 0;
		
		private var _boundingBox	: BoundingBox		= null;
		private var _boundingSphere	: BoundingSphere	= null;
		
		override public function get version() : uint	{ return _version; }
		
		public function BVHMesh(target : IMesh)
		{
			super(target);
		}
		
		override public function visited(query : ISceneVisitor) : void
		{
			if (target.version != _version || !_boundingBox)
			{
				var min				: Vector4			= new Vector4(Number.POSITIVE_INFINITY,
															 		  Number.POSITIVE_INFINITY,
																	  Number.POSITIVE_INFINITY);
				
				var max				: Vector4			= new Vector4(Number.NEGATIVE_INFINITY,
																	  Number.NEGATIVE_INFINITY,
																	  Number.NEGATIVE_INFINITY);
				
				var xyzStream		: VertexStream	= vertexStreamList.getVertexStreamByComponent(VertexComponent.XYZ);
				var xyzOffset		: int				= xyzStream.format.getOffsetForComponent(VertexComponent.XYZ);
				
				var dwords			: Vector.<Number>	= xyzStream._data;
				var dwordsPerVertex	: int				= xyzStream.format.dwordsPerVertex;
				var dwordsCount		: int				= dwords.length;
				
				for (var i : int = xyzOffset; i < dwordsCount; i += dwordsPerVertex)
				{
					var x : Number = dwords[i];
					var y : Number = dwords[int(i + 1)];
					var z : Number = dwords[int(i + 2)];
					
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
			
			super.visited(query);
		}
		
	}
}