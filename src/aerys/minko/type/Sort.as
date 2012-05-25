package aerys.minko.type
{
	public final class Sort
	{
		private static const TMP_INTS		: Vector.<int>		= new Vector.<int>(0xffff, true);

		public static function flashSort(values		: Vector.<Number>,
										 items		: Array,
										 numItems	: uint) : void
		{
			var n 		: int 		= numItems;
			var i		: int 		= 0;
			var j		: int 		= 0;
			var k		: int 		= 0;
			var t		: int		= 0;
			var anmin	: Number 	= values[0];
			var nmax	: int  		= 0;
			var p		: Number	= 0.;
			var sorted	: Boolean	= true;
			
			for (i = 0; i < n; ++i)
			{
				p = values[i];
				
				TMP_INTS[i] = 0;
				if (p < anmin)
					anmin = p;
				else if (p > Number(values[nmax]))
					nmax = i;
			}
			
			if (anmin == Number(values[nmax]))
				return ;
			
			var m		: int 	= Math.ceil(n * .125);
			var nmove	: int 	= 0;
			var c1		: Number = (m - 1) / (Number(values[nmax]) - anmin);
			
			for (i = 0; i < n; ++i)
			{
				k = int(c1 * (Number(values[i]) - anmin));
				TMP_INTS[k] = int(TMP_INTS[k]) + 1;
			}
			
			for (k = 1; k < m; ++k)
				TMP_INTS[k] = int(TMP_INTS[k]) + int(TMP_INTS[int(k - 1)]);
			
			var hold		: Number 	= Number(values[nmax]);
			var holdState 	: Object 	= items[nmax];
			
			values[nmax] = Number(values[0]);
			values[0] = hold;
			items[nmax] = items[0];
			items[0] = holdState;
			
			var flash		: Number	= 0.;
			var flashState	: Object	= null;
			
			j = 0;
			k = int(m - 1);
			i = int(n - 1);
			
			while (nmove < i)
			{
				while (j > int(TMP_INTS[k]) - 1)
				{
					++j;
					k = int(c1 * (Number(values[j]) - anmin));
				}
				
				flash = Number(values[j]);
				flashState = items[j];
				
				while (!(j == int(TMP_INTS[k])))
				{
					k = int(c1 * (flash - anmin));
					
					t = int(TMP_INTS[k]) - 1;
					hold = Number(values[t]);
					holdState = items[t];
					
					values[t] = flash;
					items[t] = flashState;
					
					flash = hold;
					flashState = holdState;
					
					TMP_INTS[k] = int(TMP_INTS[k]) - 1;
					++nmove;
				}
			}
			
			for (j = 1; j < n; ++j)
			{
				hold = Number(values[j]);
				holdState = items[j];
				
				i = int(j - 1);
				while (i >= 0 && Number(values[i]) > hold)
				{
					// not trivial
					values[int(i + 1)] = Number(values[i]);
					items[int(i + 1)] = items[i];
					
					--i;
				}
				
				values[int(i + 1)] = hold;
				items[int(i + 1)] = holdState;
			}
		}
	}
}