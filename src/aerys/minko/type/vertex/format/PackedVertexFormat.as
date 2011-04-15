package aerys.minko.type.vertex.format
{
	public class PackedVertexFormat extends VertexFormat
	{
		public static const XYZ			: PackedVertexFormat 	= new PackedVertexFormat(VertexComponent.XYZ);
		public static const XYZ_RGB		: PackedVertexFormat 	= new PackedVertexFormat(VertexComponent.XYZ,
																						   VertexComponent.RGB);
		public static const XYZ_UV		: PackedVertexFormat 	= new PackedVertexFormat(VertexComponent.XYZ,
																							VertexComponent.UV);
		public static const XYZ_UV_ST	: PackedVertexFormat	= new PackedVertexFormat(VertexComponent.XYZ,
																						   VertexComponent.UV,
																						   VertexComponent.ST);
		
		private var _dwordsPerVertex	: int		= 0;
		private var _componentOffsets	: Object	= new Object();
		private var _fieldOffsets		: Object	= new Object();
		
		public function get dwordsPerVertex()	: int { return _dwordsPerVertex; }
		
		public function PackedVertexFormat(...components)
		{
			super(components);

			initialize(components);
		}
		
		private function initialize(components : Array) : void
		{
			for each (var component : VertexComponent in components)
				addComponent(component);
		}
		
		public function getOffsetForComponent(component:VertexComponent) : int
		{
			return _componentOffsets[component.implodedFields];
		}
		
		public function getOffsetForField(fieldName:String) : int
		{ 
			return _fieldOffsets[fieldName];
		}
		
		public function addComponent(component : VertexComponent) : void
		{
			_componentOffsets[component.implodedFields] = _dwordsPerVertex;
			
			for (var fieldName : String in component.offsets)
			{
				_fieldOffsets[fieldName] = _dwordsPerVertex
										   + component.offsets[fieldName];
			}
			
			_dwordsPerVertex += component.dwords;
		}
	}
}

