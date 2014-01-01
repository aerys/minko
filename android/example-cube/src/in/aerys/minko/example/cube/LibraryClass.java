package in.aerys.minko.example.cube;

public class LibraryClass 
{
	static
	{
		System.loadLibrary("minko");
		System.loadLibrary("minko_png");
		System.loadLibrary("cube");
	}
	
	private long ptr;
	private boolean alloced;
	
	private int framen = 0;
	
	public LibraryClass()
	{
		System.out.println("Alloc");
		ptr = alloc();
		alloced = true;
	}
	
	@Override
	public void finalize()
	{
		System.out.println("Delete");
		if (alloced)
			delete(ptr);
	}
	
	public void DrawFrame()
	{
		//System.out.println("Frame: " + Integer.toString(framen++));
		drawframe(ptr);
	}
	
	private native long alloc();
	private native void delete(long ptr);
	private native void initialize(long ptr);
	private native void drawframe(long ptr);
	private native void resize(long ptr, int width, int height);

	public void initialize()
	{
		System.out.println("Init");
		initialize(ptr);
	}
	
	public void resize(int width, int height)
	{
		resize(ptr, width, height);
	}
	
}
