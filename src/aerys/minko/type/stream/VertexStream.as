package aerys.minko.type.stream
{
	import aerys.minko.ns.minko_stream;
	import aerys.minko.render.resource.VertexBuffer3DResource;
	import aerys.minko.type.Signal;
	import aerys.minko.type.stream.format.VertexComponent;
	import aerys.minko.type.stream.format.VertexComponentType;
	import aerys.minko.type.stream.format.VertexFormat;
	
	import flash.utils.ByteArray;
	import flash.utils.Dictionary;

	public final class VertexStream implements IVertexStream
	{
		use namespace minko_stream;

		public static const DEFAULT_FORMAT	: VertexFormat	= VertexFormat.XYZ_UV;

		minko_stream var _data			: Vector.<Number>	= null;
		minko_stream var _localDispose	: Boolean			= false;

		private var _usage			: uint						= 0;
		private var _format			: VertexFormat				= null;
		private var _resource		: VertexBuffer3DResource	= null;
		private var _length			: uint						= 0;
		
		private var _invalidMinMax	: Boolean					= true;
		private var _maximum		: Vector.<Number>			= null;
		private var _minimum		: Vector.<Number>			= null;
		
		private var _dataLocked		: Boolean					= false;

		private var _changed		: Signal					= new Signal('VertexStream.changed');
		private var _boundsChanged	: Signal					= new Signal('VertexStream.boundsChanged');
		
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
			_changed.execute(this, null);
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
			_resource = new VertexBuffer3DResource(this);
			_format = format || DEFAULT_FORMAT;
			
			if (data && data.length && data.length % _format.dwordsPerVertex)
				throw new Error("Incompatible vertex format: the data length does not match.");
			
			_data = data ? data.concat() : new Vector.<Number>();
			_usage = usage;
			
			_changed.add(changedHandler);
			changedHandler(this, null);
			
			updateMinMax();			
		}
		
		private function updateMinMax() : void
		{
			if (!_invalidMinMax)
				return ;
				
			_invalidMinMax = false;
			
			var size		: uint	= format.dwordsPerVertex;
			var dataLength	: uint	= data.length;
			var i			: uint	= 0;
			
			_minimum = new Vector.<Number>(size, true);
			_maximum = new Vector.<Number>(size, true);
			
			for (i = 0; i < size; ++i)
			{
				_minimum[i] = Number.MAX_VALUE;
				_maximum[i] = Number.MIN_VALUE;
			}
			
			i = 0;
			while (i < dataLength)
			{
				for (var j : uint = 0; j < size; ++j)
				{
					var value : Number = _data[uint(i + j)];
					
					if (value < _minimum[j])
						_minimum[j] = value;
					else if (value > _maximum[j])
						_maximum[j] = value;
				}
				
				i += size;
			}
			
			_boundsChanged.execute(this);
		}
		
		public function deleteVertexByIndex(index : uint) : Boolean
		{
			checkWriteUsage(this);
			
			if (index > length)
				return false;

			_data.splice(index, _format.dwordsPerVertex);
			_invalidMinMax = true;
			_changed.execute(this, null);

			return true;
		}

		public function getStreamByComponent(vertexComponent : VertexComponent) : VertexStream
		{
			return _format.hasComponent(vertexComponent) ? this : null;
		}

		public function get(i : int) : Number
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

		public function set(i : int, value : Number) : void
		{
			var minMaxChanged	: Boolean	= false;
			
			checkWriteUsage(this);
			
			_data[i] = value;
			
			i %= _format.dwordsPerVertex;
			if (value < _minimum[i])
			{
				_minimum[i] = value;
				minMaxChanged = true;
			}
			else if (value > _maximum[i])
			{
				_maximum[i] = value;
				minMaxChanged = true;
			}
			
			_changed.execute(this, null);
			_boundsChanged.execute(this);
		}
		
		public function lockData(): Vector.<Number> {
			if (_dataLocked) 
				throw new Error("Data lock is already active");
			if (_localDispose)
				throw new Error("Can not lock stream data which is marked for disposal");
			checkWriteUsage(this);
			_dataLocked = true;
			return _data;
		}
		
		public function unlockData(): void {
			if (!_dataLocked)
				throw new Error("Data is not locked");
			_dataLocked = false;
			_invalidMinMax = true;
			_changed.execute(this, null);
		}

		public function push(data : Vector.<Number>) : void
		{
			checkWriteUsage(this);
			
			var dataLength : int = data.length;

			if (dataLength % _format.dwordsPerVertex)
				throw new Error("Invalid data length.");

			for (var i : int = 0; i < dataLength; i++)
				_data.push(data[i]);

			_invalidMinMax = true;
			_changed.execute(this, null);
		}

		public function disposeLocalData(waitForUpload : Boolean = true) : void
		{
			if (_dataLocked)
				throw new Error("Stream data can not be disposed since it's locked for update");
			if (waitForUpload)
				_localDispose = true;
			else
				_data.length = 0;
		}
		
		public function dispose() : void
		{
			resource.dispose();
		}

		private function changedHandler(stream : VertexStream, property : String) : void
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
			if (stream._dataLocked)
				throw new Error(
					"Unable to write in vertex stream: stream data "
					+ "is locked for bulk update"
				);
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
											 functionReader : Dictionary	= null,
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
					
					if (functionReader[componentsOut[componentId]])
						reader = functionReader[componentsOut[componentId]];
					else if (functionReader["defaut"])
						reader = functionReader["defaut"];
					else
						reader = bytes.readFloat;
					
					switch (nativeFormats[componentId])
					{
						case VertexComponentType.FLOAT_4 :
							data[int(dataLength++)] =reader(componentsOut[componentId]);
						case VertexComponentType.FLOAT_3 :
							data[int(dataLength++)] = reader(componentsOut[componentId]);
						case VertexComponentType.FLOAT_2 :
							data[int(dataLength++)] = reader(componentsOut[componentId]);
						case VertexComponentType.FLOAT_1 :
							data[int(dataLength++)] = reader(componentsOut[componentId]);
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
