package aerys.minko.scene.node.light
{
	import aerys.minko.ns.minko_scene;
	import aerys.minko.render.resource.texture.TextureResource;
	import aerys.minko.scene.node.AbstractSceneNode;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.type.binding.DataBindings;
	import aerys.minko.type.enum.ShadowMappingType;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;

	use namespace minko_scene;
	
	/**
	 * @author Romain Gilliotte
	 */
	public class DirectionalLight extends AbstractLight
	{
		public static const TYPE			: uint				= 1;
		
		private static const SCREEN_TO_UV	: Matrix4x4			= new Matrix4x4(
			0.5,		0.0,		0.0,	0.0,
			0.0, 		-0.5,		0.0,	0.0,
			0.0,		0.0,		1.0,	0.0,
			0.5, 		0.5,		0.0, 	1.0
		);
		
		private static const Z_AXIS			: Vector4			= new Vector4(0, 0, 1);
		private static const TMP_VECTOR		: Vector4			= new Vector4();
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
		private var _worldDirection	: Vector4;
		private var _worldToScreen	: Matrix4x4;
		private var _worldToUV		: Matrix4x4;
		private var _projection		: Matrix4x4;
		private var _shadowMapWidth	: Number;
		
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
		
		public function get shadowMapSize() : uint
		{
			return getProperty('shadowMapSize');
		}
		
		public function get shadowMapWidth() : Number
		{
			return _shadowMapWidth;
		}
		
		public function get shadowMapMaxZ() : Number
		{
			return getProperty('zFar');
		}
		
		public function get shadowMapQuality() : uint
		{
			return getProperty('shadowMapQuality');
		}
		
		public function get shadowMapSamplingDistance() : uint
		{
			return getProperty('shadowMapSamplingDistance');
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
		
		public function set shadowMapSize(v : uint) : void
		{
			setProperty('shadowMapSize', v);
			
			this.shadowCastingType = this.shadowCastingType;
		}
		
		public function set shadowMapWidth(v : Number) : void
		{
			_shadowMapWidth = v;
			updateProjectionMatrix();
		}
		
		public function set shadowMapMaxZ(v : Number) : void
		{
			setProperty('zFar', v);
			updateProjectionMatrix();
		}
		
		override public function set shadowCastingType(v : uint) : void
		{
			var shadowMap		: TextureResource	= getProperty('shadowMap') as TextureResource;
			var shadowMapSize	: uint				= this.shadowMapSize;
			
			if (shadowMap)
			{
				removeProperty('shadowMap');
				shadowMap.dispose(); 
			}
			
			switch (v)
			{
				case ShadowMappingType.NONE:
					setProperty('shadowCastingType', ShadowMappingType.NONE);
					break;
				
				case ShadowMappingType.MATRIX:
					if (!((shadowMapSize & (~shadowMapSize + 1)) == shadowMapSize
						&& shadowMapSize <= 2048))
						throw new Error(shadowMapSize + ' is an invalid size for a shadow map');
					
					shadowMap = new TextureResource(shadowMapSize, shadowMapSize);
					setProperty('shadowMap', shadowMap);
					setProperty('shadowCastingType', ShadowMappingType.MATRIX);
					break;
				
				default: 
					throw new ArgumentError('Invalid shadow casting type.');
			}
		}
		
		public function set shadowMapQuality(v : uint) : void
		{
			setProperty('shadowMapQuality', v);
		}
		
		public function set shadowMapSamplingDistance(v : uint) : void
		{
			setProperty('shadowMapSamplingDistance', v);
		}
		
		public function DirectionalLight(color						: uint		= 0xFFFFFFFF,
									 	 diffuse					: Number	= .6,
										 specular					: Number	= .8,
										 shininess					: Number	= 64,
										 emissionMask				: uint		= 0x1,
										 shadowCasting				: uint		= 0,
										 shadowMapSize				: uint		= 512,
										 shadowMapMaxZ				: Number	= 1000,
										 shadowMapWidth				: Number	= 20,
										 shadowMapQuality			: uint		= 0,
										 shadowMapSamplingDistance	: uint		= 1)
		{
			_worldPosition		= new Vector4();
			_worldDirection		= new Vector4();
			_worldToScreen		= new Matrix4x4();
			_worldToUV			= new Matrix4x4();
			_projection			= new Matrix4x4();
			
			super(color, emissionMask, shadowCasting, TYPE);
			
			this.diffuse					= diffuse;
			this.specular					= specular;
			this.shininess					= shininess;
			this.shadowMapMaxZ				= shadowMapMaxZ;
			this.shadowMapWidth				= shadowMapWidth;
			this.shadowMapSize				= shadowMapSize;
			this.shadowMapQuality			= shadowMapQuality;
			this.shadowMapSamplingDistance	= shadowMapSamplingDistance;
			
			setProperty('worldPosition', _worldPosition);
			setProperty('worldDirection', _worldDirection);
			setProperty('worldToScreen', _worldToScreen);
			setProperty('worldToUV', _worldToUV);
			setProperty('projection', _projection);
			
			if ([ShadowMappingType.NONE, ShadowMappingType.MATRIX].indexOf(shadowCasting) == -1)
				throw new Error('Invalid ShadowMappingType.');
			
			localToWorld.changed.add(transformChangedHandler);
			transform.lookAt(Vector4.ZERO, new Vector4(1, -1, 1));
		}
		
		protected function transformChangedHandler(transform : Matrix4x4) : void
		{
			// compute position
			localToWorld.getTranslation(_worldPosition);
			
			// compute direction
			_worldDirection	= localToWorld.deltaTransformVector(Z_AXIS, _worldDirection);
			_worldDirection.normalize();
			
			// update world to screen/uv
			_worldToScreen.lock().copyFrom(worldToLocal).append(_projection).unlock();
			_worldToUV.lock().copyFrom(_worldToScreen).append(SCREEN_TO_UV).unlock();
		}
		
		private function updateProjectionMatrix() : void
		{
			var zFar : Number = this.shadowMapMaxZ;
			
			_projection.initialize(
				2 / _shadowMapWidth, 0, 0, 0,
				0, 2 / _shadowMapWidth, 0, 0,
				0, 0, 2 / zFar, 0,
				0, 0, 0, 1
			);
			_worldToScreen.lock().copyFrom(worldToLocal).append(_projection).unlock();
			_worldToUV.lock().copyFrom(_worldToScreen).append(SCREEN_TO_UV).unlock();
		}
		
		override minko_scene function cloneNode() : AbstractSceneNode
		{
			var light : DirectionalLight = new DirectionalLight(
				color,
				diffuse,
				specular,
				shininess,
				emissionMask,
				shadowCastingType,
				shadowMapSize
			);
			
			light.name = this.name;
			light.transform.copyFrom(this.transform);
			
			return light;
		}
	}
}
