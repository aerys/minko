package aerys.minko.type.math
{
	import aerys.minko.ns.minko_math;
	import aerys.minko.type.bounding.BoundingBox;
	import aerys.minko.type.bounding.BoundingSphere;
	import aerys.minko.type.bounding.FrustumCulling;
	import aerys.minko.type.bounding.IBoundingVolume;

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
	 *
	 * @author Jean-Marc Le Roux
	 *
	 */
	public final class Frustum
	{
		use namespace minko_math;
		
		public static const OUTSIDE				: uint				= 0x00000000;
		public static const INSIDE				: uint				= 0xf0000000;

		private static const LEFT				: int				= 0;
		private static const TOP				: int				= 1;
		private static const RIGHT				: int				= 2;
		private static const BOTTOM				: int				= 3;
		private static const NEAR				: int				= 4;
		private static const FAR				: int				= 5;

		private static const CULLING_SPHERE		: uint				= 0x00000001;
		private static const CULLING_BOX		: uint				= 0x00000002;

		private static const CLIPPING			: uint				= 0x00000001;
		private static const CLIPPING_IGNORE	: uint				= 0x00000002;
		private static const CLIPPING_DISCARD	: uint				= 0x00000004;

		private static const SPANNING			: uint				= 0x0000000f;

		public static const SPANNING_LEFT		: uint				= SPANNING << (LEFT << 2);
		public static const SPANNING_TOP		: uint				= SPANNING << (TOP << 2);
		public static const SPANNING_RIGHT		: uint				= SPANNING << (RIGHT << 2);
		public static const SPANNING_BOTTOM		: uint				= SPANNING << (BOTTOM << 2);
		public static const SPANNING_NEAR		: uint				= SPANNING << (NEAR << 2);
		public static const SPANNING_FAR		: uint				= SPANNING << (FAR << 2);

		private static const TMP_DATA			: Vector.<Number>	= new Vector.<Number>();

		private static const THICKNESS			: Number			= 0.01;

		private var _planes			: Vector.<Plane>	= new Vector.<Plane>(6, true);
		private var _points			: Vector.<Vector4>;
		
		private var _boxVertices	: Vector.<Number>	= new Vector.<Number>();

		public function get points() : Vector.<Vector4>
		{
			if (_points == null)
			{
				_points = new Vector.<Vector4>(8, true);
				_points[0] = _planes[NEAR].intersectWith(_planes[LEFT], _planes[TOP]);
				_points[1] = _planes[NEAR].intersectWith(_planes[RIGHT], _planes[TOP]);
				_points[2] = _planes[NEAR].intersectWith(_planes[RIGHT], _planes[BOTTOM]);
				_points[3] = _planes[NEAR].intersectWith(_planes[LEFT], _planes[BOTTOM]);
				_points[4] = _planes[FAR].intersectWith(_planes[LEFT], _planes[TOP]);
				_points[5] = _planes[FAR].intersectWith(_planes[RIGHT], _planes[TOP]);
				_points[6] = _planes[FAR].intersectWith(_planes[RIGHT], _planes[BOTTOM]);
				_points[7] = _planes[FAR].intersectWith(_planes[LEFT], _planes[BOTTOM]);
			}
			
			return _points;
		}
		
		public function updateFromDescription(fov	: Number,
											  ratio	: Number,
											  zNear	: Number,
											  zFar 	: Number) : void
		{
			var	y_scale		: Number	= 1. / Math.tan(fov * .5);
			var	x_scale		: Number	= y_scale / ratio;
			var	m33			: Number	= zFar / (zFar - zNear);
			var	m43			: Number	= -zNear * zFar / (zFar - zNear);

			_planes[RIGHT]	= new Plane(-x_scale,	0,			1,			0);
			_planes[LEFT]	= new Plane(x_scale,	0,			1,			0);
			_planes[TOP]	= new Plane(0,			-y_scale,	1,			0);
			_planes[BOTTOM] = new Plane(0,			y_scale,	1,			0);
			_planes[NEAR]	= new Plane(0,			0,			m33,		-m43);
			_planes[FAR]	= new Plane(0,			0,			1 - m33,	m43);
			_points			= null;
		}

		/**
		 * Update the frustum planes according to the specified project matrix.
		 *
		 * @param myTransform
		 *
		 */
		public function updateFromMatrix(matrix : Matrix4x4) : void
		{
			var data	: Vector.<Number>	= matrix.getRawData(TMP_DATA);

			_planes[RIGHT] = new Plane(data[3] - data[0],
									   data[7] - data[4],
									   data[11] - data[8],
									   data[12] - data[15]);

			_planes[LEFT] = new Plane(data[3] + data[0],
									  data[7] + data[4],
									  data[11] + data[8],
									  -data[15] - data[12]);

			_planes[TOP] = new Plane(data[3] - data[1],
									 data[7] - data[5],
									 data[11] - data[9],
									 data[13] + data[15]);

			_planes[BOTTOM] = new Plane(data[3] + data[1],
									    data[7] + data[5],
									    data[11] + data[9],
									    -data[15] - data[13]);

			_planes[NEAR] = new Plane(data[2],
									  data[6],
									  data[10],
									  -data[14]);

			_planes[FAR] = new Plane(data[3] - data[2],
									 data[7] - data[6],
									 data[11] - data[10],
									 data[14] - data[15]);
			
			_points			= null;
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
			var p 		: Plane	= null;
			var d		: Number	= 0.;

			if (transform)
				vector = transform.transformVector(vector);

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
		public function testBoundingSphere(sphere 		: BoundingSphere,
										   transform	: Matrix4x4	= null,
										   culling		: int 		= 0xffffff) : int
		{
			var center	: Vector4	= sphere.center;
			var radius	: Number	= sphere.radius;
			var result	: int		= 0;
			var p 		: Plane	= null;
			var d 		: Number	= 0.;

			if (!culling)
				return INSIDE;

			if (transform != null)
			{
				var scale	: Vector4	= transform.deltaTransformVector(Vector4.ONE);

				center = transform.transformVector(sphere.center);
				radius *= Math.max(Math.abs(scale.x), Math.abs(scale.y), Math.abs(scale.z));
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
		public function testBoundingBox(box			: BoundingBox,
										transform	: Matrix4x4 	= null,
										culling		: int			= 0xffffffff) : uint
		{
			var result		: uint				= 0;
			var count		: int				= 0;
			var vertices	: Vector.<Number>	= box._vertices;
			var p			: Plane				= null;

			if (!culling)
				return INSIDE;

			// transform vertices
			if (transform != null)
			{
				_boxVertices.length = 0;
				transform.transformRawVectors(vertices, _boxVertices);
				vertices = _boxVertices;
			}

			var x1	: Number	= vertices[0];
			var y1	: Number	= vertices[1];
			var z1	: Number	= vertices[2];
			var x2	: Number	= vertices[3];
			var y2	: Number	= vertices[4];
			var z2	: Number	= vertices[5];
			var x3	: Number	= vertices[6];
			var y3	: Number	= vertices[7];
			var z3	: Number	= vertices[8];
			var x4	: Number	= vertices[9];
			var y4	: Number	= vertices[10];
			var z4	: Number	= vertices[11];
			var x5	: Number	= vertices[12];
			var y5	: Number	= vertices[13];
			var z5	: Number	= vertices[14];
			var x6	: Number	= vertices[15];
			var y6	: Number	= vertices[16];
			var z6	: Number	= vertices[17];
			var x7	: Number	= vertices[18];
			var y7	: Number	= vertices[19];
			var z7	: Number	= vertices[20];
			var x8	: Number	= vertices[21];
			var y8	: Number	= vertices[22];
			var z8	: Number	= vertices[23];

			for (var i : int = 0; i < 6; ++i)
			{
				if (((CULLING_BOX << (i << 2)) & culling) == 0)
					continue ;

				p = _planes[i];

				var pa	: Number	= p.a;
				var pb	: Number	= p.b;
				var pc	: Number	= p.c;
				var pd	: Number	= p.d;

				// test vertices
				count = pa * x1 + pb * y1 + pc * z1 - pd > THICKNESS ? 1 : -1;
				count += pa * x2 + pb * y2 + pc * z2 - pd > THICKNESS ? 1 : -1;
				count += pa * x3 + pb * y3 + pc * z3 - pd > THICKNESS ? 1 : -1;
				count += pa * x4 + pb * y4 + pc * z4 - pd > THICKNESS ? 1 : -1;
				count += pa * x5 + pb * y5 + pc * z5 - pd > THICKNESS ? 1 : -1;
				count += pa * x6 + pb * y6 + pc * z6 - pd > THICKNESS ? 1 : -1;
				count += pa * x7 + pb * y7 + pc * z7 - pd > THICKNESS ? 1 : -1;
				count += pa * x8 + pb * y8 + pc * z8 - pd > THICKNESS ? 1 : -1;

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
		public function testBoundedVolume(volume		: IBoundingVolume,
								 	      transform		: Matrix4x4	= null,
								 		  cullingMask	: int 		= 0xffffffff) : uint
		{
			cullingMask = cullingMask & volume.frustumCulling;

			var box		: BoundingBox		= volume.boundingBox;

			if (!cullingMask)
				return INSIDE;

			if ((cullingMask & FrustumCulling.SPHERE) == 0)
				return testBoundingBox(box, transform, cullingMask);

			var sphere		: BoundingSphere	= volume.boundingSphere;
			var center		: Vector4			= sphere.center;
			var radius		: Number			= sphere.radius;
			var result		: int				= 0;
			var vertices	: Vector.<Number>	= box._vertices;
			var count		: int				= 0;

			if ((cullingMask & FrustumCulling.BOX) != 0 && transform != null)
			{
				vertices = _boxVertices;
				vertices.length = 0;
				transform.transformRawVectors(box._vertices, vertices);
			}

			var x1	: Number	= vertices[0];
			var y1	: Number	= vertices[1];
			var z1	: Number	= vertices[2];
			var x2	: Number	= vertices[3];
			var y2	: Number	= vertices[4];
			var z2	: Number	= vertices[5];
			var x3	: Number	= vertices[6];
			var y3	: Number	= vertices[7];
			var z3	: Number	= vertices[8];
			var x4	: Number	= vertices[9];
			var y4	: Number	= vertices[10];
			var z4	: Number	= vertices[11];
			var x5	: Number	= vertices[12];
			var y5	: Number	= vertices[13];
			var z5	: Number	= vertices[14];
			var x6	: Number	= vertices[15];
			var y6	: Number	= vertices[16];
			var z6	: Number	= vertices[17];
			var x7	: Number	= vertices[18];
			var y7	: Number	= vertices[19];
			var z7	: Number	= vertices[20];
			var x8	: Number	= vertices[21];
			var y8	: Number	= vertices[22];
			var z8	: Number	= vertices[23];

			if (transform)
			{
				var scale	: Vector4	= transform.deltaTransformVector(Vector4.ONE);

				radius *= Math.max(Math.abs(scale.x), Math.abs(scale.y), Math.abs(scale.z));
				center = transform.transformVector(center);
			}

			for (var i : int = 0; i < 6; i++)
			{
				var p 	: Plane		= _planes[i];
				var d 	: Number	= 0;

				// bounding sphere
				if (((CULLING_SPHERE << (i << 2)) & cullingMask))
				{
					d = p._a * center.x + p._b * center.y + p._c * center.z - p._d;

					if (d + radius < .0)
						return OUTSIDE;
					else if (d - radius > .0)
						continue ;
				}

				// bounding box
				if ((CULLING_BOX << (i << 2)) & cullingMask)
				{
					var pa	: Number	= p.a;
					var pb	: Number	= p.b;
					var pc	: Number	= p.c;
					var pd	: Number	= p.d;

					// test vertices
					count += pa * x1 + pb * y1 + pc * z1 - pd > THICKNESS ? 1 : -1;
					count += pa * x2 + pb * y2 + pc * z2 - pd > THICKNESS ? 1 : -1;
					count += pa * x3 + pb * y3 + pc * z3 - pd > THICKNESS ? 1 : -1;
					count += pa * x4 + pb * y4 + pc * z4 - pd > THICKNESS ? 1 : -1;
					count += pa * x5 + pb * y5 + pc * z5 - pd > THICKNESS ? 1 : -1;
					count += pa * x6 + pb * y6 + pc * z6 - pd > THICKNESS ? 1 : -1;
					count += pa * x7 + pb * y7 + pc * z7 - pd > THICKNESS ? 1 : -1;
					count += pa * x8 + pb * y8 + pc * z8 - pd > THICKNESS ? 1 : -1;

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
		
		public function toProjectionMatrix(out : Matrix4x4 = null) : Matrix4x4
		{
			var r	: Plane = _planes[RIGHT];
			var l	: Plane = _planes[LEFT];
			var t	: Plane = _planes[TOP];
			var b	: Plane = _planes[BOTTOM];
			var n	: Plane = _planes[NEAR];
			var f	: Plane = _planes[FAR];

			var d0	: Number = (l.a - r.a) / 2;
			var d1	: Number = (b.a - t.a) / 2;
			var d2	: Number = n.a;
			var d3	: Number = (b.a + t.a) / 2;
			var d4	: Number = (l.b - r.b) / 2;
			var d5	: Number = (b.b - t.b) / 2;
			var d6	: Number = n.b;
			var d7	: Number = (b.b + t.b) / 2;
			var d8	: Number = (l.c - r.c) / 2;
			var d9	: Number = (b.c - t.c) / 2;
			var d10	: Number = n.c;
			var d11	: Number = f.c + n.c;
			var d12	: Number = (r.d - l.d) / 2;
			var d13	: Number = t.d - (l.d + r.d) / 2;
			var d14	: Number = -n.d;
			var d15	: Number = -(l.d - r.d) / 2;
			
			out ||= new Matrix4x4();
			out.setRawData(Vector.<Number>([ 
				d0,		d1, 	d2, 	d3,
				d4, 	d5, 	d6, 	d7,
				d8, 	d9, 	d10, 	d11,
				d12,	d13,	d14,	d15
			]));

			return out;
		}
		
	}
}