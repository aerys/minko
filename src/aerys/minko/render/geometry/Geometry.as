package aerys.minko.render.geometry
{
	import flash.geom.Point;
	import flash.utils.ByteArray;
	
	import aerys.minko.ns.minko_scene;
	import aerys.minko.ns.minko_stream;
	import aerys.minko.render.geometry.stream.IVertexStream;
	import aerys.minko.render.geometry.stream.IndexStream;
	import aerys.minko.render.geometry.stream.StreamUsage;
	import aerys.minko.render.geometry.stream.VertexStream;
	import aerys.minko.render.geometry.stream.VertexStreamList;
	import aerys.minko.render.geometry.stream.format.VertexComponent;
	import aerys.minko.render.geometry.stream.format.VertexFormat;
	import aerys.minko.type.Signal;
	import aerys.minko.type.binding.IWatchable;
	import aerys.minko.type.math.BoundingBox;
	import aerys.minko.type.math.BoundingSphere;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Ray;
	import aerys.minko.type.math.Vector4;

	/**
	 * Geometry objects store and provide a secure access to the geometry of 3D objects
	 * as vertex/index streams and can be used many times to create multiple meshes working
	 * on the same geometry data.
	 * 
	 * <p>
	 * Geometry objects usually store one IVertexStream (a VertexStream or a VertexStreamList
	 * object) providing all the per-vertex data (position, uv, normal...) and an IndexStream
	 * providing the indices of the vertices to draw the triangles of the 3D geometry.
	 * </p>
	 * 
	 * <p>
	 * Geometry objects can store more than IVertexStream objects when they use morphing
	 * animation: each IVertexStream object is then considered as a "frame" containing all
	 * the vertex data (position, uv, normal...) required for that frame. Each frame must
	 * provide the same data. Thus, each IVertexStream object must have the same vertex
	 * format and the same length. An error will be thrown otherwise upon construction.
	 * </p>
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class Geometry implements IWatchable
	{
		use namespace minko_scene;
		use namespace minko_stream;
		
		private static const TMP_MATRIX	: Matrix4x4	= new Matrix4x4();
		
		minko_scene var _vertexStreams		: Vector.<IVertexStream>	= null;

		private var _indexStream	: IndexStream		= null;
		private var _firstIndex		: uint				= 0;
		private var _numTriangles	: int				= -1;

		private var _boundingSphere	: BoundingSphere	= null;
		private var _boundingBox	: BoundingBox		= null;
		
		private var _bulkUpdate		: Boolean			= false;
		
		private var _changed		: Signal;
		
		private var _name			: String;
		
		private var _contextLost	: Signal			= new Signal("Geometry.contextLost");

		public function get name() : String
		{
			return _name;
		}

		public function set name(value : String) : void
		{
			_name = value;
		}

		/**
		 * The number of IVertexStreams objects stored in the geometry. 
		 * @return 
		 * 
		 */
		public function get numVertexStreams() : uint
		{
			return _vertexStreams.length;
		}
		
		/**
		 * The indices - as an IndexStream object - containing the list and
		 * order of the vertices defining the 3D geometry as a series of triangles.
		 * 
		 * @return 
		 * 
		 */
		public function get indexStream() : IndexStream
		{
			return _indexStream;
		}
		public function set indexStream(value : IndexStream) : void
		{
			if (_indexStream != null)
				_indexStream.changed.remove(indexStreamChangedHandler);
			
			_indexStream = value;
			
			if (_indexStream != null)
				_indexStream.changed.add(indexStreamChangedHandler);
			
			_changed.execute(this);
		}
		
		/**
		 * First index in an IndexStream used by the geometry.
		 */
		public function get firstIndex(): uint
		{
			return _firstIndex;
		}
		
		/**
		 * Number of triangles in the geometry. Together with firstIndex specifies a
		 * subset of IndexStream used by the geometry.
		 */
		public function get numTriangles(): int
		{
			return _numTriangles;
		}
		
		/**
		 * The bounding sphere of the 3D geometry. 
		 * @return 
		 * 
		 */
		public function get boundingSphere() : BoundingSphere
		{
			return _boundingSphere;
		}
		
		/**
		 * The bounding box of the 3D geometry. 
		 * @return 
		 * 
		 */
		public function get boundingBox() : BoundingBox
		{
			return _boundingBox;
		}
		
		/**
		 * The format every vertex. 
		 * @return 
		 * 
		 */
		public function get format() : VertexFormat
		{
			return _vertexStreams.length ? _vertexStreams[0].format : null;
		}
		
		public function get changed() : Signal
		{
			return _changed;
		}
		
		public function get contextLost():Signal
		{
			return _contextLost;
		}
		
		/**
		 * Create a new Geometry object.
		 * 
		 * @param vertexStreams A vector of IVertexStream object where each stream is a keyframe used
		 * for hardware vertex-morphing animations. If the object does not use vertex-morphing, you should
		 * provide a vector with only one element.
		 * 
		 * @param indexStream The IndexStream objects containing the triangles to render.
		 * 
		 * @param firstIndex The index of the first vertex index in the IndexStream. Default is 0.
		 * 
		 * @param numTriangles The number of triangles to render, starting from 'firstIndex'. Use -1 to
		 * draw all the triangles. Default is -1.
		 * 
		 */
		public function Geometry(vertexStreams	: Vector.<IVertexStream>	= null,
								 indexStream	: IndexStream				= null,
								 firstIndex		: uint						= 0,
								 numTriangles	: int						= -1,
								 name			: String					= null)
		{			
			initialize(vertexStreams, indexStream, firstIndex, numTriangles, name);
		}
		
		private function initialize(vertexStreams	: Vector.<IVertexStream>	= null,
									indexStream		: IndexStream				= null,
									firstIndex		: uint						= 0,
									numTriangles	: int						= -1,
									name			: String					= null) : void
		{
			_changed = new Signal('Geometry.changed');
			
			_name			= name;
			_firstIndex 	= firstIndex;
			_numTriangles	= numTriangles;
			
			var numVertexStreams	: int	= vertexStreams ? vertexStreams.length : 0;
			
			_vertexStreams = new Vector.<IVertexStream>();
			for (var i : int = 0; i < numVertexStreams; ++i)
			{
				var vstream : IVertexStream	= vertexStreams[i];
				
				if (!vstream.format.equals(vertexStreams[0].format))
					throw new Error('All vertex streams must have the same vertex format.');
				
				setVertexStream(vstream, i);
			}
			
			_indexStream = indexStream;
			
			if (!_indexStream && _vertexStreams && _vertexStreams.length)
			{
				_indexStream = new IndexStream(
					StreamUsage.DYNAMIC,
					null,
					_vertexStreams[0].numVertices
				);
			}
			
			updateBoundingVolumes();
		}
		
		public function dispose() : void
		{
			var numStreams : uint = numVertexStreams;
			
			for (var streamId : uint = 0; streamId < numStreams; ++streamId)
				_vertexStreams[streamId].dispose();
			_vertexStreams.length = 0;
			
			_indexStream.dispose();
			_indexStream = null;
		}
		
		public function disposeLocalData(waitForUpload : Boolean = true) : Geometry
		{
			var numStreams : uint = numVertexStreams;
			
			for (var streamId : uint = 0; streamId < numStreams; ++streamId)
				_vertexStreams[streamId].disposeLocalData(waitForUpload);
			
			_indexStream.disposeLocalData(waitForUpload);
			
			return this;
		}
		
		/**
		 * Get the IVertexStream object at the specified index.
		 *  
		 * @param index
		 * @return 
		 * 
		 */
		public function getVertexStream(index : uint = 0) : IVertexStream
		{
			return _vertexStreams[index];
		}
		
		public function setVertexStream(vertexStream : IVertexStream, index : uint = 0) : void
		{
			doSetVertexStream(vertexStream, index);
		}
        
        protected function doSetVertexStream(vertexStream   : IVertexStream,
                                             index          : uint  = 0,
                                             executeChanged : Boolean = true) : void
        {
            if (index < _vertexStreams.length)
            {
                var oldStream : IVertexStream = _vertexStreams[index] as IVertexStream;
                
                if (oldStream.changed.hasCallback(vertexStreamChangedHandler))
                    oldStream.changed.remove(vertexStreamChangedHandler);
                
                if (oldStream.boundsChanged.hasCallback(vertexStreamBoundsChangedHandler))
                    oldStream.boundsChanged.remove(vertexStreamBoundsChangedHandler);
            }
            
            if (vertexStream.format.hasComponent(VertexComponent.XYZ))
                vertexStream.boundsChanged.add(vertexStreamBoundsChangedHandler);
            vertexStream.changed.add(vertexStreamChangedHandler);
            _vertexStreams[index] = vertexStream;
            
            if (executeChanged)
                _changed.execute(this);
        }
		
		/**
		 * Apply matrix transformation to all vertices
		 * 
		 * <p>This method will transform:</p>
		 * <ul>
		 * <li>vertex 3D positions (VertexComponent.XYZ)</li>
		 * <li>vertex normals, if any (VertexComponent.NORMAL)</li>
		 * <li>vertex tangents, if any (VertexComponent.TANGENT)</li>
		 * </ul>
		 * 
		 * <p>Vertex normals and tangents will be transformed without translation</p>
		 */
		public function applyTransform(transform 		: Matrix4x4,
									   updatePositions	: Boolean	= true,
									   updateNormals 	: Boolean	= true, 
									   updateTangents 	: Boolean	= true) : Geometry
		{
			_bulkUpdate = true;
			
			var numStreams	: uint 		= _vertexStreams.length;
			var updateBV	: Boolean	= false;
			var tmpMatrix	: Matrix4x4 = null;
			
			for (var streamId : uint = 0; streamId < numStreams; streamId++)
			{
				var stream : IVertexStream = _vertexStreams[streamId];
				
				if (updatePositions)
				{
					var xyzStream : VertexStream = stream.getStreamByComponent(VertexComponent.XYZ);
					
					if (xyzStream)
					{
						xyzStream.applyTransform(VertexComponent.XYZ, transform, false);
						updateBV = true;
					}
				}
				
				var normalStream  : VertexStream = null;
				var tangentStream : VertexStream = null;
				
				if (updateNormals)
					normalStream 	= stream.getStreamByComponent(VertexComponent.NORMAL);
				if (updateTangents)
					tangentStream 	= stream.getStreamByComponent(VertexComponent.TANGENT);
					
				if (!tmpMatrix && (normalStream || tangentStream))
				{
					tmpMatrix = TMP_MATRIX;
					tmpMatrix.copyFrom(transform);
					tmpMatrix.setTranslation(0, 0, 0);
				}
				
				if (normalStream)
					normalStream.applyTransform(VertexComponent.NORMAL, tmpMatrix, true);
				if (tangentStream)
					tangentStream.applyTransform(VertexComponent.TANGENT, tmpMatrix, true);
			}

			_bulkUpdate = false;
			
			if (updateBV)
				updateBoundingVolumes();
			
			return this;
		}
		
		/**
		 * Compute all the normals, store them in a new separate vertex stream and add this stream
		 * to the others to make it available in the vertex shader.
		 * 
		 * If the streams already have the VertexComponent.NORMAL is their vertex format, an error
		 * will be thrown because the same vertex component cannot be present twice.
		 * To override this security and replace the existing normals vertex stream, you have to set
		 * the "replace" argument to "true".
		 * 
		 * @param streamUsage The StreamUsage for the new vertex stream that stores the normals data.
		 * If no value is provided, the StreamUsage of the vertex stream holding the VertexComponent.XYZ
		 * will be used.
		 * 
		 * @param replace Whether the existing normals data should be replaced if it exists.
		 * 
		 */
		public function computeNormals(streamUsage : int = -1, triangles : Vector.<uint> = null) : Geometry
		{
			var numStreams		: uint			= this.numVertexStreams;
			var indices			: ByteArray		= indexStream.lock();
			var numTriangles	: int			= indexStream.length / 3;
			var stream			: IVertexStream	= null;
			
			if (numStreams != 0)
			{
				stream = getVertexStream();
				
				if (!stream.format.hasComponent(VertexComponent.XYZ))
					throw new Error('Unable to compute normals without VertexComponent.XYZ');
				
				if (streamUsage < 0)
					streamUsage = stream.getStreamByComponent(VertexComponent.XYZ).usage;
				streamUsage |= StreamUsage.WRITE;
					
				for (var streamId : uint = 0; streamId < numStreams; ++streamId)
				{
					stream = getVertexStream(streamId);
					
					var numVertices		: uint				= stream.numVertices;
					var xyzStream		: VertexStream		= stream.getStreamByComponent(VertexComponent.XYZ);
					var normalsStream 	: VertexStream		= stream.getStreamByComponent(VertexComponent.NORMAL);
					var hasNormals		: Boolean			= normalsStream != null;
					
					if (!hasNormals)
						normalsStream = new VertexStream(streamUsage, new VertexFormat(VertexComponent.NORMAL));
					
					fillNormalsData(indices, normalsStream, xyzStream, triangles);
					
					if (!hasNormals)
						pushVertexStreamInList(streamId, normalsStream);
				}
			}
			
			return this;
		}
		
		/**
		 * Compute the normals and tangents vertex streams required for tangent space computations
		 * (such as normal mapping) and store them in a new single vertex stream.
		 * 
		 * <p>This method will compute:</p>
		 * <ul>
		 * <li>normals data based on the 3D positions of the vertices (VertexComponent.XYZ)</li>
		 * <li>tangents data based on the UV texture coordinates of the vertices (VertexComponent.UV)</li>
		 * </ul>
		 * 
		 * <p>The bi-tangent vector can then be computed in the vertex shader with a cross-product:</p>
		 * 
		 * <code>
		 * var biTangent : SFloat = crossProduct(vertexNormal, vertexTangent);
		 * </code>
		 * 
		 * @param streamUsage The StreamUsage for the new computed vertex stream holding normals and
		 * tangents data.
		 * @param replace Whether to replace existing normals or tangents data.
		 * 
		 */
		public function computeTangentSpace(computeNormals 	: Boolean 		= true,
											streamUsage 	: int 			= -1,
											triangles 		: Vector.<uint>	= null) : Geometry
		{
			var numStreams		: uint			= this.numVertexStreams;
			var indices			: ByteArray		= indexStream.lock();
			var stream			: IVertexStream	= null;
			
			if (numStreams != 0)
			{
				stream = getVertexStream();
				
				var format : VertexFormat = stream.format;
				
				if (!format.hasComponent(VertexComponent.XYZ))
					throw new Error('Unable to compute normals without VertexComponent.XYZ');
				if (!stream.format.hasComponent(VertexComponent.UV))
					throw new Error('Unable to compute tangents without VertexComponent.UV');
				
				if (streamUsage < 0)
					streamUsage = stream.getStreamByComponent(VertexComponent.XYZ).usage;
				streamUsage |= StreamUsage.WRITE;
				
				for (var streamId : uint = 0; streamId < numStreams; ++streamId)
				{
					stream = getVertexStream(streamId);
					
					var xyzStream		: VertexStream		= stream.getStreamByComponent(VertexComponent.XYZ);
					var uvStream		: VertexStream		= stream.getStreamByComponent(VertexComponent.UV);
					var normalsStream 	: VertexStream		= stream.getStreamByComponent(VertexComponent.NORMAL);
					var tangentsStream	: VertexStream		= stream.getStreamByComponent(VertexComponent.TANGENT);
					var hasNormals		: Boolean			= normalsStream != null;
					var hasTangents		: Boolean			= tangentsStream != null;
					
					if (!hasNormals && computeNormals && !hasTangents)
					{
						normalsStream = tangentsStream = new VertexStream(
							streamUsage,
							new VertexFormat(VertexComponent.NORMAL, VertexComponent.TANGENT)
						);
					}
					else if (!hasNormals && computeNormals)
					{
						normalsStream = new VertexStream(
							streamUsage,
							new VertexFormat(VertexComponent.NORMAL)
						);
					}
					else if (!hasTangents)
					{
						tangentsStream = new VertexStream(
							streamUsage,
							new VertexFormat(VertexComponent.TANGENT)
						);
					}
					
					fillTangentsData(indices, tangentsStream, xyzStream, uvStream, triangles);
					if (computeNormals)
						fillNormalsData(indices, normalsStream, xyzStream, triangles);
					
					if (!hasNormals && normalsStream != null)
						pushVertexStreamInList(streamId, normalsStream);
					if (!hasTangents && tangentsStream != normalsStream)
						pushVertexStreamInList(streamId, tangentsStream);
				}
			}
			
			return this;
		}
		
		private function fillNormalsData(indices		: ByteArray,
										 normalsStream	: VertexStream,
										 xyzStream		: VertexStream,
										 triangles		: Vector.<uint>) : void
		{
			var numTriangles		: uint				= triangles ? triangles.length : indices.length / 6;
			var nx					: Number 			= 0.;
			var ny					: Number		 	= 0.;
			var nz					: Number 			= 0.;
			var mag					: Number			= 0.;
			var xyzOffset			: uint				= xyzStream.format.getBytesOffsetForComponent(VertexComponent.XYZ);
			var xyzVertexSize		: uint				= xyzStream.format.numBytesPerVertex;
			var xyzData				: ByteArray			= xyzStream.lock();
			var normalsOffset		: uint				= normalsStream.format.getBytesOffsetForComponent(VertexComponent.NORMAL);
			var normalsVertexSize	: uint				= normalsStream.format.numBytesPerVertex;
			var normalsData			: ByteArray			= null;
			var numVertices			: uint				= xyzStream.numVertices;
			var i 					: uint 				= 0;
			var ii					: uint				= 0;
			var i0					: uint				= 0;
			var i1					: uint				= 0;
			var i2					: uint				= 0;
			
			normalsData = normalsStream == xyzStream ? xyzData : normalsStream.lock();
			
			for (i = 0; i < numTriangles; ++i)
			{
				ii = triangles ? triangles[i] * 3 : i * 3;
				i0 = indices.readUnsignedShort();
				i1 = indices.readUnsignedShort();
				i2 = indices.readUnsignedShort();
				
				normalsData.position = i0 * normalsVertexSize + normalsOffset;
				normalsData.writeFloat(0.);
				normalsData.writeFloat(0.);
				normalsData.writeFloat(0.);
				
				normalsData.position = i1 * normalsVertexSize + normalsOffset;
				normalsData.writeFloat(0.);
				normalsData.writeFloat(0.);
				normalsData.writeFloat(0.);
				
				normalsData.position = i2 * normalsVertexSize + normalsOffset;
				normalsData.writeFloat(0.);
				normalsData.writeFloat(0.);
				normalsData.writeFloat(0.);
			}
			
			for (i = 0; i < numTriangles; ++i)
			{
				ii = triangles ? triangles[i] * 3 : i * 3;
				
				indices.position = ii << 1;
				i0 = indices.readUnsignedShort();
				i1 = indices.readUnsignedShort();
				i2 = indices.readUnsignedShort();
				
				xyzData.position = xyzOffset + xyzVertexSize * i0;
				var x0	: Number	= xyzData.readFloat();
				var y0	: Number 	= xyzData.readFloat();
				var z0	: Number 	= xyzData.readFloat();
				
				xyzData.position = xyzOffset + xyzVertexSize * i1;
				var x1	: Number 	= xyzData.readFloat();
				var y1	: Number 	= xyzData.readFloat();
				var z1	: Number 	= xyzData.readFloat();
				
				xyzData.position = xyzOffset + xyzVertexSize * i2;
				var x2	: Number 	= xyzData.readFloat();
				var y2	: Number 	= xyzData.readFloat();
				var z2	: Number 	= xyzData.readFloat();
				
				nx = (y0 - y2) * (z0 - z1) - (z0 - z2) * (y0 - y1);
				ny = (z0 - z2) * (x0 - x1) - (x0 - x2) * (z0 - z1);
				nz = (x0 - x2) * (y0 - y1) - (y0 - y2) * (x0 - x1);
				
//				mag = Math.sqrt(nx * nx + ny * ny + nz * nz);
//				
//				if (mag != 0.)
//				{
//					nx /= mag;
//					ny /= mag;
//					nz /= mag;
//				}
				
				ii = i0 * normalsVertexSize + normalsOffset;
				normalsData.position = ii;
				var nx0 : Number = nx + normalsData.readFloat();
				var ny0 : Number = ny + normalsData.readFloat();
				var nz0 : Number = nz + normalsData.readFloat();
				
				normalsData.position = ii;
				normalsData.writeFloat(nx0);
				normalsData.writeFloat(ny0);
				normalsData.writeFloat(nz0);
				
				ii = i1 * normalsVertexSize + normalsOffset;
				normalsData.position = ii;
				var nx1 : Number = nx + normalsData.readFloat();
				var ny1 : Number = ny + normalsData.readFloat();
				var nz1 : Number = nz + normalsData.readFloat();
				
				normalsData.position = ii;
				normalsData.writeFloat(nx1);
				normalsData.writeFloat(ny1);
				normalsData.writeFloat(nz1);
			
				ii = i2 * normalsVertexSize + normalsOffset;
				normalsData.position = ii;
				var nx2 : Number = nx + normalsData.readFloat();
				var ny2 : Number = ny + normalsData.readFloat();
				var nz2 : Number = nz + normalsData.readFloat();
				
				normalsData.position = ii;
				normalsData.writeFloat(nx2);
				normalsData.writeFloat(ny2);
				normalsData.writeFloat(nz2);
			}
			
			for (i = 0; i < numVertices; ++i)
			{
				ii = i * normalsVertexSize + normalsOffset;
				normalsData.position = ii;
				nx = normalsData.readFloat();
				ny = normalsData.readFloat();
				nz = normalsData.readFloat();
				
				mag = Math.sqrt(nx * nx + ny * ny + nz * nz);
				
				if (mag != 0.)
				{
					normalsData.position = ii;
					normalsData.writeFloat(nx / mag);
					normalsData.writeFloat(ny / mag);
					normalsData.writeFloat(nz / mag);
				}
			}
			
			normalsData.position = 0;
			indices.position = 0;
			normalsStream.unlock();
			if (xyzStream.locked)
				xyzStream.unlock(false);
		}
		
		private function fillTangentsData(indices			: ByteArray,
										  tangentsStream	: VertexStream,
										  xyzStream			: VertexStream,
										  uvStream			: VertexStream,
										  triangles			: Vector.<uint>) : void
		{
			var numTriangles		: uint		= triangles ? triangles.length : indices.length / 6;
			var xyzOffset			: uint		= xyzStream.format.getBytesOffsetForComponent(VertexComponent.XYZ);
			var xyzVertexSize		: uint		= xyzStream.format.numBytesPerVertex;
			var xyzData				: ByteArray	= xyzStream.lock();
			var uvOffset			: uint		= uvStream.format.getBytesOffsetForComponent(VertexComponent.UV);
			var uvVertexSize		: uint		= uvStream.format.numBytesPerVertex;
			var uvData				: ByteArray	= null;
			var tangentsOffset		: uint		= tangentsStream.format.getBytesOffsetForComponent(VertexComponent.TANGENT);
			var tangentsVertexSize	: uint		= tangentsStream.format.numBytesPerVertex;
			var tangentsData		: ByteArray	= null;
			var numVertices			: uint		= xyzStream.numVertices;
			var i					: uint		= 0;
			var ii 					: uint		= 0;
			var i0 					: uint		= 0;
			var i1 					: uint		= 0;
			var i2 					: uint		= 0;
			
			uvData = uvStream == xyzStream ? xyzData : uvStream.lock();
			tangentsData = tangentsStream == xyzStream ? xyzData : tangentsStream.lock();
			
			for (i = 0; i < numTriangles; ++i)
			{
				ii = triangles ? triangles[i] * 3 : i * 3;
				indices.position = ii << 1;
				i0 = indices.readUnsignedShort();
				i1 = indices.readUnsignedShort();
				i2 = indices.readUnsignedShort();
				
				tangentsData.position = i0 * tangentsVertexSize + tangentsOffset;
				tangentsData.writeFloat(0.);
				tangentsData.writeFloat(0.);
				tangentsData.writeFloat(0.);
				
				tangentsData.position = i1 * tangentsVertexSize + tangentsOffset;
				tangentsData.writeFloat(0.);
				tangentsData.writeFloat(0.);
				tangentsData.writeFloat(0.);
				
				tangentsData.position = i2 * tangentsVertexSize + tangentsOffset;
				tangentsData.writeFloat(0.);
				tangentsData.writeFloat(0.);
				tangentsData.writeFloat(0.);
			}
			
			for (i = 0; i < numTriangles; ++i)
			{
				ii = triangles ? triangles[i] * 3 : i * 3;
			
				indices.position = ii << 1;
				i0 = indices.readUnsignedShort();
				i1 = indices.readUnsignedShort();
				i2 = indices.readUnsignedShort();
				
				xyzData.position = xyzOffset + xyzVertexSize * i0;
				var x0		: Number 	= xyzData.readFloat();
				var y0		: Number 	= xyzData.readFloat();
				var z0		: Number 	= xyzData.readFloat();
				
				uvData.position = uvOffset + uvVertexSize * i0;
				var u0		: Number	= uvData.readFloat();
				var v0		: Number	= uvData.readFloat();
				
				xyzData.position = xyzOffset + xyzVertexSize * i1;
				var x1		: Number 	= xyzData.readFloat();
				var y1		: Number 	= xyzData.readFloat();
				var z1		: Number 	= xyzData.readFloat();
				
				uvData.position = uvOffset + uvVertexSize * i1;
				var u1		: Number	= uvData.readFloat();
				var v1		: Number	= uvData.readFloat();
				
				xyzData.position = xyzOffset + xyzVertexSize * i2;
				var x2		: Number 	= xyzData.readFloat();
				var y2		: Number 	= xyzData.readFloat();
				var z2		: Number 	= xyzData.readFloat();
				
				uvData.position = uvOffset + uvVertexSize * i2;
				var u2		: Number	= uvData.readFloat();
				var v2		: Number	= uvData.readFloat();
				
				var v0v2	: Number 	= v0 - v2;
				var v1v2 	: Number 	= v1 - v2;
				var coef 	: Number 	= (u0 - u2) * v1v2 - (u1 - u2) * v0v2;
				
				if (coef == 0.)
					coef = 1.;
				else
					coef = 1. / coef;
				
				var tx 	: Number 	= coef * (v1v2 * (x0 - x2) - v0v2 * (x1 - x2));
				var ty 	: Number 	= coef * (v1v2 * (y0 - y2) - v0v2 * (y1 - y2));
				var tz 	: Number 	= coef * (v1v2 * (z0 - z2) - v0v2 * (z1 - z2));
				
				ii = i0 * tangentsVertexSize + tangentsOffset;
				tangentsData.position = ii;
				var tx0 : Number = tx + tangentsData.readFloat();
				var ty0 : Number = ty + tangentsData.readFloat();
				var tz0 : Number = tz + tangentsData.readFloat();
				
				tangentsData.position = ii;
				tangentsData.writeFloat(tx0);
				tangentsData.writeFloat(ty0);
				tangentsData.writeFloat(tz0);
				
				ii = i1 * tangentsVertexSize + tangentsOffset;
				tangentsData.position = ii;
				var tx1 : Number = tx + tangentsData.readFloat();
				var ty1 : Number = ty + tangentsData.readFloat();
				var tz1 : Number = tz + tangentsData.readFloat();
				
				tangentsData.position = ii;
				tangentsData.writeFloat(tx1);
				tangentsData.writeFloat(ty1);
				tangentsData.writeFloat(tz1);
				
				ii = i2 * tangentsVertexSize + tangentsOffset;
				tangentsData.position = ii;
				var tx2 : Number = tx + tangentsData.readFloat();
				var ty2 : Number = ty + tangentsData.readFloat();
				var tz2 : Number = tz + tangentsData.readFloat();
				
				tangentsData.position = ii;
				tangentsData.writeFloat(tx2);
				tangentsData.writeFloat(ty2);
				tangentsData.writeFloat(tz2);
			}
			
			for (i = 0; i < numVertices; ++i)
			{
				ii = i * tangentsVertexSize + tangentsOffset;
				tangentsData.position = ii;
				tx = tangentsData.readFloat();
				ty = tangentsData.readFloat();
				tz = tangentsData.readFloat();
				
				var mag : Number = Math.sqrt(tx * tx + ty * ty + tz * tz);
				
				if (mag != 0.)
				{
					tx /= mag;
					ty /= mag;
					tz /= mag;
				}
				
				tangentsData.position = ii;
				tangentsData.writeFloat(tx);
				tangentsData.writeFloat(ty);
				tangentsData.writeFloat(tz);
			}
			
			indices.position = 0;
			xyzStream.unlock(tangentsStream == xyzStream);
			if (uvStream.locked)
				uvStream.unlock(false);
			if (tangentsStream.locked)
				tangentsStream.unlock();
		}
		
		private function pushVertexStreamInList(index 			: uint,
												vertexStream 	: VertexStream,
												force 			: Boolean	= false) : void
		{
			var stream	: IVertexStream	= _vertexStreams[index];
			
			if (!(stream is VertexStreamList))
			{
				stream = new VertexStreamList(stream);
                doSetVertexStream(stream, index, false);
			}
			
			(stream as VertexStreamList).pushVertexStream(vertexStream, force);
		}
		
		protected function updateBoundingVolumes() : void
		{
			if (_bulkUpdate)
				return ;

			var numStreams	: uint		= _vertexStreams.length;
			var minX		: Number	= Number.MAX_VALUE;
			var minY		: Number	= Number.MAX_VALUE;
			var minZ		: Number	= Number.MAX_VALUE;
			var maxX		: Number	= -Number.MAX_VALUE;
			var maxY		: Number	= -Number.MAX_VALUE;
			var maxZ		: Number	= -Number.MAX_VALUE;
			var streamMinX	: Number	= 0;
			var streamMinY	: Number	= 0;
			var streamMinZ	: Number	= 0;
			var streamMaxX	: Number	= 0;
			var streamMaxY	: Number	= 0;
			var streamMaxZ	: Number	= 0;

			for (var i : int = 0; i < numStreams; ++i)
			{
				var xyzStream	: VertexStream	= _vertexStreams[i].getStreamByComponent(
					VertexComponent.XYZ
				);
				
				if (!xyzStream)
					return ;
				
				var offset		: uint		= xyzStream.format.getOffsetForComponent(
					VertexComponent.XYZ
				);
				
				if (firstIndex != 0 || numTriangles != -1)
				{
					var stride	: uint				= xyzStream.format.numBytesPerVertex >>> 2;
					var minimum	: Vector.<Number>	= new Vector.<Number>(stride - offset, true);
					var maximum	: Vector.<Number>	= new Vector.<Number>(stride - offset, true);
					
					xyzStream.getMinMaxBetween(firstIndex, numTriangles * 3, offset, _indexStream, minimum, maximum);
					streamMinX = minimum[0];
					streamMinY = minimum[1];
					streamMinZ = minimum[2];
					streamMaxX = maximum[0];
					streamMaxY = maximum[1];
					streamMaxZ = maximum[2];
				}
				else
				{
					streamMinX = xyzStream.getMinimum(offset);
					streamMinY = xyzStream.getMinimum(offset + 1);
					streamMinZ = xyzStream.getMinimum(offset + 2);
					streamMaxX = xyzStream.getMaximum(offset);
					streamMaxY = xyzStream.getMaximum(offset + 1);
					streamMaxZ = xyzStream.getMaximum(offset + 2);
				}
				
				if (streamMinX < minX)
					minX = streamMinX;
				if (streamMaxX > maxX)
					maxX = streamMaxX;
				
				if (streamMinY < minY)
					minY = streamMinY;
				if (streamMaxY > maxY)
					maxY = streamMaxY;
				
				if (streamMinZ < minZ)
					minZ = streamMinZ;
				if (streamMaxZ > maxZ)
					maxZ = streamMaxZ;
			}
			
			var min : Vector4	= new Vector4(minX, minY, minZ);
			var max : Vector4	= new Vector4(maxX, maxY, maxZ);
			
			_boundingSphere = BoundingSphere.fromMinMax(min, max);
			_boundingBox = new BoundingBox(min, max);
		}
		
		/**
		 * Merge another Geometry object. The Geometry object calling the merge() method is
		 * modified in place.
		 *  
		 * @param geometry The Geometry to merge.
		 * @param vertexStreamUsage The StreamUsage to use for the new VertexStream objects.
		 * @param indexStreamUsage The StreamUsage to use for the new IndexStream objects.
		 * 
		 * @return 
		 * 
		 */
		public function merge(geometry			: Geometry,
							  vertexStreamUsage : uint = 3,
							  indexStreamUsage 	: uint = 3) : Geometry
		{
			var vertexStreamsToConcat 	: Vector.<IVertexStream> = new <IVertexStream>[];
			
			if (numVertexStreams != 0 && geometry.numVertexStreams != 0
				&& numVertexStreams != geometry.numVertexStreams)
			{
				throw new Error('Both geometries must have the same number of vertex streams.');
			}

			if (numVertexStreams != 0)
				vertexStreamsToConcat.push(_vertexStreams[0]);
			
			if (geometry.numVertexStreams != 0)
				vertexStreamsToConcat.push(geometry._vertexStreams[0]);

			var indexOffset : uint = vertexStreamsToConcat.length != 1
				? (vertexStreamsToConcat[0] as IVertexStream).numVertices
				: 0;
			
			if (vertexStreamsToConcat.length != 0)
			{
				_vertexStreams = new <IVertexStream>[
					VertexStream.concat(vertexStreamsToConcat, vertexStreamUsage)
				];
			}
			
			_indexStream = _indexStream != null
				? _indexStream.clone(indexStreamUsage)
				: new IndexStream(indexStreamUsage);
			
			if (geometry._indexStream != null)
				_indexStream = _indexStream.concat(geometry._indexStream, 0, 0, indexOffset);
			
			updateBoundingVolumes();
			
			_changed.execute(this);

			return this;				
		}
		
		/**
		 * Flip (multiply by -1) the normals of the geometry.
		 *  
		 * @param flipTangents Whether to flip the tangents too, default value is true.
		 * @return 
		 * 
		 */
		public function flipNormals(flipTangents : Boolean = true) : Geometry
		{
			var numStreams 	: uint 		= this.numVertexStreams;
			var m 			: Matrix4x4 = new Matrix4x4();
			
			m.appendScale(-1, -1, -1);
			
			for (var streamId : uint = 0; streamId < numStreams; ++streamId)
			{
				var vertexStream	: IVertexStream		= getVertexStream(streamId);
				var normalsStream 	: VertexStream 		= vertexStream.getStreamByComponent(VertexComponent.NORMAL);
				var tangentsStream 	: VertexStream 		= vertexStream.getStreamByComponent(VertexComponent.TANGENT);
				
				if (normalsStream != null)
					normalsStream.applyTransform(VertexComponent.NORMAL, m, true);
				
				if (tangentsStream != null && flipTangents)
					tangentsStream.applyTransform(VertexComponent.TANGENT, m, true);
			}
			
			return this;
		}
		
		private function vertexStreamChangedHandler(stream : IVertexStream) : void
		{
			_changed.execute(this);
		}
		
		private function vertexStreamBoundsChangedHandler(stream : IVertexStream) : void
		{
			updateBoundingVolumes();
		}
		
		private function indexStreamChangedHandler(stream : IndexStream) : void
		{
			_changed.execute(this);
		}
		
		/**
		 * Cast a ray on the 3D geometry to retrieve the ID of the hit triangle.
		 * 
		 * @param ray The ray to cast on the geometry.
		 * @param transform An optional 3D transform to transform the ray into the geometry local space.
		 * @param hitXYZ An optional reference to a Vector4 object to fill with the (x, y, z) coordinates
		 * of the actual ray/geometry hit point in local space.
		 * @param hitUV An optional reference to a Point object to fill with the (u, v) texture coordinates
		 * of the actual ray/geometry hit point.
		 * @param hitNormal An optional reference to a Vector4 object to fill the normal of the intersection
		 * point if any.
		 * 
		 * @return The triangle ID (the ID of the first index of the triangle in the IndexStream) that was
		 * hit by the specified ray, -1 otherwise.
		 * 
		 */
		public function cast(ray 		: Ray,
							 transform 	: Matrix4x4 = null,
							 hitXYZ 	: Vector4 	= null,
							 hitUV		: Point		= null,
							 hitNormal	: Vector4	= null) : int
		{
			var numVertices : uint 	= indexStream._data.length / 2;
			
			var u 		: Number 	= 0;
			var v 		: Number 	= 0;
			var t		: Number 	= 0;
			var det 	: Number 	= 0;
			var invDet	: Number 	= 0;
			var EPSILON : Number 	= 0.00001;
			
			var localOrigin 	: Vector4	= ray.origin;
			var localDirection 	: Vector4 	= ray.direction;
			
			if (transform)
			{
				localOrigin = transform.transformVector(ray.origin);

				localDirection = transform.deltaTransformVector(ray.direction);
				localDirection.normalize();
			}
			
			var vertexStream 	: IVertexStream = getVertexStream(0);
			
			var xyzStream 		: VertexStream 	= vertexStream.getStreamByComponent(VertexComponent.XYZ);
			var format 			: VertexFormat 	= xyzStream.format;
			var xyzVertexSize 	: uint 			= format.numBytesPerVertex;
			var offsetInBytes	: uint 			= format.getBytesOffsetForComponent(VertexComponent.XYZ);
			var stride			: uint			= VertexComponent.XYZ.numProperties;
			
			var localDirectionX : Number 		= localDirection.x;
			var localDirectionY	: Number 		= localDirection.y;
			var localDirectionZ	: Number 		= localDirection.z;
			
			var localOriginX 	: Number 		= localOrigin.x;
			var localOriginY	: Number 		= localOrigin.y;
			var localOriginZ	: Number 		= localOrigin.z;
			
			var indicesData 	: ByteArray 	= _indexStream.lock();
			var xyzData			: ByteArray		= xyzStream.lock();
			var minDistance		: Number 		= Number.POSITIVE_INFINITY;
			var triangleIndice	: int 			= -3;
			
			var lambda			: Vector4		= new Vector4();
			
			//vertex 1
			var v0X 			: Number 		= .0;
			var v0Y 			: Number 		= .0;
			var v0Z 			: Number 		= .0;
			
			//vertex2
			var v1X 			: Number 		= .0;
			var v1Y 			: Number 		= .0;
			var v1Z 			: Number 		= .0;
			
			//vertex3
			var v2X 			: Number 		= .0;
			var v2Y 			: Number 		= .0;
			var v2Z 			: Number 		= .0;
			
			//edge 1
			var edge1X 			: Number 		= .0;
			var edge1Y 			: Number 		= .0;
			var edge1Z 			: Number 		= .0;
			
			//edge 2
			var edge2X 			: Number 		= .0;
			var edge2Y 			: Number 		= .0;
			var edge2Z 			: Number 		= .0;
            
			for (var verticeIndex : uint = 0; verticeIndex < numVertices; verticeIndex += stride)
			{
				indicesData.position = verticeIndex * 2;
				
				xyzData.position = indicesData.readUnsignedShort() * xyzVertexSize + offsetInBytes;
				v0X = xyzData.readFloat();
				v0Y = xyzData.readFloat();
				v0Z = xyzData.readFloat();

				xyzData.position = indicesData.readUnsignedShort() * xyzVertexSize + offsetInBytes;
				v1X = xyzData.readFloat();
				v1Y = xyzData.readFloat();
				v1Z = xyzData.readFloat();
				
				xyzData.position = indicesData.readUnsignedShort() * xyzVertexSize + offsetInBytes;
				v2X = xyzData.readFloat();
				v2Y = xyzData.readFloat();
				v2Z = xyzData.readFloat();

				edge1X = v1X - v0X;
				edge1Y = v1Y - v0Y;
				edge1Z = v1Z - v0Z;
				
				edge2X = v2X - v0X;
				edge2Y = v2Y - v0Y;
				edge2Z = v2Z - v0Z;				
				
				// cross product
				var pvecX : Number = localDirectionY * edge2Z - edge2Y * localDirectionZ;
				var pvecY : Number = localDirectionZ * edge2X - edge2Z * localDirectionX;
				var pvecZ : Number = localDirectionX * edge2Y - edge2X * localDirectionY;
				
				// dot product
				det = (edge1X * pvecX + edge1Y * pvecY + edge1Z * pvecZ);
				
				if (det > - EPSILON && det < EPSILON)
					continue;
				
				invDet = 1.0 / det;
				
				var tvecX : Number = localOriginX - v0X;
				var tvecY : Number = localOriginY - v0Y;
				var tvecZ : Number = localOriginZ - v0Z;
				
				u = (tvecX * pvecX + tvecY * pvecY + tvecZ * pvecZ) * invDet;
				
				if (u < 0 || u > 1)
					continue;
				
				var qvecX : Number = tvecY * edge1Z - edge1Y * tvecZ;
				var qvecY : Number = tvecZ * edge1X - edge1Z * tvecX;
				var qvecZ : Number = tvecX * edge1Y - edge1X * tvecY;
				
				v = (localDirectionX * qvecX + localDirectionY * qvecY + localDirectionZ * qvecZ) * invDet;
				
				if (v < 0 || u + v > 1)
					continue;
				
				t =  (edge2X * qvecX + edge2Y * qvecY + edge2Z * qvecZ) * invDet; 
				
				if (t < minDistance)
				{
					minDistance = t;
					triangleIndice = verticeIndex;
					
					if (hitUV)
					{
						lambda.x = u
						lambda.y = v
					}
				}
			}
			
			if (triangleIndice >= 0)
			{
				if (hitXYZ)
				{
					hitXYZ.x = localOrigin.x + minDistance * localDirection.x;
					hitXYZ.y = localOrigin.y + minDistance * localDirection.y;
					hitXYZ.z = localOrigin.z + minDistance * localDirection.z;
				}
				
				if (hitUV)
				{
					var uvStream : VertexStream = vertexStream.getStreamByComponent(VertexComponent.UV);
					
					if (!uvStream)
						throw new Error('Missing vertex component: u, v');
					
					var uvData 			: ByteArray = uvStream == xyzStream ? xyzData : uvStream.lock();
					var uvVertexSize 	: uint 		= uvStream.format.numBytesPerVertex;
					var uvOffset 		: uint 		= uvStream.format.getOffsetForComponent(VertexComponent.UV);
					
					indicesData.position = triangleIndice * 2;
					
					uvData.position = indicesData.readUnsignedShort() * uvVertexSize + uvOffset;
					var u0  : Number = uvData.readFloat();
					var v0  : Number = uvData.readFloat();
					
					uvData.position = indicesData.readUnsignedShort() * uvVertexSize + uvOffset;
					var u1  : Number = uvData.readFloat();
					var v1  : Number = uvData.readFloat();
					
					uvData.position = indicesData.readUnsignedShort() * uvVertexSize + uvOffset;
					var u2  : Number = uvData.readFloat();
					var v2  : Number = uvData.readFloat(); 
					
					lambda.z = 1 - lambda.x - lambda.y;
					
					hitUV.x = lambda.z * u0 + lambda.x*u1 + lambda.y * u2;
					hitUV.y = lambda.z * v0 + lambda.x*v1 + lambda.y * v2;
					
					if (uvStream != xyzStream)
						uvStream.unlock(false);
				}
				
				if (hitNormal)
				{
					indicesData.position = triangleIndice * 2;
					
					xyzData.position = indicesData.readUnsignedShort() * xyzVertexSize + offsetInBytes
					v0X = xyzData.readFloat();
					v0Y = xyzData.readFloat();
					v0Z = xyzData.readFloat();
					
					xyzData.position = indicesData.readUnsignedShort() * xyzVertexSize + offsetInBytes;
					v1X = xyzData.readFloat();
					v1Y = xyzData.readFloat();
					v1Z = xyzData.readFloat();
					
					xyzData.position = indicesData.readUnsignedShort() * xyzVertexSize + offsetInBytes;
					v2X = xyzData.readFloat();
					v2Y = xyzData.readFloat();
					v2Z = xyzData.readFloat();
					
					edge1X = v1X - v0X;
					edge1Y = v1Y - v0Y;
					edge1Z = v1Z - v0Z;
					
					edge2X = v2X - v0X;
					edge2Y = v2Y - v0Y;
					edge2Z = v2Z - v0Z;
					
					// length
					var edge1Length		: Number	= Math.sqrt(edge1X * edge1X + edge1Y * edge1Y + edge1Z * edge1Z);
					var edge2Length		: Number	= Math.sqrt(edge2X * edge2X + edge2Y * edge2Y + edge2Z * edge2Z);
					
					// normalize
					edge1X /= edge1Length;
					edge1Y /= edge1Length;
					edge1Z /= edge1Length;
					edge2X /= edge2Length;
					edge2Y /= edge2Length;
					edge2Z /= edge2Length;
					
					// cross product
					hitNormal.x = edge2Y * edge1Z - edge2Z * edge1Y;
					hitNormal.y = edge2Z * edge1X - edge2X * edge1Z;
					hitNormal.z = edge2X * edge1Y - edge2Y * edge1X;
				}
			}
			
			_indexStream.unlock(false);
			xyzStream.unlock(false);
			
			return triangleIndice / 3;				
		}
		
		/**
		 * Invert the winding of the triangles by rewritting the IndexStream. Inverting the winding
		 * means that the front faces will become back faces and vice versa.
		 *  
		 * @return The Geometry object itself.
		 * 
		 */
		public function invertWinding() : Geometry
		{
			var numIndices		: uint		= indexStream.length;
			var numTriangles	: uint		= numIndices / 3;
			var indices			: ByteArray	= indexStream.lock();
			
			for (var i : uint = 0; i < numTriangles; ++i)
			{
				var i0 : uint = indices.readUnsignedShort();
				var i1 : uint = indices.readUnsignedShort();
				var i2 : uint = indices.readUnsignedShort();
				
				indices.position -= 6;
				indices.writeShort(i2);
				indices.writeShort(i1);
				indices.writeShort(i0);
			}
			
			indexStream.unlock();
			
			return this;
		}
	}
}
