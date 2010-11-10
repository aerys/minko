package aerys.minko.scene.mesh.parser.max3ds
{
	import aerys.minko.Minko;
	
	import flash.geom.Matrix3D;
	import flash.utils.ByteArray;
	
	internal class Max3DSMeshParser extends Max3DSObjectParser
	{
		//{ region consts
		private static const TRANSFORM	: Matrix3D	= new Matrix3D(Vector.<Number>([-1., 0., 0.,  0.,
																					0.,	 0., -1., 0.,
																					0.,	 1., 0.,  0.,
																					0.,	 0., 0.,  1.]));
		//} endregion
		
		//{ region vars
		protected var _vertices		: Vector.<Number>	= null;
		protected var _indices		: Vector.<uint>		= null;
		protected var _uvData		: Vector.<Number>	= null;
				
		protected var _materialName	: String			= null;
		protected var _mappedFaces	: Vector.<int>		= null;
		//} endregion
		
		//{ region getters/setters
		public function get vertices()		: Vector.<Number>	{ return _vertices; }
		public function get uvData()		: Vector.<Number>	{ return _uvData; }
		public function get indices()		: Vector.<uint>		{ return _indices; }
		public function get materialName()	: String			{ return _materialName; }
		public function get mappedFaces()	: Vector.<int>		{ return _mappedFaces; }
		//} endregion
		
		public function Max3DSMeshParser(myChunk : Max3DSChunk, myName : String)
		{
			super(myChunk, myName);
		}
		
		//{ region methods
		override protected function initialize() : void
		{
			super.initialize();
			
			parseFunctions[Max3DSChunk.MESH] = enterChunk;
			parseFunctions[Max3DSChunk.MESH_VERTICES] = parseVertices;
			parseFunctions[Max3DSChunk.MESH_INDICES] = parseIndices;
			parseFunctions[Max3DSChunk.MESH_MAPPING] = parseUVData;
			parseFunctions[Max3DSChunk.MESH_MATERIAL] = parseMaterial;
		}
		
		protected function parseVertices(myChunk	: Max3DSChunk) : void
		{
			var	nbVertices	: int	= myChunk.data.readUnsignedShort() * 3;
			
			Minko.log(int(nbVertices / 3) + " vertices", this);
			
			_vertices = new Vector.<Number>(nbVertices, true);			
			for (var i : int = 0; i < nbVertices; i += 3)
			{
				_vertices[i] = myChunk.data.readFloat();
				_vertices[i + 1] = myChunk.data.readFloat();
				_vertices[i + 2] = myChunk.data.readFloat();
			}
		}
		
		protected function parseIndices(myChunk	: Max3DSChunk) : void
		{
			var data	: ByteArray		= myChunk.data;
			var	nbFaces	: int			= data.readUnsignedShort() * 3;
			
			Minko.log(nbFaces + " indices", this);
			
			_indices = new Vector.<uint>(nbFaces, true)
			for (var i : int = 0; i < nbFaces; i += 3)
			{
				_indices[i] = data.readUnsignedShort();
				_indices[i + 1] = data.readUnsignedShort();
				_indices[i + 2] = data.readUnsignedShort();
				
				data.position += 2;
			}
		}
	
		protected function parseUVData(myChunk	: Max3DSChunk) : void
		{
			var data			: ByteArray	= myChunk.data; 
			var	nbCoordinates	: int		= data.readUnsignedShort() * 2;
			
			Minko.log(int(nbCoordinates / 2) + " texture coordinates", this);
			
			_uvData = new Vector.<Number>(nbCoordinates, true);
			for (var i : int = 0; i < nbCoordinates; i += 2)
			{
				_uvData[i] = data.readFloat();
				_uvData[i + 1] = 1. - data.readFloat();
			}
		}
		
		protected function parseMaterial(myChunk	: Max3DSChunk) : void
		{
			var data		: ByteArray	= myChunk.data;
			var	nbFaces		: int		= 0;
			
			_materialName = myChunk.readString();
			
			nbFaces = data.readUnsignedShort();
			_mappedFaces = new Vector.<int>(nbFaces, true);
			
			for (var i : int = 0; i < nbFaces; i++)
				_mappedFaces[i] = data.readUnsignedShort();
		}
		
		override protected function finalize() : void
		{
			super.finalize();
		
			Minko.log("Transforming vertices into left-handed coordinates system...", this);
			
			var vertices : Vector.<Number>	= new Vector.<Number>();
			
			TRANSFORM.transformVectors(_vertices, vertices);
			_vertices = vertices;
		}
		//} endregion
	}
}