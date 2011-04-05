package aerys.minko.type
{
	import aerys.common.Factory;
	import aerys.common.IVersionnable;
	import aerys.minko.ns.minko;
	import aerys.minko.type.math.ConstVector4;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	
	import flash.geom.Matrix3D;
	import flash.geom.Orientation3D;
	import flash.geom.Utils3D;
	import flash.geom.Vector3D;
	
	public class Transform3D extends Matrix4x4
	{
		use namespace minko;
		
		private static const FACTORY		: Factory			= Factory.getFactory(Transform3D);
		private static const RAD2DEG		: Number			= 180. / Math.PI;
		private static const ORIENTATION	: String			= Orientation3D.EULER_ANGLES;
		private static const TMP			: Vector.<Vector3D>	= new Vector.<Vector3D>(3, true);
		
		private var _position		: Vector4	= new Vector4();
		private var _rotation		: Vector4	= new Vector4();
		private var _scale			: Vector4	= new Vector4(1., 1., 1.);
				
		private var _invalid		: Boolean	= false;
		private var _tv				: uint		= 0;
		private var _rv				: uint		= 0;
		private var _sv				: uint		= 0;
		
		override protected function get invalid() : Boolean
		{
			return super.invalid
				   || _invalid
				   || _position.version != _tv
				   || _rotation.version != _rv
				   || _scale.version != _sv;
		}
		
		public function get position() : Vector4
		{
			return _position;
		}
		
		public function get rotation() : Vector4
		{
			return _rotation;
		}
		public function get scale() : Vector4
		{
			return _scale;
		}
		
		public function set rotation(value : Vector4) : void
		{
			if (!Vector4.equals(value, _rotation, true))
			{
				_rotation.set(value.x, value.y, value.z, value.w);
				invalidate();
			}
		}
		
		public function set scale(value : Vector4) : void
		{
			if (!Vector4.equals(value, _scale, true))
			{
				_scale.set(value.x, value.y, value.z, value.w);
				invalidate();
			}
		}
		
		public function set position(value : Vector4) : void
		{
			if (!Vector4.equals(value, _position, true))
			{
				_position.set(value.x, value.y, value.z, value.w);				
				invalidate();
			}
		}
		
		override public function get version() : uint
		{
			return super.version + _position.version + _rotation.version
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
		
		override protected function update() : void
		{
			if (super.invalid)
			{
				super.update();
				
				var c : Vector.<Vector3D> = _matrix.decompose(ORIENTATION);
				
				_position.set(c[0].x, c[0].y, c[0].z, c[0].w);
				_rotation.set(c[1].x, c[1].y, c[1].z, c[1].w);
				_scale.set(c[2].x, c[2].y, c[2].z, c[2].w);
			}
			else
			{
				_matrix.recompose(Vector.<Vector3D>([_position._vector,
											 	     _rotation._vector,
													 _scale._vector]));
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
			
			return this;
		}
		
		public function appendScale(x	: Number,
									y	: Number	= 1.,
									z	: Number	= 1.) : Transform3D
		{
			validMatrix3D.appendScale(x, y, z);
			
			return this;
		}
		
		public function appendUniformScale(scale : Number) : Transform3D
		{
			validMatrix3D.appendScale(scale, scale, scale);
			
			return this;
		}
		
		public function appendTranslation(x : Number,
										  y : Number = 0.,
										  z : Number = 0.) : Transform3D
		{
			validMatrix3D.appendTranslation(x, y, z);
			
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
			
			return this;
		}
		
		public function prependScale(x	: Number,
									 y	: Number = 1.,
									 z	: Number = 1.) : Transform3D
		{
			validMatrix3D.prependScale(x, y, z);
			
			return this;
		}
		
		public function prependTranslation(x : Number,
										   y : Number	= 1.,
										   z : Number	= 1.) : Transform3D
		{
			validMatrix3D.prependTranslation(x, y, z);
			
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