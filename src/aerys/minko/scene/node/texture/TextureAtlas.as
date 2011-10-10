package aerys.minko.scene.node.texture
{
	import aerys.minko.render.effect.basic.BasicStyle;
	import aerys.minko.render.resource.Texture3DResource;
	import aerys.minko.scene.action.texture.TextureAction;
	import aerys.minko.scene.node.AbstractScene;

	import flash.display.BitmapData;
	import flash.geom.Rectangle;

	public class TextureAtlas extends AbstractScene implements ITexture
	{
		private var _version		: uint				= 0;
		private var _bitmapData		: BitmapData		= null;
		private var _size			: uint				= 0;
		private var _nodes			: Array				= new Array();
		private var _empty			: Array				= new Array();
		private var _resource		: Texture3DResource	= new Texture3DResource();
		private var _styleProperty	: int				= 0;

		public function get styleProperty() : int				{ return _styleProperty; }
		public function get resource() 	: Texture3DResource	{ return _resource; }
		public function get version() 		: uint				{ return _version; }
		public function get size() 			: uint				{ return _bitmapData.width; }

		public function set styleProperty(value : int) : void
		{
			_styleProperty = value;
		}

		public function TextureAtlas(size 				: uint 		= 2048,
									 transparent 		: Boolean 	= true,
									 backgroundColor 	: uint 		= 0x000000)
		{
			_size = size;
			_nodes[0] = new Rectangle(0, 0, _size, _size);
			_styleProperty = BasicStyle.DIFFUSE;
			_bitmapData = new BitmapData(_size, _size, transparent, backgroundColor);

			actions.push(new TextureAction());
		}

		public function addBitmapData(bitmapData : BitmapData) : Rectangle
		{
			var rectangle : Rectangle	= getRectangle(bitmapData.width, bitmapData.height);

			_bitmapData.copyPixels(bitmapData, bitmapData.rect, rectangle.topLeft);
			_resource.setContentFromBitmapData(_bitmapData, true);

			return rectangle.clone();
		}

		private function getRectangle(width : uint, height : uint, rootId : int = 0) : Rectangle
		{
			var node 	: Rectangle = _nodes[rootId];
			var first	: Rectangle	= _nodes[int(rootId * 2 + 1)];
			var second	: Rectangle	= _nodes[int(rootId * 2 + 2)];
			if (!first && !second)
			{
				if (_empty[rootId] === false || width > node.width || height > node.height)
					return null;

				if (width == node.width && height == node.height)
				{
					_empty[rootId] = false;

					return node;
				}
				else
				{
					var dw : uint = node.width - width;
					var dh : uint = node.height - height;

					if (dw > dh)
					{
						first = new Rectangle(node.left, node.top,
											  width, node.height);
						second = new Rectangle(node.left + width, node.top,
											   node.width - width, node.height);
					}
					else
					{
						first = new Rectangle(node.left, node.top,
											  node.width, height);
						second = new Rectangle(node.left, node.top + height,
											   node.width, node.height - height);
					}

					_nodes[int(rootId * 2 + 1)] = first;
					_nodes[int(rootId * 2 + 2)] = second;

					return getRectangle(width, height, rootId * 2 + 1);
				}
			}

			if ((node = getRectangle(width, height, rootId * 2 + 1)))
				return node;

			return getRectangle(width, height, rootId * 2 + 2);
		}
	}
}