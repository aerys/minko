package aerys.minko.type.stream
{
	import aerys.minko.ns.minko_stream;
	import aerys.minko.render.resource.VertexBuffer3DResource;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.Signal;
	import aerys.minko.type.stream.format.VertexComponent;
	import aerys.minko.type.stream.format.VertexComponentType;
	import aerys.minko.type.stream.format.VertexFormat;
	
	import flash.utils.ByteArray;
	import flash.utils.Dictionary;

	public final class VertexStream implements IVertexStream
	{
		use namespace minko_stream;

		public static const DEFAULT_FORMAT	: VertexFormat		= VertexFormat.XYZ_UV;

		private static const TMP_NUMBERS	: Vector.<Number>	= new <Number>[];
		
		minko_stream var _data			: Vector.<Number>	= null;
		minko_stream var _localDispose	: Boolean			= false;

		private var _usage				: uint						= 0;
		private var _format				: VertexFormat				= null;
		private var _resource			: VertexBuffer3DResource	= null;
		private var _length				: uint						= 0;
		
		private var _invalidMinMax		: Boolean					= true;
		private var _maximum			: Vector.<Number>			= null;
		private var _minimum			: Vector.<Number>			= null;
		
		private var _locked				: Boolean					= false;

		private var _boundsHaveChanged	: Boolean					= false;
		private var _dataHasChanged		: Boolean					= false;
		
		private var _changed			: Signal					= new Signal('VertexStream.changed');
		private var _boundsChanged		: Signal					= new Signal('VertexStream.boundsChanged');
		
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
		
		public function get length() : uint
		{
			return _length;
		}
		
		protected function get data() : Vector.<Number>
		{
			return _data;
		}

		protected function set data(value : Vector.<Number>) : void
		{
			_data = value;
			_changed.execute(this);
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

		public function VertexStream(usage		: uint,
									 format		: VertexFormat,
									 data 		: Vector.<Number>	= null)
		{
			super();

			initialize(data, format, usage);
		}

		private function initialize(data 		: Vector.<Number>,
									format		: VertexFormat,
									usage		: uint) : void
		{
			_changed.add(changedHandler);
			
			_resource = new VertexBuffer3DResource(this);
			_format = format || DEFAULT_FORMAT;
			
			if (data && data.length && data.length % _format.dwordsPerVertex)
				throw new Error("Incompatible vertex format: the data length does not match.");
			
			_data = data ? data.concat() : new <Number>[];
			_usage = usage;
			
			changedHandler(this);
			
			updateMinMax(true);
		}
		
		private function updateMinMax(forceReset : Boolean = false) : void
		{
			if (!_invalidMinMax)
				return ;
				
			_invalidMinMax = false;
			
			var dataLength	: uint	= data.length;
			var i			: uint	= 0;
			
			if (forceReset)
			{
				var size	: uint	= format.dwordsPerVertex;
				
				_minimum = new Vector.<Number>(size, true);
				_maximum = new Vector.<Number>(size, true);
				
				for (i = 0; i < size; ++i)
				{
					_minimum[i] = Number.MAX_VALUE;
					_maximum[i] = -Number.MAX_VALUE;
				}
			}
			
			_boundsHaveChanged = false;
			for (i = 0; i < dataLength; i += size)
			{
				for (var j : uint = 0; j < size; ++j)
				{
					var value : Number = _data[uint(i + j)];
					
					if (value < _minimum[j])
					{
						_minimum[j] = value;
						_boundsHaveChanged = true;
					}
					if (value > _maximum[j])
					{
						_maximum[j] = value;
						_boundsHaveChanged = true;
					}
				}
			}
			
			if (_boundsHaveChanged)
			{
				_boundsHaveChanged = false;
				_boundsChanged.execute(this);
			}
		}
		
		public function deleteVertexByIndex(index : uint) : Boolean
		{
			checkWriteUsage(this);
			
			if (index > length)
				return false;

			_data.splice(index, _format.dwordsPerVertex);
			
			if (_locked)
			{
				_invalidMinMax = true;
				_dataHasChanged = true;
			}
			else
			{
				_changed.execute(this);
				updateMinMax();
			}

			return true;
		}

		public function getStreamByComponent(vertexComponent : VertexComponent) : VertexStream
		{
			return _format.hasComponent(vertexComponent) ? this : null;
		}

		public function get(i : uint) : Number
		{
			checkReadUsage(this);
			
			return _data[i];
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
		public function lock() : Vector.<Number>
		{
			if (_locked) 
				throw new Error("The stream is already locked.");
			if (_localDispose)
				throw new Error("Cannot lock stream data which is already disposed.");
			
			checkWriteUsage(this);
			
			_locked = true;
			_invalidMinMax = false;
			
			return _data;
		}
		
		/**
		 * Unlock the stream to reactivate signals. The VertexStream.changed signal
		 * is always executed when this method is called. The VertexStream.boundsChanged
		 * signal is called only if the bounds of the stream actually changed.
		 * 
		 */
		public function unlock() : void
		{
			if (!_locked)
				throw new Error("Cannot unlock a stream that is not locked.");
			
			var invalidMinMax : Boolean = _invalidMinMax;
			
			_locked = false;
			_invalidMinMax = true;
			
			if (_dataHasChanged)
			{
				_dataHasChanged = false;
				_changed.execute(this);
			}
			
			if (invalidMinMax)
				updateMinMax();
			if (_boundsHaveChanged)
				_boundsChanged.execute(this);
		}

		/**
		 * The the value at the specified position in the stream.
		 * 
		 * @param offset
		 * @param value
		 * 
		 */
		public function set(offset : uint, value : Number) : void
		{
			checkWriteUsage(this);
			
			_data[offset] = value;
			
			offset %= _format.dwordsPerVertex;
			if (value < _minimum[offset])
			{
				_minimum[offset] = value;
				_boundsHaveChanged = true;
			}
			else if (value > _maximum[offset])
			{
				_maximum[offset] = value;
				_boundsHaveChanged = true;
			}
			
			if (!_locked)
			{
				_changed.execute(this);
				
				if (_boundsHaveChanged)
				{
					_boundsHaveChanged = false;
					_boundsChanged.execute(this);
				}
			}
			else
			{
				_dataHasChanged = true;
			}
		}
		
		/**
		 * Push values at the end of stream. The pushed data must have
		 * a valid length regarding the vertex format of the stream.
		 *  
		 * @param data
		 * 
		 */
		public function push(data : Vector.<Number>) : void
		{
			checkWriteUsage(this);
			
			var numValues 	: int 	= data.length;

			if (numValues % _format.dwordsPerVertex)
				throw new Error("Invalid data length.");

			for (var i : int = 0; i < numValues; i++)
			{
				var value 	: Number	= data[i];
				var ii		: uint		= i % _format.dwordsPerVertex;
				
				if (value < _minimum[ii])
				{
					_minimum[ii] = value;
					_boundsHaveChanged = true;
				}
				else if (value > _maximum[ii])
				{
					_maximum[ii] = value;
					_boundsHaveChanged = true;
				}
				
				_data.push(value);
			}

			if (!_locked)
			{
				_changed.execute(this);
				
				if (_boundsHaveChanged)
				{
					_boundsHaveChanged = false;
					_boundsChanged.execute(this);
				}
			}
			else
			{
				_dataHasChanged = true;
			}
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
									   normalize	: Boolean) : void
		{
			if (component.dwords < 3)
				throw new Error("Not a vector component");
				
			var vertexOffset	: int				= _format.getOffsetForComponent(component);
			var vertexLength	: int				= _format.dwordsPerVertex;
			var vertices		: Vector.<Number>	= lock();
			var numVertices		: int				= vertices.length / vertexLength;
			var tmpLength		: int				= numVertices * 3;
				
			TMP_NUMBERS.length = tmpLength;
					
			for (var i : int = 0, k : int = vertexOffset; i < tmpLength; i += 3, k += vertexLength)
			{
				TMP_NUMBERS[i]		= vertices[k];
				TMP_NUMBERS[i + 1]	= vertices[k + 1];
				TMP_NUMBERS[i + 2]	= vertices[k + 2];
			}
				
			transform.transformRawVectors(TMP_NUMBERS, TMP_NUMBERS);
			
			if (normalize)
				for (i = 0, k = vertexOffset; i < tmpLength; i += 3, k += vertexLength)
				{
					var x	: Number	= TMP_NUMBERS[i];
					var y	: Number 	= TMP_NUMBERS[i + 1];
					var z	: Number 	= TMP_NUMBERS[i + 2];
					var m	: Number 	= 1.0 / Math.sqrt(x * x + y * y + z * z);
					vertices[k]			= x * m;
					vertices[k + 1]		= y * m;
					vertices[k + 2]		= z * m;
				}
			else
				for (i = 0, k = vertexOffset; i < tmpLength; i += 3, k += vertexLength)
				{
					vertices[k]		= TMP_NUMBERS[i];
					vertices[k + 1]	= TMP_NUMBERS[i + 1];
					vertices[k + 2]	= TMP_NUMBERS[i + 2];
				}
				
			unlock();
		}
		
		public function disposeLocalData(waitForUpload : Boolean = true) : void
		{
			if (_locked)
				throw new Error("Stream data can not be disposed since it's locked for update");
			if (waitForUpload)
				_localDispose = true;
			else
				_data = null;
		}
		
		public function dispose() : void
		{
			resource.dispose();
		}

		private function changedHandler(stream : VertexStream) : void
		{
			_length = _data.length / _format.dwordsPerVertex;
		}
		
		public static function fromPositionsAndUVs(positions 	: Vector.<Number>,
												   uvs		 	: Vector.<Number> 	= null,
												   usage		: uint				= 0) : VertexStream
		{
			var numVertices : int 				= positions.length / 3;
			var stride 		: int 				= uvs ? 5 : 3;
			var data 		: Vector.<Number> 	= new Vector.<Number>(numVertices * stride, true);

			for (var i : int = 0; i < numVertices; ++i)
			{
				var offset : int = i * stride;

				data[offset] = positions[int(i * 3)];
				data[int(offset + 1)] = positions[int(i * 3 + 1)];
				data[int(offset + 2)] = positions[int(i * 3 + 2)];

				if (uvs)
				{
					data[int(offset + 3)] = uvs[int(i * 2)];
					data[int(offset + 4)] = uvs[int(i * 2 + 1)];
				}
			}

			return new VertexStream(
				usage,
				uvs ? VertexFormat.XYZ_UV : VertexFormat.XYZ,
				data
			);
		}

		public static function extractSubStream(source			: IVertexStream,
												usage			: uint,
										 		vertexFormat 	: VertexFormat	= null) : VertexStream
		{
			vertexFormat ||= source.format;
			
			var components					: Vector.<VertexComponent>	= vertexFormat.components;
			var numComponents				: uint						= components.length;
			var componentOffsets			: Vector.<uint>				= new Vector.<uint>(numComponents, true);
			var componentSizes				: Vector.<uint>				= new Vector.<uint>(numComponents, true);
			var componentDwordsPerVertex	: Vector.<uint>				= new Vector.<uint>(numComponents, true);
			var componentDatas				: Vector.<Vector.<Number>>	= new Vector.<Vector.<Number>>(numComponents, true);

			var totalVertices				: int						= 0;

			// cache get offsets, sizes, and buffers for each components
			for (var k : int = 0; k < numComponents; ++k)
			{
				var vertexComponent	: VertexComponent	= components[k];
				var subVertexStream	: VertexStream		= source.getStreamByComponent(vertexComponent);
				var subvertexFormat	: VertexFormat		= subVertexStream.format;
				
				checkReadUsage(subVertexStream);
				
				componentOffsets[k]			= subvertexFormat.getOffsetForComponent(vertexComponent);
				componentDwordsPerVertex[k]	= subvertexFormat.dwordsPerVertex;
				componentSizes[k]			= vertexComponent.dwords;
				componentDatas[k]			= subVertexStream._data;
			}
			
			// push vertex data into the new buffer.
			var numVertices			: uint 				= source.length;
			var newVertexStreamData	: Vector.<Number>	= new Vector.<Number>(numVertices * vertexFormat.dwordsPerVertex);
			
			for (var vertexId : uint = 0; vertexId < numVertices; ++vertexId)
			{
				for (var componentId : int = 0; componentId < numComponents; ++componentId)
				{
					var vertexData		: Vector.<Number>	= componentDatas[componentId];
					var componentSize	: uint				= componentSizes[componentId];
					var componentOffset	: uint				= 
						componentOffsets[componentId] + vertexId * componentDwordsPerVertex[componentId];
					var componentLimit	: uint				= componentSize + componentOffset;
					
					for (var n : int = componentOffset; n < componentLimit; ++n, ++totalVertices)
						newVertexStreamData[totalVertices] = vertexData[n];
				}
			}
			
			// avoid copying data vectors
			var newVertexStream	 : VertexStream	= new VertexStream(usage, vertexFormat);
			
			newVertexStream.data = newVertexStreamData;
			
			return newVertexStream;
		}
		
		private static function checkReadUsage(stream : VertexStream) : void
		{
			if (!(stream._usage & StreamUsage.READ))
			{
				throw new Error(
					"Unable to read from vertex stream: stream usage "
					+ "is not set to StreamUsage.READ."
				);
			}
		}
		
		private static function checkWriteUsage(stream : VertexStream) : void
		{
			if (!(stream._usage & StreamUsage.WRITE))
			{
				throw new Error(
					"Unable to write in vertex stream: stream usage "
					+ "is not set to StreamUsage.WRITE."
				);
			}
		}

		public static function concat(streams : Vector.<IVertexStream>, usage : uint) : VertexStream
		{
			var format		: VertexFormat	= streams[0].format;
			var numStreams 	: int			= streams.length;

			for (var i : int = 0; i < numStreams; ++i)
				if (!streams[i].format.equals(format))
					throw new Error("All vertex streams must have the same format.");

			// a bit expensive... but hey, it works :)
			var stream	: VertexStream	= extractSubStream(streams[0], usage, format);

			for  (i = 1; i < numStreams; ++i)
				stream.data = stream.data.concat(extractSubStream(streams[i], usage, format).data);

			return stream;
		}

		public static function fromByteArray(bytes 			: ByteArray,
											 count			: int,
											 formatIn		: VertexFormat,
											 formatOut		: VertexFormat	= null,
											 usage			: uint			= 0,
											 readFunctions 	: Dictionary	= null,
											 dwordSize		: uint			= 4) : VertexStream
		{
			formatOut ||= formatIn;
			
			var dataLength		: int						= 0;
			var data			: Vector.<Number>			= null;
			var stream			: VertexStream				= new VertexStream(usage, formatOut, null);
			var start			: int						= bytes.position;
			var componentsOut	: Vector.<VertexComponent>	= formatOut.components;
			var numComponents	: int						= componentsOut.length;
			var nativeFormats	: Vector.<int>				= new Vector.<int>(numComponents, true);
			
			for (var k : int = 0; k < numComponents; k++)
				nativeFormats[k] = componentsOut[k].nativeFormat;
			
			data = new Vector.<Number>(formatOut.dwordsPerVertex * count, true);
			for (var vertexId : int = 0; vertexId < count; ++vertexId)
			{
				for (var componentId : int = 0; componentId < numComponents; ++componentId)
				{
					bytes.position = start + formatIn.dwordsPerVertex * vertexId * dwordSize
						+ formatIn.getOffsetForComponent(componentsOut[componentId]) * dwordSize;
					
					var reader : Function = null;
					
					if (readFunctions[componentsOut[componentId]])
						reader = readFunctions[componentsOut[componentId]];
					else if (readFunctions["defaut"])
						reader = readFunctions["defaut"];
					else
						reader = bytes.readFloat;
					
					switch (nativeFormats[componentId])
					{
						case VertexComponentType.FLOAT_4 :
							data[int(dataLength++)] = reader(bytes, componentsOut[componentId]);
						case VertexComponentType.FLOAT_3 :
							data[int(dataLength++)] = reader(bytes, componentsOut[componentId]);
						case VertexComponentType.FLOAT_2 :
							data[int(dataLength++)] = reader(bytes, componentsOut[componentId]);
						case VertexComponentType.FLOAT_1 :
							data[int(dataLength++)] = reader(bytes, componentsOut[componentId]);
							break ;
					}
				}
			}
			// make sure the ByteArray position is at the end of the buffer
			bytes.position = start + formatIn.dwordsPerVertex * count * dwordSize;
			
			stream.data = data;
			
			return stream;
		}
	}
}
