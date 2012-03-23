package aerys.minko.type.animation
{
	public final class TimeLabel
	{
		private var _name : String;
		private var _time : Number;

		public function set name(value : String) : void
		{
			_name = value;
		}
		
		public function set time(value : Number) : void
		{
			_time = value;
		}
		
		public function get name() : String
		{
			return _name;
		}

		public function get time() : Number
		{
			return _time;
		}

		public function TimeLabel(name : String, time : Number)
		{
			_name = name;
			_time = time;
		}

		public static function sort(label1 : TimeLabel, label2 : TimeLabel) : int
		{
			return label2._time - label1._time;
		}
	}
}