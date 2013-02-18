package aerys.minko.render.geometry.primitive
{
	import aerys.minko.render.geometry.Geometry;
	import aerys.minko.render.geometry.stream.IVertexStream;
	import aerys.minko.render.geometry.stream.IndexStream;
	import aerys.minko.render.geometry.stream.StreamUsage;
	import aerys.minko.render.geometry.stream.VertexStream;
	import aerys.minko.render.geometry.stream.format.VertexComponent;
	import aerys.minko.render.geometry.stream.format.VertexComponentType;
	import aerys.minko.render.geometry.stream.format.VertexFormat;
	
	public class LineGeometry extends Geometry
	{
		public static const VERTEX_LINE_START	: VertexComponent	= new VertexComponent(
			['lineStartX', 'lineStartY', 'lineStartZ', 'lineStartWeight'],
			VertexComponentType.FLOAT_4
		);
		public static const VERTEX_LINE_STOP	: VertexComponent	= new VertexComponent(
			['lineStopX', 'lineStopY', 'lineStopZ', 'lineStopWeight'],
			VertexComponentType.FLOAT_4
		);
		
		public static const VERTEX_LINE_SPREAD	: VertexComponent	= new VertexComponent(
			['lineSpread'],
			VertexComponentType.FLOAT_1
		);
		
		public static const VERTEX_LINE_FORMAT	: VertexFormat		= new VertexFormat(
			VERTEX_LINE_START,
			VERTEX_LINE_STOP,
			VERTEX_LINE_SPREAD
		);
		
		public static const MAX_NUM_LINES		: uint				= 16000;
		
		private var _currentX	: Number;
		private var _currentY	: Number;
		private var _currentZ	: Number;
		
		private var _numLines	: uint;
		
		private var _vstream	: VertexStream;
		private var _istream	: IndexStream;
		
		public function LineGeometry()
		{
			_vstream = new VertexStream(StreamUsage.DYNAMIC, VERTEX_LINE_FORMAT);
			_istream = new IndexStream(StreamUsage.DYNAMIC);
			
			super(new <IVertexStream>[_vstream], _istream);
            
            moveTo(0, 0, 0);
		}
		
		public function moveTo(x : Number, y : Number, z : Number) : LineGeometry
		{
			_currentX = x;
			_currentY = y;
			_currentZ = z;
			
			return this;
		}
		
		public function lineTo(x            : Number,
                               y            : Number,
                               z            : Number,
                               numSegments  : uint      = 1) : LineGeometry
		{
            var vertices    : Vector.<Number>   = new <Number>[];
            var indices     : Vector.<uint>     = new <uint>[];
            
            for (var segmentId : uint = 0; segmentId < numSegments; ++segmentId)
            {
                if (_numLines == MAX_NUM_LINES)
                    throw new Error();
                
                var sx : Number = _currentX + 1 / numSegments * (x - _currentX);
                var sy : Number = _currentY + 1 / numSegments * (y - _currentY);
                var sz : Number = _currentZ + 1 / numSegments * (z - _currentZ);
                
                vertices.push(
                    _currentX, _currentY, _currentZ, 1, sx, sy, sz, 0, -1,
                    _currentX, _currentY, _currentZ, 1, sx, sy, sz, 0, 1,
                    _currentX, _currentY, _currentZ, 0, sx, sy, sz, 1, 1,
                    _currentX, _currentY, _currentZ, 0, sx, sy, sz, 1, -1
                );
                
                var indexOffset : uint = _numLines * 4;
                
                indices.push(
                    indexOffset, indexOffset + 1, indexOffset + 2,
                    indexOffset, indexOffset + 2, indexOffset + 3
                );
                
                _currentX = sx;
                _currentY = sy;
                _currentZ = sz;
                ++_numLines;
            }
            
            _vstream.pushFloats(vertices);
            _istream.pushVector(indices);
			
			return this;
		}
	}
}