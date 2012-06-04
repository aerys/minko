package aerys.minko.type.math
{
	import aerys.minko.ns.minko_math;
	import aerys.minko.type.Factory;
	import aerys.minko.type.Signal;
	import aerys.minko.type.data.DataProvider;
	import aerys.minko.type.data.IDataProvider;
	
	import flash.geom.Matrix3D;
	import flash.geom.Utils3D;
	import flash.geom.Vector3D;

	use namespace minko_math;
		
	public final class Matrix4x4 implements IDataProvider
	{
		private static const FACTORY			: Factory			= Factory.getFactory(Matrix4x4);
		private static const RAD2DEG			: Number			= 180. / Math.PI;
		private static const DEG2RAD			: Number			= Math.PI / 180.;
		private static const EPSILON			: Number			= 1e-100;

		private static const TMP_VECTOR			: Vector.<Number>	= new Vector.<Number>();
		private static const TMP_VECTOR4		: Vector4			= new Vector4();
		private static const TMP_MATRIX			: Matrix4x4			= new Matrix4x4();
		
		private static const UPDATE_NONE		: uint				= 0;
		private static const UPDATE_DATA		: uint				= 1;
		private static const UPDATE_MATRIX		: uint				= 2;
		private static const UPDATE_ALL			: uint				= UPDATE_DATA | UPDATE_MATRIX;

		private var _data		: Vector.<Number>		= new Vector.<Number>();
		private var _numPushes	: int					= 0;
		
		private var _locked		: Boolean				= false;
		private var _changed	: Signal				= new Signal('Matrix4x4.changed');
		
		minko_math var _matrix	: flash.geom.Matrix3D	= new flash.geom.Matrix3D();
		
		public function get dataDescriptor() : Object
		{
			return null;
		}

		public function get translationX() : Number
		{
			return getTranslation(TMP_VECTOR4).x;
		}
		public function set translationX(value : Number) : void
		{
			setTranslation(value, NaN, NaN);
		}
		
		public function get translationY() : Number
		{
			return getTranslation(TMP_VECTOR4).y;
		}
		public function set translationY(value : Number) : void
		{
			setTranslation(NaN, value, NaN);
		}
		
		public function get translationZ() : Number
		{
			return getTranslation(TMP_VECTOR4).z;
		}
		public function set translationZ(value : Number) : void
		{
			setTranslation(NaN, NaN, value);
		}
		
		public function get rotationX() : Number
		{
			return getRotation(TMP_VECTOR4).x;
		}
		public function set rotationX(value : Number) : void
		{
			setRotation(value, NaN, NaN);
		}
		
		public function get rotationY() : Number
		{
			return getRotation(TMP_VECTOR4).y;
		}
		public function set rotationY(value : Number) : void
		{
			setRotation(NaN, value, NaN);
		}
		
		public function get rotationZ() : Number
		{
			return getRotation(TMP_VECTOR4).z;
		}
		public function set rotationZ(value : Number) : void
		{
			setRotation(NaN, NaN, value);
		}
		
		public function get scaleX() : Number
		{
			return getScale(TMP_VECTOR4).x;
		}
		public function set scaleX(value : Number) : void
		{
			setScale(value, NaN, NaN);
		}
		
		public function get scaleY() : Number
		{
			return getScale(TMP_VECTOR4).y;
		}
		public function set scaleY(value : Number) : void
		{
			setScale(NaN, value, NaN);
		}
		
		public function get scaleZ() : Number
		{
			return getScale(TMP_VECTOR4).z;
		}
		public function set scaleZ(value : Number) : void
		{
			setScale(NaN, NaN, value);
		}
		
		public function get determinant(): Number
		{
			return _matrix.determinant;
		}
		
		public function get locked() : Boolean
		{
			return _locked;
		}

		public function get changed() : Signal
		{
			return _changed;
		}
		
		public function Matrix4x4(m11 : Number	= 1., m12 : Number	= 0., m13 : Number	= 0., m14 : Number	= 0.,
								  m21 : Number	= 0., m22 : Number	= 1., m23 : Number	= 0., m24 : Number	= 0.,
								  m31 : Number	= 0., m32 : Number	= 0., m33 : Number	= 1., m34 : Number	= 0.,
								  m41 : Number	= 0., m42 : Number	= 0., m43 : Number	= 0., m44 : Number	= 1.)
		{
			initialize(
				m11, m12, m13, m14,
				m21, m22, m23, m24,
				m31, m32, m33, m34,
				m41, m42, m43, m44
			);
		}

		public function initialize(m11 : Number, m12 : Number, m13 : Number, m14 : Number,
								   m21 : Number, m22 : Number, m23 : Number, m24 : Number,
								   m31 : Number, m32 : Number, m33 : Number, m34 : Number,
								   m41 : Number, m42 : Number, m43 : Number, m44 : Number) : void
		{
			TMP_VECTOR.length = 0;
			TMP_VECTOR.push(
				m11, m12, m13, m14,
				m21, m22, m23, m24,
				m31, m32, m33, m34,
				m41, m42, m43, m44
			);

			_matrix.copyRawDataFrom(TMP_VECTOR);
			
			if (!_locked)
				_changed.execute(this, null);
		}
		
		public function copyFrom(matrix : Matrix4x4) : Matrix4x4
		{
			_matrix.copyFrom(matrix._matrix);
			
			if (!_locked)
				_changed.execute(this, null);
			
			return this;
		}
		
		public function clone() : Matrix4x4
		{
			return new Matrix4x4().copyFrom(this);
		}
		
		public function push() : Matrix4x4
		{
			_matrix.copyRawDataTo(_data, _numPushes * 16);
			_numPushes++;
			
			return this;
		}
		
		public function pop() : Matrix4x4
		{
			if (_numPushes == 0)
				return this;
			
			_numPushes--;
			_matrix.copyRawDataFrom(_data, _numPushes * 16);
			
			if (!_locked)
				_changed.execute(this, null);

			return this;
		}
		
		public function prepend(m : Matrix4x4) : Matrix4x4
		{
			_matrix.prepend(m._matrix);
			
			if (!_locked)
				_changed.execute(this, null);

			return this;
		}

		public function append(m : Matrix4x4) : Matrix4x4
		{
			_matrix.append(m._matrix);
			
			if (!_locked)
				_changed.execute(this, null);

			return this;
		}

		public function appendRotation(radians		: Number,
									   axis			: Vector4,
									   pivotPoint	: Vector4	= null) : Matrix4x4
		{
			_matrix.appendRotation(
				radians * RAD2DEG,
				axis._vector,
				pivotPoint ? pivotPoint._vector : null
			);
			
			if (!_locked)
				_changed.execute(this, null);

			return this;
		}

		public function appendScale(x	: Number,
									y	: Number	= 1.,
									z	: Number	= 1.) : Matrix4x4
		{
			_matrix.appendScale(x, y, z);
			
			if (!_locked)
				_changed.execute(this, null);

			return this;
		}
		
		public function appendUniformScale(scale : Number) : Matrix4x4
		{
			_matrix.appendScale(scale, scale, scale);
			
			if (!_locked)
				_changed.execute(this, null);

			return this;
		}

		public function appendTranslation(x : Number,
										  y : Number = 0.,
										  z : Number = 0.) : Matrix4x4
		{
			_matrix.appendTranslation(x, y, z);
			
			if (!_locked)
				_changed.execute(this, null);

			return this;
		}

		public function prependRotation(radians		: Number,
										axis		: Vector4,
										pivotPoint	: Vector4 = null) : Matrix4x4
		{
			_matrix.prependRotation(radians * RAD2DEG,
									axis._vector,
									pivotPoint ? pivotPoint._vector : null);
			
			if (!_locked)
				_changed.execute(this, null);

			return this;
		}

		public function prependScale(x	: Number,
									 y	: Number = 1.,
									 z	: Number = 1.) : Matrix4x4
		{
			_matrix.prependScale(x, y, z);
			
			if (!_locked)
				_changed.execute(this, null);

			return this;
		}
		
		public function prependUniformScale(scale : Number) : Matrix4x4
		{
			_matrix.prependScale(scale, scale, scale);
			
			if (!_locked)
				_changed.execute(this, null);
			
			return this;
		}

		public function prependTranslation(x : Number,
										   y : Number	= 1.,
										   z : Number	= 1.) : Matrix4x4
		{
			_matrix.prependTranslation(x, y, z);
			
			if (!_locked)
				_changed.execute(this, null);

			return this;
		}

		public function transformVector(input 	: Vector4,
									    output	: Vector4	= null) : Vector4
		{
			var v : Vector3D = _matrix.transformVector(input._vector);

			output ||= new Vector4();
			output.set(v.x, v.y, v.z, v.w);

			return output;
		}

		public function deltaTransformVector(input 	: Vector4,
											 output	: Vector4	= null) : Vector4
		{
			var v : Vector3D = _matrix.deltaTransformVector(input._vector);

			output ||= new Vector4();
			output.set(v.x, v.y, v.z, v.w);

			return output;
		}

		public function transformRawVectors(input 	: Vector.<Number>,
										    output	: Vector.<Number> = null) : Vector.<Number>
		{
			output ||= new Vector.<Number>();
			_matrix.transformVectors(input, output);

			return output;
		}
		
		public function identity() : Matrix4x4
		{
			_matrix.identity();

			if (!_locked)
				_changed.execute(this, null);

			return this;
		}
		
		public function invert() : Matrix4x4
		{
			_matrix.invert();

			if (!_locked)
				_changed.execute(this, null);

			return this;
		}
		
		public function transpose() : Matrix4x4
		{
			_matrix.transpose();

			if (!_locked)
				_changed.execute(this, null);

			return this;
		}
		
		public function projectVector(input 	: Vector4,
									  output	: Vector4 = null) : Vector4
		{
			var v : Vector3D = Utils3D.projectVector(_matrix, input._vector);

			output ||= new Vector4();
			output.set(v.x, v.y, v.z, v.w);

			return output;
		}
		
		public function projectRawVectors(input 	: Vector.<Number>,
										  output	: Vector.<Number>,
										  uvt		: Vector.<Number>) : void
		{
			Utils3D.projectVectors(_matrix, input, output, uvt);
		}


		public function getRawData(out 			: Vector.<Number> = null,
								   offset		: int			  = 0,
								   transposed	: Boolean 		  = false) : Vector.<Number>
		{
			out ||= new Vector.<Number>();
			_matrix.copyRawDataTo(out, offset, transposed);

			return out;
		}

		public function setRawData(input		: Vector.<Number>,
								   offset		: int		= 0,
								   transposed	: Boolean	= false) : Matrix4x4
		{
			_matrix.copyRawDataFrom(input, offset, transposed);

			if (!_locked)
				_changed.execute(this, null);

			return this;
		}

		public function pointAt(pos	: Vector4,
								at	: Vector4	= null,
								up	: Vector4	= null) : Matrix4x4
		{
			_matrix.pointAt(
				pos._vector,
				at ? at._vector : null,
				up ? up._vector : null
			);
			
			if (!_locked)
				_changed.execute(this, null);

			return this;
		}
		
		public function interpolateTo(target : Matrix4x4, percent : Number) : Matrix4x4
		{
			_matrix.interpolateTo(target._matrix, percent);

			if (!_locked)
				_changed.execute(this, null);

			return this;
		}
		
		public function interpolateBetween(m1 : Matrix4x4, m2 : Matrix4x4, ratio : Number) : Matrix4x4
		{
			_matrix.copyFrom(m1._matrix);
			_matrix.interpolateTo(m2._matrix, ratio);
			
			if (!_locked)
				_changed.execute(this, null);
			
			return this;
		}
		
		public function getTranslation(output : Vector4 = null) : Vector4
		{
			output ||= new Vector4();
			_matrix.copyColumnTo(3, output._vector);
			
			output.changed.execute(output, null);
			
			return output;
		}
		
		public function setTranslation(x : Number, y : Number, z : Number) : Matrix4x4
		{
			var position : Vector4	= getTranslation(TMP_VECTOR4);
			
			if (!isNaN(x))
				position.x = x;
			if (!isNaN(y))
				position.y = y;
			if (!isNaN(z))
				position.z = z;
			
			_matrix.copyColumnFrom(3, position._vector);
			
			if (!_locked)
				_changed.execute(this, null);
			
			return this;
		}
		
		public function getRotation(output : Vector4 = null) : Vector4
		{
			var components 	: Vector.<Vector3D>	= _matrix.decompose();
			
			output ||= new Vector4();
			output._vector = components[1];
			
			output.changed.execute(output, null);
			
			return output;
		}
		
		public function setRotation(x : Number, y : Number, z : Number) : Matrix4x4
		{
			var components 	: Vector.<Vector3D>	= _matrix.decompose();
			var rotation	: Vector3D			= components[1];
			
			if (!isNaN(x))
				rotation.x = x;
			if (!isNaN(y))
				rotation.y = y;
			if (!isNaN(z))
				rotation.z = z;
			
			_matrix.recompose(components);
			
			if (!_locked)
				_changed.execute(this, null);
			
			return this;
		}
		
		public function getScale(output : Vector4 = null) : Vector4
		{
			var components 	: Vector.<Vector3D>	= _matrix.decompose();
			
			output ||= new Vector4();
			output._vector = components[2];
			
			output.changed.execute(output, null);
			
			return output;
		}
		
		public function setScale(x : Number, y : Number, z : Number) : Matrix4x4
		{
			var components 	: Vector.<Vector3D>	= _matrix.decompose();
			var scale		: Vector3D			= components[2];
			
			if (!isNaN(x))
				scale.x = x;
			if (!isNaN(y))
				scale.y = y;
			if (!isNaN(z))
				scale.z = z;
			
			_matrix.recompose(components);
			
			if (!_locked)
				_changed.execute(this, null);
			
			return this;
		}

		public function toDualQuaternion(n : Vector4,
										 d : Vector4) : void
		{
			var m : Vector.<Number> = TMP_VECTOR;
			
			_matrix.copyRawDataTo(m, 0, true);

			var mTrace	: Number = m[0] + m[5] + m[10];
			var s		: Number;
			var nw		: Number;
			var nx		: Number;
			var ny		: Number;
			var nz		: Number;

			if (mTrace > 0)
			{
				s = 2.0 * Math.sqrt(mTrace + 1.0);
				nw = 0.25 * s;
				nx = (m[9] - m[6]) / s;
				ny = (m[2] - m[8]) / s;
				nz = (m[4] - m[1]) / s;
			}
			else if (m[0] > m[5] && m[0] > m[10])
			{
				s = 2.0 * Math.sqrt(1.0 + m[0] - m[5] - m[10]);

				nw = (m[9] - m[6]) / s
				nx = 0.25 * s;
				ny = (m[1] + m[4]) / s;
				nz = (m[2] + m[8]) / s;
			}
			else if (m[5] > m[10])
			{
				s = 2.0 * Math.sqrt(1.0 + m[5] - m[0] - m[10]);

				nw = (m[2] - m[8]) / s;
				nx = (m[1] + m[4]) / s;
				ny = 0.25 * s;
				nz = (m[6] + m[9]) / s;
			}
			else
			{
				s = 2.0 * Math.sqrt(1.0 + m[10] - m[0] - m[5]);

				nw = (m[4] - m[1]) / s;
				nx = (m[2] + m[8]) / s;
				ny = (m[6] + m[9]) / s;
				nz = 0.25 * s;
			}

			var dw : Number = -	0.5 * (	 m[3] * nx + m[7] * ny + m[11] * nz);
			var dx : Number = 	0.5 * (	 m[3] * nw + m[7] * nz - m[11] * ny);
			var dy : Number = 	0.5 * (- m[3] * nz + m[7] * nw + m[11] * nx);
			var dz : Number = 	0.5 * (	 m[3] * ny - m[7] * nx + m[11] * nw);

			n.set(nx, ny, nz, nw);
			d.set(dx, dy, dz, dw);
		}

		public function lock() : Matrix4x4
		{
			_locked = true;
			
			return this;
		}
		
		public function unlock() : Matrix4x4
		{
			_locked = false;
			_changed.execute(this, null);
			
			return this;
		}
		
		public function compareTo(matrix : Matrix4x4) : Boolean
		{
			_matrix.copyRawDataTo(TMP_VECTOR);
			matrix._matrix.copyRawDataTo(TMP_VECTOR, 16);
			
			for (var i : int = 0; i < 16; ++i)
				if (TMP_VECTOR[i] != TMP_VECTOR[int(i + 16)])
					return false;
			
			return true;
		}
		
		public function orient(eye			: Vector4,
							   direction	: Vector4,
							   up			: Vector4   = null) : Matrix4x4
		{
			
			var eye_X		: Number = eye._vector.x;
			var eye_Y		: Number = eye._vector.y;
			var eye_Z		: Number = eye._vector.z;
			
			var z_axis_X	: Number = direction._vector.x;
			var z_axis_Y	: Number = direction._vector.y;
			var z_axis_Z	: Number = direction._vector.z;
			
			var up_axis_x	: Number;
			var up_axis_y	: Number;
			var up_axis_z	: Number;
			
			if (up != null)
			{
				// if up axis was given, take it. An error will be raised later if it is colinear to direction
				up_axis_x = up._vector.x;
				up_axis_y = up._vector.y;
				up_axis_z = up._vector.z;	
			}
			else
			{
				// if direction is colinear to (0, 1, 0), take (1, 0, 0) as up vector
				if (z_axis_X == 0 && z_axis_Y != 0 && z_axis_Z == 0)
				{
					up_axis_x = 1;
					up_axis_y = 0;
					up_axis_z = 0;
				}
				// else, take (0, 0, 1)
				else
				{
					up_axis_x = 0;
					up_axis_y = 1;
					up_axis_z = 0;
				}
			}
			
			var l : Number;
			
			l = 1 / Math.sqrt(z_axis_X * z_axis_X + z_axis_Y * z_axis_Y + z_axis_Z * z_axis_Z);
			
			z_axis_X *= l;
			z_axis_Y *= l;
			z_axis_Z *= l;
			
			var x_axis_X : Number = up_axis_y * z_axis_Z - z_axis_Y * up_axis_z;
			var x_axis_Y : Number = up_axis_z * z_axis_X - z_axis_Z * up_axis_x;
			var x_axis_Z : Number = up_axis_x * z_axis_Y - z_axis_X * up_axis_y;
			
			l = 1 / Math.sqrt(x_axis_X * x_axis_X + x_axis_Y * x_axis_Y + x_axis_Z * x_axis_Z);
			
			x_axis_X *= l;
			x_axis_Y *= l;
			x_axis_Z *= l;
			
			var y_axis_X : Number = z_axis_Y * x_axis_Z - x_axis_Y * z_axis_Z;
			var y_axis_Y : Number = z_axis_Z * x_axis_X - x_axis_Z * z_axis_X;
			var y_axis_Z : Number = z_axis_X * x_axis_Y - x_axis_X * z_axis_Y;
			
			l = 1 / Math.sqrt(y_axis_X * y_axis_X + y_axis_Y * y_axis_Y + y_axis_Z * y_axis_Z);
			
			y_axis_X *= l;
			y_axis_Y *= l;
			y_axis_Z *= l;
			
			if ((x_axis_X == 0 && x_axis_Y == 0 && x_axis_Z == 0) 
				|| (y_axis_X == 0 && y_axis_Y == 0 && y_axis_Z == 0))
			{
				throw new Error(
					'Invalid argument(s): the eye direction (look at - eye position) '
					+ 'and the up vector appear to be the same.'
				);
			}
			
			var	m41	: Number	= - (x_axis_X * eye_X + x_axis_Y * eye_Y + x_axis_Z * eye_Z);
			var	m42	: Number	= - (y_axis_X * eye_X + y_axis_Y * eye_Y + y_axis_Z * eye_Z);
			var	m43	: Number	= - (z_axis_X * eye_X + z_axis_Y * eye_Y + z_axis_Z * eye_Z);
			
			initialize(
				x_axis_X,	y_axis_X,	z_axis_X,	0.,
				x_axis_Y,	y_axis_Y,	z_axis_Y,	0.,
				x_axis_Z,	y_axis_Z,	z_axis_Z,	0.,
				m41,		m42,		m43,		1.
			);
			
			return this;
		}
		
		public function lookAt(target	: Vector4,
							   position	: Vector4	= null,
							   up		: Vector4 	= null) : Matrix4x4
		{
			return view(position || getTranslation(), target, up).invert();
		}
		
		/**
		 * Builds a (left-handed) view transform.
		 * <br /><br />
		 * Eye : eye position, At : eye direction, Up : up vector
		 * <br /><br />
		 * zaxis = normal(At - Eye)<br />
		 * xaxis = normal(cross(Up, zaxis))<br />
		 * yaxis = cross(zaxis, xaxis)<br />
		 * <br />
		 * [      xaxis.x          yaxis.x            zaxis.x  	     0 ]<br />
		 * [      xaxis.y          yaxis.y            zaxis.y        0 ]<br />
		 * [      xaxis.z          yaxis.z            zaxis.z        0 ]<br />
		 * [ -dot(xaxis, eye)  -dot(yaxis, eye)  -dot(zaxis, eye)    1 ]<br />
		 *
		 * @return Returns a left-handed view Matrix3D to convert world coordinates into eye coordinates
		 *
		 */
		public function view(eye	: Vector4,
							 lookAt : Vector4,
							 up		: Vector4	= null) : Matrix4x4
		{
			var direction : Vector4 = Vector4.subtract(lookAt, eye, TMP_VECTOR4);
			
			return orient(eye, direction, up);
		}

		public function perspectiveFoV(fov		: Number,
									   ratio	: Number,
									   zNear	: Number,
									   zFar 	: Number) : Matrix4x4
		{
			var fd : Number = 1. / Math.tan(fov * 0.5);
			
			initialize(
				fd / ratio,	0.,								0.,		0.,
				0.,			fd,								0.,		0.,
				0.,			0.,			 zFar / (zFar - zNear),		1.,
				0.,			0.,	-zNear * zFar / (zFar - zNear),		0.
			);
			
			return this;
		}

		public function ortho(w 	: Number,
							  h		: Number,
							  zNear	: Number,
							  zFar	: Number) : Matrix4x4
		{
			initialize(
				2. / w,	0.,		0.,						0.,
				0.,		2. / h,	0.,						0.,
				0.,		0.,		1. / (zFar - zNear),	0.,
				0.,		0.,		zNear / (zNear - zFar),	1.
			);

			return this;
		}

		public function orthoOffCenter(l		: Number,
									   r		: Number,
									   b		: Number,
									   t		: Number,
									   zNear	: Number,
									   zFar		: Number) : Matrix4x4
		{
			initialize(
				2. / (r - l),		0.,					0.,						0.,
				0.,					2. / (t - b),		0.,						0.,
				0.,					0.,					1. / (zFar - zNear),	0.,
				(l + r) / (l - r),	(t + b) / (b - t),	zNear / (zNear - zFar),	1.
			);

			return this;
		}
		
		public function fromQuaternion(quaternion : Vector4) : Matrix4x4
		{
			var x : Number = quaternion.x;
			var y : Number = quaternion.y;
			var z : Number = quaternion.z;
			var w : Number = quaternion.w;
			var xy2 : Number = 2.* x * y;
			var xz2 : Number = 2.* x * z;
			var xw2 : Number = 2.* x * w;
			var yz2 : Number = 2.* y * z;
			var yw2 : Number = 2.* y * w;
			var zw2 : Number = 2.* z * w;
			var xx : Number = x * x;
			var yy : Number = y * y;
			var zz : Number = z * z;
			var ww : Number = w * w;

			initialize(
				xx - yy - zz + ww, 	xy2 + zw2, 			xz2 - yw2, 			0.,
				xy2 - zw2,			-xx + yy - zz + ww,	yz2 + xw2,			0.,
				xz2 + yw2,			yz2 - xw2,			-xx - yy + zz + ww, 0.,
				0.,					0.,					0.,					1.
			);

			return this;
		}
		
		public function fromDualQuaternion(dQn : Vector4, 
										   dQd : Vector4) : Matrix4x4
		{
			var len2Inv	: Number = 1 / (dQn.w * dQn.w + dQn.x * dQn.x + dQn.y * dQn.y + dQn.z * dQn.z);

			var w		: Number = dQn.w;
			var x		: Number = dQn.x;
			var y		: Number = dQn.y;
			var z		: Number = dQn.z;

			var t0		: Number = dQd.w;
			var t1		: Number = dQd.x;
			var t2		: Number = dQd.y;
			var t3		: Number = dQd.z;

			initialize(
				len2Inv * (w * w + x * x - y * y - z * z),
				len2Inv * (2 * x * y + 2 * w * z),
				len2Inv * (2 * x * z - 2 * w * y),
				0,

				len2Inv * (2 * x * y - 2 * w * z),
				len2Inv * (w * w + y * y - x * x - z * z),
				len2Inv * (2 * y * z + 2 * w * x),
				0,

				len2Inv * (2 * x * z + 2 * w * y),
				len2Inv * (2 * y * z - 2 * w * x),
				len2Inv * (w * w + z * z - x * x - y * y),
				0,

				len2Inv * (-2 * t0 * x + 2 * w * t1 - 2 * t2 * z + 2 * y * t3),
				len2Inv * (-2 * t0 * y + 2 * t1 * z - 2 * x * t3 + 2 * w * t2),
				len2Inv * (-2 * t0 * z + 2 * x * t2 + 2 * w * t3 - 2 * t1 * y),
				1
			);

			return this;
		}
	}
}
