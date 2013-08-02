package aerys.minko.render.geometry.stream
{
	import flash.utils.ByteArray;
	import flash.utils.Dictionary;
	import flash.utils.Endian;
	
	import aerys.minko.ns.minko_stream;
	import aerys.minko.render.geometry.stream.format.VertexComponent;
	import aerys.minko.render.geometry.stream.format.VertexComponentType;
	import aerys.minko.render.geometry.stream.format.VertexFormat;
	import aerys.minko.render.geometry.stream.iterator.VertexReference;
	import aerys.minko.render.resource.VertexBuffer3DResource;
	import aerys.minko.type.Signal;
	import aerys.minko.type.math.Matrix4x4;

	public final class VertexStream implements IVertexStream
	{
		use namespace minko_stream;

		public static const DEFAULT_FORMAT	: VertexFormat		= VertexFormat.XYZ_UV;
		
		private static const TMP_NUMBERS	: Vector.<Number>	= new <Number>[];
		
		minko_stream var _localDispose	: Boolean;
		minko_stream var _data			: ByteArray;
		
		private var _usage		    	: uint;
		private var _format		    	: VertexFormat;
		private var _resource	    	: VertexBuffer3DResource;
		
        private var _autoUpdateMinMax   : Boolean;
		private var _minimum	    	: Vector.<Number>;
		private var _maximum	    	: Vector.<Number>;
		
		private var _locked		    	: Boolean;
		
		private var _changed	    	: Signal;
		private var _boundsChanged  	: Signal;
		private var _contextLost		: Signal;
		
		public function get contextLost():Signal
		{
			return _contextLost;
		}

		public function get format() : VertexFormat
		{
			return _format;
		}
		
		public function get usage() : uint
		{
			return _usage;
		}
		
		public function get resource() : VertexBuffer3DResource
		{
			return _resource;
		}
		
		public function get numVertices() : uint
		{
			_data.position = 0;
			
			return _data.bytesAvailable / format.numBytesPerVertex;
		}
		
		public function get length() : uint
		{
			_data.position = 0;
			
			return _data.bytesAvailable >>> 2;
		}
		
		protected function get data() : ByteArray
		{
			_data.position = 0;
			
			return _data;
		}
		
		protected function set data(value : ByteArray) : void
		{
			if (value && value.length != 0 && value.bytesAvailable % _format.numBytesPerVertex)
			{
				throw new Error(
					'Invalid data: length does not match with the size of a vertex.'
				);
			}
			
			_data = value;
			_changed.execute(this);
			updateMinMax(true);
		}
		
		public function get locked() : Boolean
		{
			return _locked;
		}
		
		public function get changed() : Signal
		{
			return _changed;
		}
		
		public function get boundsChanged() : Signal
		{
			return _boundsChanged;
		}
        
        public function get autoUpdateMinMax() : Boolean
        {
            return _autoUpdateMinMax;
        }
        public function set autoUpdateMinMax(value : Boolean) : void
        {
            _autoUpdateMinMax = value;
        }

		public function VertexStream(usage	: uint,
									 format	: VertexFormat,
									 data 	: ByteArray	= null,
									 length	: uint		= 0)
		{
			super();
			
			initialize(format, usage, data, length);
		}
		
		private function contextLostHandler(resource : VertexBuffer3DResource) : void
		{
			_contextLost.execute(this);
		}
		
		private function initialize(format	: VertexFormat,
									usage	: uint,
									data 	: ByteArray,
									length	: uint) : void
		{
			_changed = new Signal('VertexStream.changed');
			_boundsChanged = new Signal('VertexStream.boundsChanged');
			_contextLost = new Signal('VertexStream.contextLost');
            
            _autoUpdateMinMax = true;
			_minimum = new <Number>[];
			_maximum = new <Number>[];
			
			_resource = new VertexBuffer3DResource(this);
			_resource.contextLost.add(contextLostHandler);
			
			_format = format || DEFAULT_FORMAT;
			_usage = usage;
			_localDispose = false;
			
			_data = new ByteArray();
			_data.endian = Endian.LITTLE_ENDIAN;
			if (data)
			{
				length ||= data.bytesAvailable;
				validateData(data, length);
				
				data.readBytes(_data, 0, length);
				_data.position = 0;
			}
            
   			updateMinMax(true);
		}
		
		private function validateData(data : ByteArray, length : uint = 0) : void
		{
			length ||= data.bytesAvailable;
			
			if (data.endian != Endian.LITTLE_ENDIAN)
				throw new Error('Endianness must be Endian.LITTLE_ENDIAN.');
			if (length % format.numBytesPerVertex != 0)
				throw new Error('Incompatible format/length.');
		}
		
		public function updateMinMax(forceReadUsage : Boolean = false) : void
		{
			if (!forceReadUsage)
				checkReadUsage(this);
			
			var numFloatsPerVertex	: uint	= format.numBytesPerVertex >>> 2;
			var i					: uint	= 0;
			
			_minimum = new Vector.<Number>(numFloatsPerVertex, true);
			_maximum = new Vector.<Number>(numFloatsPerVertex, true);
			for (i = 0; i < numFloatsPerVertex; ++i)
			{
				_minimum[i] = Number.MAX_VALUE;
				_maximum[i] = -Number.MAX_VALUE;
			}
			
			var boundsHaveChanged 	: Boolean 	= false;
			var numFloats			: uint		= _data.bytesAvailable >>> 2;
			
			_data.position = 0;
			for (i = 0; i < numFloats; i += numFloatsPerVertex)
			{
				for (var j : uint = 0; j < numFloatsPerVertex; ++j)
				{
					var value : Number = _data.readFloat();
					
					if (value < _minimum[j])
					{
						_minimum[j] = value;
						boundsHaveChanged = true;
					}
					if (value > _maximum[j])
					{
						_maximum[j] = value;
						boundsHaveChanged = true;
					}
				}
			}
			_data.position = 0;
			
			if (boundsHaveChanged)
				_boundsChanged.execute(this);
		}
		
		/**
		 * Compute the minimum and maximum bounds of this VertexStream within the specified range
		 * 
		 * @param firstIndex The index of the first vertex to consider in the stream.
		 * @param numVertices The number of vertices to consider, starting from the previous parameter
		 * @param offsetComponents The offset in the VertexComponents
		 * @param indexStream The IndexStream where the indices will be read. If null, the indices will range from firstIndex to numVertices.
		 * @param min The Vector where the minimum bound will be stored.
		 * @param max The Vector where the maximum bound will be stored.
		 * 
		 * @return The number of components in a Vertex.
		 * Note that if min and max are too small and fixed, this function will do nothing and
		 * will just return the required length.
		 * 
		 */
		public function getMinMaxBetween(firstIndex			: uint,
										 numVertices		: uint,
										 offsetComponents	: uint,
										 indexStream		: IndexStream,
										 min				: Vector.<Number>,
										 max				: Vector.<Number>) : uint
		{
			var stride			: uint		= format.numBytesPerVertex >>> 2;
			var vectorLength	: uint		= stride - offsetComponents;
			var i				: uint		= 0;
			var j				: uint		= 0;
			var index			: uint		= 0;
			var value			: Number	= .0;
			
			if (min == null || min.length < vectorLength ||
				max == null || max.length < vectorLength)
			{
				if (min.fixed || max.fixed)
				{
					return vectorLength;
				}
				else
				{
					min.length = vectorLength;
					max.length = vectorLength;
				}
			}
			
			for (i = 0; i < vectorLength; ++i)
			{
				min[i] = Number.MAX_VALUE;
				max[i] = -Number.MAX_VALUE;
			}
			
			for (i = 0; i < numVertices; ++i)
			{
				index 			= indexStream != null ? indexStream.get(firstIndex + i) : i;
				_data.position	= (index * stride + offsetComponents) * 4;
				for (j = 0; j < vectorLength; ++j)
				{
					value = _data.readFloat();
					
					if (value < min[j])
					{
						min[j] = value;
					}
					if (value > max[j])
					{
						max[j] = value;
					}
				}
			}
			_data.position = 0;
			
			return stride;
		}
		
		public function deleteVertex(index : uint) : IVertexStream
		{
			checkWriteUsage(this);
			
			if (index > numVertices)
				throw new Error('Index out of bounds.');

			_data.position = 0;
			_data.readBytes(_data, _format.numBytesPerVertex * index);
			_data.length -= _format.numBytesPerVertex;
			
			if (!_locked)
			{
				_changed.execute(this);
				
				// FIXME: check if bounds have actually changed?
                if (_autoUpdateMinMax)
    				updateMinMax();
			}

			return this;
		}
		
		public function duplicateVertex(index : uint) : IVertexStream
		{
			var numBytesPerVertex : uint = format.numBytesPerVertex;
			
			_data.position = _data.length;
			_data.writeBytes(_data, numBytesPerVertex * index, numBytesPerVertex);
			_data.position = 0;
			
			if (!_locked)
				_changed.execute(this);
			
			return this;
		}

		public function getStreamByComponent(vertexComponent : VertexComponent) : VertexStream
		{
			return _format.hasComponent(vertexComponent) ? this : null;
		}

		public function get(index : uint) : Number
		{
			var value : Number = 0.;
			
			checkReadUsage(this);
			
			_data.position = index << 2;
			value = _data.readFloat();
			_data.position = 0;
			
			return value;
		}
		
		public function getVertexProperty(index 		: uint,
										  component 	: VertexComponent 	= null,
										  offset 		: uint 				= 0) : Number
		{
			index *= format.numBytesPerVertex >> 2;
			if (component)
				index += format.getOffsetForComponent(component);
			index += offset;
			
			return get(index);
		}
		
		public function getVertex(index : uint) : VertexReference
		{
			return new VertexReference(this, index);
		}
		
		public function getMinimum(index : uint) : Number
		{
			return _minimum[index];
		}
		
		public function getMaximum(index : uint) : Number
		{
			return _maximum[index];
		}
		
		/**
		 * Lock the stream in order to perform bulk updates without triggering
		 * any signal to get better performances.
		 *  
		 * @return 
		 * 
		 */
		public function lock() : ByteArray
		{
			if (_locked)
				throw new Error('The stream is already locked.');
			if (_localDispose)
				throw new Error('Cannot lock stream data which is already disposed.');
			
//			checkReadUsage(this);
//			checkWriteUsage(this);
			
			_locked = true;
			_data.position = 0;
			
			return _data;
		}
		
		/**
		 * Unlock the stream to reactivate signals. The VertexStream.changed signal
		 * is always executed when this method is called. The VertexStream.boundsChanged
		 * signal is called only if the bounds of the stream actually changed.
		 * 
		 */
		public function unlock(hasChanged : Boolean = true) : void
		{
			if (!_locked)
				throw new Error('Cannot unlock a stream that is not locked.');
			
			_locked = false;
			_data.position = 0;
			
			validateData(_data);
			
			if (hasChanged)
			{
				checkWriteUsage(this);
				
				_changed.execute(this);
                
                if (_autoUpdateMinMax)
    				updateMinMax(true);
			}
		}
		
		/**
		 * Set the value at the specified position in the stream.
		 * 
		 * @param offset
		 * @param value
		 * 
		 */
		public function set(index 		: uint,
							value 		: Number,
							component 	: VertexComponent 	= null,
							offset 		: uint 				= 0) : void
		{
			checkWriteUsage(this);
			
			_data.position = index << 2;
			_data.writeFloat(value);
			_data.position = 0;
			
			offset %= _format.numBytesPerVertex / 4;
			
			var boundsHaveChanged : Boolean = false;
			
			if (value < _minimum[offset])
			{
				_minimum[offset] = value;
				boundsHaveChanged = true;
			}
			else if (value > _maximum[offset])
			{
				_maximum[offset] = value;
				boundsHaveChanged = true;
			}
			
			if (!_locked)
			{
				_changed.execute(this);
                
                if (_autoUpdateMinMax)
                    updateMinMax(true);
				
				if (boundsHaveChanged)
					_boundsChanged.execute(this);
			}
		}
		
		public function setVertexProperty(index 		: uint,
										  value 		: Number,
										  component 	: VertexComponent 	= null,
										  offset	 	: uint 				= 0) : void
		{
			index *= format.numBytesPerVertex >> 2;
			if (component)
				index += format.getOffsetForComponent(component);
			index += offset;
			
			set(index, value, component, offset);
		}
		
		/**
		 * Push values at the end of stream. The pushed data must have
		 * a valid length regarding the vertex format of the stream.
		 *  
		 * @param data
		 * 
		 */
		public function pushBytes(data : ByteArray, offset : uint = 0, length : uint = 0) : VertexStream
		{
			checkWriteUsage(this);
			
			if (data.bytesAvailable % _format.numBytesPerVertex)
				throw new Error('Invalid data length.');
			
			_data.position = _data.bytesAvailable;
			_data.writeBytes(data, offset, length);
			_data.position = 0;
			
			if (!_locked)
			{
				_changed.execute(this);
				_boundsChanged.execute(this);
			}
			
			return this;
		}
		
		public function pushFloat(value : Number) : VertexStream
		{
			_data.position = _data.bytesAvailable;
			_data.writeFloat(value);
			_data.position = 0;
			
			if (!_locked)
			{
				_changed.execute(this);
				_boundsChanged.execute(this);
			}
			
			return this;
		}
		
		public function pushFloats(values : Vector.<Number>) : VertexStream
		{
			var numFloats : uint = values.length;
			
			_data.position = _data.bytesAvailable;
			for (var i : uint = 0; i < numFloats; ++i)
				_data.writeFloat(values[i]);
			_data.position = 0;
			
			if (!_locked)
			{
				_changed.execute(this);
				_boundsChanged.execute(this);
			}
			
			return this;
		}
		
		/**
		 * Apply matrix transformation to all vertices
		 * 
		 * @param component Vertex component to transform. Must be a 3-component vector
		 * @param transform Transformation matrix
		 * @param normalize Normalize vectors after transform
		 */
		public function applyTransform(component	: VertexComponent, 
									   transform	: Matrix4x4,
									   normalize	: Boolean	= false) : void
		{
			if (component.numProperties != 3)
				throw new Error('Vertex component \'' + component.toString() + '\' does not have a size of 3.');
				
			checkReadUsage(this);
			checkWriteUsage(this);
			
			var offset		: uint	= _format.getBytesOffsetForComponent(component);
			var vertexSize	: uint	= _format.numBytesPerVertex;
			var numVertices	: uint	= this.numVertices;
			
			TMP_NUMBERS.length = numVertices * 3;
			
			for (var i : uint = 0; i < numVertices; ++i)
			{
				_data.position = vertexSize * i + offset;
				
				TMP_NUMBERS[uint(i * 3)]		= _data.readFloat();
				TMP_NUMBERS[uint(i * 3 + 1)]	= _data.readFloat();
				TMP_NUMBERS[uint(i * 3 + 2)]	= _data.readFloat();
			}
				
			transform.transformRawVectors(TMP_NUMBERS, TMP_NUMBERS);
			
			for (i = 0; i < numVertices; ++i)
			{
				var x	: Number	= TMP_NUMBERS[uint(i * 3)];
				var y	: Number 	= TMP_NUMBERS[uint(i * 3 + 1)];
				var z	: Number 	= TMP_NUMBERS[uint(i * 3 + 2)];
		
				if (normalize)
				{
					var m	: Number 	= Math.sqrt(x * x + y * y + z * z);
					
					x /= m;
					y /= m;
					z /= m;
				}
			
				_data.position = vertexSize * i + offset;
				
				_data.writeFloat(x);
				_data.writeFloat(y);
				_data.writeFloat(z);
			}
			
			_data.position = 0;
			
			if (!_locked)
				_changed.execute(this);
			
            if (_autoUpdateMinMax)
    			updateMinMax();
		}
		
		public function disposeLocalData(waitForUpload : Boolean = true) : void
		{
			if (_locked)
				throw new Error('Stream data can not be disposed since it\'s locked for update.');
			if (waitForUpload && numVertices != resource.numVertices)
				_localDispose = true;
			else
			{
				_data = null;
				_usage = StreamUsage.STATIC;
			}
		}
		
		public function dispose() : void
		{
			resource.dispose();
		}

		public static function extractSubStream(source			: IVertexStream,
												usage			: uint,
										 		vertexFormat 	: VertexFormat	= null) : VertexStream
		{
			vertexFormat ||= source.format;
			
			var numComponents			: uint					= vertexFormat.numComponents;
			var componentOffsets		: Vector.<uint>			= new Vector.<uint>(numComponents, true);
			var componentSizes			: Vector.<uint>			= new Vector.<uint>(numComponents, true);
			var componentBytesPerVertex	: Vector.<uint>			= new Vector.<uint>(numComponents, true);
			var componentData			: Vector.<ByteArray>	= new Vector.<ByteArray>(numComponents, true);
			var streamToData			: Dictionary		 	= new Dictionary();
			
			// cache get offsets, sizes, and buffers for each components
			for (var k : int = 0; k < numComponents; ++k)
			{
				var vertexComponent	: VertexComponent	= vertexFormat.getComponent(k);
				var subVertexStream	: VertexStream		= source.getStreamByComponent(vertexComponent);
				var subvertexFormat	: VertexFormat		= subVertexStream.format;
				
				componentOffsets[k]			= subvertexFormat.getBytesOffsetForComponent(vertexComponent);
				componentBytesPerVertex[k]	= subvertexFormat.numBytesPerVertex;
				componentSizes[k]			= vertexComponent.numBytes;
				componentData[k]			= subVertexStream.locked
					? streamToData[subVertexStream]
					: (streamToData[subVertexStream] = subVertexStream.lock());
			}
			
			// push vertex data into the new buffer.
			var numVertices			: uint 		= source.numVertices;
			var newVertexStreamData	: ByteArray	= new ByteArray();
			
			newVertexStreamData.endian = Endian.LITTLE_ENDIAN;
			
			for (var vertexId : uint = 0; vertexId < numVertices; ++vertexId)
			{
				for (var componentId : uint = 0; componentId < numComponents; ++componentId)
				{
					newVertexStreamData.writeBytes(
						componentData[componentId] as ByteArray,
						componentOffsets[componentId] + vertexId * componentBytesPerVertex[componentId],
						componentSizes[componentId]
					);
				}
			}
			
			for (var subStream : Object in streamToData)
			{
				subVertexStream = subStream as VertexStream;
				if (subVertexStream.locked)
					subVertexStream.unlock(false);
			}
			
			// avoid copying data vectors
			var newVertexStream	 	: VertexStream	= new VertexStream(usage, vertexFormat);
			
			newVertexStreamData.position = 0;
			newVertexStream._data = newVertexStreamData;
            
			newVertexStream.updateMinMax();
			
			return newVertexStream;
		}
		
		private static function checkReadUsage(stream : VertexStream) : void
		{
			if (!(stream._usage & StreamUsage.READ))
			{
				throw new Error(
					'Unable to read from vertex stream: stream usage '
					+ 'is not set to StreamUsage.READ.'
				);
			}
		}
		
		private static function checkWriteUsage(stream : VertexStream) : void
		{
			if (!(stream._usage & StreamUsage.WRITE))
			{
				throw new Error(
					'Unable to write in vertex stream: stream usage '
					+ 'is not set to StreamUsage.WRITE.'
				);
			}
		}

		public static function concat(streams 	: Vector.<IVertexStream>,
									  usage 	: uint	= StreamUsage.STATIC) : VertexStream
		{
			var format		: VertexFormat	= streams[0].format;
			var numStreams 	: int			= streams.length;

			for (var i : int = 0; i < numStreams; ++i)
				if (!streams[i].format.equals(format))
					throw new Error('All vertex streams must have the same format.');

			// a bit expensive... but hey, it works :)
			var stream	: VertexStream	= extractSubStream(streams[0], usage, format);
			var data	: ByteArray		= stream._data;
			
			data.position = data.length;
			for  (i = 1; i < numStreams; ++i)
				data.writeBytes(extractSubStream(streams[i], usage, format)._data);
			data.position = 0;
            stream.updateMinMax(true);

			return stream;
		}
		
		public static function fromVector(usage 	: uint,
										  format 	: VertexFormat,
										  data 		: Vector.<Number>,
										  stream	: VertexStream = null) : VertexStream
		{
            if (data.length % (format.numBytesPerVertex >> 2))
                throw new Error('Invalid data length.');
			
			var numValues 	: uint 			= data.length;
			var bytes		: ByteArray 	= new ByteArray();
			bytes.endian = Endian.LITTLE_ENDIAN;
			
			stream ||= new VertexStream(usage, format);
			stream._data = bytes;
			
			for (var i : uint = 0; i < numValues; ++i)
				bytes.writeFloat(data[i]);
			
			bytes.position = 0;
			stream.updateMinMax(true);
			
			return stream;
		}

		public static function fromByteArray(bytes 			: ByteArray,
											 count			: int,
											 formatIn		: VertexFormat,
											 formatOut		: VertexFormat	= null,
											 usage			: uint			= 0,
											 readFunctions 	: Dictionary	= null,
											 dwordSize		: uint			= 4, 
											 stream			: VertexStream	= null) : VertexStream
		{
			if (bytes.endian != Endian.LITTLE_ENDIAN)
				throw new Error('Bytes must be in little endian.');
			
			formatOut ||= formatIn;
			
			stream ||= new VertexStream(usage, formatOut, null);
			stream._data = null;
			
			var dataLength		: uint				= 0;
			var start			: int				= bytes.position;
			var numComponents	: int				= formatOut.numComponents;
			var nativeFormats	: Vector.<int>		= new Vector.<int>(numComponents, true);
			
			for (var k : int = 0; k < numComponents; k++)
				nativeFormats[k] = formatOut.getComponent(k).nativeFormat;
			
			var data : ByteArray = new ByteArray();
			data.endian = Endian.LITTLE_ENDIAN;
			
			for (var vertexId : int = 0; vertexId < count; ++vertexId)
			{
				for (var componentId : int = 0; componentId < numComponents; ++componentId)
				{
					var componentOut : VertexComponent = formatOut.getComponent(componentId);
					
					bytes.position = start + (formatIn.numBytesPerVertex / 4) * vertexId * dwordSize
						+ formatIn.getBytesOffsetForComponent(componentOut) / 4 * dwordSize;
					
					var reader : Function = null;
					
					if (readFunctions[formatOut.getComponent(componentId)])
						reader = readFunctions[componentOut];
					else
						reader = bytes.readFloat;
					
					switch (nativeFormats[componentId])
					{
						case VertexComponentType.FLOAT_4 :
							data.writeFloat(reader(bytes, componentOut));
						case VertexComponentType.FLOAT_3 :
							data.writeFloat(reader(bytes, componentOut));
						case VertexComponentType.FLOAT_2 :
							data.writeFloat(reader(bytes, componentOut));
						case VertexComponentType.FLOAT_1 :
							data.writeFloat(reader(bytes, componentOut));
							break ;
					}
				}
			}
			// make sure the ByteArray position is at the end of the buffer
			bytes.position = start + formatIn.numBytesPerVertex * count * dwordSize;
			
			stream.data = data;
			
			return stream;
		}
	}
}
