package aerys.minko.scene.mesh.parser.max3ds
{
	import aerys.minko.Minko;
	import aerys.minko.scene.mesh.IMesh3D;
	import aerys.minko.scene.mesh.Mesh3D;
	import aerys.minko.scene.mesh.parser.IMeshParser;
	import aerys.minko.type.stream.IndexStream3D;
	import aerys.minko.type.stream.VertexStream3D;
	
	import flash.utils.ByteArray;
	import flash.utils.Endian;
		
	public final class Max3DSParser extends AbstractMax3DSParser implements IMeshParser
	{
		protected var _meshes			: Vector.<IMesh3D>	= new Vector.<IMesh3D>();
		protected var _materials		: Object			= new Object();
		protected var _meshesMap		: Object			= new Object();
		
		public function get meshes()	: Vector.<IMesh3D>	{ return (_meshes); }
		public function get meshesMap()	: Object			{ return (_meshesMap); }
		
		public function Max3DSParser()
		{
			super();
		}
		
		override protected function initialize() : void
		{
			super.initialize();
			
			parseFunctions[Max3DSChunk.PRIMARY] = parsePrimary;
			parseFunctions[Max3DSChunk.SCENE] = enterChunk;
			parseFunctions[Max3DSChunk.MATERIAL] = parseMaterial;
			parseFunctions[Max3DSChunk.OBJECT] = parseObject;
		}
		
		public function parse(myData : ByteArray) : void
		{
			myData.endian = Endian.LITTLE_ENDIAN;
			
			Minko.log("Parsing " + myData.bytesAvailable + " bytes", this);
			
			parseChunk(new Max3DSChunk(myData));
		}
		
		protected function parsePrimary(myChunk	: Max3DSChunk) : void
		{
			// throw an error if the first chunk is not a primary chunk
			if (myChunk.identifier != Max3DSChunk.PRIMARY)
				Minko.logError("Wrong file format!", this);
		}
		
		protected function parseObject(myChunk : Max3DSChunk) : void
		{
			var name	: String	= myChunk.readString();
			
			myChunk = new Max3DSChunk(myChunk.data);
			if (myChunk.identifier == Max3DSChunk.MESH)
			{
				Minko.log("Parsing mesh \"" + name + "\"", this);
				
				var parser	: Max3DSMeshParser 	= new Max3DSMeshParser(myChunk, name);
				var mesh	: Mesh3D			= new Mesh3D(VertexStream3D.fromVerticesAndUVs(parser.vertices, parser.uvData),
														     new IndexStream3D(parser.indices));
					
				_meshesMap[name] = mesh;
				_meshes.push(mesh);
			}
			else
			{
				myChunk.skip();
			}
		}
		
		protected function parseMaterial(myChunk : Max3DSChunk) : void
		{
			var material : Max3DSMaterialParser = new Max3DSMaterialParser(myChunk);
			
			_materials[material.name] = material;
		}
	}
}