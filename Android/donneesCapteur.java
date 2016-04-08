private SensorManager mSensorManager;
private SensorEventListener mSensorListener;

////


mSensorManager = (SensorManager) this
                .getSystemService(Context.SENSOR_SERVICE);
mSensorListener = new SensorEventListener() {
    @Override
    public void onAccuracyChanged(Sensor arg0, int arg1) {
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        Sensor sensor = event.sensor;
        if (sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
            //TODO: get values
        }
        else if (sensor.getType() == Sensor.TYPE_ORIENTATION) {
            //TODO: get values
        }
    }
}

mSensorManager.registerListener(mSensorListener, mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER), SensorManager.SENSOR_DELAY_GAME);
mSensorManager.registerListener(mSensorListener, mSensorManager.getDefaultSensor(Sensor.TYPE_ORIENTATION), SensorManager.SENSOR_DELAY_GAME);