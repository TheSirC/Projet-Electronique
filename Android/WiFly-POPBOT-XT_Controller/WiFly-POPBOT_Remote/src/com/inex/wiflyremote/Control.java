package com.inex.wiflyremote;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.UnknownHostException;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.os.Vibrator;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.View.OnTouchListener;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

public class Control extends Activity{
    public static final int MESSAGE_DATA_RECEIVE = 0;

    TextView textLeft, textRight, textWarning;
    Button buttonUp, buttonDown, buttonLeft, buttonRight;

	Boolean task_state = true;
	Vibrator vibrator;

	Socket s;
	String ip;
	int port;

	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
		setContentView(R.layout.control);
		ip = getIntent().getExtras().getString("IP");
		port = Integer.parseInt(getIntent().getExtras().getString("PORT"));

		vibrator = (Vibrator) getSystemService(Context.VIBRATOR_SERVICE);

		textLeft = (TextView)findViewById(R.id.textLeft);
		textRight = (TextView)findViewById(R.id.textRight);

		textWarning = (TextView)findViewById(R.id.textWarning);
		textWarning.setVisibility(View.INVISIBLE);

		buttonUp = (Button)findViewById(R.id.buttonUp);
		buttonUp.setEnabled(false);
		buttonUp.setOnTouchListener(new OnTouchListener() {
			public boolean onTouch(View arg0, MotionEvent arg1) {
				if(arg1.getAction() == MotionEvent.ACTION_DOWN) {
					buttonUp.setPressed(true);
					sendData("F80;;");
				} else if(arg1.getAction() == MotionEvent.ACTION_UP) {
					buttonUp.setPressed(false);
					sendData("S0;;");
				}

				return true;
			}
		});

		buttonDown = (Button)findViewById(R.id.buttonDown);
		buttonDown.setEnabled(false);
		buttonDown.setOnTouchListener(new OnTouchListener() {
			public boolean onTouch(View arg0, MotionEvent arg1) {
				if(arg1.getAction() == MotionEvent.ACTION_DOWN) {
					buttonDown.setPressed(true);
					sendData("B80;;");
				} else if(arg1.getAction() == MotionEvent.ACTION_UP) {
					buttonDown.setPressed(false);
					sendData("S0;;");
				}

				return true;
			}
		});

		buttonLeft = (Button)findViewById(R.id.buttonLeft);
		buttonLeft.setEnabled(false);
		buttonLeft.setOnTouchListener(new OnTouchListener() {
			public boolean onTouch(View arg0, MotionEvent arg1) {
				if(arg1.getAction() == MotionEvent.ACTION_DOWN) {
					buttonLeft.setPressed(true);
					sendData("L80;;");
				} else if(arg1.getAction() == MotionEvent.ACTION_UP) {
					buttonLeft.setPressed(false);
					sendData("S0;;");
				}

				return true;
			}
		});

		buttonRight = (Button)findViewById(R.id.buttonRight);
		buttonRight.setEnabled(false);
		buttonRight.setOnTouchListener(new OnTouchListener() {
			public boolean onTouch(View arg0, MotionEvent arg1) {
				if(arg1.getAction() == MotionEvent.ACTION_DOWN) {
					buttonRight.setPressed(true);
					sendData("R80;;");
				} else if(arg1.getAction() == MotionEvent.ACTION_UP) {
					buttonRight.setPressed(false);
					sendData("S0;;");
				}

				return true;
			}
		});

    buttonStop = (Button)findViewById(R.id.buttonStop);
    buttonStop.setEnabled(false);
    buttonStop.setOnTouchListener(new OnTouchListener() {
      public boolean onTouch(View arg0, MotionEvent arg1) {
        if(arg1.getAction() == MotionEvent.ACTION_DOWN) {
          buttonStop.setPressed(true);
          sendData("S80;;");
        } else if(arg1.getAction() == MotionEvent.ACTION_UP) {
          buttonStop.setPressed(false);
          sendData("S0;;");
        }

        return true;
      }
    });

		Runnable readThread = new Runnable() {
			public void buttonEnable() {
				runOnUiThread(new Runnable() {
					public void run() {
						buttonUp.setEnabled(true);
						buttonDown.setEnabled(true);
						buttonLeft.setEnabled(true);
						buttonRight.setEnabled(true);
            buttonStop.setEnabled(true);
					}
				});
			}

			public void run() {
				try {
					s = new Socket();
					s.connect((new InetSocketAddress(InetAddress.getByName(ip), port)), 2000);
					buttonEnable();

					while(task_state) {
						try {
							BufferedReader in = new BufferedReader(new InputStreamReader(s.getInputStream()));
							String strRead = in.readLine();

							if(strRead.endsWith(";")) {
								strRead = strRead.substring(0, strRead.length() - 1);
							}

							String[] strData = strRead.split(":");
							for(int i = 0; i < strData.length; i++) {
								try {
									if(strData[i].charAt(0) == 'L') {
										String[] data = strData[i].split(",");
										data[0] = data[0].substring(1);
										final String[] tmp = data;
										runOnUiThread(new Runnable() {
											public void run() {
												textLeft.setText(tmp[0]);
												textRight.setText(tmp[1]);
											}
										});
									} else if(strData[i].charAt(0) == 'S') {
										String[] data = strData[i].split(",");
										data[0] = data[0].substring(1);
										final String[] tmp = data;
										if(tmp[0].equals("0") || tmp[1].equals("0")) {
											vibrator.vibrate(200);
											runOnUiThread(new Runnable() {
												public void run() {
													textWarning.setVisibility(View.VISIBLE);
												}
											});
										} else {
											runOnUiThread(new Runnable() {
												public void run() {
													textWarning.setVisibility(View.INVISIBLE);
												}
											});
										}
									}
								} catch (StringIndexOutOfBoundsException e) { }
							}
						} catch (NumberFormatException e) {
							e.printStackTrace();
						} catch (UnknownHostException e) {
							e.printStackTrace();
						} catch (IOException e) {
							e.printStackTrace();
						}
					}
				} catch (NumberFormatException e) {
					e.printStackTrace();
				} catch (UnknownHostException e) {
					e.printStackTrace();
				} catch (IOException e) {
					runOnUiThread(new Runnable() {
						public void run() {
							Toast.makeText(getApplicationContext()
									, "Connection Failed"
									, Toast.LENGTH_SHORT).show();
						}
					});
					finish();
				}
			}
		};
		new Thread(readThread).start();


		Log.i("Check IP", ip + ":" + port);
	}

	public void onPause() {
		super.onPause();
		task_state = false;

		try {
			s.close();
		} catch (IOException e) {
			e.printStackTrace();
		} catch (NullPointerException e) {
			e.printStackTrace();
		}

		finish();
	}

	public void sendData(String str) {
		try {
			OutputStream out = s.getOutputStream();
			out.write(str.getBytes());
			out.flush();
		} catch (IOException e) {
			e.printStackTrace();
		} catch (NullPointerException e) {
			Toast.makeText(getApplicationContext()
					, "Please check your WiFi Connection"
					, Toast.LENGTH_SHORT).show();
		}
	}
}