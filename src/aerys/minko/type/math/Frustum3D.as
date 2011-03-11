package aerys.minko.type.math
{
	import aerys.common.Factory;
	import aerys.minko.ns.minko;
	import aerys.minko.render.state.FrustumCulling;
	import aerys.minko.transform.Transform3D;
	import aerys.minko.type.bounding.BoundingBox3D;
	import aerys.minko.type.bounding.BoundingSphere3D;
	import aerys.minko.type.bounding.IBoundedVolume3D;
	import aerys.minko.type.stream.IndexStream3D;
	import aerys.minko.type.stream.VertexStream3D;
	
	import flash.display.TriangleCulling;
	
	/**
	 * The frustum is the geometrical six-sided 3D shape which represents
	 * the visible space. This space is contained inside a truncated pyramid
	 * called a "frustum" defined with 6 planes.
	 *
	 * The frustum is defined in view space. It exposes a set a visibility
	 * check methods such as <code>testBoundingBox</code>,
	 * <code>testBoundingSphere</code> or <code>testBoundingVolume</code>.
	 * They can be used to test whether a bounding volume is inside, outside
	 * or spanning the view volume. This process is called "frustum culling".
	 * 
	 * Each visiblity check method can take up to 3 arguments:
	 * <ul>
	 * <li>
	 * The volume primitive to test.
	 * </li>
	 * <li>
	 * The 3D transform to apply to get the primitive volume into view space
	 * (optional).
	 * </li>
	 * <li>
	 * The culling bitmask that describes the sides that need to be tested
	 * (optional).
	 * </li>
	 * </ul>
	 * 
	 * <p>Bitmasks are divided into 6 quartets (or half octet or 4-uplet of
	 * bits). Each quartet stands for a side. Sides are distributed as follow:
	 * </p>
	 * <p><table class="innertable">
	 * <tr>
	 * 	<td><b>side</b></td>
	 * 	<td></td>
	 * 	<td></td>
	 * 	<td>far</td>
	 * 	<td>near</td>
	 * 	<td>bottom</td>
	 * 	<td>right</td>
	 * 	<td>top</td>
	 * 	<td>left</td>
	 * </tr>
	 * <tr>
	 * 	<td><b>bits #</b></td>
	 * 	<td>31-28</td>
	 * 	<td>27-24</td>
	 * 	<td>23-20</td>
	 * 	<td>19-16</td>
	 * 	<td>15-12</td>
	 * 	<td>11-8</td>
	 * 	<td>7-4</td>
	 * 	<td>3-0</td>
	 * </tr>
	 * </table>
	 * </p>
	 * 
	 * <p>
	 * The semantic of each quartet depends on the use of the bitmask:
	 * <ul>
	 * <li>
	 * when used as a visibility check method argument, any side with
	 * a corresponding quartet set to a non-zero value will be tested
	 * </li>
	 * <li>
	 * when returned as a visilibty check method result, any side with
	 * a corresponding quartet set to a non-zero value was tested "positive"
	 * and is spanning
	 * </li>
	 * </ul>
	 * As a result, the bitmask returned by a visiblity check method can not 
	 * have non-zero quartet if the corresponding argument bitmask quartet was
	 * set to zero: sides that are not tested can not give test results!
	 * </p>
	 * 
	 * <p>
	 * Visibility check methods use bitmasks both as arguments and return
	 * values. This way, it is easy to specify which sides must be tested and 
	 * which sides where subsequently tested positive as a result.
	 * </p>
	 * 
	 * <p>
	 * The culling bitmask of a visibility check method is build using the
	 * <code>FrustumCulling</code> enumeration.
	 * </p>
	 * 
	 * Examples:
	 * <listing>
	 * // test "sphere" against all the 6 sides of the view frustum
	 * frustum.testBoundingSphere(sphere, transform, FrustumCulling.ENABLED);
	 * 
	 * // test "sphere" against both the left and the right sides of the view frustum
	 * frustum.testBoundingSphere(sphere, transform, FrustumCulling.LEFT | FrustumCulling.RIGHT);	
	 * 
	 * // test "box" against every side of the view frustum except the far one
	 * frustum.testBoundingBox(sphere, transform, FrustumCulling.ENABLED ^ FrustumCulling.FAR);
	 * </listing>
	 * 
	 * <p>
	 * The result bitmask of a visibility check method is build as follow:
	 * </p>
	 * <ul>
	 * <li>
	 * if the volume is "inside" the view frustum, the method returns
	 * <code>Frustum.INSIDE</code>
	 * </li>
	 * <li>
	 * if the volume is "outside" the view frustum, the method returns
	 * <code>Frustum.OUTSIDE</code>
	 * </li>
	 * <li>
	 * if the volume is "spanning" the view frustum, each quartet (or half
	 * octet, or 4-uplet of bits) represents a side and is set to 0xF if the
	 * side is spanning or 0x0 otherwise:
 	 * <p>Therefore, if the tested volume spans only the right and left sides
	 * the returned bitmask will be <code>0x00000F0F</code>. If it spans all
	 * the 6 sides the returned bitmask will be <code>0x00FFFFFF</code> etc...
	 * To make it easier to work with those bitmasks, the <code>Frustum</code>
	 * class declares a set of static flags that can be used to test whether a
	 * volume spans one or more specific sides.</p>
	 * Examples:
	 * <listing>
	 * // if "volume" spans the far side when transformed with "transform" and tested against all 6 sides
	 * if (frustum.testBoundingVolume(volume, transform, FrustumCulling.ENABLED) &amp; Frustum.SPANNING_FAR)
	 * {
	 * 	// code...
	 * }
	 * 
	 * // if "sphere" spans the left side when transformed with "transform" and tested against left and right sides
	 * if (frustum.testBoundingSphere(sphere, transform, FrustumCulling.LEFT | FrustumCulling.RIGHT) &amp; Frustum.SPANNING_LEFT)
	 * {
	 * 	// code...
	 * }
	 * 
	 * // if "box" is outside the view frustum when transformed with "transform"
	 * // note: as Frustum.OUTSIDE == 0, we could test "if (!frustum.testBoundingSphere..."
	 * if (frustum.testBoundingBox(box, transform, FrustumCulling.ENABLED) == Frustum.OUTSIDE)
	 * {
	 * 	// code...
	 * }
	 * </listing>
	 * 
	 * <p>
	 * The returned bitmask can then be used as the clipping bitmask for the
	 * <code>clipTrianglePath</code> method. As a result, clipping will occur
	 * only on the spanning planes.
	 * </p>
	 * 
	 * Example:
	 * <listing>
	 * // check the visibility of "volume" and clip "path" against the apropriate sides:
	 * // return true if the volume is visible and the path should be drawn, false otherwise
	 * public function testAndClip(frustum : Frustum, volume : IBoundedVolume, path : GraphicsTrianglePath) : Boolean
	 * {
	 * 	var culling : uint = frustum.testBoundingVolume(volume, transform, FrustumCulling.ENABLED);
	 * 
	 * 	// if culling == Frustum.OUTSIDE == 0
	 * 	if (!culling)
	 * 		return false; // the volume is not visible
	 * 
	 * 	// clip the triangle path only on the spanning planes
	 * 	return frustum.clipTrianglePath(path, culling);
	 * }
	 * </listing>
	 * </li>
	 * </ul>
	 * 
	 * @see FrustumCulling
	 * @see FrustumClipping
	 *
	 * @author Jean-Marc Le Roux
	 *
	 */
	public class Frustum3D
	{
		use namespace minko;
		
		public static const OUTSIDE				: uint		= 0x00000000;
		public static const INSIDE				: uint		= 0xf0000000;
		
		minko static const LEFT					: int		= 0;
		minko static const TOP					: int		= 1;
		minko static const RIGHT				: int		= 2;
		minko static const BOTTOM				: int		= 3;
		minko static const NEAR					: int		= 4;
		minko static const FAR					: int		= 5;

		minko static const CULLING_SPHERE		: uint		= 0x00000001;
		minko static const CULLING_BOX			: uint		= 0x00000002;
		
		minko static const CLIPPING				: uint		= 0x00000001;
		minko static const CLIPPING_IGNORE		: uint		= 0x00000002;
		minko static const CLIPPING_DISCARD		: uint		= 0x00000004;

		private static const SPANNING			: uint		= 0x0000000f;

		public static const SPANNING_LEFT		: uint		= SPANNING << (LEFT << 2);
		public static const SPANNING_TOP		: uint		= SPANNING << (TOP << 2);
		public static const SPANNING_RIGHT		: uint		= SPANNING << (RIGHT << 2);
		public static const SPANNING_BOTTOM		: uint		= SPANNING << (BOTTOM << 2);
		public static const SPANNING_NEAR		: uint		= SPANNING << (NEAR << 2);
		public static const SPANNING_FAR		: uint		= SPANNING << (FAR << 2);

		private static const VERTEX_BUFFER		: Factory	= Factory.getFactory(VertexStream3D);
		private static const INDICES			: Factory	= Factory.getFactory(IndexStream3D);
		
		private static const THICKNESS			: Number	= .0001;
				
		private static const POINT_BEHIND		: uint		= Plane3D.POINT_BEHIND;
		private static const POINT_COINCIDING	: uint		= Plane3D.POINT_COINCIDING;
		private static const POINT_INFRONT		: uint		= Plane3D.POINT_INFRONT;
		
		private static const POLYGON_COINCIDING	: uint		= Plane3D.POLYGON_COINCIDING;
		private static const POLYGON_BEHIND		: uint		= Plane3D.POLYGON_BEHIND;
		private static const POLYGON_INFRONT	: uint		= Plane3D.POLYGON_INFRONT;
		
		private static const A_BEHIND			: uint		= Plane3D.A_BEHIND;
		private static const B_BEHIND			: uint		= Plane3D.B_BEHIND;
		private static const C_BEHIND			: uint		= Plane3D.C_BEHIND;
		
		private static const A_COINCIDING		: uint		= Plane3D.A_COINCIDING;
		private static const B_COINCIDING		: uint		= Plane3D.B_COINCIDING;
		private static const C_COINCIDING		: uint		= Plane3D.C_COINCIDING;
		
		private static const AB_INFRONT			: uint		= Plane3D.A_INFRONT | Plane3D.B_INFRONT | Plane3D.C_BEHIND;
		private static const BC_INFRONT			: uint		= Plane3D.A_BEHIND | Plane3D.B_INFRONT | Plane3D.C_INFRONT;
		private static const CA_INFRONT			: uint		= Plane3D.A_INFRONT | Plane3D.B_BEHIND | Plane3D.C_INFRONT;
		
		private static const AB_BEHIND			: uint		= Plane3D.A_BEHIND | Plane3D.B_BEHIND | Plane3D.C_INFRONT;
		private static const BC_BEHIND			: uint		= Plane3D.A_INFRONT | Plane3D.B_BEHIND | Plane3D.C_BEHIND;
		private static const CA_BEHIND			: uint		= Plane3D.A_BEHIND | Plane3D.B_INFRONT | Plane3D.C_BEHIND;

		private static const CULLING_POSITIVE	: String	= TriangleCulling.POSITIVE;
		private static const CULLING_NEGATIVE	: String	= TriangleCulling.NEGATIVE;
		
		minko var _planes			: Vector.<Plane3D>	= new Vector.<Plane3D>(6, true);
		
		private var _boxVertices	: Vector.<Number>	= new Vector.<Number>();
		
		/**
		 * Update the frustum planes according to the specified project matrix.
		 *
		 * @param myTransform
		 *
		 */
		public function update(matrix : Matrix4x4) : void
		{
			var data	: Vector.<Number>	= matrix.getRawData();
			
			_planes[RIGHT] = new Plane3D(data[3] - data[0],
									     data[7] - data[4],
									     data[11] - data[8],
									     data[12] - data[15]);
			
			_planes[LEFT] = new Plane3D(data[3] + data[0],
									    data[7] + data[4],
									    data[11] + data[8],
									    -data[15] - data[12]);
			
			_planes[TOP] = new Plane3D(data[3] - data[1],
									   data[7] - data[5],
									   data[11] - data[9],
									   data[13] + data[15]);
			
			_planes[BOTTOM] = new Plane3D(data[3] + data[1],
									      data[7] + data[5],
									      data[11] + data[9],
									      -data[15] - data[13]);
			
			_planes[NEAR] = new Plane3D(data[2],
									    data[6],
									    data[10],
									    -data[14]);
			
			_planes[FAR] = new Plane3D(data[3] - data[2],
									   data[7] - data[6],
									   data[11] - data[10],
									   data[14] - data[15]);
		}
		
		/**
		 * Test whether a 3D point is inside or outside the frustum.
		 * 
		 * @param myVector The 3D position to test.
		 * @param myTransform The 3D transform to apply to the 3D position before testing it.
		 * @param myMask The culling mask.
		 * @return 
		 * 
		 */
		public function testVector(vector 		: Vector4,
								   transform	: Matrix4x4 	= null,
								   mask			: int 			= 0xffffff) : uint
		{
			var result	: uint 		= 0;
			var p 		: Plane3D	= null;
			var d		: Number	= 0.;
			
			if (transform)
				vector = transform.multiplyVector(vector);
			
			for (var i : int = 0; i < 6; ++i)
			{
				if (((CULLING_SPHERE << (i << 2)) & mask) == 0)
					continue ;
				
				p = _planes[i];
				d = p._a * vector.x + p._b * vector.y + p._c * vector.z - p._d;
				
				if (d < 0.)
					return OUTSIDE;
			}
			
			return INSIDE;
		}
		
		/**
		 * Test wether a bounding sphere is inside, outside or spanning the planes of the frustum.
		 *
		 * @param mySphere The bouding sphere to test.
		 * @param myTransform The world transform matrix of the corresponding IObject3D.
		 * @param myCulling The bitmask that describes the planes to test.
		 * @return A bitmask where each plane test is store in a 4-bits value.
		 */
		public function testBoundingSphere(sphere 		: BoundingSphere3D,
										   transform	: Transform3D	= null,
										   culling		: int 			= 0xffffff) : int
		{
			var center	: Vector4	= sphere.center;
			var radius	: Number	= sphere.radius;
			var result	: int		= 0;
			var p 		: Plane3D	= null;
			var d 		: Number	= 0.;
		
			if (!culling)
				return INSIDE;
			
			if (transform != null)
			{
				var scale	: Vector4	= transform.scale;

				center = transform.multiplyVector(sphere.center);
				radius = Math.max(radius * scale.x, radius * scale.y, radius * scale.z)
			}
						
			for (var i : int = 0; i < 6; ++i)
			{
				if (((CULLING_SPHERE << (i << 2)) & culling) == 0)
					continue ;
				
				p = _planes[i];
				d = p._a * center.x + p._b * center.y + p._c * center.z - p._d;
				
				if (d + radius < 0.0)
					return OUTSIDE;
				else if (d - radius <= 0.0)
					result |= SPANNING << (i << 2);
			}
			
			return result || INSIDE;
		}
		
		/**
		 * Test wether a bounding sphere is inside, outside or spanning the planes of the frustum.
		 *
		 * @param myBox The bouding box to test.
		 * @param myTransform The world transform matrix of the corresponding IObject3D.
		 * @param myCulling The bitmask that describes the planes to test.
		 * @return A bitmask where each plane test is store in a 4-bits value.
		 *
		 */
		public function testBoundingBox(box			: BoundingBox3D,
										transform	: Matrix4x4 	= null,
										culling		: int			= 0xffffff) : uint
		{
			var result		: uint				= 0;
			var count		: int				= 0;
			var vertices	: Vector.<Number>	= box._vertices;
			var p			: Plane3D			= null;
			
			if (!culling)
				return INSIDE;
			
			// transform vertices
			if (transform != null)
			{
				_boxVertices.length = 0;
				transform.multiplyRawVectors(vertices, _boxVertices);
				vertices = _boxVertices;
			}
			
			for (var i : int = 0; i < 6; ++i)
			{
				if (((CULLING_BOX << (i << 2)) & culling) == 0)
					continue ;
				
				p = _planes[i];
				
				// test vertices
				count = p.testPoint(vertices[0], vertices[1], vertices[2]) != POINT_INFRONT ? -1 : 1;
				count += p.testPoint(vertices[3], vertices[4], vertices[5]) != POINT_INFRONT ? -1 : 1;
				count += p.testPoint(vertices[6], vertices[7], vertices[8]) != POINT_INFRONT ? -1 : 1;
				count += p.testPoint(vertices[9], vertices[10], vertices[11]) != POINT_INFRONT ? -1 : 1;
				count += p.testPoint(vertices[12], vertices[13], vertices[14]) != POINT_INFRONT ? -1 : 1;
				count += p.testPoint(vertices[15], vertices[16], vertices[17]) != POINT_INFRONT ? -1 : 1;
				count += p.testPoint(vertices[18], vertices[19], vertices[20]) != POINT_INFRONT ? -1 : 1;
				count += p.testPoint(vertices[21], vertices[22], vertices[23]) != POINT_INFRONT ? -1 : 1;
				
				if (count == -8)
					return OUTSIDE;
				else if (count != 8)
					result |= SPANNING << (i << 2);
			}
			
			return result || INSIDE;
		}
		
		/**
		 * Test both the bounding sphere and the bouding box of a IBoundedVolume object with the
		 * specified 3D transform against the specified sides.
		 * 
		 * The bounding sphere is tested first. If spanning, the bounding box is tested against
		 * the spanning sides only. The result is a bitmask with the following possible values:
		 * <ul>
		 * <li>if the IBoundedVolume is "inside" the view frustum, the method returns <code>Frustum.INSIDE</code></li>
		 * <li>if the IBoundedVolume is "outside" the view frustum, the method returns <code>Frustum.OUTSIDE</code></li>
		 * <li>if the IBoundedVolume is "spanning" the view frustum, each of the last 6 quartets are set to
		 * 0xf if the corresponding side is spanning and 0 otherwise.</li>
		 * </ul>
		 * 
		 * Example:
		 * <code>
		 * // if "volume" spans the far plane when transformed with "transform"
		 * if (frustum.testBoundingVolume(volume, transform, FrustumCulling.ENABLED) Frustum.SPANNING_FAR)
		 * {
		 * 	// code...
		 * }
		 * </code>
		 * 
		 * @param myVolume The IBoundedVolume object to test.
		 * @param myTransform The local-to-view space tranfrom to use.
		 * @param myCulling The bitmask of the sides to test.
		 * 
		 * @return A bitmask describing whether the volume is inside/outside the view frustum or
		 * the spanning planes.
		 * 
		 */
		public function testBoundedVolume(volume	: IBoundedVolume3D,
								 	      transform	: Matrix4x4	= null,
								 		  culling	: int 			= 0xffffff) : uint
		{
			var box		: BoundingBox3D		= volume.boundingBox;
			
			if (!culling)
				return INSIDE;
			
			if ((culling & FrustumCulling.SPHERE) == 0)
				return testBoundingBox(box, transform, culling);
			
			var sphere		: BoundingSphere3D	= volume.boundingSphere;
			var center		: Vector4			= sphere.center;
			//var scale		: Vector3D			= transform.decompose()[2];
			var radius		: Number			= sphere.radius;
			var result		: int				= 0;
			var vertices	: Vector.<Number>	= transform ? null : box._vertices;
			
			//radius = Math.max(radius * scale.x, radius * scale.y, radius * scale.z)
			
			if (transform)
				center = transform.multiplyVector(center);
			
			for (var i : int = 0; i < 6; i++)
			{
				var p 	: Plane3D		= _planes[i];
				var d 	: Number	= 0;
				
				// bounding sphere
				if (((CULLING_SPHERE << (i << 2)) & culling))
				{
					d = p._a * center.x + p._b * center.y + p._c * center.z - p._d;
					
					if (d + radius < .0)
						return OUTSIDE;
					else if (d - radius > .0)
						continue ;
				}

				// bounding box
				if ((CULLING_BOX << (i << 2)) & culling)
				{
					var count	: int	= 0;
					var test	: uint	= 0;
					
					// transform vertices
					if (vertices == null)
					{
						vertices = _boxVertices;
						vertices.length = 0;
						transform.multiplyRawVectors(box._vertices, vertices);
					}
					
					// test vertices
					count = p.testPoint(vertices[0], vertices[1], vertices[2]) != POINT_INFRONT ? -1 : 1;
					count += p.testPoint(vertices[3], vertices[4], vertices[5]) != POINT_INFRONT ? -1 : 1;
					count += p.testPoint(vertices[6], vertices[7], vertices[8]) != POINT_INFRONT ? -1 : 1;
					count += p.testPoint(vertices[9], vertices[10], vertices[11]) != POINT_INFRONT ? -1 : 1;
					count += p.testPoint(vertices[12], vertices[13], vertices[14]) != POINT_INFRONT ? -1 : 1;
					count += p.testPoint(vertices[15], vertices[16], vertices[17]) != POINT_INFRONT ? -1 : 1;
					count += p.testPoint(vertices[18], vertices[19], vertices[20]) != POINT_INFRONT ? -1 : 1;
					count += p.testPoint(vertices[21], vertices[22], vertices[23]) != POINT_INFRONT ? -1 : 1;
					
					if (count == -8)
						return OUTSIDE;
					else if (count != 8)
						result |= SPANNING << (i << 2);
				}
				else
				{
					result |= SPANNING << (i << 2);
				}
			}
			
			return result || INSIDE;
		}
	}
}