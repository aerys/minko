package aerys.minko.render.resource.texture
{
	import aerys.minko.render.resource.Context3DResource;
	import aerys.minko.render.resource.IResource;
	
	import flash.display.BitmapData;
	import flash.display3D.Context3D;
	import flash.display3D.Context3DTextureFormat;
	import flash.display3D.textures.CubeTexture;
	import flash.display3D.textures.TextureBase;
	import flash.geom.Matrix;
	import flash.utils.ByteArray;

	/**
	 * @inheritdoc
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public final class CubeTextureResource implements ITextureResource
	{
		private static const SIDE_X : Vector.<Number> = new <Number>[2, 0, 1, 1, 1, 3];
		private static const SIDE_Y : Vector.<Number> = new <Number>[1, 1, 0, 2, 1, 1];
		
		private var _bitmapDatas	: Vector.<BitmapData>;
		private var _resource		: CubeTexture;
		private var _size			: uint;
		
		public function CubeTextureResource(size : uint)
		{
			_size = size;
		}
		
		public function get size() : uint
		{
			return _size;
		}
		
		public function setContentFromBitmapData(bitmapData	: BitmapData,
												 mipmap		: Boolean,
												 downSample	: Boolean	= false) : void
		{
			_bitmapDatas = new <BitmapData>[];
			
			var width	: Number = bitmapData.width / 4;
			var height	: Number = bitmapData.height / 3;
			
			var tmpMatrix		: Matrix		= new Matrix(1, 0, 0, 1);
			for (var side : uint = 0; side < 6; ++side)
			{
				var sideBitmapData	: BitmapData	= new BitmapData(width, height, false, 0);
				tmpMatrix.tx	= - SIDE_X[side] * width;
				tmpMatrix.ty	= - SIDE_Y[side] * height;
				
				sideBitmapData.draw(bitmapData, tmpMatrix);
				_bitmapDatas.push(sideBitmapData);
			}
		}
		
		public function setContentFromBitmapDatas(right 	: BitmapData,
												  left		: BitmapData,
												  top		: BitmapData,
												  bottom	: BitmapData,
												  front		: BitmapData,
												  back		: BitmapData,
												  mipmap	: Boolean) : void
		{
			_bitmapDatas = new <BitmapData>[right, left, top, bottom, front, back];
		}
		
		public function setContentFromATF(atf : ByteArray) : void
		{
			throw new Error('Not yet implemented');
		}
		
		public function getNativeTexture(context : Context3DResource) : TextureBase
		{
			if (!_resource)
				_resource = context.createCubeTexture(_size, Context3DTextureFormat.BGRA, true);
			
			if (_bitmapDatas != null)
			{
				for (var side : uint = 0; side < 6; ++side)
				{
					var mipmapId	: uint			= 0;
					var mySize		: uint			= _size;
					var bitmapData	: BitmapData	= _bitmapDatas[side];
					
					while (mySize >= 1)
					{
						var tmpBitmapData	: BitmapData	= new BitmapData(mySize, mySize, false, 0x005500);
						var tmpMatrix		: Matrix		= new Matrix();
						
						tmpMatrix.a		= mySize / bitmapData.width;
						tmpMatrix.d		= mySize / bitmapData.height;
						
						tmpBitmapData.draw(bitmapData, tmpMatrix);
						_resource.uploadFromBitmapData(tmpBitmapData, side, mipmapId);
						
						++mipmapId;
						mySize /= 2;
					}
				}
				
				_bitmapDatas = null;
			}
			
			return _resource;
		}
		
		public function dispose() : void
		{
			_resource.dispose();
		}
	}
}
