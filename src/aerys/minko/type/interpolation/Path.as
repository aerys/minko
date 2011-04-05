package aerys.minko.type.interpolation
{
	import aerys.minko.type.Transform3D;
	import aerys.minko.type.math.ConstVector4;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;

	public class Path implements IInterpolation
	{
		private static const DEFAULT_AT	: ConstVector4 = new ConstVector4(0, 0, -1);
		private static const DEFAULT_UP	: ConstVector4 = new ConstVector4(0, -1, 0);
		
		protected var _at					: Vector4;
		protected var _up					: Vector4;
		
		protected var _start				: Vector4;
		
		protected var _segments				: Vector.<IInterpolation>;
		protected var _length				: Number;
		
		/**
		 * Used only for debug.
		 * Instanciate a new vector at every call.
		 *  
		 * @return list of checkpoints in the path. 
		 */		
		public function get checkpoints() : Vector.<Vector4>
		{
			var r : Vector.<Vector4> = new Vector.<Vector4>();
			r.push(_start);
			for each (var segment : IInterpolation in _segments)
				r.push(segment.end);
			return r;
		}
		
		public function get start() : Vector4
		{
			return _start;
		}
		
		public function get end() : Vector4
		{
			var segmentCount : uint = _segments.length;
			return segmentCount != 0 ? _lastSegment.end : _start;
		}
		
		public function get length() : Number
		{
			if (isNaN(_length))
			{
				_length = 0;
				for each (var segment : AbstractSegment in _segments)
					_length += segment.length;
			}
			
			return _length;
		}
		
		public function set start(value : Vector4) : void
		{
			var segmentCount : uint = _segments.length;
			if (segmentCount != 0)
			{
				_firstSegment.start = value;
				
				// if this is a closed loop, we also replace the last point.
				if (_lastSegment.end == _start)
					_lastSegment.end = value;
			}
			_start = value;
		}
		
		public function set end(value : Vector4) : void
		{
			var segmentCount : uint = _segments.length;
			if (segmentCount == 0)
				_start = value;
			else
			{
				// if this is a closed loop, we also replace the last point.
				if (_firstSegment.start == _lastSegment.end)
					_firstSegment.start = value;
				
				_lastSegment.end = value;
			}
		}
		
		public function position(t : Number, out : Vector4) : Vector4
		{
			var localT	: Number = getLocalT(t);
			var segment	: IInterpolation = getSegment(t);
			return segment.position(localT, out);
		}
		
		public function tangent(t : Number, out : Vector4) : Vector4
		{
			var localT	: Number = getLocalT(t);
			var segment	: IInterpolation = getSegment(t);
			return segment.tangent(localT, out);
		}
		
		public function pointAt(t : Number, out : Vector4) : Vector4
		{
			var localT	: Number = getLocalT(t);
			var segment	: IInterpolation = getSegment(t);
			return segment.tangent(localT, out);
		}
		
		/**
		 * Alias for position
		 */
		public function translation(t : Number, out : Vector4) : Vector4
		{
			return position(t, out);
		}
		
		/**
		 * Alias for tangent
		 */
		public function lookAt(t : Number, out : Vector4) : Vector4
		{
			return tangent(t, out);
		}
		
		public function updateMatrix(matrix : Matrix4x4, t : Number) : void
		{
			var localT	: Number = getLocalT(t);
			var segment	: IInterpolation = getSegment(t);
			segment.updateMatrix(matrix, localT);
		}
		
		public function updateTransform(transform : Transform3D, t : Number) : void
		{
			var localT	: Number = getLocalT(t);
			var segment	: IInterpolation = getSegment(t);
			segment.updateTransform(transform, localT);
		}
		
		protected function get _firstSegment() : IInterpolation
		{
			var segmentCount : uint = _segments.length;
			return segmentCount != 0 ? _segments[0] : null;
		}
		
		protected function get _lastSegment() : IInterpolation
		{
			var segmentCount : uint = _segments.length;
			return segmentCount != 0 ? _segments[segmentCount - 1] : null;
		}
		
		/**
		 * @param start First checkpoint of the path.
		 * @param at forward direction of the elements in this group. This vector will be kept parralel to the tangent of the path when updating the ratio value.
		 * @param up  up direction of the elements in this group. This vector will determine the rotation around the tangent of the path when updateing the ratio value.
		 * @param ...childen Children to be addChilded at the group creation
		 */
		public function Path(start 	: Vector4,
							 at		: Vector4 = null,
							 up		: Vector4 = null)
		{
			_at			= at || DEFAULT_AT;
			_up			= up || DEFAULT_UP;
			_start		= start;
			_segments	= new Vector.<IInterpolation>();
		}
		
		/**
		 * Append a straight line to the path.
		 * 
		 * @param end End of the segment, default to the first point of the path
		 * @param begin Begin of the segment, default to last checkpoint.
		 */
		public function addLinearSegment(end	: Vector4 = null, 
										 begin	: Vector4 = null) : Path
		{
			if (end == null) end = this.start;
			if (begin == null) begin = this.end;
			
			_segments.push(new LinearSegment(begin, end, _at, _up));
			
			return this;
		}
		
		/**
		 * Append a cubic segment to the path
		 * 
		 * @param end End of the segment, default to the first point of the path
		 * @param begin Begin of the segment, default to last checkpoint.
		 */
		public function addCubicSegment(end		: Vector4 = null,
										begin	: Vector4 = null) : Path
		{
			if (begin == null) begin = this.end;
			if (end == null) end = this.start;
			
			var lastSegmentStart : Vector4 = _segments.length == 0 ? _start : _lastSegment.start;
			var newSegment : IInterpolation =
				new CubicSegment(begin, end, lastSegmentStart, null, _at, _up);
			
			updateLastSegment(newSegment);
			_segments.push(newSegment);
			
			return this;
		}
		
		/**
		 * Append a Catmull-Rom cubic segment to the path
		 * 
		 * @param end End of the segment, default to the first point of the path
		 * @param begin Begin of the segment, default to last checkpoint.
		 */
		public function addCatmullRomSegment(end	: Vector4 = null, 
											 begin	: Vector4 = null) : Path
		{
			if (begin == null) begin = this.end;
			if (end == null) end = this.start;
			
			var lastSegmentStart : Vector4 = _segments.length == 0 ? _start : _lastSegment.start;
			var newSegment : IInterpolation =
				new CatmullRomSegment(begin, end, lastSegmentStart, null, _at, _up);
			
			updateLastSegment(newSegment);
			_segments.push(newSegment);
			
			return this;
		}
		
		/**
		 * Append an Hermite segment to the path
		 * 
		 * @param end End of the segment, default to the first point of the path
		 * @param tension used to tighten up the curvature at the known points. 1 is high, 0 normal, -1 is low.
		 * @param bias used to twist the curve about the known points. 0 is even, positive is towards first segment, negative towards the other
		 * @param begin Begin of the segment, default to last checkpoint.
		 */
		public function addHermiteSegment(end		: Vector4 = null,
										  tension	: Number = 0,
										  bias		: Number = 0,
										  begin		: Vector4 = null) : Path
		{
			if (begin == null) begin = this.end;
			if (end == null) end = this.start;
			
			var lastSegmentStart : Vector4 = _segments.length == 0 ? _start : _lastSegment.start;
			var newSegment : IInterpolation =
				new HermiteSegment(begin, end, bias, tension, lastSegmentStart, null, _at, _up);
			
			updateLastSegment(newSegment);
			_segments.push(newSegment);
			
			return this;
		}
		
		/**
		 * Append a Bezier quadratic segment to the path
		 * 
		 * @param control Bezier control point
		 * @param end End of the segment, default to the first point of the path
		 * @param begin Begin of the segment, default to last checkpoint.
		 * @see Wikipedia article about bezier curves <http://en.wikipedia.org/wiki/B%C3%A9zier_curve#Quadratic_curves>
		 */
		public function addBezierQuadSegment(control	: Vector4,
											 end		: Vector4 = null,
											 begin		: Vector4 = null) : Path
		{
			if (begin == null) begin = this.end;
			if (end == null) end = this.start;
			
			var newSegment : IInterpolation =
				new BezierQuadSegment(begin, control, end, _at, _up);
			
			_segments.push(newSegment);
			
			return this;
		}
		
		/**
		 * Append a Bezier cubic segment to the path.
		 * 
		 * @param control1 Bezier control point
		 * @param control2 Bezier control point
		 * @param end End of the segment, default to the first point of the path
		 * @param begin Begin of the segment, default to last checkpoint.
		 * @see http://en.wikipedia.org/wiki/B%C3%A9zier_curve
		 */
		public function addBezierCubicSegment(control1	: Vector4, 
											  control2	: Vector4, 
											  end		: Vector4 = null,
											  begin		: Vector4 = null) : Path
		{
			if (begin == null) begin = this.end;
			if (end == null) end = this.start;
			
			var newSegment : IInterpolation = 
				new BezierCubicSegment(begin, control1, control2, end, _at, _up);
			
			_segments.push(newSegment);
			
			return this;
		}
		
		/**
		 * Append a straight line to the path, going in the same direction.
		 * that the tangent at the last checkpoint.
		 * 
		 * @param length The length of the segment which is appended
		 */
		public function addSmoothLinearSegment(length : Number) : Path
		{
			var segmentCount : uint = _segments.length;
			if (segmentCount == 0)
				throw new Error('There is no initial segment to be C1 with');
			
			var lastSegment : AbstractBezierSegment = _lastSegment as AbstractBezierSegment;
			if (lastSegment == null)
				throw new Error('Last segment was not a bezier segment. Cannot append a smooth segment.');
			
			var end : Vector4 = 
				getControlSymetric(length, lastSegment.lastControl, lastSegment.end);
			
			return addLinearSegment(end);
		}
		
		/**
		 * Append a bezier quadratic segment to the path, which smoothly extends it.
		 * To use this method, last segment must be a linear or bezier segment.
		 * 
		 * @param end End of the segment, default to the first point of the path
		 * @param controlDistance
		 * @see Wikipedia article about bezier curves <http://en.wikipedia.org/wiki/B%C3%A9zier_curve#Quadratic_curves>
		 */
		public function addSmoothBezierQuadSegment(end				: Vector4 = null,
												   controlDistance	: Number = 0) : Path
		{
			var segmentCount : uint = _segments.length;
			if (segmentCount == 0)
				throw new Error('There is no initial segment to be C1 with');
			
			var lastSegment : AbstractBezierSegment = _lastSegment as AbstractBezierSegment;
			if (lastSegment == null)
				throw new Error('Last segment was not a bezier segment. Cannot append a smooth segment.');
			
			var control : Vector4 = 
				getControlSymetric(controlDistance, lastSegment.lastControl, lastSegment.end);
			
			return addBezierQuadSegment(control, end);
		}
		
		/**
		 * Append a bezier cubic segment to the path, which smoothly extends it.
		 * To use this method, last segment must be a linear or bezier segment.
		 * 
		 * @param control2
		 * @param end End of the segment, default to the first point of the path
		 * @param control1Distance
		 * @see http://en.wikipedia.org/wiki/B%C3%A9zier_curve
		 */
		public function addSmoothBezierCubicSegment(control2			: Vector4,
													end					: Vector4 = null,
													control1Distance	: Number = 0) : Path
		{
			var segmentCount : uint = _segments.length;
			if (segmentCount == 0)
				throw new Error('There is no initial segment to be C1 with');
			
			var lastSegment : AbstractBezierSegment = _lastSegment as AbstractBezierSegment;
			if (lastSegment == null)
				throw new Error('Last segment was not a bezier segment. Cannot append a smooth segment.');
			
			var control1 : Vector4 = 
				getControlSymetric(control1Distance, lastSegment.lastControl, lastSegment.end);
			
			addBezierCubicSegment(control1, control2, end);
			return this;
		}
		
		public function addSegment(segment : IInterpolation) : Path
		{
			throw new Error('implement me');
		}
		
		
		/**
		 * Close the loop, adding a straight line between the last and first checkpoint.
		 */
		public function closeLoopLinear() : Path
		{
			return addLinearSegment();
		}
		
		/**
		 * Close the loop, adding a cubic segment between the first and last checkpoint.
		 * If the first segment of the path is cubic, the path will be smooth.
		 */
		public function closeLoopCubic() : Path
		{
			addCubicSegment();
			
			var firstCubicSegment : CubicSegment = _firstSegment as CubicSegment;
			if (firstCubicSegment)
				firstCubicSegment.previous = _lastSegment.start;
			
			// always is cubic, because we just added it.
			var lastSegment : CubicSegment = _lastSegment as CubicSegment;
			lastSegment.next = _firstSegment.end; // even if first is not cubic
			
			
			return this;
		}
		
		/**
		 * Close the loop, adding a Catmull-Rom segment between the first and last checkpoint.
		 * If the first segment of the path is Catmull-Rom, the path will be smooth.
		 */
		public function closeLoopCatmullRom() : Path
		{
			addCatmullRomSegment();
			
			var firstCatmullSegment : CatmullRomSegment = _firstSegment as CatmullRomSegment;
			if (firstCatmullSegment)
				firstCatmullSegment.previous = _lastSegment.start;
			
			return this;
		}
		
		/**
		 * Close the loop, adding a Hermite segment between the first and last checkpoint.
		 * If the first segment of the path is Hermite, the path will be smooth.
		 */
		public function closeLoopHermite() : Path
		{
			addHermiteSegment();
			
			var firstHermiteSegment : HermiteSegment = _firstSegment as HermiteSegment;
			if (firstHermiteSegment)
				firstHermiteSegment.previous = _lastSegment.start;
			
			return this;
		}
		
		/**
		 * Close the loop adding a Bezier Cubic segment.
		 * First and last segment must be Bezier.
		 * 
		 * @param control1Distance 
		 * @param control2Distance 
		 * @see http://en.wikipedia.org/wiki/B%C3%A9zier_curve#Quadratic_curves
		 */
		public function closeLoopSmoothBezierCubic(control1Distance : Number = 0,
												   control2Distance : Number = 0) : Path
		{
			if (_segments.length == 0)
				throw new Error('There is no path to close');
			
			if (control1Distance < 0 || control2Distance < 0)
				throw new Error('Control point distances cannot be negative');
			
			var firstSegment : AbstractBezierSegment = _firstSegment as AbstractBezierSegment;
			var lastSegment	 : AbstractBezierSegment = _lastSegment as AbstractBezierSegment;
			
			if (firstSegment == null || lastSegment == null)
				throw new Error('First an last segment must be of Bezier type to be able to close the loop.');
			
			var control1 : Vector4 = 
				getControlSymetric(control1Distance, lastSegment.lastControl, lastSegment.end);
			
			var control2 : Vector4 = 
				getControlSymetric(control2Distance, firstSegment.lastControl, firstSegment.start);
			
			return addBezierCubicSegment(control1, control2);
		}
		
		private function updateLastSegment(newSegment : IInterpolation) : void
		{
			var lastSegment : IInterpolation = _lastSegment;
			if (lastSegment == null)
			{
			}
			else if (lastSegment is CubicSegment)
			{
				CubicSegment(lastSegment).next = newSegment.end;
			}
			else if (lastSegment is HermiteSegment)
			{
				HermiteSegment(lastSegment).next = newSegment.end;
			}
		}
		
		private function getLocalT(t : Number) : Number
		{
			t = (t < 0 ? - t : t) % 1;
			return (t * _segments.length) % 1;
		}
		
		private function getSegment(t : Number) : IInterpolation
		{
			t = (t < 0 ? - t : t) % 1;
			return _segments[int(t * _segments.length)];
		}
		
		private function getControlSymetric(distance	: Number, 
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
					2 * center.w - control.w
				);
			}
			else
			{
				control = new Vector4(
					center.x - control.x,
					center.y - control.y,
					center.z - control.z,
					center.w - control.w
				);
				control.scaleBy(distance / control.length);
				control.set(
					control.x + center.x, 
					control.y + center.y, 
					control.z + center.z, 
					control.w + center.w
				);
			}
			return control;
		}
	}
}