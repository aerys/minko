Using DevIL from Visual Basic 6.0
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


The idea:

1) Start by installing the DLLs.

2) In your VB-project, add the .BAS files that you need as modules.

3) Just use the subs/functions and constants.



The buts:

1) DevIL's data types have been converted to native VB-types.  This should generally
   work alright, except that

   a) VB only knows unsigned bytes and signed integers/longs.  So sometimes a value is
      defined as signed in VB where it is actually unsigned, and vice-versa.

   b) DevIL's ILboolean type has been converted to byte.  Don't treat these as VB
      booleans; only use them to compare to IL_TRUE and IL_FALSE.

2) Subs/functions that take a pointer to an array as parameter have been declared to
   take a ByRef Element As ElementType.  You should define an array of ElementType (not
   array of variants!), and pass the first element to the function.  The function will
   then access the whole array.

   Example:  Dim Names(2) As Long
             ilGenImages 3, Names(0)    ' Fills all 3 elements of Names

   (If you only use a single element, you do not really need an array.  You can just
   pass a variable of the element type.)

   The following subs/functions uses arrays of byte (binary data):

	ilCopyPixels	(Outputs data.  Make sure your array is big enough!)
	ilIsValidL
	ilLoadL
	ilRegisterPal
	ilSetPixels
	ilTexImage

   ilTexImage is a special case, because you might want to pass it a null-pointer
   instead of actual data.  This cannot be done with the same VB declaration, so I've
   made ilTexImage0 for this purpose.  Pass ilTexImage0 a zero as the last parameter.

   Other functions that operate on arrays:

	ilDeleteImages		array of long (image "names")
	ilGenImages		array of long (image "names")

3) Functions that return pointers have been converted to return a long.  But they are
   effectively useless in VB.  These are:

	ilGetData
	ilGetPalette
	ilGetString
	iluErrorString
	iluGetString
	ilutGetString
	ilutGetPaddedData

4) ilutGetBmpInfo and all ilutD3D8-functions are not available.

5) You will generally not be able to implement the call-back functions in VB, because
   they must be able to handle pointers.  But the call-back functions are only used if
   you want to modify the way DevIL loads and saves images, and usually you won't need
   to do that.

6) The old VB-examples supplied by Timo Heister don't seem to work. I've yet to write
   some new ones.

7) The VB-headers have only seen minimal testing.  Bugs may lure on the unwary.
   (I'm especially nervous about iluGetImageInfo).

8) The header-files are created specificly for VB 6.0.  I very much doubt that they will
   be of any use in VB.net.


Feel free to send me any comments.


Rune Kock (rune@vupti.com)