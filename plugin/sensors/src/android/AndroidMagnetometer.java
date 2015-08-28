package minko.plugin.sensors;

import android.app.Activity;
import android.content.Context;
import android.view.Display;
import android.view.WindowManager;
import android.view.Surface;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.opengl.Matrix;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import org.libsdl.app.*;
import java.util.ArrayList;

/**
 * Provides head tracking information from the device IMU. 
 */
public class AndroidMagnetometer
{
	private Activity _sdlActivity;
	private final Context _context;
	private Looper _sensorLooper;
	private SensorManager _sensorManager;
	private SensorEventListener _sensorEventListener;
	private volatile boolean _tracking;
	private boolean _isSupported;

	private Display _display;

	// Native functions
	public native void minkoNativeOnMagnetometerEvent(float[] magnetometer);

	public AndroidMagnetometer(Activity sdlActivity)
	{
		_sdlActivity = sdlActivity;
		_context = SDLActivity.getContext();
		_isSupported = true;
		_tracking = false;

		_display = ((WindowManager)_context.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
	}

	public void startTracking()
	{
		if (_tracking) {
			return;
		}

		_sensorEventListener = new SensorEventListener()
		{
			public void onSensorChanged(SensorEvent event) {
				AndroidMagnetometer.this.processSensorEvent(event);
			}

			public void onAccuracyChanged(Sensor sensor, int accuracy)
			{
			}
		};
		
		Thread sensorThread = new Thread(new Runnable()
		{
			public void run() {
				Looper.prepare();

				_sensorLooper = Looper.myLooper();
				Handler handler = new Handler();

				_sensorManager = (SensorManager)_context.getSystemService("sensor");

				Sensor sensor = _sensorManager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD);
				
				if (sensor == null)
				{
					Log.i("minko-java", "[AndroidMagnetometer] Warning: Magnetometer sensor not found on this device.");
					_isSupported = false;
				}

				_sensorManager.registerListener(_sensorEventListener, sensor, 0, handler);

				_tracking = true;

				Looper.loop();
			}
		});

		sensorThread.start();
	}

	public void stopTracking()
	{
		if (!_tracking) {
			return;
		}

		SensorManager _sensorManager = (SensorManager)_context.getSystemService("sensor");

		_sensorManager.unregisterListener(_sensorEventListener);
		_sensorEventListener = null;

		_sensorLooper.quit();
		_sensorLooper = null;
		_tracking = false;
	}

	private synchronized void processSensorEvent(SensorEvent event)
	{
		if (event.sensor.getType() == Sensor.TYPE_MAGNETIC_FIELD)
		{
			float[] values = event.values;

			if ((values[0] == 0.0F) && (values[1] == 0.0F) && (values[2] == 0.0F)) {
				return;
			}

			minkoNativeOnMagnetometerEvent(values);
		}
	}

	public boolean isSupported()
	{
		if (!_tracking)
		{
			SensorManager sensorManager = (SensorManager)_context.getSystemService("sensor");

			Sensor sensor = sensorManager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD);

			if (sensor == null)
			{
				_isSupported = false;
				Log.i("minko-java", "[AndroidAttitude] Warning: No Magnetometer sensors found on this device.");
			}
		}

		Log.i("minko-java", "[AndroidAttitude] Is supported: " + _isSupported + " (tracking: " + _tracking + ")");

		return _isSupported;
	}
}