package aerys.minko.render.material.line
{
	import aerys.minko.render.Effect;
	import aerys.minko.render.material.Material;
	import aerys.minko.render.material.basic.BasicProperties;
	
	public class LineMaterial extends Material
	{
		public function get diffuseColor() : Object
		{
			return getProperty(BasicProperties.DIFFUSE_COLOR);
		}
		public function set diffuseColor(value : Object) : void
		{
			setProperty(BasicProperties.DIFFUSE_COLOR, value);
		}
		
		public function get thickness() : Number
		{
			return getProperty('lineThickness');
		}
		public function set thickness(value : Number) : void
		{
			setProperty('lineThickness', value);
		}
		
		public function LineMaterial(properties	: Object	= null,
									  name			: String	= null)
		{
			super(new Effect(new LineShader()), properties, name);
		}
	}
}