package com.inex.wiflyremote;

import android.os.Bundle;
import android.app.Activity;
import android.content.Intent;
import android.content.SharedPreferences;
import android.view.Menu;
import android.view.View;
import android.view.Window;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;

public class Main extends Activity {
	EditText etxtIP, etxtPort;
	
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
		setContentView(R.layout.main);
		SharedPreferences settings = getSharedPreferences("IPAddress", 0);
	       
		etxtIP = (EditText)findViewById(R.id.etxtIP);
		etxtIP.setText(settings.getString("IP", "192.168.4.1"));
		
		etxtPort = (EditText)findViewById(R.id.etxtPort);
		etxtPort.setText(settings.getString("PORT", "2000"));
		
		Button buttonConnect = (Button)findViewById(R.id.buttonConnect);
		buttonConnect.setOnClickListener(new OnClickListener() {
			public void onClick(View v) {
				Intent intent = new Intent(Main.this, Control.class);
				intent.putExtra("IP", etxtIP.getText().toString());
				intent.putExtra("PORT", etxtPort.getText().toString());
				startActivity(intent);
			}
		});
	}
	
	public void onPause() {
		super.onPause();
		SharedPreferences settings = getSharedPreferences("IPAddress", 0);
	    SharedPreferences.Editor editor = settings.edit();
	    editor.putString("IP", etxtIP.getText().toString());
	    editor.putString("PORT", etxtPort.getText().toString());
	    editor.commit();
	}
}
