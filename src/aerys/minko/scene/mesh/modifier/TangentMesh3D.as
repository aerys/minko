package aerys.minko.scene.mesh.modifier
{
	import aerys.minko.scene.mesh.IMesh3D;
	import aerys.minko.type.triangle.Triangle3DIterator;
	import aerys.minko.type.triangle.Triangle3DReference;
	import aerys.minko.type.vertex.Vertex3DIterator;
	import aerys.minko.type.vertex.Vertex3DReference;
	import aerys.minko.type.vertex.format.IVertex3DFormat;
	import aerys.minko.type.vertex.format.NativeFormat;
	import aerys.minko.type.vertex.format.Vertex3DFormat;
	
	import flash.geom.Matrix3D;
	import flash.geom.Vector3D;
	
	public class TangentMesh3D extends AbstractMeshModifier3D
	{
		public static const VERTEX_FORMAT	: IVertex3DFormat	= new Vertex3DFormat(["tx", "ty", "tz", "nx", "ny", "nz", "bx", "by", "bz"],
																					 [NativeFormat.FLOAT_3, NativeFormat.FLOAT_3, NativeFormat.FLOAT_3]);
		
		public function TangentMesh3D(target : IMesh3D)
		{
			super(target);
			
			if (vertexStream.format != Vertex3DFormat.XYZ_UV)
				throw new Error("Incompatible vertex format.");
			
			updateTangentMatrices();
		}
		
		private function updateTangentMatrices() : void
		{
			var triangles	: Triangle3DIterator	= new Triangle3DIterator(vertexStream, indexStream);
			var matrices	: Vector.<Matrix3D>		= new Vector.<Matrix3D>();
			var normal		: Vector3D				= new Vector3D();
			var tangent 	: Vector3D 				= new Vector3D();
			var binormal	: Vector3D				= new Vector3D();
			var count		: Vector.<int>			= new Vector.<int>(vertexStream.length);
			
			// per triangle tangent space matrix
			for each (var triangle : Triangle3DReference in triangles)
			{
				var v0 		: Vertex3DReference = triangle.v0;
				var v1 		: Vertex3DReference = triangle.v1;
				var u		: Number			= (v1.u - v0.u) || (triangle.v2.u - v0.u);
				
				normal.x = triangle.normalX;
				normal.y = triangle.normalY;
				normal.z = triangle.normalZ;
				
				tangent.x = (v1.x - v0.x) / u;
				tangent.y = (v1.y - v0.y) / u;
				tangent.z = (v1.z - v0.z) / u;
				tangent.normalize();
				
				count[triangle.i0]++;
				count[triangle.i1]++;
				count[triangle.i2]++;
				
				binormal = tangent.crossProduct(normal);
				
				matrices.push(new Matrix3D(Vector.<Number>([tangent.x, 	tangent.y, 	tangent.z, 	0.,
															binormal.x, binormal.y, binormal.z,	0.,
															normal.x,	normal.x,	normal.z,	0.,
															0.,			0.,			0.,			1.])));
			}
			
			// per vertex tangent space matrix
			var vertices : Vertex3DIterator = new Vertex3DIterator(vertexStream);
			
			for each (var vertex : Vertex3DReference in vertices)
			{
				
			}
		}
	}
}