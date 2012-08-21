package aerys.minko.render.geometry
{
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
	import aerys.minko.type.bounding.BoundingBox;
	import aerys.minko.type.bounding.BoundingSphere;
	import aerys.minko.type.bounding.IBoundingVolume;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	
	import flash.utils.ByteArray;

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
	public class Geometry implements IBoundingVolume
	{
		use namespace minko_scene;
		use namespace minko_stream;
		
		private static const FORMAT_NORMALS		: VertexFormat		= new VertexFormat(
			VertexComponent.NORMAL
		);
		private static const FORMAT_TANGENTS	: VertexFormat		= new VertexFormat(
			VertexComponent.TANGENT
		);
		private static const FORMAT_TN			: VertexFormat		= new VertexFormat(
			VertexComponent.TANGENT,
			VertexComponent.NORMAL
		);
		private static const TMP_MATRIX			: Matrix4x4			= new Matrix4x4();
		
		minko_scene var _vertexStreams		: Vector.<IVertexStream>	= null;

		private var _indexStream	: IndexStream		= null;
		private var _firstIndex		: uint				= 0;
		private var _numTriangles	: int				= -1;

		private var _boundingSphere	: BoundingSphere	= null;
		private var _boundingBox	: BoundingBox		= null;
		
		private var _bulkUpdate		: Boolean			= false;
		
		private var _changed		: Signal;
		
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
								 numTriangles	: int						= -1)
		{			
			initialize(vertexStreams, indexStream, firstIndex, numTriangles);
		}
		
		private function initialize(vertexStreams	: Vector.<IVertexStream>	= null,
									indexStream		: IndexStream				= null,
									firstIndex		: uint						= 0,
									numTriangles	: int						= -1) : void
		{
			_changed = new Signal('Geometry.changed');
			
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
			if (index < _vertexStreams.length)
			{
				var oldStream : IVertexStream = _vertexStreams[index] as IVertexStream;
				
				oldStream.changed.remove(vertexStreamChangedHandler);
				oldStream.boundsChanged.remove(vertexStreamBoundsChangedHandler);
			}
			
			vertexStream.boundsChanged.add(vertexStreamBoundsChangedHandler);
			vertexStream.changed.add(vertexStreamChangedHandler);
			_vertexStreams[index] = vertexStream;
			
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
					
				for (var streamId : uint = 0; streamId < numStreams; ++streamId)
				{
					stream = getVertexStream(streamId);
					
					var numVertices		: uint				= stream.numVertices;
					var xyzStream		: VertexStream		= stream.getStreamByComponent(VertexComponent.XYZ);
					var normalsStream 	: VertexStream		= stream.getStreamByComponent(VertexComponent.NORMAL);
					var hasNormals		: Boolean			= normalsStream != null;
					
					if (!hasNormals)
						normalsStream = new VertexStream(streamUsage, FORMAT_NORMALS, new Vector.<Number>(numVertices * 3));
					
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
			var numTriangles	: int			= indexStream.length / 3;
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
				
				for (var streamId : uint = 0; streamId < numStreams; ++streamId)
				{
					stream = getVertexStream(streamId);
					
					var numVertices		: uint				= stream.numVertices;
					var xyzStream		: VertexStream		= stream.getStreamByComponent(VertexComponent.XYZ);
					var uvStream		: VertexStream		= stream.getStreamByComponent(VertexComponent.UV);
					var normalsStream 	: VertexStream		= stream.getStreamByComponent(VertexComponent.NORMAL);
					var tangentsStream	: VertexStream		= stream.getStreamByComponent(VertexComponent.TANGENT);
					var hasNormals		: Boolean			= normalsStream != null;
					var hasTangents		: Boolean			= tangentsStream != null;
					
					if (!hasNormals && !hasTangents)
					{
						normalsStream = tangentsStream = new VertexStream(
							streamUsage,
							new VertexFormat(VertexComponent.NORMAL, VertexComponent.TANGENT),
							new Vector.<Number>(numVertices * 6)
						);
					}
					else if (!hasNormals && computeNormals)
					{
						normalsStream = new VertexStream(
							streamUsage,
							new VertexFormat(VertexComponent.NORMAL),
							new Vector.<Number>(numVertices * 3)
						);
					}
					else if (!hasTangents)
					{
						tangentsStream = new VertexStream(
							streamUsage,
							new VertexFormat(VertexComponent.TANGENT),
							new Vector.<Number>(numVertices * 3)
						);
					}
					
					if (computeNormals)
						fillNormalsData(indices, normalsStream, xyzStream, triangles);
					
					fillTangentsData(indices, tangentsStream, xyzStream, uvStream, triangles);
					
					if (!hasNormals)
						pushVertexStreamInList(streamId, normalsStream);
					if (!hasTangents && hasNormals)
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
			var numTriangles		: uint		= triangles ? triangles.length : indices.length / 3;
			var nx					: Number 	= 0.;
			var ny					: Number 	= 0.;
			var nz					: Number 	= 0.;
			var mag					: Number	= 0.;
			var xyzOffset			: uint		= xyzStream.format.getBytesOffsetForComponent(VertexComponent.XYZ);
			var xyzVertexSize		: uint		= xyzStream.format.numBytesPerVertex;
			var xyzData				: ByteArray	= xyzStream.lock();
			var normalsOffset		: uint		= normalsStream.format.getBytesOffsetForComponent(VertexComponent.NORMAL);
			var normalsVertexSize	: uint		= normalsStream.format.numBytesPerVertex;
			var normalsData			: ByteArray	= null;
			var numVertices			: uint		= xyzStream.numVertices / xyzVertexSize;
			
			normalsData = normalsStream == xyzStream ? xyzData : normalsStream.lock();
			
			for (var i : uint = 0; i < numTriangles; ++i)
			{
				var ii	: uint		= triangles ? triangles[i] * 3 : i * 3;
				
				indices.position = ii << 2;
				var i0	: uint 		= indices.readUnsignedInt();
				var i1	: uint 		= indices.readUnsignedInt();
				var i2	: uint 		= indices.readUnsignedInt();
				
				var ii0	: uint 		= xyzOffset + xyzVertexSize * i0;
				var ii1	: uint		= xyzOffset + xyzVertexSize * i1;
				var ii2	: uint 		= xyzOffset + xyzVertexSize * i2;
				
				xyzData.position = ii0;
				var x0	: Number	= xyzData.readFloat();
				var y0	: Number 	= xyzData.readFloat();
				var z0	: Number 	= xyzData.readFloat();
				
				xyzData.position = ii1;
				var x1	: Number 	= xyzData.readFloat();
				var y1	: Number 	= xyzData.readFloat();
				var z1	: Number 	= xyzData.readFloat();
				
				xyzData.position = ii2;
				var x2	: Number 	= xyzData.readFloat();
				var y2	: Number 	= xyzData.readFloat();
				var z2	: Number 	= xyzData.readFloat();
				
				nx = (y0 - y2) * (z0 - z1) - (z0 - z2) * (y0 - y1);
				ny = (z0 - z2) * (x0 - x1) - (x0 - x2) * (z0 - z1);
				nz = (x0 - x2) * (y0 - y1) - (y0 - y2) * (x0 - x1);
				
				mag = Math.sqrt(nx * nx + ny * ny + nz * nz);
				
				if (mag != 0.)
				{
					nx /= mag;
					ny /= mag;
					nz /= mag;
				}
				
				ii0 = i0 * normalsVertexSize + normalsOffset;
				ii1 = i1 * normalsVertexSize + normalsOffset;
				ii2 = i2 * normalsVertexSize + normalsOffset;
				
				normalsData.position = ii0;
				normalsData.writeFloat(nx);
				normalsData.writeFloat(ny);
				normalsData.writeFloat(nz);
				
				normalsData.position = ii1;
				normalsData.writeFloat(nx);
				normalsData.writeFloat(ny);
				normalsData.writeFloat(nz);
			
				normalsData.position = ii2;
				normalsData.writeFloat(nx);
				normalsData.writeFloat(ny);
				normalsData.writeFloat(nz);
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
			var numTriangles		: uint		= triangles ? triangles.length : indices.length / 3;
			var xyzOffset			: uint		= xyzStream.format.getBytesOffsetForComponent(VertexComponent.XYZ);
			var xyzVertexSize		: uint		= xyzStream.format.numBytesPerVertex;
			var xyzData				: ByteArray	= xyzStream.lock();
			var uvOffset			: uint		= uvStream.format.getBytesOffsetForComponent(VertexComponent.UV);
			var uvVertexSize		: uint		= uvStream.format.numBytesPerVertex;
			var uvData				: ByteArray	= null;
			var tangentsOffset		: uint		= tangentsStream.format.getBytesOffsetForComponent(VertexComponent.TANGENT);
			var tangentsVertexSize	: uint		= tangentsStream.format.numBytesPerVertex;
			var tangentsData		: ByteArray	= null;
			var numVertices			: uint		= xyzStream.numVertices / xyzVertexSize;
			
			uvData = uvStream == xyzStream ? xyzData : uvStream.lock();
			tangentsData = tangentsStream == xyzStream ? xyzData : tangentsStream.lock();
			
			for (var i : uint = 0; i < numTriangles; ++i)
			{
				var ii 		: uint		= triangles ? triangles[i] * 3 : i * 3;
			
				indices.position = ii << 2;
				var i0		: uint 		= indices.readUnsignedInt();
				var i1		: uint 		= indices.readUnsignedInt();
				var i2		: uint 		= indices.readUnsignedInt();
				
				var ii1		: uint		= xyzOffset + xyzVertexSize * i1;
				var ii2		: uint 		= xyzOffset + xyzVertexSize * i2;
				
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
				var mag	: Number	= Math.sqrt(tx * tx + ty * ty + tz * tz);
				
				if (mag != 0.)
				{
					tx /= mag;
					ty /= mag;
					tz /= mag;
				}
				
				tangentsData.position = i0 * tangentsVertexSize + tangentsOffset;
				tangentsData.writeFloat(tx);
				tangentsData.writeFloat(ty);
				tangentsData.writeFloat(tz);
				
				tangentsData.position = i1 * tangentsVertexSize + tangentsOffset;
				tangentsData.writeFloat(tx);
				tangentsData.writeFloat(ty);
				tangentsData.writeFloat(tz);
				
				tangentsData.position = i2 * tangentsVertexSize + tangentsOffset;
				tangentsData.writeFloat(tx);
				tangentsData.writeFloat(ty);
				tangentsData.writeFloat(tz);
			}
			
			for (i = 0; i < numVertices; ++i)
			{
				ii = i * tangentsVertexSize + tangentsOffset;
				tangentsData.position = ii;
				tx = tangentsData.readFloat();
				ty = tangentsData.readFloat();
				tz = tangentsData.readFloat();
				mag = Math.sqrt(tx * tx + ty * ty + tz * tz);
				
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
				setVertexStream(stream, index);
			}
			
			(stream as VertexStreamList).pushVertexStream(vertexStream, force);
		}
		
		private function updateBoundingVolumes() : void
		{
			if (_bulkUpdate)
				return ;
				
			var numStreams	: int		= _vertexStreams.length;
			var minX		: Number	= Number.MAX_VALUE;
			var minY		: Number	= Number.MAX_VALUE;
			var minZ		: Number	= Number.MAX_VALUE;
			var maxX		: Number	= -Number.MAX_VALUE;
			var maxY		: Number	= -Number.MAX_VALUE;
			var maxZ		: Number	= -Number.MAX_VALUE;
			
			for (var i : int = 0; i < numStreams; ++i)
			{
				var xyzStream	: VertexStream	= _vertexStreams[i].getStreamByComponent(
					VertexComponent.XYZ
				);
				
				if (!xyzStream)
					return ;
				
				var offset		: uint		= xyzStream.format.getBytesOffsetForComponent(
					VertexComponent.XYZ
				);
				var streamMinX	: Number	= xyzStream.getMinimum(offset);
				var streamMinY	: Number	= xyzStream.getMinimum(offset + 1);
				var streamMinZ	: Number	= xyzStream.getMinimum(offset + 2);
				var streamMaxX	: Number	= xyzStream.getMaximum(offset);
				var streamMaxY	: Number	= xyzStream.getMaximum(offset + 1);
				var streamMaxZ	: Number	= xyzStream.getMaximum(offset + 2);
				
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
		
		public function split() : Vector.<Geometry>
		{
			var geometries		: Vector.<Geometry> = new <Geometry>[];
			var numGeometries	: uint				= 0;
			var geometryId 		: uint				= 0;
			var numStreams		: uint				= _vertexStreams.length; 
			var indexData		: ByteArray			= _indexStream.minko_stream::_data;
			
			for (var streamId : uint = 0; streamId < numStreams; ++streamId)
			{
				var stream	: VertexStream	= _vertexStreams[streamId] as VertexStream;
				if (stream == null)
					stream = VertexStream.extractSubStream(_vertexStreams[streamId], StreamUsage.DYNAMIC);
				
				var vertexData	: ByteArray				= stream.minko_stream::_data;
				var vertexDatas	: Vector.<ByteArray>	= new <ByteArray>[];
				var indexDatas	: Vector.<ByteArray>	= new <ByteArray>[];
				
				GeometrySanitizer.splitBuffers(vertexData, indexData, vertexDatas, indexDatas, stream.format.numBytesPerVertex);
				
				if (streamId == 0)
				{
					numGeometries = geometries.length = vertexDatas.length;
					for (geometryId = 0; geometryId < numGeometries; ++geometryId)
						geometries[geometryId] = new Geometry(new <IVertexStream>[], new IndexStream(StreamUsage.DYNAMIC, indexDatas[geometryId]));
				}
				
				for (geometryId = 0; geometryId < numGeometries; ++geometryId)
					geometries[geometryId].setVertexStream(new VertexStream(StreamUsage.DYNAMIC, stream.format, vertexDatas[geometryId]), streamId);
			}
			
			return geometries;
		}
		
		public function merge(geometry			: Geometry,
							  vertexStreamUsage : uint = 3,
							  indexStreamUsage 	: uint = 3) : Geometry
		{
			var vertexStreamsToConcat 	: Vector.<IVertexStream> = new <IVertexStream>[];
			
			if (numVertexStreams != 0 && geometry.numVertexStreams != 0
				&& numVertexStreams != geometry.numVertexStreams)
			{
				throw new Error('Both geometry must have the same number of vertex streams.');
			}

			if (numVertexStreams != 0)
				vertexStreamsToConcat.push(_vertexStreams[0]);
			
			if (geometry.numVertexStreams != 0)
				vertexStreamsToConcat.push(geometry._vertexStreams[0]);

			var indexOffset : uint = vertexStreamsToConcat.length != 1
				? (vertexStreamsToConcat[0] as VertexStream).numVertices
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

			return this;				
		}
		
		public function flipNormals(flipTangents : Boolean = true) : Geometry
		{
			var numStreams : uint = this.numVertexStreams;
			
			for (var streamId : uint = 0; streamId < numStreams; ++streamId)
			{
				var vertexStream	: IVertexStream		= getVertexStream(streamId);
				var normalsStream 	: VertexStream 		= vertexStream.getStreamByComponent(VertexComponent.NORMAL);
				var tangentsStream 	: VertexStream 		= vertexStream.getStreamByComponent(VertexComponent.NORMAL);
				
				if (normalsStream != null)
					invertVertexStreamComponent(normalsStream, VertexComponent.NORMAL);
				
				if (tangentsStream != null)
					invertVertexStreamComponent(tangentsStream, VertexComponent.TANGENT);
			}
			
			return this;
		}
		
		private function invertVertexStreamComponent(vertexStream : VertexStream, component : VertexComponent) : void
		{
			var componentOffset	: uint				= vertexStream.format.getBytesOffsetForComponent(component);
			var vertexSize		: uint				= vertexStream.format.numBytesPerVertex;
			var numVertices		: uint				= vertexStream.numVertices;
			var data			: ByteArray			= vertexStream.lock();
			var numValues		: uint				= component.numProperties >>> 2;
			var tmp				: Vector.<Number>	= new Vector.<Number>(numValues);
			
			for (var i : uint = 0; i < numVertices; ++i)
			{
				data.position = i * vertexSize + componentOffset;
				for (var j : uint = 0; j < numValues; ++j)
					tmp[j] = data.readFloat();
				
				data.position = i * vertexSize + componentOffset;
				for (j = 0; j < numValues; ++j)
					data.writeFloat(-tmp[j]);
			}
			
			vertexStream.unlock();
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
	}
}
