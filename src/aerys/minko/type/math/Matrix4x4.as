package aerys.minko.type.math
{
	import flash.geom.Matrix3D;
	import flash.geom.Orientation3D;
	import flash.geom.Utils3D;
	import flash.geom.Vector3D;
	
	import aerys.minko.ns.minko_math;
	import aerys.minko.type.Factory;
	import aerys.minko.type.Signal;
	import aerys.minko.type.binding.IWatchable;

	use namespace minko_math;
		
	public class Matrix4x4 implements IWatchable
	{
		private static const FACTORY				: Factory			= Factory.getFactory(Matrix4x4);
		private static const RAD2DEG				: Number			= 180. / Math.PI;
		private static const DEG2RAD				: Number			= Math.PI / 180.;
		private static const EPSILON				: Number			= 1e-100;

		private static const TMP_VECTOR				: Vector.<Number>	= new <Number>[];
		private static const TMP_VECTOR4			: Vector4			= new Vector4();
        private static const TMP_VECTOR4_2			: Vector4			= new Vector4();
		private static const TMP_MATRIX				: Matrix4x4			= new Matrix4x4();
		
        private static const COMPONENT_NONE			: uint				= 0;
		private static const COMPONENT_TRANSLATION	: uint				= 1;
		private static const COMPONENT_ROTATION		: uint				= 2;
		private static const COMPONENT_SCALE		: uint				= 4;
		private static const COMPONENT_ALL			: uint				= 7;
		
		private var _data				: Vector.<Number>		= new <Number>[];
		private var _rotation			: Vector4				= new Vector4();
		private var _scale				: Vector4				= new Vector4();
		private var _translation		: Vector4				= new Vector4();
		private var _invalidComponents	: uint					= 7;
		private var _numPushes			: uint					= 0;
		
		private var _locked				: Boolean				= false;
		private var _changed			: Signal				= new Signal('Matrix4x4.changed');
		
		minko_math var _matrix			: flash.geom.Matrix3D	= new flash.geom.Matrix3D();
		minko_math var _hasChanged		: Boolean				= true;
		
		final public function get translationX() : Number
		{
			if (_invalidComponents)
				updateComponents(COMPONENT_TRANSLATION);
			
			return _translation.x;
		}
		
        final public function set translationX(value : Number) : void
		{
			if (_invalidComponents)
				updateComponents(COMPONENT_TRANSLATION);
			setTranslation(value, _translation.y, _translation.z);
		}
		
        final public function get translationY() : Number
		{
			if (_invalidComponents)
				updateComponents(COMPONENT_TRANSLATION);
			
			return _translation.y;
		}
        final public function set translationY(value : Number) : void
		{
			if (_invalidComponents)
				updateComponents(COMPONENT_TRANSLATION);
			setTranslation(_translation.x, value, _translation.z);
		}
		
        final public function get translationZ() : Number
		{
			if (_invalidComponents)
				updateComponents(COMPONENT_TRANSLATION);
			
			return _translation.z;
		}
        final public function set translationZ(value : Number) : void
		{
			if (_invalidComponents)
				updateComponents(COMPONENT_TRANSLATION);
			setTranslation(_translation.x, _translation.y, value);
		}
		
        final public function get rotationX() : Number
		{
			if (_invalidComponents)
				updateComponents(COMPONENT_ROTATION);
			
			return _rotation.x;
		}
        final public function set rotationX(value : Number) : void
		{
			if (_invalidComponents)
				updateComponents(COMPONENT_ROTATION);
			setRotation(value, _rotation.y, _rotation.z);
		}
		
        final public function get rotationY() : Number
		{
			if (_invalidComponents)
				updateComponents(COMPONENT_ROTATION);
			
			return _rotation.y;
		}
        final public function set rotationY(value : Number) : void
		{
			if (_invalidComponents)
				updateComponents(COMPONENT_ROTATION);
			setRotation(_rotation.x, value, _rotation.z);
		}
		
        final public function get rotationZ() : Number
		{
			if (_invalidComponents)
				updateComponents(COMPONENT_ROTATION);
			
			return _rotation.z;
		}
        final public function set rotationZ(value : Number) : void
		{
			if (_invalidComponents)
				updateComponents(COMPONENT_ROTATION);
			setRotation(_rotation.x, _rotation.y, value);
		}
		
        final public function get scaleX() : Number
		{
			if (_invalidComponents)
				updateComponents(COMPONENT_SCALE);
			
			return _scale.x;
		}
        final public function set scaleX(value : Number) : void
		{
			if (_invalidComponents)
				updateComponents(COMPONENT_SCALE);
			setScale(value, _scale.y, _scale.z);
		}
		
        final public function get scaleY() : Number
		{
			if (_invalidComponents)
				updateComponents(COMPONENT_SCALE);
			
			return _scale.y;
		}
        final public function set scaleY(value : Number) : void
		{
			if (_invalidComponents)
				updateComponents(COMPONENT_SCALE);
			setScale(_scale.x, value, _scale.z);
		}
		
        final public function get scaleZ() : Number
		{
			if (_invalidComponents)
				updateComponents(COMPONENT_SCALE);
			
			return _scale.z;
		}
        final public function set scaleZ(value : Number) : void
		{
			if (_invalidComponents)
				updateComponents(COMPONENT_SCALE);
			setScale(_scale.x, _scale.y, value);
		}
		
        final public function get determinant(): Number
		{
			return _matrix.determinant;
		}
		
        final public function get locked() : Boolean
		{
			return _locked;
		}

        final public function get changed() : Signal
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

        final public function initialize(m11 : Number, m12 : Number, m13 : Number, m14 : Number,
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
			_invalidComponents = COMPONENT_ALL;
			
			_hasChanged = true;
			if (!_locked)
				_changed.execute(this);
		}
		
        final public function copyFrom(matrix : Matrix4x4) : Matrix4x4
		{
			_matrix.copyFrom(matrix._matrix);
			_invalidComponents = COMPONENT_ALL;
			
			_hasChanged = true;
			if (!_locked)
				_changed.execute(this);
			
			return this;
		}
		
        final public function copyFromMatrix3D(matrix : Matrix3D) : Matrix4x4
		{
			_matrix.copyFrom(matrix);
			_invalidComponents = COMPONENT_ALL;
			
			_hasChanged = true;
			if (!_locked)
				_changed.execute(this);
			
			return this;
		}
		
        public function clone() : Matrix4x4
		{
			return new Matrix4x4().copyFrom(this);
		}
		
        final public function push() : Matrix4x4
		{
			_matrix.copyRawDataTo(_data, _numPushes * 16);
			_numPushes++;
			
			return this;
		}
		
        final public function pop(restore : Boolean = true) : Matrix4x4
		{
			if (_numPushes == 0)
				return this;
			
			_numPushes--;
			if (restore)
			{
				_matrix.copyRawDataFrom(_data, _numPushes * 16);
				_invalidComponents = COMPONENT_ALL;
			}
			
			_hasChanged = true;
			if (!_locked)
				_changed.execute(this);

			return this;
		}
		
        final public function prepend(m : Matrix4x4) : Matrix4x4
		{
			_invalidComponents = COMPONENT_ALL;
			_matrix.prepend(m._matrix);
			
			_hasChanged = true;
			if (!_locked)
				_changed.execute(this);

			return this;
		}

        final public function append(m : Matrix4x4) : Matrix4x4
		{
			_invalidComponents = COMPONENT_ALL;
			_matrix.append(m._matrix);
			
			_hasChanged = true;
			if (!_locked)
				_changed.execute(this);

			return this;
		}

        final public function appendRotation(radians	: Number,
                                             axis	    : Vector4,
                                             pivotPoint	: Vector4	= null) : Matrix4x4
		{
			_invalidComponents = COMPONENT_ALL;
			_matrix.appendRotation(
				radians * RAD2DEG,
				axis._vector,
				pivotPoint ? pivotPoint._vector : null
			);
			
			_hasChanged = true;
			if (!_locked)
				_changed.execute(this);

			return this;
		}

        final public function appendScale(x	: Number,
                                          y	: Number	= 1.,
                                          z	: Number	= 1.) : Matrix4x4
		{
			_invalidComponents = COMPONENT_ALL;
			_matrix.appendScale(x, y, z);

			_hasChanged = true;
			if (!_locked)
				_changed.execute(this);

			return this;
		}
		
        final public function appendUniformScale(scale : Number) : Matrix4x4
		{
            _invalidComponents = COMPONENT_ALL;
            _matrix.appendScale(scale, scale, scale);
            
			_hasChanged = true;
            if (!_locked)
                _changed.execute(this);
            
            return this;
		}

        final public function appendTranslation(x : Number,
                                                y : Number = 0.,
                                                z : Number = 0.) : Matrix4x4
		{
			_invalidComponents |= COMPONENT_TRANSLATION;
			_matrix.appendTranslation(x, y, z);
			
			TMP_VECTOR4.set(x, y, z);
			
			_hasChanged = true;
			if (!_locked)
				_changed.execute(this);

			return this;
		}

        final public function prependRotation(radians		: Number,
                                              axis		: Vector4,
                                              pivotPoint	: Vector4 = null) : Matrix4x4
		{
			_invalidComponents = COMPONENT_ALL;
			_matrix.prependRotation(
				radians * RAD2DEG,
				axis._vector,
				pivotPoint ? pivotPoint._vector : null
			);
			
			_hasChanged = true;
			if (!_locked)
				_changed.execute(this);

			return this;
		}

        final public function prependScale(x	: Number,
                                           y	: Number = 1.,
                                           z	: Number = 1.) : Matrix4x4
		{
			_invalidComponents = COMPONENT_ALL;
			_matrix.prependScale(x, y, z);
		
			_hasChanged = true;
			if (!_locked)
				_changed.execute(this);

			return this;
		}
		
        final public function prependUniformScale(scale : Number) : Matrix4x4
		{
			_invalidComponents = COMPONENT_ALL;
			_matrix.prependScale(scale, scale, scale);
			
			_hasChanged = true;
			if (!_locked)
				_changed.execute(this);
			
			return this;
		}

        final public function prependTranslation(x : Number,
                                                 y : Number	= 1.,
                                                 z : Number	= 1.) : Matrix4x4
		{
			_invalidComponents |= COMPONENT_TRANSLATION;
			_matrix.prependTranslation(x, y, z);
			
			_hasChanged = true;
			if (!_locked)
				_changed.execute(this);

			return this;
		}

        final public function transformVector(input 	: Vector4,
                                              output	: Vector4	= null) : Vector4
		{
			TMP_VECTOR[0] = input.x;
			TMP_VECTOR[1] = input.y;
			TMP_VECTOR[2] = input.z;
			TMP_VECTOR.length = 3;
			
			_matrix.transformVectors(TMP_VECTOR, TMP_VECTOR);
			
			output ||= new Vector4();
			output.set(TMP_VECTOR[0], TMP_VECTOR[1], TMP_VECTOR[2], 1);

			return output;
		}

        final public function deltaTransformVector(input 	: Vector4,
                                                   output	: Vector4	= null) : Vector4
		{
			var v : Vector3D = _matrix.deltaTransformVector(input._vector);

			output ||= new Vector4();
			output.set(v.x, v.y, v.z, v.w);

			return output;
		}

        final public function transformRawVectors(input 	: Vector.<Number>,
                                                  output	: Vector.<Number> = null) : Vector.<Number>
		{
			output ||= new Vector.<Number>();
			_matrix.transformVectors(input, output);

			return output;
		}
		
        final public function identity() : Matrix4x4
		{
			_invalidComponents = COMPONENT_ALL;
			_matrix.identity();

			_hasChanged = true;
			if (!_locked)
				_changed.execute(this);

			return this;
		}
		
        final public function invert() : Matrix4x4
		{
			_invalidComponents = COMPONENT_ALL;
			_matrix.invert();

			_hasChanged = true;
			if (!_locked)
				_changed.execute(this);

			return this;
		}
		
        final public function transpose() : Matrix4x4
		{
			_invalidComponents = COMPONENT_ALL;
			_matrix.transpose();

			_hasChanged = true;
			if (!_locked)
				_changed.execute(this);

			return this;
		}
		
        final public function projectVector(input 	: Vector4,
                                            output	: Vector4 = null) : Vector4
		{
			var v : Vector3D = Utils3D.projectVector(_matrix, input._vector);

			output ||= new Vector4();
			output.set(v.x, v.y, v.z, v.w);

			return output;
		}
		
        final public function projectRawVectors(input 	: Vector.<Number>,
                                                output	: Vector.<Number>,
                                                uvt		: Vector.<Number>) : void
		{
			Utils3D.projectVectors(_matrix, input, output, uvt);
		}


        final public function getRawData(out 		: Vector.<Number> = null,
                                         offset		: uint			  = 0,
                                         transposed	: Boolean 		  = false) : Vector.<Number>
		{
			out ||= new Vector.<Number>();
			_matrix.copyRawDataTo(out, offset, transposed);

			return out;
		}

        final public function setRawData(input		: Vector.<Number>,
                                         offset		: uint		= 0,
                                         transposed	: Boolean	= false) : Matrix4x4
		{
			_matrix.copyRawDataFrom(input, offset, transposed);

			_hasChanged = true;
            _invalidComponents |= COMPONENT_ALL;
			if (!_locked)
				_changed.execute(this);

			return this;
		}
		
        final public function getColumn(column : uint, out : Vector4 = null) : Vector4
		{
			out ||= new Vector4();
			
			_matrix.copyColumnTo(column, out._vector);
			out._update |= Vector4.UPDATE_ALL;
			
			if (!out.locked)
				out.changed.execute(out);
			
			return out;
		}
		
        final public function setColumn(column : uint, value : Vector4) : Matrix4x4
		{
			_matrix.copyColumnFrom(column, value._vector);
		
			_hasChanged = true;
            _invalidComponents |= COMPONENT_ALL;
			if (!_locked)
				_changed.execute(this);
			
			return this;
		}
		
        final public function getRow(row : uint, out : Vector4 = null) : Vector4
		{
			out ||= new Vector4();
			
			_matrix.copyColumnTo(row, out._vector);
			out._update |= Vector4.UPDATE_ALL;
			
			if (!out.locked)
				out.changed.execute(out);
			
			return out;
		}		
		
        final public function setRow(row : uint, value : Vector4) : Matrix4x4
		{
			_matrix.copyRowFrom(row, value._vector);

			_hasChanged = true;
            _invalidComponents |= COMPONENT_ALL;
			if (!_locked)
				_changed.execute(this);
			
			return this;
		}

        final public function interpolateTo(target            : Matrix4x4,
                                            ratio             : Number,
                                            interpolateScale  : Boolean = true,
                                            interpolateW      : Boolean = true) : Matrix4x4
		{
			_invalidComponents = COMPONENT_ROTATION | COMPONENT_TRANSLATION;
			
			if (interpolateScale)
			{
				_invalidComponents |= COMPONENT_SCALE;
				
				var scale   : Vector4	= getScale(TMP_VECTOR4);
				var sx	    : Number	= scale.x;
				var sy	    : Number	= scale.y;
				var sz	    : Number	= scale.z;
				
				scale = target.getScale(TMP_VECTOR4);
                sx += (scale.x - sx) * ratio;
                sy += (scale.y - sy) * ratio;
                sz += (scale.z - sz) * ratio;

				_matrix.interpolateTo(target._matrix, ratio);
                _matrix.prependScale(sx, sy, sz);
                
                if (interpolateW)
                {
                    var translation : Vector4   = getTranslation(TMP_VECTOR4);
                    
                    translation.lerp(target.getTranslation(TMP_VECTOR4_2), ratio);
                    setColumn(3, translation);
                }
			}
			else
			{
				_matrix.interpolateTo(target._matrix, ratio);
			}
			
			_hasChanged = true;
			if (!_locked)
				_changed.execute(this);

			return this;
		}
		
        final public function interpolateBetween(m1 			    : Matrix4x4,
                                                 m2 			    : Matrix4x4,
                                                 ratio 		        : Number,
                                                 interpolateScale   : Boolean   = true,
                                                 interpolateW       : Boolean   = true) : Matrix4x4
		{
			_matrix.copyFrom(m1._matrix);
			interpolateTo(m2, ratio, interpolateScale, interpolateW);
			
			return this;
		}
		
        final public function getTranslation(output : Vector4 = null) : Vector4
		{
			output ||= new Vector4();
			_matrix.copyColumnTo(3, output._vector);
			
			output._update |= Vector4.UPDATE_ALL;
			output.changed.execute(output);
			
			return output;
		}
		
        final public function setTranslation(x : Number, y : Number, z : Number) : Matrix4x4
		{
			if (_invalidComponents)
				updateComponents(COMPONENT_TRANSLATION);
			
			_translation.x = x;
			_translation.y = y;
			_translation.z = z;
			
			_matrix.copyColumnFrom(3, _translation._vector);

			_hasChanged = true;
			if (!_locked)
				_changed.execute(this);
			
			return this;
		}
		
        final public function getRotation(output : Vector4 = null) : Vector4
		{
			var components 	: Vector.<Vector3D>	= _matrix.decompose();
			
			output ||= new Vector4();
			output._vector = components[1];
			output._update |= Vector4.UPDATE_ALL;
			output.changed.execute(output, null);
			
			return output;
		}
		
        final public function setRotation(x : Number, y : Number, z : Number) : Matrix4x4
		{
			if (_invalidComponents)
				updateComponents();
			
			_rotation._vector.setTo(x, y, z);
			
			_matrix.recompose(new <Vector3D>[
				_translation._vector,
				_rotation._vector,
				_scale._vector
			]);
			
			_hasChanged = true;
			if (!_locked)
				_changed.execute(this);
			
			return this;
		}
		
        final public function getRotationQuaternion(output : Quaternion = null) : Quaternion
		{
			var components 	: Vector.<Vector3D>	= _matrix.decompose(Orientation3D.QUATERNION);
			var rotation	: Vector3D			= components[1];
			
			output ||= new Quaternion();
			output.set(rotation.w, rotation.x, rotation.y, rotation.z);
			
			return output;
		}
		
        final public function getScale(output : Vector4 = null) : Vector4
		{
			var components 	: Vector.<Vector3D>	= _matrix.decompose();
			
			output ||= new Vector4();
			output._vector = components[2];
			output._update |= Vector4.UPDATE_ALL;
			
			output.changed.execute(output, null);
			
			return output;
		}
		
        final public function setScale(x : Number, y : Number, z : Number) : Matrix4x4
		{
			if (_invalidComponents)
				updateComponents();
			
			_scale.x = x;
			_scale.y = y;
			_scale.z = z;
			
			_scale._vector.setTo(x, y, z);
			
			_matrix.recompose(new <Vector3D>[
				_translation._vector,
				_rotation._vector,
				_scale._vector
			]);
			
			_hasChanged = true;
			if (!_locked)
				_changed.execute(this);
			
			return this;
		}
		
		final public function getForward(output : Vector4 = null) : Vector4
		{
			output ||= new Vector4();
			_matrix.copyColumnTo(2, output._vector);
			output._update |= Vector4.UPDATE_ALL;
			
			output.changed.execute(output);
			
			return output;
		}
		
		final public function getSide(output : Vector4 = null) : Vector4
		{
			output ||= new Vector4();
			_matrix.copyColumnTo(0, output._vector);
			output._update |= Vector4.UPDATE_ALL;
			
			output.changed.execute(output);
			
			return output;
		}
		
		final public function getUp(output : Vector4 = null) : Vector4
		{
			output ||= new Vector4();
			_matrix.copyColumnTo(1, output._vector);
			output._update |= Vector4.UPDATE_ALL;
			
			output.changed.execute(output);
			
			return output;
		}
		
        final public function toDualQuaternion(n : Vector4,
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

        final public function lock() : Matrix4x4
		{
			_locked = true;
			
			return this;
		}
		
        final public function unlock() : Matrix4x4
		{
			_locked = false;
			_changed.execute(this);
			
			return this;
		}
		
        final public function compareTo(matrix : Matrix4x4) : Boolean
		{
			_matrix.copyRawDataTo(TMP_VECTOR);
			matrix._matrix.copyRawDataTo(TMP_VECTOR, 16);
			
			for (var i : int = 0; i < 16; ++i)
				if (TMP_VECTOR[i] != TMP_VECTOR[int(i + 16)])
					return false;
			
			return true;
		}
		
        final public function orientTo(direction	: Vector4,
                                       position	    : Vector4	= null,
                                       up			: Vector4   = null) : Matrix4x4
		{
			position ||= getTranslation();
			
			view(
				position,
				Vector4.add(position, direction),
				up// || deltaTransformVector(Vector4.Y_AXIS)
			);
			
			return invert();
		}
		
        final public function lookAt(target	    : Vector4,
                                     position	: Vector4	= null,
                                     up		    : Vector4	= null) : Matrix4x4
		{
			view(
				position,
				target,
				up || deltaTransformVector(Vector4.Y_AXIS)
			);
			
			return invert();
		}
		
        final public function world(translation : Vector4,
                                    rotation	: Vector4,
                                    scale		: Vector4) : Matrix4x4
		{
			_matrix.recompose(new <Vector3D>[translation._vector, rotation._vector, scale._vector]);
            _translation.copyFrom(translation);
            _rotation.copyFrom(rotation);
            _scale.copyFrom(scale);
            _invalidComponents = 0;

			_hasChanged = true;
			if (!_locked)
				_changed.execute(this);
			
			return this;
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
		final public function view(eye	    : Vector4,
                                   lookAt   : Vector4,
                                   up		: Vector4	= null) : Matrix4x4
		{
			eye ||= getTranslation();
			
			var direction : Vector4 = Vector4.subtract(lookAt, eye, TMP_VECTOR4);
			
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
			
			var	m41	: Number	= -(x_axis_X * eye_X + x_axis_Y * eye_Y + x_axis_Z * eye_Z);
			var	m42	: Number	= -(y_axis_X * eye_X + y_axis_Y * eye_Y + y_axis_Z * eye_Z);
			var	m43	: Number	= -(z_axis_X * eye_X + z_axis_Y * eye_Y + z_axis_Z * eye_Z);
			
			initialize(
				x_axis_X,	y_axis_X,	z_axis_X,	0.,
				x_axis_Y,	y_axis_Y,	z_axis_Y,	0.,
				x_axis_Z,	y_axis_Z,	z_axis_Z,	0.,
				m41,		m42,		m43,		1.
			);
			
			return this;
		}

		final public function perspectiveFoV(fov	: Number,
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

		final public function ortho(w 	    : Number,
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

		final public function orthoOffCenter(l		: Number,
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
		
		final public function fromQuaternion(quaternion : Quaternion) : Matrix4x4
		{
			var x : Number = quaternion.i;
			var y : Number = quaternion.j;
			var z : Number = quaternion.k;
			var w : Number = quaternion.r;
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
		
		final public function fromDualQuaternion(n : Vector4, 
                                                 d : Vector4) : Matrix4x4
		{
			var len2Inv	: Number = 1 / (n.w * n.w + n.x * n.x + n.y * n.y + n.z * n.z);

			var w		: Number = n.w;
			var x		: Number = n.x;
			var y		: Number = n.y;
			var z		: Number = n.z;

			var t0		: Number = d.w;
			var t1		: Number = d.x;
			var t2		: Number = d.y;
			var t3		: Number = d.z;

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
		
		private function updateComponents(components : uint = 7) : void
		{
			if (_invalidComponents & components & COMPONENT_TRANSLATION)
			{
				_invalidComponents &= ~COMPONENT_TRANSLATION;
				getTranslation(_translation);
			}
			if (_invalidComponents & components & COMPONENT_ROTATION)
			{
				_invalidComponents &= ~COMPONENT_ROTATION;
				getRotation(_rotation);
			}
			if (_invalidComponents & components & COMPONENT_SCALE)
			{
				_invalidComponents &= ~COMPONENT_SCALE;
				getScale(_scale);
			}
		}
        
        public function toString() : String
        {
        	TMP_VECTOR.length = 16;
            return '[Matrix4x4 (' + getRawData(TMP_VECTOR) + ')]';
        }
	}
}
