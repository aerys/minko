package aerys.minko.scene.group
{
	import aerys.minko.scene.Model3D;
	import aerys.minko.scene.material.ColorMaterial3D;
	import aerys.minko.scene.material.IMaterial3D;
	import aerys.minko.scene.mesh.IMesh3D;
	import aerys.minko.scene.mesh.primitive.TriangleMesh3D;
	import aerys.minko.type.Transform3D;
	import aerys.minko.type.math.Vector4;
	import aerys.minko.type.interpolation.BezierCubicSegment;
	import aerys.minko.type.interpolation.BezierQuadSegment;
	import aerys.minko.type.interpolation.CatmullRomSegment;
	import aerys.minko.type.interpolation.CubicSegment;
	import aerys.minko.type.interpolation.HermiteSegment;
	import aerys.minko.type.interpolation.AbstractSegment;
	import aerys.minko.type.interpolation.LinearSegment;
	
	public class PathGroup3D extends TransformGroup3D
	{
		protected var _debugContainer		: IGroup3D;
		
		protected var _atVector				: Vector4;
		protected var _upVector				: Vector4;
		
		protected var _begin				: Vector4;
		protected var _lastBegin			: Vector4;
		protected var _firstBezierControl	: Vector4;
		protected var _lastBezierControl	: Vector4;
		protected var _end					: Vector4;
		
		protected var _segments				: Vector.<AbstractSegment>;
		protected var _lastRatio			: Number;
		
		public function set debugContainer(v : IGroup3D) : void
		{
			_debugContainer = v;
		}
		
		public function get ratio() : Number
		{
			return _lastRatio;
		}
		
		public function set ratio(t : Number) : void
		{
			_lastRatio = t;
			var segmentCount : uint	= _segments.length;
			t = (t < 0 ? - t : t) % 1;
			
			if (segmentCount != 0)
			{
				var segment	: AbstractSegment = _segments[int(t * segmentCount)];
				var localT	: Number		  = (t * segmentCount) % 1;
				segment.setTransform(transform, localT);
			}
			else
			{
				transform.position.set(_end.x, _end.y, _end.z, 0);
			}
		}
		
		public function PathGroup3D(start 		: Vector4,
									atVector	: Vector4,
									upVector	: Vector4,
									...children)
		{
			super(children);
			
			_upVector	= _upVector;
			_begin		= start;
			_end		= start;
			_segments	= new Vector.<AbstractSegment>();
			
			ratio = 0;
		}
		
		/**
		 * Append a straight line to the path
		 * 
		 * @var end
		 * @var begin
		 */
		public function addLinearSegment(end	: Vector4, 
										 begin	: Vector4 = null) : PathGroup3D
		{
			if (begin == null)
				begin = _end;
			
			_segments.push(new LinearSegment(begin, end));
			_lastBegin = begin;
			_lastBezierControl = begin;
			_end = end;
			
			if (_segments.length > 1 && _segments[_segments.length - 2] is CubicSegment)
				CubicSegment(_segments[_segments.length - 2]).next = end;
			
			if (_firstBezierControl == null && _segments.length == 1)
				_firstBezierControl = end;
			
			if (_debugContainer)
				addDebugMarker(end, false);
			
			return this;
		}
		
		/**
		 * Append a cosine segment to the path
		 * 
		 * @var end
		 * @var begin
		 */
		public function addCosineSegment(end	: Vector4,
										 begin	: Vector4 = null) : PathGroup3D
		{
			if (begin == null)
				begin = _end;
			
			_segments.push(new CubicSegment(begin, end, _lastBegin, null));
			_lastBegin = begin;
			_lastBezierControl = null;
			_end = end;
			
			if (_segments.length > 1 && _segments[_segments.length - 2] is CubicSegment)
				CubicSegment(_segments[_segments.length - 2]).next = end;
			
			return this;
		}
		
		/**
		 * Append a cubic segment to the path
		 * 
		 * @var end
		 * @var begin
		 */
		public function addCubicSegment(end		: Vector4, 
										begin	: Vector4 = null) : PathGroup3D
		{
			if (begin == null)
				begin = _end;
			
			_segments.push(new CubicSegment(begin, end, _lastBegin, null));
			_lastBegin = begin;
			_lastBezierControl = null;
			_end = end;
			
			if (_segments.length > 1 && _segments[_segments.length - 2] is CubicSegment)
				CubicSegment(_segments[_segments.length - 2]).next = end;
			
			return this;
		}
		
		/**
		 * Append a Catmull-Rom cubic segment to the path
		 * 
		 * @var end
		 * @var begin
		 */
		public function addCatmullRomSegment(end	: Vector4, 
											 begin	: Vector4 = null) : PathGroup3D
		{
			if (begin == null)
				begin = _end;
			
			_segments.push(new CatmullRomSegment(begin, end, _lastBegin, null));
			_lastBegin = begin;
			_lastBezierControl = null;
			_end = end;
			
			if (_segments.length > 1 && _segments[_segments.length - 2] is CubicSegment)
				CubicSegment(_segments[_segments.length - 2]).next = end;
			
			
			return this;
		}
		
		/**
		 * Append an Hermite segment to the path
		 * 
		 * @var end
		 * @var tension
		 * @var bias
		 * @var begin
		 */
		public function addHermiteSegment(end		: Vector4,
										  tension	: Number = 0,
										  bias		: Number = 0,
										  begin		: Vector4 = null) : PathGroup3D
		{
			if (begin == null)
				begin = _end;
			
			_segments.push(new HermiteSegment(begin, end, bias, tension, _lastBegin, null));
			_lastBegin = begin;
			_lastBezierControl = null;
			_end = end;
			
			if (_segments.length > 1 && _segments[_segments.length - 2] is CubicSegment)
				CubicSegment(_segments[_segments.length - 2]).next = end;
			
			return this;	
		}
		
		/**
		 * Append a Bezier quadratic segment to the path
		 * 
		 * @var control
		 * @var end
		 * @var begin
		 */
		public function addBezierQuadSegment(control	: Vector4,
											 end		: Vector4,
											 begin		: Vector4 = null) : PathGroup3D
		{
			if (begin == null)
				begin = _end;
			
			_segments.push(new BezierQuadSegment(begin, control, end));
			_lastBegin = begin;
			_lastBezierControl = control;
			_end = end;
			
			if (_segments.length > 1 && _segments[_segments.length - 2] is CubicSegment)
				CubicSegment(_segments[_segments.length - 2]).next = end;
			
			if (_firstBezierControl == null && _segments.length == 1)
				_firstBezierControl = control;
			
			if (_debugContainer)
			{
				addDebugMarker(control, true, false);
				addDebugMarker(end, false);
			}
			
			return this;
		}
		
		/**
		 * Append a Bezier cubic segment to the path.
		 * 
		 * @var control1 
		 * @var control2
		 * @var end
		 * @var begin
		 */
		public function addBezierCubicSegment(control1	: Vector4, 
											  control2	: Vector4, 
								   			  end		: Vector4,
											  begin		: Vector4 = null) : PathGroup3D
		{
			if (begin == null)
				begin = _end;
			
			_segments.push(new BezierCubicSegment(begin, control1, control2, end));
			_lastBegin = begin;
			_lastBezierControl = control2;
			_end = end;
			
			if (_segments.length > 1 && _segments[_segments.length - 2] is CubicSegment)
				CubicSegment(_segments[_segments.length - 2]).next = end;
			
			if (_firstBezierControl == null && _segments.length == 1)
				_firstBezierControl = control1;
			
			if (_debugContainer)
			{
				addDebugMarker(control1, true, false);
				addDebugMarker(control2, true, false);
				addDebugMarker(end, false);
			}
			
			return this;
		}
		
		/**
		 * Append a straight line to the path, going in the same direction.
		 * that the tangent at the last checkpoint.
		 * 
		 * @var length The length of the segment which is appended
		 */
		public function addSmoothLinearSegment(length : Number) : PathGroup3D
		{
			var segmentCount : uint = _segments.length;
			if (segmentCount == 0)
				throw new Error('There is no initial segment to be C1 with');
			
			var end : Vector4 = getControlSymetric(length, _lastBezierControl, _end);
			
			_segments.push(new LinearSegment(_end, end));
			_lastBegin = _end;
			_lastBezierControl = _end;
			_end = end;
			
			if (_segments.length > 1 && _segments[_segments.length - 2] is CubicSegment)
				CubicSegment(_segments[_segments.length - 2]).next = end;
			
			if (_debugContainer)
			{
				addDebugMarker(end, false);
			}
			
			return this;
		}
		
		/**
		 * Append a bezier quadratic segment to the path, which smoothly extends it.
		 * To use this method, last segment must be a linear or bezier segment.
		 * 
		 * @var end
		 * @var controlDistance
		 */
		public function addSmoothBezierQuadSegment(end				: Vector4, 
												   controlDistance	: Number = 0) : PathGroup3D
		{
			var segmentCount : uint = _segments.length;
			if (segmentCount == 0)
				throw new Error('There is no initial segment to be C1 with');
			
			if (_lastBezierControl == null)
				throw new Error('Last segment was not a bezier segment. Cannot append a smooth segment.');
			
			var control : Vector4 = getControlSymetric(controlDistance, _lastBezierControl, _end);
			
			_segments.push(new BezierQuadSegment(_end, control, end));
			_lastBegin = _end;
			_lastBezierControl = control;
			_end = end;
			
			if (_segments.length > 1 && _segments[_segments.length - 2] is CubicSegment)
				CubicSegment(_segments[_segments.length - 2]).next = end;
			
			if (_debugContainer)
			{
				addDebugMarker(control, true, true);
				addDebugMarker(end, false);
			}
			
			return this;
		}
		
		/**
		 * Append a bezier cubic segment to the path, which smoothly extends it.
		 * To use this method, last segment must be a linear or bezier segment.
		 * 
		 * @var control2
		 * @var end
		 * @var control1Distance
		 */
		public function addSmoothBezierCubicSegment(control2			: Vector4,
													end					: Vector4,
													control1Distance	: Number = 0) : PathGroup3D
		{
			var segmentCount : uint = _segments.length;
			if (segmentCount == 0)
				throw new Error('There is no initial segment to be C1 with');
			
			if (_lastBezierControl == null)
				throw new Error('Last segment was not a bezier segment. Cannot append a smooth segment.');
			
			var control1 : Vector4 = getControlSymetric(control1Distance, _lastBezierControl, _end);
			
			_segments.push(new BezierCubicSegment(_end, control1, control2, end));
			_lastBegin = _end;
			_lastBezierControl = control2;
			_end = end;
			
			if (_segments.length > 1 && _segments[_segments.length - 2] is CubicSegment)
				CubicSegment(_segments[_segments.length - 2]).next = end;
			
			if (_debugContainer)
			{
				addDebugMarker(control1, true, true);
				addDebugMarker(control2, true, false);
				addDebugMarker(end, false);
			}
			
			return this;
		}
		
		/**
		 * Close the loop, adding a line between the last and first checkpoint.
		 */
		public function closeLoopLinear() : PathGroup3D
		{
			if (_segments.length == 0)
				throw new Error('There is no path to close');
			
			_segments.push(new LinearSegment(_end, _begin));
			_lastBegin = _end;
			_lastBezierControl = _end;
			_end = _begin;
			
			if (_segments.length > 1 && _segments[_segments.length - 2] is CubicSegment)
				CubicSegment(_segments[_segments.length - 2]).next = _begin;
			
			if (_debugContainer)
				addDebugMarker(_begin, false);
			
			return this;
		}
		
		/**
		 * Close the loop adding a Bezier Cubic segment.
		 * First and last segment must be Bezier.
		 * 
		 * @var control1Distance 
		 * @var control2Distance 
		 */
		public function closeLoopBezierCubic(control1Distance : Number = 0,
											 control2Distance : Number = 0) : PathGroup3D
		{
			if (_segments.length == 0)
				throw new Error('There is no path to close');
			
			if (control1Distance < 0 || control2Distance < 0)
				throw new Error('Control point distances cannot be negative');
			
			if (_firstBezierControl == null || _lastBezierControl == null)
				throw new Error('First an last segment must be of Bezier type to be able to close the loop.');
			
			var control1 : Vector4 = getControlSymetric(control1Distance, _lastBezierControl, _end);
			var control2 : Vector4 = getControlSymetric(control2Distance, _firstBezierControl, _begin);
			
			_segments.push(new BezierCubicSegment(_end, control1, control2, _begin));
			_lastBegin = _end;
			_lastBezierControl = control2;
			_end = _begin;
			
			if (_segments.length > 1 && _segments[_segments.length - 2] is CubicSegment)
				CubicSegment(_segments[_segments.length - 2]).next = _begin;
			
			if (_debugContainer)
			{
				addDebugMarker(control1, true, true);
				addDebugMarker(control2, true, false);
				addDebugMarker(_begin, false);
			}
			
			return this;
		}
		
		protected function getControlSymetric(distance	: Number, 
											  control	: Vector4, 
											  center	: Vector4) : Vector4
		{
			var control : Vector4;
			
			if (distance < 0)
			{
				throw new Error('Vector length cannot be a negative Number');
			}
			else if (distance == 0)
			{
				control = new Vector4(
					2 * center.x - control.x,
					2 * center.y - control.y,
					2 * center.z - control.z,
					0
				);
			}
			else
			{
				control = new Vector4(
					center.x - control.x,
					center.y - control.y,
					center.z - control.z,
					0
				);
				control.scaleBy(distance / control.length);
				control.set(
					control.x + center.x, 
					control.y + center.y, 
					control.z + center.z, 
					0
				);
			}
			return control;
		}
		
		protected function addDebugMarker(position	: Vector4, 
										  isControl	: Boolean, 
										  isAuto	: Boolean = false) : void 
		{
			var markerMesh	: IMesh3D = new TriangleMesh3D();
			var material	: IMaterial3D = isAuto ? ColorMaterial3D.ORANGE : isControl ? ColorMaterial3D.PURPLE : ColorMaterial3D.BLUE;
			
			var marker : Model3D = new Model3D(markerMesh, material);
			marker.transform.position.set(position.x, position.y, position.z, 0);
			_debugContainer.addChild(marker);
		}
	}
}
