package aerys.minko.scene.mesh.modifier
{
	import aerys.minko.ns.minko;
	import aerys.minko.query.IScene3DQuery;
	import aerys.minko.scene.mesh.IMesh3D;
	import aerys.minko.type.bounding.BoundingBox3D;
	import aerys.minko.type.bounding.BoundingSphere3D;
	import aerys.minko.type.math.Vector4;
	import aerys.minko.type.stream.VertexStream3D;
	import aerys.minko.type.vertex.format.Vertex3DComponent;
	
	public class BVHMesh extends AbstractMeshModifier3D
	{
		use namespace minko;
		
		private var _version		: uint				= 0;
		
		private var _boundingBox	: BoundingBox3D		= null;
		private var _boundingSphere	: BoundingSphere3D	= null;
		
		override public function get version() : uint	{ return _version; }
		
		public function BVHMesh(target : IMesh3D)
		{
			super(target);
		}
		
		override public function accept(query : IScene3DQuery) : void
		{
			if (target.version != _version || !_boundingBox)
			{
				_version = target.version;
				
				var min				: Vector4			= new Vector4(Number.POSITIVE_INFINITY,
															 		  Number.POSITIVE_INFINITY,
																	  Number.POSITIVE_INFINITY);
				
				var max				: Vector4			= new Vector4(Number.NEGATIVE_INFINITY,
																	  Number.NEGATIVE_INFINITY,
																	  Number.NEGATIVE_INFINITY);
				
				var xyzStream		: VertexStream3D	= vertexStreamList.getComponentStream(Vertex3DComponent.XYZ);
				var xyzOffset		: int				= xyzStream.format.getOffsetForComponent(Vertex3DComponent.XYZ);
				
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
				
				_boundingSphere = BoundingSphere3D.fromMinMax(min, max);
				_boundingBox = new BoundingBox3D(min, max);
			}
			
			super.accept(query);
		}
		
	}
}