package aerys.minko.scene.controller.mesh
{
	import aerys.minko.render.Viewport;
	import aerys.minko.render.resource.texture.TextureResource;
	import aerys.minko.scene.controller.EnterFrameController;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.scene.node.mesh.Mesh;
	
	import flash.display.BitmapData;
	import flash.display.DisplayObject;
	import flash.geom.Matrix;
	
	public final class DynamicTextureController extends EnterFrameController
	{
		private var _source			: DisplayObject		= null;
		private var _framerate		: Number			= 0.;
		private var _mipMapping		: Boolean			= false;
		private var _propertyName	: String			= null;
		private var _matrix			: Matrix			= null;
		
		private var _bitmapData	: BitmapData			= null;
		private var _texture	: TextureResource		= new TextureResource();
		
		private var _lastDraw	: Number				= 0.;
		
		public function DynamicTextureController(source			: DisplayObject,
												 framerate		: Number	= 30.,
												 mipMapping		: Boolean	= false,
												 propertyName	: String	= "diffuseMap",
												 matrix			: Matrix	= null)
		{
			super(Mesh);
			
			_source = source;
			_framerate = framerate;
			_mipMapping = mipMapping;
			_propertyName = propertyName;
			_matrix = matrix;
		}
		
		override protected function targetAddedHandler(ctrl		: EnterFrameController,
													   target	: ISceneNode) : void
		{
			super.targetAddedHandler(ctrl, target);
			
			(target as Mesh).properties.setProperty(_propertyName, _texture);
		}
		
		override protected function targetRemovedHandler(ctrl	: EnterFrameController,
														 target	: ISceneNode) : void
		{
			super.targetRemovedHandler(ctrl, target);
			
			(target as Mesh).properties.removeProperty(_propertyName);
		}
		
		override protected function sceneEnterFrameHandler(scene	: Scene,
														   viewport	: Viewport,
														   target	: BitmapData,
														   time		: Number) : void
		{
			super.sceneEnterFrameHandler(scene, viewport, target, time);
			
			if (time - _lastDraw > 1000. / _framerate)
			{
				_lastDraw = time;
				
				_bitmapData ||= new BitmapData(_source.width, _source.height);
				_bitmapData.draw(_source, _matrix);
				
				_texture.setContentFromBitmapData(_bitmapData, _mipMapping);
			}
		}
	}
}