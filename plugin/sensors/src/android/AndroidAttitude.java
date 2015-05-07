package minko.plugin.sensors;

import android.app.Activity;
import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.opengl.Matrix;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import org.libsdl.app.*;

/**
 * Provides head tracking information from the device IMU. 
 */
public class AndroidAttitude
{
	private Activity _sdlActivity;
	private static final String TAG = "Attitude";
	private static final double NS2S = 1.E-09D;
	private static final int[] INPUT_SENSORS = { 1, 4 };
	private final Context mContext;
	private final float[] mTmpRotatedEvent = new float[3];
	private Looper mSensorLooper;
	private SensorEventListener mSensorEventListener;
	private volatile boolean mTracking;
	private long mLastGyroEventTimeNanos;

	// Native functions
	public native void minkoNativeOnAttitudeEvent(String message);

	public AndroidAttitude(Activity sdlActivity)
	{
		_sdlActivity = sdlActivity;
		mContext = SDLActivity.getContext();
	}

	public void startTracking()
	{
		Log.i("minko-java", "[AndroidAttitude] Start tracking");

		if (mTracking) {
			return;
		}

		mSensorEventListener = new SensorEventListener()
		{
			public void onSensorChanged(SensorEvent event) {
				AndroidAttitude.this.processSensorEvent(event);
			}

			public void onAccuracyChanged(Sensor sensor, int accuracy)
			{
			}
		};
		
		Thread sensorThread = new Thread(new Runnable()
		{
			public void run() {
				Looper.prepare();

				mSensorLooper = Looper.myLooper();
				Handler handler = new Handler();

				SensorManager sensorManager = (SensorManager)mContext.getSystemService("sensor");

				for (int sensorType : AndroidAttitude.INPUT_SENSORS) {
					Sensor sensor = sensorManager.getDefaultSensor(sensorType);
					sensorManager.registerListener(mSensorEventListener, sensor, 0, handler);
				}

				Looper.loop();
			}
		});

		sensorThread.start();
		mTracking = true;
	}

	public void stopTracking()
	{
		Log.i("minko-java", "[AndroidAttitude] Stop tracking");

		if (!mTracking) {
			return;
		}

		SensorManager sensorManager = (SensorManager)mContext.getSystemService("sensor");

		sensorManager.unregisterListener(mSensorEventListener);
		mSensorEventListener = null;

		mSensorLooper.quit();
		mSensorLooper = null;
		mTracking = false;
	}

	private void processSensorEvent(SensorEvent event)
	{
		Log.i("minko-java", "[AndroidAttitude] Process sensor event");

		long timeNanos = System.nanoTime();

		mTmpRotatedEvent[0] = (-event.values[1]);
		mTmpRotatedEvent[1] = event.values[0];
		mTmpRotatedEvent[2] = event.values[2];

		String eventData = (-event.values[1]) + " " + event.values[0] + " " + event.values[2] + " " + event.timestamp;
		
		// TYPE_ACCELEROMETER
		if (event.sensor.getType() == 1) 
		{
			eventData = "accelerometer " + eventData;
		} 
		// TYPE_GYROSCOPE
		else if (event.sensor.getType() == 4) 
		{
			mLastGyroEventTimeNanos = timeNanos;
			eventData = "gyroscope " + eventData;
		}

		minkoNativeOnAttitudeEvent(eventData);
	}
}