package aerys.minko.type.math
{
	import aerys.minko.ns.minko;
	import aerys.minko.type.Factory;
	import aerys.minko.type.IVersionable;
	
	import flash.geom.Matrix3D;
	import flash.geom.Orientation3D;
	import flash.geom.Utils3D;
	import flash.geom.Vector3D;
	
	public final class Transform3D extends Matrix4x4
	{
		use namespace minko;
		
		private static const FACTORY		: Factory			= Factory.getFactory(Transform3D);
		
		private static const RAD2DEG		: Number			= 180. / Math.PI;
		private static const DEG2RAD		: Number			= Math.PI / 180.;
		private static const ZERO3			: Vector3D			= new Vector3D();
		private static const ONE3			: Vector3D			= new Vector3D(1., 1., 1.);
		
		private static const ORIENTATION	: String			= Orientation3D.EULER_ANGLES;
		private static const TMP			: Vector.<Vector3D>	= new Vector.<Vector3D>(3, true);
		
		private var _position		: Vector4	= new Vector4();
		private var _rotation		: Vector4	= new Vector4();
		private var _scale			: Vector4	= new Vector4(1., 1., 1.);
				
		private var _tv				: uint		= 0;
		private var _rv				: uint		= 0;
		private var _sv				: uint		= 0;
		private var _mv				: uint		= 0;
		
		override protected function get invalid() : Boolean
		{
			return super.invalid
				   || _position.version != _tv
				   || _rotation.version != _rv
				   || _scale.version != _sv;
		}
		
		public function get position() : Vector4
		{
			if (super.invalid)
				updateComponents();
			
			return _position;
		}
		
		public function get rotation() : Vector4
		{
			var invalid : Boolean = super.invalid;
			
			if (invalid)
				updateComponents();

			return _rotation;
		}
		public function get scale() : Vector4
		{
			if (super.invalid)
				updateComponents();

			return _scale;
		}
		
		public function set position(value : Vector4) : void
		{
			if (!Vector4.equals(value, position, true))
			{
				_position.set(value.x, value.y, value.z, value.w);				
				invalidate();
			}
		}
		
		public function set rotation(value : Vector4) : void
		{
			if (!Vector4.equals(value, rotation, true))
			{
				_rotation.set(value.x, value.y, value.z, value.w);
				invalidate();
			}
		}
		
		public function set scale(value : Vector4) : void
		{
			if (!Vector4.equals(value, scale, true))
			{
				_scale.set(value.x, value.y, value.z, value.w);
				invalidate();
			}
		}
		
		override public function get version() : uint
		{
			return super.version
				   + _position.version
				   + _rotation.version
				   + _scale.version;
		}
		
		public function Transform3D(translation	: Vector4	= null,
									rotation	: Vector4	= null,
									scale		: Vector4	= null)
		{
			super();
			
			if (translation)
				_position.set(translation.x, translation.y, translation.z);
			if (rotation)
				_rotation.set(rotation.x, rotation.y, rotation.z);
			if (scale)
				_scale.set(scale.x, scale.y, scale.z);
		}
		
		protected function updateComponents() : void
		{
			super.updateMatrix();
			
			var c : Vector.<Vector3D> = _matrix.decompose(ORIENTATION);
			
			c[1].scaleBy(DEG2RAD);
			
			_position._vector = c[0];
			_rotation._vector = c[1];
			_scale._vector = c[2];
		}
		
		override protected function updateMatrix() : void
		{
			var invalidPosition : Boolean 	= _position.version != _tv;
			var invalidRotation : Boolean	= _rotation.version != _rv;
			var invalidScale	: Boolean	= _scale.version != _sv;

			if (invalidPosition || invalidRotation || invalidScale)
			{
				var c : Vector.<Vector3D> = null;
				
				if (!invalidPosition || !invalidRotation || !invalidScale)
				{
					c = _matrix.decompose(ORIENTATION);
				
					if (invalidPosition)
						c[0] = _position._vector;
					if (invalidRotation)
						c[1] = _rotation._vector;
					if (invalidScale)
						c[2] = _scale._vector;
				}
				else
				{
					c = Vector.<Vector3D>(_position._vector,
										  _rotation._vector,
										  _scale._vector)
				}
				
				_matrix.recompose(c);
			}
			
			_tv = _position.version;
			_rv = _rotation.version;
			_sv = _scale.version;
		}
				
		public function append(lhs : Transform3D) : Transform3D
		{
			multiplyInverse(lhs);
			
			return this;
		}
		
		public function appendRotation(radians		: Number,
									   axis			: Vector4,
									   pivotPoint	: Vector4	= null) : Transform3D
		{
			validMatrix3D.appendRotation(radians * RAD2DEG,
								  		 axis._vector,
								  		 pivotPoint ? pivotPoint._vector : null);
			invalidate();
			
			return this;
		}
		
		public function appendScale(x	: Number,
									y	: Number	= 1.,
									z	: Number	= 1.) : Transform3D
		{
			validMatrix3D.appendScale(x, y, z);
			invalidate();
			
			return this;
		}
		
		public function appendUniformScale(scale : Number) : Transform3D
		{
			validMatrix3D.appendScale(scale, scale, scale);
			invalidate();
			
			return this;
		}
		
		public function appendTranslation(x : Number,
										  y : Number = 0.,
										  z : Number = 0.) : Transform3D
		{
			validMatrix3D.appendTranslation(x, y, z);
			invalidate();
			
			return this;
		}
		
		public function prepend(rhs : Transform3D) : Transform3D
		{
			multiply(rhs);
			
			return this;
		}
		
		public function prependRotation(radians		: Number,
										axis		: Vector4,
										pivotPoint	: Vector4 = null) : Transform3D
		{
			validMatrix3D.prependRotation(radians * RAD2DEG, 
								    axis._vector,
								    pivotPoint ? pivotPoint._vector : null);
			invalidate();
			
			return this;
		}
		
		public function prependScale(x	: Number,
									 y	: Number = 1.,
									 z	: Number = 1.) : Transform3D
		{
			validMatrix3D.prependScale(x, y, z);
			invalidate();
			
			return this;
		}
		
		public function prependTranslation(x : Number,
										   y : Number	= 1.,
										   z : Number	= 1.) : Transform3D
		{
			validMatrix3D.prependTranslation(x, y, z);
			invalidate();
			
			return this;
		}
		
		public function setTranslation(translation : Vector4) : Transform3D
		{
			this.position = translation;
			
			return this;
		}
		
		public function setRotation(rotation : Vector4) : Transform3D
		{
			this.rotation = rotation;
			
			return this;
		}
		
		public function setScale(scale : Vector4) : Transform3D
		{
			this.scale = scale;
			
			return this;
		}
		
		public function reset() : Transform3D
		{
			identity();
			
			return this;
		}
	}
}