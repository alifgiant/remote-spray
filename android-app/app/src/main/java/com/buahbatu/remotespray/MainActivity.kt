package com.buahbatu.remotespray

import androidx.appcompat.app.AppCompatActivity

import android.os.Bundle
import kotlinx.android.synthetic.main.activity_main.*
import org.eclipse.paho.android.service.MqttAndroidClient
import org.eclipse.paho.client.mqttv3.*

class MainActivity : AppCompatActivity() {
    companion object {
        const val SERVER_URI = "tcp://broker.mqtt-dashboard.com:1883"
        const val PUBLISH_TOPIC = "remote-spray-alif"
        var CLIENT_ID = "alif-remote-spray-android"
    }

    private val mqttClient by lazy {
        MqttAndroidClient(this, SERVER_URI, CLIENT_ID)
    }

    private fun setupMqtt () {
        try {
            mqttClient.connect(MqttConnectOptions().apply {
                isAutomaticReconnect = true
                isCleanSession = false
            }, null, object : IMqttActionListener {
                override fun onSuccess(asyncActionToken: IMqttToken?) {
                    println("onSuccess: ${asyncActionToken.toString()}")

                    lamp.setBackgroundResource(R.drawable.ic_circle_green)
                    connection_status.text = "Connected"
                }

                override fun onFailure(asyncActionToken: IMqttToken?, exception: Throwable?) {
                    println("Failed to connect to: $SERVER_URI")
                }
            })
        } catch (e: MqttException) {
            System.err.println("JANUAR: Error connect: " + e.message)
            e.printStackTrace()
        }
    }

    private fun publishMessage() {
        if (mqttClient.isConnected) {
            try {
                mqttClient.publish(PUBLISH_TOPIC, MqttMessage().apply {
                    payload = "1".toByteArray()
                })
                println("Sending: 1")
            } catch (e: MqttException) {
                System.err.println("Error Publishing: " + e.message)
                e.printStackTrace()
            }
        } else {
            println("MQTT is not connected yet")
        }
    }

    override fun onDestroy() {
        if (mqttClient.isConnected) mqttClient.disconnect()
        mqttClient.unregisterResources()
        super.onDestroy()
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        setupMqtt()
        btn_spray.setOnClickListener { publishMessage() }
    }
}
