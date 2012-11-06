package aerys.minko.type.enum
{
    public final class PickingTechnique
    {
        public static const RAYCASTING_BOX          : uint  = 1;
        public static const RAYCASTING_GEOMETRY     : uint  = 2;
        public static const RAYCASTING              : uint  = RAYCASTING_BOX | RAYCASTING_GEOMETRY;
        
        public static const PIXEL_PICKING           : uint  = 4;
    }
}