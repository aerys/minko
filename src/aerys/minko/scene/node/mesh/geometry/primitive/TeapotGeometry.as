package aerys.minko.scene.node.mesh.geometry.primitive
{
	import aerys.minko.scene.node.mesh.geometry.Geometry;
	import aerys.minko.scene.node.mesh.geometry.GeometrySanitizer;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.IndexStream;
	import aerys.minko.type.stream.VertexStream;
	import aerys.minko.type.stream.format.VertexFormat;
	
	import flash.geom.Vector3D;
	
	/**
	 * Utah Teapot
	 * Generated from original Newell bezier patches.
	 * 
	 * Note: The tesselation of the bezier patches is (very) far from being optimized.
	 * 
	 * @author Romain Gilliotte
	 * @see http://en.wikipedia.org/wiki/Utah_teapot Wikipedia article about Utah Teapot
	 * @see http://www.gamasutra.com/view/feature/3387/an_indepth_look_at_bicubic_bezier_.php?print=1 How to (properly) tesselate bezier patches.
	 */	
	public class TeapotGeometry extends Geometry
	{
		private static const PATCHES : Vector.<Vector.<uint>> = new <Vector.<uint>>[
			new <uint>[  1,  2,    3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,  16], 
			new <uint>[  4,  17,  18,  19,   8,  20,  21,  22,  12,  23,  24,  25,  16,  26,  27,  28], 
			new <uint>[ 19,  29,  30,  31,  22,  32,  33,  34,  25,  35,  36,  37,  28,  38,  39,  40], 
			new <uint>[ 31,  41,  42,   1,  34,  43,  44,   5,  37,  45,  46,   9,  40,  47,  48,  13], 
			new <uint>[ 13,  14,  15,  16,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60], 
			new <uint>[ 16,  26,  27,  28,  52,  61,  62,  63,  56,  64,  65,  66,  60,  67,  68,  69], 
			new <uint>[ 28,  38,  39,  40,  63,  70,  71,  72,  66,  73,  74,  75,  69,  76,  77,  78], 
			new <uint>[ 40,  47,  48,  13,  72,  79,  80,  49,  75,  81,  82,  53,  78,  83,  84,  57], 
			new <uint>[ 57,  58,  59,  60,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,  96], 
			new <uint>[ 60,  67,  68,  69,  88,  97,  98,  99,  92, 100, 101, 102,  96, 103, 104, 105], 
			new <uint>[ 69,  76,  77,  78,  99, 106, 107, 108, 102, 109, 110, 111, 105, 112, 113, 114], 
			new <uint>[ 78,  83,  84,  57, 108, 115, 116,  85, 111, 117, 118,  89, 114, 119, 120,  93], 
			new <uint>[121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136], 
			new <uint>[124, 137, 138, 121, 128, 139, 140, 125, 132, 141, 142, 129, 136, 143, 144, 133], 
			new <uint>[133, 134, 135, 136, 145, 146, 147, 148, 149, 150, 151, 152,  69, 153, 154, 155], 
			new <uint>[136, 143, 144, 133, 148, 156, 157, 145, 152, 158, 159, 149, 155, 160, 161,  69], 
			new <uint>[162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177], 
			new <uint>[165, 178, 179, 162, 169, 180, 181, 166, 173, 182, 183, 170, 177, 184, 185, 174], 
			new <uint>[174, 175, 176, 177, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197], 
			new <uint>[177, 184, 185, 174, 189, 198, 199, 186, 193, 200, 201, 190, 197, 202, 203, 194], 
			new <uint>[204, 204, 204, 204, 207, 208, 209, 210, 211, 211, 211, 211, 212, 213, 214, 215], 
			new <uint>[204, 204, 204, 204, 210, 217, 218, 219, 211, 211, 211, 211, 215, 220, 221, 222], 
			new <uint>[204, 204, 204, 204, 219, 224, 225, 226, 211, 211, 211, 211, 222, 227, 228, 229], 
			new <uint>[204, 204, 204, 204, 226, 230, 231, 207, 211, 211, 211, 211, 229, 232, 233, 212], 
			new <uint>[212, 213, 214, 215, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245], 
			new <uint>[215, 220, 221, 222, 237, 246, 247, 248, 241, 249, 250, 251, 245, 252, 253, 254], 
			new <uint>[222, 227, 228, 229, 248, 255, 256, 257, 251, 258, 259, 260, 254, 261, 262, 263], 
			new <uint>[229, 232, 233, 212, 257, 264, 265, 234, 260, 266, 267, 238, 263, 268, 269, 242], 
			new <uint>[270, 270, 270, 270, 279, 280, 281, 282, 275, 276, 277, 278, 271, 272, 273, 274], 
			new <uint>[270, 270, 270, 270, 282, 289, 290, 291, 278, 286, 287, 288, 274, 283, 284, 285], 
			new <uint>[270, 270, 270, 270, 291, 298, 299, 300, 288, 295, 296, 297, 285, 292, 293, 294], 
			new <uint>[270, 270, 270, 270, 300, 305, 306, 279, 297, 303, 304, 275, 294, 301, 302, 271]
		];
		
		private static const POINTS : Vector.<Vector3D> = new <Vector3D>[
			new Vector3D(1.4, 0.0, 2.4),			new Vector3D(1.4, -0.784, 2.4),			new Vector3D(0.784, -1.4, 2.4), 
			new Vector3D(0.0, -1.4, 2.4),			new Vector3D(1.3375, 0.0, 2.53125),		new Vector3D(1.3375, -0.749, 2.53125), 
			new Vector3D(0.749, -1.3375, 2.53125),	new Vector3D(0.0, -1.3375, 2.53125),	new Vector3D(1.4375, 0.0, 2.53125), 
			new Vector3D(1.4375, -0.805, 2.53125),	new Vector3D(0.805, -1.4375, 2.53125),	new Vector3D(0.0, -1.4375, 2.53125), 
			new Vector3D(1.5, 0.0, 2.4),			new Vector3D(1.5, -0.84, 2.4),			new Vector3D(0.84, -1.5, 2.4), 
			new Vector3D(0.0, -1.5, 2.4),			new Vector3D(-0.784, -1.4, 2.4), 		new Vector3D(-1.4, -0.784, 2.4),
			new Vector3D(-1.4, 0.0, 2.4),			new Vector3D(-0.749, -1.3375, 2.53125),	new Vector3D(-1.3375, -0.749, 2.53125), 
			new Vector3D(-1.3375, 0.0, 2.53125), 	new Vector3D(-0.805, -1.4375, 2.53125),	new Vector3D(-1.4375, -0.805, 2.53125), 
			new Vector3D(-1.4375, 0.0, 2.53125), 	new Vector3D(-0.84, -1.5, 2.4),			new Vector3D(-1.5, -0.84, 2.4), 
			new Vector3D(-1.5, 0.0, 2.4), 			new Vector3D(-1.4, 0.784, 2.4),			new Vector3D(-0.784, 1.4, 2.4), 
			new Vector3D(0.0, 1.4, 2.4), 			new Vector3D(-1.3375, 0.749, 2.53125),	new Vector3D(-0.749, 1.3375, 2.53125), 
			new Vector3D(0.0, 1.3375, 2.53125), 	new Vector3D(-1.4375, 0.805, 2.53125),	new Vector3D(-0.805, 1.4375, 2.53125), 
			new Vector3D(0.0, 1.4375, 2.53125), 	new Vector3D(-1.5, 0.84, 2.4),			new Vector3D(-0.84, 1.5, 2.4), 
			new Vector3D(0.0, 1.5, 2.4), 			new Vector3D(0.784, 1.4, 2.4),			new Vector3D(1.4, 0.784, 2.4), 
			new Vector3D(0.749, 1.3375, 2.53125), 	new Vector3D(1.3375, 0.749, 2.53125),	new Vector3D(0.805, 1.4375, 2.53125), 
			new Vector3D(1.4375, 0.805, 2.53125), 	new Vector3D(0.84, 1.5, 2.4),			new Vector3D(1.5, 0.84, 2.4), 
			new Vector3D(1.75, 0.0, 1.875), 		new Vector3D(1.75, -0.98, 1.875),		new Vector3D(0.98, -1.75, 1.875), 
			new Vector3D(0.0, -1.75, 1.875), 		new Vector3D(2.0, 0.0, 1.35),			new Vector3D(2.0, -1.12, 1.35), 
			new Vector3D(1.12, -2.0, 1.35), 		new Vector3D(0.0, -2.0, 1.35),			new Vector3D(2.0, 0.0, 0.9), 
			new Vector3D(2.0, -1.12, 0.9), 			new Vector3D(1.12, -2.0, 0.9),			new Vector3D(0.0, -2.0, 0.9), 
			new Vector3D(-0.98, -1.75, 1.875), 		new Vector3D(-1.75, -0.98, 1.875),		new Vector3D(-1.75, 0.0, 1.875), 
			new Vector3D(-1.12, -2.0, 1.35), 		new Vector3D(-2.0, -1.12, 1.35),		new Vector3D(-2.0, 0.0, 1.35), 
			new Vector3D(-1.12, -2.0, 0.9), 		new Vector3D(-2.0, -1.12, 0.9),			new Vector3D(-2.0, 0.0, 0.9), 
			new Vector3D(-1.75, 0.98, 1.875), 		new Vector3D(-0.98, 1.75, 1.875),		new Vector3D(0.0, 1.75, 1.875), 
			new Vector3D(-2.0, 1.12, 1.35), 		new Vector3D(-1.12, 2.0, 1.35),			new Vector3D(0.0, 2.0, 1.35), 
			new Vector3D(-2.0, 1.12, 0.9), 			new Vector3D(-1.12, 2.0, 0.9),			new Vector3D(0.0, 2.0, 0.9), 
			new Vector3D(0.98, 1.75, 1.875), 		new Vector3D(1.75, 0.98, 1.875),		new Vector3D(1.12, 2.0, 1.35), 
			new Vector3D(2.0, 1.12, 1.35), 			new Vector3D(1.12, 2.0, 0.9),			new Vector3D(2.0, 1.12, 0.9), 
			new Vector3D(2.0, 0.0, 0.45), 			new Vector3D(2.0, -1.12, 0.45),			new Vector3D(1.12, -2.0, 0.45), 
			new Vector3D(0.0, -2.0, 0.45), 			new Vector3D(1.5, 0.0, 0.225),			new Vector3D(1.5, -0.84, 0.225), 
			new Vector3D(0.84, -1.5, 0.225),		new Vector3D(0.0, -1.5, 0.225),			new Vector3D(1.5, 0.0, 0.15), 
			new Vector3D(1.5, -0.84, 0.15), 		new Vector3D(0.84, -1.5, 0.15),			new Vector3D(0.0, -1.5, 0.15), 
			new Vector3D(-1.12, -2.0, 0.45), 		new Vector3D(-2.0, -1.12, 0.45),		new Vector3D(-2.0, 0.0, 0.45), 
			new Vector3D(-0.84, -1.5, 0.225), 		new Vector3D(-1.5, -0.84, 0.225),		new Vector3D(-1.5, 0.0, 0.225), 
			new Vector3D(-0.84, -1.5, 0.15), 		new Vector3D(-1.5, -0.84, 0.15),		new Vector3D(-1.5, 0.0, 0.15), 
			new Vector3D(-2.0, 1.12, 0.45), 		new Vector3D(-1.12, 2.0, 0.45),			new Vector3D(0.0, 2.0, 0.45), 
			new Vector3D(-1.5, 0.84, 0.225), 		new Vector3D(-0.84, 1.5, 0.225),		new Vector3D(0.0, 1.5, 0.225), 
			new Vector3D(-1.5, 0.84, 0.15), 		new Vector3D(-0.84, 1.5, 0.15),			new Vector3D(0.0, 1.5, 0.15), 
			new Vector3D(1.12, 2.0, 0.45), 			new Vector3D(2.0, 1.12, 0.45),			new Vector3D(0.84, 1.5, 0.225), 
			new Vector3D(1.5, 0.84, 0.225), 		new Vector3D(0.84, 1.5, 0.15),			new Vector3D(1.5, 0.84, 0.15), 
			new Vector3D(-1.6, 0.0, 2.025), 		new Vector3D(-1.6, -0.3, 2.025),		new Vector3D(-1.5, -0.3, 2.25), 
			new Vector3D(-1.5, 0.0, 2.25), 			new Vector3D(-2.3, 0.0, 2.025),			new Vector3D(-2.3, -0.3, 2.025), 
			new Vector3D(-2.5, -0.3, 2.25), 		new Vector3D(-2.5, 0.0, 2.25),			new Vector3D(-2.7, 0.0, 2.025), 
			new Vector3D(-2.7, -0.3, 2.025), 		new Vector3D(-3.0, -0.3, 2.25),			new Vector3D(-3.0, 0.0, 2.25), 
			new Vector3D(-2.7, 0.0, 1.8), 			new Vector3D(-2.7, -0.3, 1.8),			new Vector3D(-3.0, -0.3, 1.8), 
			new Vector3D(-3.0, 0.0, 1.8), 			new Vector3D(-1.5, 0.3, 2.25),			new Vector3D(-1.6, 0.3, 2.025), 
			new Vector3D(-2.5, 0.3, 2.25), 			new Vector3D(-2.3, 0.3, 2.025),			new Vector3D(-3.0, 0.3, 2.25), 
			new Vector3D(-2.7, 0.3, 2.025), 		new Vector3D(-3.0, 0.3, 1.8),			new Vector3D(-2.7, 0.3, 1.8), 
			new Vector3D(-2.7, 0.0, 1.575), 		new Vector3D(-2.7, -0.3, 1.575),		new Vector3D(-3.0, -0.3, 1.35), 
			new Vector3D(-3.0, 0.0, 1.35), 			new Vector3D(-2.5, 0.0, 1.125),			new Vector3D(-2.5, -0.3, 1.125), 
			new Vector3D(-2.65, -0.3, 0.9375),		new Vector3D(-2.65, 0.0, 0.9375),		new Vector3D(-2.0, -0.3, 0.9), 
			new Vector3D(-1.9, -0.3, 0.6), 			new Vector3D(-1.9, 0.0, 0.6),			new Vector3D(-3.0, 0.3, 1.35), 
			new Vector3D(-2.7, 0.3, 1.575), 		new Vector3D(-2.65, 0.3, 0.9375),		new Vector3D(-2.5, 0.3, 1.125), 
			new Vector3D(-1.9, 0.3, 0.6), 			new Vector3D(-2.0, 0.3, 0.9),			new Vector3D(1.7, 0.0, 1.425), 
			new Vector3D(1.7, -0.66, 1.425),		new Vector3D(1.7, -0.66, 0.6),			new Vector3D(1.7, 0.0, 0.6), 
			new Vector3D(2.6, 0.0, 1.425), 			new Vector3D(2.6, -0.66, 1.425),		new Vector3D(3.1, -0.66, 0.825),
			new Vector3D(3.1, 0.0, 0.825), 			new Vector3D(2.3, 0.0, 2.1),			new Vector3D(2.3, -0.25, 2.1), 
			new Vector3D(2.4, -0.25, 2.025), 		new Vector3D(2.4, 0.0, 2.025),			new Vector3D(2.7, 0.0, 2.4), 
			new Vector3D(2.7, -0.25, 2.4), 			new Vector3D(3.3, -0.25, 2.4),			new Vector3D(3.3, 0.0, 2.4), 
			new Vector3D(1.7, 0.66, 0.6), 			new Vector3D(1.7, 0.66, 1.425),			new Vector3D(3.1, 0.66, 0.825), 
			new Vector3D(2.6, 0.66, 1.425), 		new Vector3D(2.4, 0.25, 2.025),			new Vector3D(2.3, 0.25, 2.1), 
			new Vector3D(3.3, 0.25, 2.4), 			new Vector3D(2.7, 0.25, 2.4),			new Vector3D(2.8, 0.0, 2.475), 
			new Vector3D(2.8, -0.25, 2.475), 		new Vector3D(3.525, -0.25, 2.49375),	new Vector3D(3.525, 0.0, 2.49375), 
			new Vector3D(2.9, 0.0, 2.475), 			new Vector3D(2.9, -0.15, 2.475),		new Vector3D(3.45, -0.15, 2.5125), 
			new Vector3D(3.45, 0.0, 2.5125), 		new Vector3D(2.8, 0.0, 2.4),			new Vector3D(2.8, -0.15, 2.4), 
			new Vector3D(3.2, -0.15, 2.4), 			new Vector3D(3.2, 0.0, 2.4),			new Vector3D(3.525, 0.25, 2.49375), 
			new Vector3D(2.8, 0.25, 2.475), 		new Vector3D(3.45, 0.15, 2.5125),		new Vector3D(2.9, 0.15, 2.475), 
			new Vector3D(3.2, 0.15, 2.4), 			new Vector3D(2.8, 0.15, 2.4),			new Vector3D(0.0, 0.0, 3.15), 
			new Vector3D(0.0, -0.002, 3.15), 		new Vector3D(0.002, 0.0, 3.15),			new Vector3D(0.8, 0.0, 3.15), 
			new Vector3D(0.8, -0.45, 3.15), 		new Vector3D(0.45, -0.8, 3.15),			new Vector3D(0.0, -0.8, 3.15), 
			new Vector3D(0.0, 0.0, 2.85), 			new Vector3D(0.2, 0.0, 2.7),			new Vector3D(0.2, -0.112, 2.7), 
			new Vector3D(0.112, -0.2, 2.7), 		new Vector3D(0.0, -0.2, 2.7),			new Vector3D(-0.002, 0.0, 3.15), 
			new Vector3D(-0.45, -0.8, 3.15), 		new Vector3D(-0.8, -0.45, 3.15),		new Vector3D(-0.8, 0.0, 3.15), 
			new Vector3D(-0.112, -0.2, 2.7), 		new Vector3D(-0.2, -0.112, 2.7),		new Vector3D(-0.2, 0.0, 2.7), 
			new Vector3D(0.0, 0.002, 3.15), 		new Vector3D(-0.8, 0.45, 3.15),			new Vector3D(-0.45, 0.8, 3.15), 
			new Vector3D(0.0, 0.8, 3.15), 			new Vector3D(-0.2, 0.112, 2.7),			new Vector3D(-0.112, 0.2, 2.7), 
			new Vector3D(0.0, 0.2, 2.7), 			new Vector3D(0.45, 0.8, 3.15),			new Vector3D(0.8, 0.45, 3.15), 
			new Vector3D(0.112, 0.2, 2.7), 			new Vector3D(0.2, 0.112, 2.7),			new Vector3D(0.4, 0.0, 2.55), 
			new Vector3D(0.4, -0.224, 2.55),		new Vector3D(0.224, -0.4, 2.55),		new Vector3D(0.0, -0.4, 2.55), 
			new Vector3D(1.3, 0.0, 2.55), 			new Vector3D(1.3, -0.728, 2.55),		new Vector3D(0.728, -1.3, 2.55), 
			new Vector3D(0.0, -1.3, 2.55), 			new Vector3D(1.3, 0.0, 2.4),			new Vector3D(1.3, -0.728, 2.4), 
			new Vector3D(0.728, -1.3, 2.4), 		new Vector3D(0.0, -1.3, 2.4),			new Vector3D(-0.224, -0.4, 2.55), 
			new Vector3D(-0.4, -0.224, 2.55), 		new Vector3D(-0.4, 0.0, 2.55),			new Vector3D(-0.728, -1.3, 2.55), 
			new Vector3D(-1.3, -0.728, 2.55), 		new Vector3D(-1.3, 0.0, 2.55),			new Vector3D(-0.728, -1.3, 2.4), 
			new Vector3D(-1.3, -0.728, 2.4), 		new Vector3D(-1.3, 0.0, 2.4),			new Vector3D(-0.4, 0.224, 2.55), 
			new Vector3D(-0.224, 0.4, 2.55), 		new Vector3D(0.0, 0.4, 2.55),			new Vector3D(-1.3, 0.728, 2.55), 
			new Vector3D(-0.728, 1.3, 2.55), 		new Vector3D(0.0, 1.3, 2.55),			new Vector3D(-1.3, 0.728, 2.4), 
			new Vector3D(-0.728, 1.3, 2.4), 		new Vector3D(0.0, 1.3, 2.4),			new Vector3D(0.224, 0.4, 2.55), 
			new Vector3D(0.4, 0.224, 2.55), 		new Vector3D(0.728, 1.3, 2.55),			new Vector3D(1.3, 0.728, 2.55),
			new Vector3D(0.728, 1.3, 2.4),			new Vector3D(1.3, 0.728, 2.4),			new Vector3D(0.0, 0.0, 0.0), 
			new Vector3D(1.5, 0.0, 0.15), 			new Vector3D(1.5, 0.84, 0.15),			new Vector3D(0.84, 1.5, 0.15), 
			new Vector3D(0.0, 1.5, 0.15), 			new Vector3D(1.5, 0.0, 0.075),			new Vector3D(1.5, 0.84, 0.075), 
			new Vector3D(0.84, 1.5, 0.075), 		new Vector3D(0.0, 1.5, 0.075),			new Vector3D(1.425, 0.0, 0.0), 
			new Vector3D(1.425, 0.798, 0.0), 		new Vector3D(0.798, 1.425, 0.0),		new Vector3D(0.0, 1.425, 0.0), 
			new Vector3D(-0.84, 1.5, 0.15), 		new Vector3D(-1.5, 0.84, 0.15),			new Vector3D(-1.5, 0.0, 0.15), 
			new Vector3D(-0.84, 1.5, 0.075), 		new Vector3D(-1.5, 0.84, 0.075),		new Vector3D(-1.5, 0.0, 0.075), 
			new Vector3D(-0.798, 1.425, 0.0), 		new Vector3D(-1.425, 0.798, 0.0),		new Vector3D(-1.425, 0.0, 0.0), 
			new Vector3D(-1.5, -0.84, 0.15), 		new Vector3D(-0.84, -1.5, 0.15),		new Vector3D(0.0, -1.5, 0.15), 
			new Vector3D(-1.5, -0.84, 0.075), 		new Vector3D(-0.84, -1.5, 0.075),		new Vector3D(0.0, -1.5, 0.075), 
			new Vector3D(-1.425, -0.798, 0.0), 		new Vector3D(-0.798, -1.425, 0.0),		new Vector3D(0.0, -1.425, 0.0), 
			new Vector3D(0.84, -1.5, 0.15), 		new Vector3D(1.5, -0.84, 0.15),			new Vector3D(0.84, -1.5, 0.075), 
			new Vector3D(1.5, -0.84, 0.075), 		new Vector3D(0.798, -1.425, 0.0),		new Vector3D(1.425, -0.798, 0.0)
		];
		
		public function TeapotGeometry(divs 		: uint 	= 10,
									   streamsUsage : uint 	= 0)
		{
			var indexData	: Vector.<uint>		= new Vector.<uint>();
			var vertexData	: Vector.<Number>	= new Vector.<Number>();
			
			var patch			: Vector.<Vector3D>	= new Vector.<Vector3D>(16, true);
			var currentVertexId	: uint				= 0;
			
			for each (var bezierPatch : Vector.<uint> in PATCHES)
			{
				for (var i : uint = 0; i < 16; ++i)
					patch[i] = POINTS[bezierPatch[i] - 1];
				
				genPatchVertexData(patch, divs, vertexData);
				genPatchIndexData(currentVertexId, divs, indexData);
				
				currentVertexId += (divs + 1) * (divs + 1);
			}
			
			// this is slow and memory consuming and could be avoided by non duplicating all border
			// vertices on genPatchVertexData and genPatchIndexData...
			GeometrySanitizer.removeDuplicatedVertices(vertexData, indexData, 3);
			
			super(
				new <IVertexStream>[
					new VertexStream(streamsUsage, VertexFormat.XYZ, vertexData)
				],
				new IndexStream(streamsUsage, indexData)
			);
			
			if (divs < 1)
				throw new Error();
		}
		
		private function genPatchVertexData(patch		: Vector.<Vector3D>, 
								   			divs		: uint,
											vertexData	: Vector.<Number>) : void
		{
			// create 2 temporary vertex vectors and initialize them
			var last : Vector.<Vector3D> = new Vector.<Vector3D>(divs + 1, true);
			var temp : Vector.<Vector3D> = new Vector.<Vector3D>(4, true);
			
			for (v = 0; v <= divs; ++v)	last[v] = new Vector3D();
			for (v = 0; v < 4; ++v)		temp[v] = new Vector3D();
			
			for (var u : int = 0; u <= divs; ++u)
			{
				var py : Number = u / divs;
				
				bernstein(py, patch[0],		patch[1],	patch[2],	patch[3],	temp[0]);
				bernstein(py, patch[4],		patch[5],	patch[6],	patch[7],	temp[1]);
				bernstein(py, patch[8],		patch[9],	patch[10],	patch[11],	temp[2]);
				bernstein(py, patch[12],	patch[13],	patch[14],	patch[15],	temp[3]);
				
				for (var v : int = 0; v <= divs; ++v) 
				{
					var px : Number = v / divs;
					
					bernstein(px, temp[0], temp[1], temp[2], temp[3], last[v]);
					vertexData.push(last[v].x, last[v].z, last[v].y); // y up in minko, z up in the model
				}
			}
		}
		
		private function genPatchIndexData(indexOffset	: uint,
										   divs			: uint,
										   indexData	: Vector.<uint>) : void
		{
			for (var u : uint = 0; u < divs; ++u)
				for (var v : uint = 0; v < divs; ++v)
					indexData.push(
						indexOffset + u * (divs + 1) + v,
						indexOffset + (u + 1) * (divs + 1) + v,
						indexOffset + u * (divs + 1) + v + 1,
						
						indexOffset + u * (divs + 1) + v + 1,
						indexOffset + (u + 1) * (divs + 1) + v,
						indexOffset + (u + 1) * (divs + 1) + v + 1
					);
		}
		
		private function bernstein(u	: Number,
								   p0	: Vector3D,
								   p1	: Vector3D,
								   p2	: Vector3D,
								   p3	: Vector3D,
								   out	: Vector3D = null) : Vector3D 
		{
			out ||= new Vector3D();
			
			var um	: Number = 1 - u;
			var um2	: Number = um * um;
			var u2	: Number = u * u;
			
			var c0 : Number = u * u2;
			var c1 : Number = 3 * u2 * um;
			var c2 : Number = 3 * u * um2;
			var c3 : Number = um * um2;
			
			out.x = c0 * p0.x + c1 * p1.x + c2 * p2.x + c3 * p3.x;
			out.y = c0 * p0.y + c1 * p1.y + c2 * p2.y + c3 * p3.y;
			out.z = c0 * p0.z + c1 * p1.z + c2 * p2.z + c3 * p3.z;
			
			return out;
		}
		
	}
}
