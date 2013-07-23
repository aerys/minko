package aerys.minko.type.loader
{
	import aerys.minko.render.geometry.Geometry;
	import aerys.minko.render.material.Material;
	import aerys.minko.render.material.basic.BasicMaterial;
	import aerys.minko.render.resource.texture.TextureResource;
	import aerys.minko.type.Signal;

	public class AssetsLibrary
	{
		private var _numGeometries 	: uint 						= 0;
		private var _numMaterials	: uint 						= 0;
		private var _numTextures	: uint 						= 0;
		
		private var _geometryList	: Vector.<Geometry> 		= new Vector.<Geometry>();
		private var _materialList	: Vector.<Material> 		= new Vector.<Material>();
		private var _textureList	: Vector.<TextureResource> 	= new Vector.<TextureResource>();
		
		private var _geometries		: Object;
		private var _textures		: Object;
		private var _materials		: Object;
		private var _layers			: Vector.<String>;
		private var _nameToLayer	: Object;
		
		private var _geometryAdded	: Signal;
		private var _textureAdded	: Signal;
		private var _materialAdded	: Signal;
		private var _layerAdded		: Signal;
		
		public function AssetsLibrary()
		{
			initialize();
			initializeSignals();
		}
		
		public function get numTextures() : uint
		{
			return _numTextures;
		}
		
		public function get numGeometries() : uint
		{
			return _numGeometries;
		}
		
		public function get numMaterials() : uint
		{
			return _numMaterials;
		}
		
		public function getMaterialAt(index : uint): Material
		{
			return _materialList[index];
		}
		
		public function getGeometryAt(index : uint): Geometry
		{
			return _geometryList[index];
		}
		
		public function getTextureAt(index : uint): TextureResource
		{
			return _textureList[index];
		}
		
		public function getMaterialByName(name : String) : Material
		{
			return _materials[name];
		}
		
		public function getTextureByName(name : String) : TextureResource
		{
			return _textures[name];
		}
		
		public function getGeometryByName(name : String) : Geometry
		{
			return _geometries[name];
		}
		
		private function initialize() : void
		{
			_geometries		= {};
			_textures		= {};
			_materials		= {};
			
			_layers			= new Vector.<String>(32, true);
			_layers[0]		= 'Default';
			for (var i : uint = 1; i < _layers.length; ++i)
				_layers[i]	= 'Layer_' + i;
			
			_nameToLayer	= {};
		}
		
		private function initializeSignals() : void
		{
			_geometryAdded	= new Signal('AssetsLibrary.geometryAdded');
			_textureAdded	= new Signal('AssetsLibrary.textureAdded');
			_materialAdded	= new Signal('AssetsLibrary.materialAdded');
			_layerAdded		= new Signal('AssetsLibrary.layerAdded');
		}
		
		public function get layerAdded() : Signal
		{
			return _layerAdded;
		}

		public function get materialAdded() : Signal
		{
			return _materialAdded;
		}

		public function get textureAdded() : Signal
		{
			return _textureAdded;
		}

		public function get geometryAdded() : Signal
		{
			return _geometryAdded;
		}
		
		public function setGeometry(name : String, geometry : Geometry) : void
		{
			if (_geometryList.indexOf(geometry) == -1)
			{
				++_numGeometries;
				_geometryList.push(geometry);
				_geometries[name] = geometry;
				_geometryAdded.execute(this, name, geometry);
			}
		}

		public function setTexture(name : String, texture : TextureResource) : void
		{
			if (_textureList.indexOf(texture) == -1)
			{
				++_numTextures;
				_textureList.push(texture);
				_textures[name] = texture;
				_textureAdded.execute(this, name, texture);
			}
		}
		
		public function setMaterial(name : String, material : Material) : void
		{
			material ||= new BasicMaterial();
			name ||= material.name;
			
			if (_materialList.indexOf(material) == -1)
			{
				++_numMaterials;
				_materialList.push(material);	
				_materials[name] = material;
				_materialAdded.execute(this, name, material);
			}
		}
		
		public function setLayer(name : String, layer : uint) : void
		{
			name ||= 'Layer_' + layer;
			_layers[layer] = name;
			_layerAdded.execute(this, name, layer);
		}
		
		public function getLayerTag(name : String, ... params) : uint
		{
			var tag	: uint	= 0;
			
			if (_nameToLayer[name])
			{
				tag |= 1 << _nameToLayer[name];
			}
			
			for each (name in params)
			{
				if (_nameToLayer[name])
				{
					tag |= 1 << _nameToLayer[name];
				}
			}
			
			return tag;
		}
		
		public function getLayerName(tag : uint) : String
		{
			var names	: Vector.<String>	= new <String>[];
			var name	: String 			= null;
			var i		: uint				= 0;
			
			for (i = 0; i < 32; ++i)
			{
				if (tag & (1 << i))
					names.push(_layers[i]);
			}
			name = names.join('|');
			
			return name;
		}
		
		public function merge(other : AssetsLibrary) : AssetsLibrary
		{
			var name	: String	= null;
			var i		: uint		= 0;
			
			for (name in other._geometries)
				setGeometry(name, other._geometries[name]);
			for (name in other._materials)
				setMaterial(name, other._materials[name]);
			for (name in other._textures)
				setTexture(name, other._textures[name]);
			for each (name in other._layers)
				setLayer(name, i++);
			
			return this;
		}
	}
}