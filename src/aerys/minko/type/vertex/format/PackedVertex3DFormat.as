package aerys.minko.type.vertex.format
{
	public class PackedVertex3DFormat extends Vertex3DFormat
	{
		public static const XYZ			: PackedVertex3DFormat 	= new PackedVertex3DFormat(Vertex3DComponent.XYZ);
		public static const XYZ_RGB		: PackedVertex3DFormat 	= new PackedVertex3DFormat(Vertex3DComponent.XYZ,
																						   Vertex3DComponent.RGB);
		public static const XYZ_UV		: PackedVertex3DFormat 	= new PackedVertex3DFormat(Vertex3DComponent.XYZ,
																							Vertex3DComponent.UV);
		public static const XYZ_UV_ST	: PackedVertex3DFormat	= new PackedVertex3DFormat(Vertex3DComponent.XYZ,
																						   Vertex3DComponent.UV,
																						   Vertex3DComponent.ST);
		
		private var _dwordsPerVertex	: int;
		private var _componentOffsets	: Object;
		private var _fieldOffsets		: Object;
		
		public function get dwordsPerVertex()	: int { return _dwordsPerVertex; }
		
		public function PackedVertex3DFormat(...components)
		{
			super(components);
			
			_dwordsPerVertex	= 0;
			_componentOffsets	= new Object();
			_fieldOffsets		= new Object();
			
			for each (var component:Vertex3DComponent in components)
			{
				_componentOffsets[component.implodedFields] = _dwordsPerVertex;
				for (var fieldName:String in component.offsets)
					_fieldOffsets[fieldName] = _dwordsPerVertex + component.offsets[fieldName];
				
				_dwordsPerVertex += component.dwords;
			}
		}
		
		public function getOffsetForComponent(component:Vertex3DComponent) : int {
			return _componentOffsets[component.implodedFields];
		}
		
		public function getOffsetForField(fieldName:String) : int { 
			return _fieldOffsets[fieldName];
		}
		
	}
}

