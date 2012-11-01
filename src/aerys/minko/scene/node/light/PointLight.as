package aerys.minko.scene.node.light
{
	import aerys.minko.ns.minko_scene;
	import aerys.minko.render.resource.texture.CubeTextureResource;
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.render.resource.texture.TextureResource;
	import aerys.minko.scene.node.AbstractSceneNode;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.type.binding.DataBindings;
	import aerys.minko.type.enum.ShadowMappingType;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;

	use namespace minko_scene;
	
	public class PointLight extends AbstractLight
	{
		public static const TYPE : uint = 2;
		
		private static const MAP_NAMES : Vector.<String> = new <String>[
			'shadowMapCube',
			'shadowMapDPFront',
			'shadowMapDPBack'
		];
		
		private static const TMP_VECTOR		: Vector4 = new Vector4();
		
		private static const FRUSTUM_POINTS	: Vector.<Vector4>	= new <Vector4>[
			new Vector4(-1, -1, 0, 1),
			new Vector4(-1, -1, 1, 1),
			new Vector4(-1, +1, 0, 1),
			new Vector4(-1, +1, 1, 1),
			new Vector4(+1, -1, 0, 1),
			new Vector4(+1, -1, 1, 1),
			new Vector4(+1, +1, 0, 1),
			new Vector4(+1, +1, 1, 1)
		];
		
		private var _worldPosition	: Vector4;
		private var _projection		: Matrix4x4;
		private var _shadowMapSize	: uint;
		
		public function get diffuse() : Number
		{
			return getProperty('diffuse') as Number;
		}
		
		public function get specular() : Number
		{
			return getProperty('specular') as Number;
		}
		
		public function get shininess() : Number
		{
			return getProperty('shininess') as Number;
		}
		
		public function get attenuationDistance() : Number
		{
			return getProperty('attenuationDistance') as Number;
		}
		
		public function get shadowMapSize() : uint
		{
			return _shadowMapSize;
		}
		
		public function get shadowMapZNear() : Number
		{
			return getProperty('zNear');
		}
		
		public function get shadowMapZFar() : Number
		{
			return getProperty('zFar');
		}
		
		public function set diffuse(v : Number)	: void
		{
			setProperty('diffuse', v);
			
			if (getProperty('diffuseEnabled') != (v != 0))
				setProperty('diffuseEnabled', v != 0);
		}
		
		public function set specular(v : Number) : void
		{
			setProperty('specular', v);
			
			if (getProperty('specularEnabled') != (v != 0))
				setProperty('specularEnabled', v != 0);
		}
		
		public function set shininess(v : Number) : void
		{
			setProperty('shininess', v);
		}
		
		public function set attenuationDistance(v : Number) : void
		{
			setProperty('attenuationDistance', v);
			
			if (getProperty('attenuationEnabled') != (v != 0))
				setProperty('attenuationEnabled', v != 0);
		}
		
		public function set shadowMapSize(v : uint) : void
		{
			_shadowMapSize = v;
			
			this.shadowCastingType = this.shadowCastingType
		}
		
		public function set shadowMapZNear(v : Number) : void
		{
			setProperty('zNear', v);
			updateProjectionMatrix();
		}
		
		public function set shadowMapZFar(v : Number) : void
		{
			setProperty('zFar', v);
			updateProjectionMatrix();
		}
		
		override public function set shadowCastingType(v : uint) : void
		{
			var shadowMap : ITextureResource;
			
			// start by clearing current shadow maps.
			for each (var mapName : String in MAP_NAMES)
			{
				shadowMap = getProperty(mapName) as ITextureResource;
				if (shadowMap !== null)
				{
					shadowMap.dispose();
					removeProperty(mapName);
				}
			}
			
			switch (v)
			{
				case ShadowMappingType.NONE:
					setProperty('shadowCastingType', ShadowMappingType.NONE);
					break;
				
				case ShadowMappingType.DUAL_PARABOLOID:
					if (!((_shadowMapSize & (~_shadowMapSize + 1)) == _shadowMapSize
						&& _shadowMapSize <= 2048))
						throw new Error(_shadowMapSize + ' is an invalid size for dual paraboloid shadow maps');
					
					// set textures and shadowmaptype
					shadowMap = new TextureResource(_shadowMapSize, _shadowMapSize);
					setProperty('shadowMapDPFront', shadowMap);
					shadowMap = new TextureResource(_shadowMapSize, _shadowMapSize);
					setProperty('shadowMapDPBack', shadowMap);
					setProperty('shadowCastingType', ShadowMappingType.DUAL_PARABOLOID);
					break;
				
				case ShadowMappingType.CUBE:
					if (!((_shadowMapSize & (~_shadowMapSize + 1)) == _shadowMapSize
						&& _shadowMapSize <= 1024))
						throw new Error(_shadowMapSize + ' is an invalid size for cubic shadow maps');
					
					shadowMap = new CubeTextureResource(_shadowMapSize);
					setProperty('shadowMapCube', shadowMap);
					setProperty('shadowCastingType', ShadowMappingType.CUBE);
					break;
				
				default: 
					throw new ArgumentError('Invalid shadow casting type.');
			}
		}
		
		public function PointLight(color				: uint		= 0xFFFFFFFF,
								   diffuse				: Number	= .6,
								   specular				: Number	= .8,
								   shininess			: Number	= 64,
								   attenuationDistance	: Number	= 0,
								   emissionMask			: uint		= 0x1,
								   shadowCastingType	: uint		= 0,
								   shadowMapSize		: uint		= 512,
								   shadowMapZNear		: Number	= 0.1,
								   shadowMapZFar		: Number	= 1000)
		{
			_worldPosition				= new Vector4();
			_projection					= new Matrix4x4();				
			_shadowMapSize				= shadowMapSize;
			
			super(color, emissionMask, shadowCastingType, TYPE); 
			
			this.diffuse				= diffuse;
			this.specular				= specular;
			this.shininess				= shininess;
			this.attenuationDistance	= attenuationDistance;
			this.shadowMapZNear			= shadowMapZNear;
			this.shadowMapZFar			= shadowMapZFar;
			
			setProperty('worldPosition', _worldPosition);
			setProperty('projection', _projection);
			
			if ([ShadowMappingType.NONE,
				 ShadowMappingType.DUAL_PARABOLOID,
				 ShadowMappingType.CUBE].indexOf(shadowCastingType) == -1)
				throw new Error('Invalid ShadowMappingType.');
			
			localToWorld.changed.add(transformChangedHandler);
		}
		
		protected function transformChangedHandler(transform : Matrix4x4) : void
		{
			localToWorld.getTranslation(_worldPosition);
		}
		
		private function updateProjectionMatrix() : void
		{
			var zNear	: Number = this.shadowMapZNear;
			var zFar	: Number = this.shadowMapZFar;
			var fd		: Number = 1. / Math.tan(Math.PI / 4);
			var m33		: Number = 1. / (zFar - zNear);
			var m43		: Number = -zNear / (zFar - zNear);
			
			_projection.initialize(fd, 0, 0, 0, 0, fd, 0, 0, 0, 0, m33, 1, 0, 0, m43, 0);
		}
		
		override minko_scene function cloneNode() : AbstractSceneNode
		{
			var light : PointLight = new PointLight(
				color, diffuse, specular, shininess, 
				attenuationDistance, emissionMask, 
				shadowCastingType, shadowMapSize
			);		
			
			light.name = this.name;
			light.transform.copyFrom(this.transform);
			
			return light;
		}
	}
}
