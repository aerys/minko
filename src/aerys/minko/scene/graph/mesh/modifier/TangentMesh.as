package aerys.minko.scene.graph.mesh.modifier
{
	import aerys.minko.scene.graph.mesh.IMesh;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.triangle.TriangleIterator;
	import aerys.minko.type.triangle.TriangleReference;
	import aerys.minko.type.vertex.VertexIterator;
	import aerys.minko.type.vertex.VertexReference;
	import aerys.minko.type.vertex.format.NativeFormat;
	import aerys.minko.type.vertex.format.VertexComponent;
	import aerys.minko.type.vertex.format.PackedVertexFormat;
	
	import flash.geom.Matrix3D;
	import flash.geom.Vector3D;
	
	public class TangentMesh extends AbstractMeshModifier
	{
		public static const VERTEX_FORMAT	: PackedVertexFormat	=
			new PackedVertexFormat([
				new VertexComponent(["tx", "ty", "tz"], NativeFormat.FLOAT_3),
				new VertexComponent(["nx", "ny", "nz"], NativeFormat.FLOAT_3),
				new VertexComponent(["bx", "by", "bz"], NativeFormat.FLOAT_3)
			]);
		
		public function TangentMesh(target : IMesh)
		{
			super(target);
//			
//			if (vertexStream.format != Vertex3DFormat.XYZ_UV)
//				throw new Error("Incompatible vertex format.");
//			
//			updateTangentMatrices();
		}
//		
//		private function updateTangentMatrices() : void
//		{
//			var triangles	: Triangle3DIterator	= new Triangle3DIterator(vertexStream, indexStream);
//			var matrices	: Vector.<Matrix4x4>	= new Vector.<Matrix4x4>();
//			var normal		: Vector3D				= new Vector3D();
//			var tangent 	: Vector3D 				= new Vector3D();
//			var binormal	: Vector3D				= new Vector3D();
//			var count		: Vector.<int>			= new Vector.<int>(vertexStream.length);
//			
//			// per triangle tangent space matrix
//			for each (var triangle : Triangle3DReference in triangles)
//			{
//				var v0 		: Vertex3DReference = triangle.v0;
//				var v1 		: Vertex3DReference = triangle.v1;
//				var u		: Number			= (v1.u - v0.u) || (triangle.v2.u - v0.u);
//				
//				normal.x = triangle.normalX;
//				normal.y = triangle.normalY;
//				normal.z = triangle.normalZ;
//				
//				tangent.x = (v1.x - v0.x) / u;
//				tangent.y = (v1.y - v0.y) / u;
//				tangent.z = (v1.z - v0.z) / u;
//				tangent.normalize();
//				
//				count[triangle.i0]++;
//				count[triangle.i1]++;
//				count[triangle.i2]++;
//				
//				binormal = tangent.crossProduct(normal);
//				
//				matrices.push(new Matrix4x4(tangent.x, 	tangent.y, 	tangent.z, 	0.,
//											binormal.x, binormal.y, binormal.z,	0.,
//											normal.x,	normal.x,	normal.z,	0.,
//											0.,			0.,			0.,			1.));
//			}
//			
//			// per vertex tangent space matrix
//			var vertices : Vertex3DIterator = new Vertex3DIterator(vertexStream);
//			
//			for each (var vertex : Vertex3DReference in vertices)
//			{
//				
//			}
//		}
	}
}