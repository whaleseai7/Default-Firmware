
// Kode dibuat oleh Alvaro Ariel Ilanunu.

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP32httpUpdate.h>

#define VERSI 1
#define ALAMAT_IP_SERVER "192.168.1.22:5000"
#define JALUR_PEMBARUAN "/update"

String SSID_Wifi = "Griya Ilham";
String sandi_Wifi = "Siput_Ngebut";

void koneksiWiFi();
void periksaServer();
t_httpUpdate_return pembaruanFirmware(String url_pembaruan);

void setup()
{
  Serial.begin(9600);
  koneksiWiFi();
}

void loop()
{
  periksaServer();
  delay(1000);
}

void periksaServer() {
  Serial.println("\n\n⤫~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~⤫");
  Serial.println("|");
  Serial.println("◉➞➞➞  Mengambil Data dari Server...\n");
  Serial.println("⤫--------------------------------------------------------");
  Serial.println("|");

  String url = "http://" + String(ALAMAT_IP_SERVER) + String(JALUR_PEMBARUAN) +
               "?mac_address=" + WiFi.macAddress();

  HTTPClient http;
  String respon_dari_server;
  http.begin(url);
  int httpCode = http.GET();

  if(httpCode > 0) {
    respon_dari_server = http.getString();
    Serial.println("◉➞  Respons dari Server: ");
    Serial.println("\n" + respon_dari_server);
    Serial.println("⤫--------------------------------------------------------");
    Serial.println("|");

    StaticJsonDocument<1024> doc;
    deserializeJson(doc, respon_dari_server);
    JsonObject obj = doc.as<JsonObject>();

    String versi = obj[String("versi")];
    String url_pembaruan = obj[String("url")];
    String versi_di_server = url_pembaruan.substring(url_pembaruan.lastIndexOf('v') + 1, url_pembaruan.lastIndexOf('.'));

    Serial.println("◉➞➞➞  Versi Firmware Tertinggi yang Tersedia: " + versi_di_server + "\n|");

    if (versi_di_server.toInt() > VERSI) {
      Serial.println("◉➞➞➞  Pembaruan Dimulai --- ✓");
      if(pembaruanFirmware(url_pembaruan) == HTTP_UPDATE_OK) {
        ESP.restart();
      } else {
        Serial.println("◉➞➞➞  Pembaruan Gagal! --- ✕\n");
      }
    } else {
      Serial.println("◉➞➞➞  Firmware Versi Terbaru Belum Tersedia --- ◯\n");
      Serial.println("⤫~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~⤫\n\n");
    }
  } else {
    Serial.println("|\n◉➞➞➞  Permintaan HTTP Gagal! --- ✕\n");
  }

  http.end();
}

t_httpUpdate_return pembaruanFirmware(String url_pembaruan)
{
  t_httpUpdate_return ret;

  if(WiFi.status()==WL_CONNECTED){
    
    ret= ESPhttpUpdate.update(url_pembaruan);

    switch(ret)
    {
      case HTTP_UPDATE_FAILED:
        Serial.println("[update]|\n◉➞➞➞  Pembaruan Tidak Dapat Dilakukan! --- ✕\n");
        return ret;
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("[update]|\n◉➞➞➞  Tidak Ada Pembaruan --- ◯\n");
        return ret;
        break;

      case HTTP_UPDATE_OK:
        Serial.println("[update]|\n◉➞➞➞  Pembaruan Dilakukan! --- ✓\n");
        break;
    }
  }
}

void koneksiWiFi()
{
  Serial.println("\n|\n◉➞➞➞  Menghubungkan ke Wi-Fi\n");
  WiFi.begin(SSID_Wifi.c_str(), sandi_Wifi.c_str());
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }

  Serial.println("\n|\n◉➞➞➞  Wi-Fi Terhubung! --- ✓\n");
  Serial.println(WiFi.SSID());
  Serial.println(WiFi.RSSI());
  Serial.println(WiFi.macAddress());
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.gatewayIP());
  Serial.println(WiFi.dnsIP());
}
