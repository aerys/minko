package aerys.minko.transform
{
	import aerys.common.Factory;
	import aerys.common.IVersionnable;
	import aerys.minko.ns.minko;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	
	import flash.geom.Matrix3D;
	import flash.geom.Orientation3D;
	import flash.geom.Utils3D;
	import flash.geom.Vector3D;
	
	public class Transform3D extends Matrix4x4
	{
		use namespace minko;
		
		private static const FACTORY		: Factory	= Factory.getFactory(Transform3D);
		private static const RAD2DEG		: Number	= 180. / Math.PI;
		private static const ORIENTATION	: String	= Orientation3D.EULER_ANGLES;
		
		private var _translation	: Vector4	= new Vector4();
		private var _rotation		: Vector4	= new Vector4();
		private var _scale			: Vector4	= new Vector4(1., 1., 1.);
		
		private var _update			: Boolean	= false;
		private var _tv				: uint		= 0;
		private var _rv				: uint		= 0;
		private var _sv				: uint		= 0;
		
		protected function get invalidComponents() : Boolean
		{
			return _update || _tv != _translation.version || _rv != _rotation.version
				   || _sv != _scale.version;
		}
		
		override protected function get invalidMatrix() : Boolean
		{
			return invalidComponents || super.invalidMatrix;
		}
		
		override protected function get invalidRawData() : Boolean
		{
			return invalidComponents || super.invalidRawData;
		}
		
		public function get translation() : Vector4
		{
			invalidComponents && updateComponents();
			
			return _translation;
		}
		public function get rotation() : Vector4
		{
			invalidComponents && updateComponents();
			
			return _rotation;
		}
		public function get scale() : Vector4
		{
			invalidComponents && updateComponents();
			
			return _scale;
		}
		
		public function set rotation(value : Vector4) : void
		{
			if (!Vector4.equals(value, _rotation, true))
			{
				_rotation.set(value.x, value.y, value.z, value.w);
				invalidateComponents();
			}
		}
		
		public function set scale(value : Vector4) : void
		{
			if (!Vector4.equals(value, _scale, true))
			{
				_scale.set(value.x, value.y, value.z, value.w);
				invalidateComponents();
			}
		}
		
		public function set translation(value : Vector4) : void
		{
			if (!Vector4.equals(value, _translation, true))
			{
				_translation.set(value.x, value.y, value.z, value.w);				
				invalidateComponents();
			}
		}
		
		override public function get version() : uint
		{
			return super.version + _translation.version + _rotation.version
				   + _scale.version;
		}
		
		public function Transform3D(translation	: Vector4	= null,
									rotation	: Vector4	= null,
									scale		: Vector4	= null)
		{
			super();
			
			_translation.set(translation.x, translation.y, translation.z);
			_rotation.set(rotation.x, rotation.y, rotation.z);
			_scale.set(scale.x, scale.y, scale.z);
		}
		
		protected function updateComponents() : void
		{
			var c : Vector.<Vector3D> = matrix.decompose(ORIENTATION);
			
			_translation.set(c[0].x, c[0].y, c[0].z, c[0].w);
			_rotation.set(c[1].x, c[1].y, c[1].z, c[1].w);
			_scale.set(c[2].x, c[2].y, c[2].z, c[2].w);
			
			_update = false;
		}
		
		protected function invalidateComponents() : void
		{
			_update = true;
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
			invalidateRawData();
			matrix.appendRotation(radians * RAD2DEG,
								  axis._vector,
								  pivotPoint ? pivotPoint._vector : null);
			
			return this;
		}
		
		public function appendScale(x	: Number,
									y	: Number	= 1.,
									z	: Number	= 1.) : Transform3D
		{
			invalidateRawData();
			matrix.appendScale(x, y, z);
			
			return this;
		}
		
		public function appendUniformScale(scale : Number) : Transform3D
		{
			invalidateRawData();
			matrix.appendScale(scale, scale, scale);
			
			return this;
		}
		
		public function appendTranslation(x : Number,
										  y : Number = 0.,
										  z : Number = 0.) : Transform3D
		{
			invalidateRawData();
			matrix.appendTranslation(x, y, z);
			
			return this;
		}
		
		public function pointAt(pos	: Vector4,
								at	: Vector4	= null,
								up	: Vector4	= null) : Transform3D
		{
			invalidateRawData();
			matrix.pointAt(pos._vector,
						   at._vector,
						   up._vector);
			
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
			invalidateRawData();
			matrix.prependRotation(radians * RAD2DEG, 
								   axis._vector,
								   pivotPoint ? pivotPoint._vector : null);
			
			return this;
		}
		
		public function prependScale(x	: Number,
									 y	: Number = 1.,
									 z	: Number = 1.) : Transform3D
		{
			invalidateRawData();
			matrix.prependScale(x, y, z);
			
			return this;
		}
		
		public function prependTranslation(x : Number,
										   y : Number	= 1.,
										   z : Number	= 1.) : Transform3D
		{
			invalidateRawData();
			matrix.prependTranslation(x, y, z);
			
			return this;
		}
		
		public function setTranslation(translation : Vector4) : Transform3D
		{
			this.translation = translation;
			
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