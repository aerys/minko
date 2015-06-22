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
public class AndroidAttitude
{
	private Activity _sdlActivity;
	private final Context _context;
	private Looper _sensorLooper;
	private SensorManager _sensorManager;
	private SensorEventListener _sensorEventListener;
	private volatile boolean _tracking;

	private Display _display;
	private ArrayList<float[]> _sensorMagnetData;
	private float[] _offsets = new float[20];
	private boolean _magnetDown = false;

	// Native functions
	public native void minkoNativeOnAttitudeEvent(float[] rotationMatrix, float[] quaternion, boolean magnet);

	public AndroidAttitude(Activity sdlActivity)
	{
		_sdlActivity = sdlActivity;
		_context = SDLActivity.getContext();
		_sensorMagnetData = new ArrayList();

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

				_sensorLooper = Looper.myLooper();
				Handler handler = new Handler();

				_sensorManager = (SensorManager)_context.getSystemService("sensor");

				Sensor sensor = _sensorManager.getDefaultSensor(Sensor.TYPE_GAME_ROTATION_VECTOR);
				
				if(sensor == null)
					sensor = _sensorManager.getDefaultSensor(Sensor.TYPE_ROTATION_VECTOR);

				_sensorManager.registerListener(_sensorEventListener, sensor, 0, handler);
				_sensorManager.registerListener(_sensorEventListener, _sensorManager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD), 0, handler);

				Looper.loop();
			}
		});

		sensorThread.start();
		_tracking = true;
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
		if(event.sensor.getType() == Sensor.TYPE_MAGNETIC_FIELD)
		{
			float[] values = event.values;

			if ((values[0] == 0.0F) && (values[1] == 0.0F) && (values[2] == 0.0F)) {
				return;
			}
			addMagnetData((float[])event.values.clone(), event.timestamp);
		
		} else {
			float[] rotationVector = { event.values[0], event.values[1], event.values[2] };

			float[] quaternion = new float[4];
			float[] rotationMatrix = new float[16];
			float[] rotationMatrixTransformed = new float[16];

			_sensorManager.getQuaternionFromVector(quaternion, rotationVector);
			_sensorManager.getRotationMatrixFromVector(rotationMatrix, rotationVector);

			switch (_display.getRotation()) {
				case Surface.ROTATION_0:
				    _sensorManager.remapCoordinateSystem(rotationMatrix, _sensorManager.AXIS_X, _sensorManager.AXIS_Y, rotationMatrixTransformed);
				    break;
				case Surface.ROTATION_90:
					_sensorManager.remapCoordinateSystem(rotationMatrix, _sensorManager.AXIS_Y, _sensorManager.AXIS_MINUS_X, rotationMatrixTransformed);
				    break;
				case Surface.ROTATION_180:
				    _sensorManager.remapCoordinateSystem(rotationMatrix, _sensorManager.AXIS_MINUS_X, _sensorManager.AXIS_MINUS_Y, rotationMatrixTransformed);
				    break;
				case Surface.ROTATION_270:
				    _sensorManager.remapCoordinateSystem(rotationMatrix, _sensorManager.AXIS_MINUS_Y, _sensorManager.AXIS_X, rotationMatrixTransformed);
				    break;
			}

			minkoNativeOnAttitudeEvent(rotationMatrixTransformed, quaternion, _magnetDown);
		}
	}

	private void addMagnetData(float[] values, long time) {
		if (_sensorMagnetData.size() > 40) {
			_sensorMagnetData.remove(0);
		}
		_sensorMagnetData.add(values);

		evaluateMagnetModel();
	}

	private void evaluateMagnetModel()
	{
		if (_sensorMagnetData.size() < 40) {
			return;
		}

		float[] means = new float[2];
		float[] maximums = new float[2];
		float[] minimums = new float[2];

		float[] baseline = (float[])_sensorMagnetData.get(_sensorMagnetData.size() - 1);

		for (int i = 0; i < 2; i++) {
			int segmentStart = 20 * i;

			float[] mOffsets = computeOffsets(segmentStart, baseline);

			means[i] = computeMean(mOffsets);
			maximums[i] = computeMaximum(mOffsets);
			minimums[i] = computeMinimum(mOffsets);
		}

		float min1 = minimums[0];
		float max2 = maximums[1];

		_magnetDown = ((min1 < 30.0F) && (max2 > 130.0F));
	}

	private float[] computeOffsets(int start, float[] baseline)
	{
		for (int i = 0; i < 20; i++) {
			float[] point = (float[])_sensorMagnetData.get(start + i);
			float[] o = { point[0] - baseline[0], point[1] - baseline[1], point[2] - baseline[2] };
			float magnitude = (float)Math.sqrt(o[0] * o[0] + o[1] * o[1] + o[2] * o[2]);
			_offsets[i] = magnitude;
		}
		return _offsets;
	}

	private float computeMean(float[] offsets) {
		float sum = 0.0F;
		for (float o : offsets) {
			sum += o;
		}
		return sum / offsets.length;
	}

	private float computeMaximum(float[] offsets) {
		float max = (1.0F / -1.0F);
		for (float o : offsets) {
			max = Math.max(o, max);
		}
		return max;
	}

	private float computeMinimum(float[] offsets) {
		float min = (1.0F / 1.0F);
		for (float o : offsets) {
			min = Math.min(o, min);
		}
		return min;
	}
}