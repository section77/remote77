//wlan
const char* ssid = "DpfyMAq4";
const char* password = "7gS}V2F%?nqW/.Uw.*9c:!!Q:&=9GousaX=&jf.[hZs{{V7HgW)ic;b=[VbvBc%";

//device anpassen - 1/3
const char* device_name = "fernbedienung";

//mqtt
const char* mqtt_server = "robbi";
// name anpassen - 2/3
const char* mqtt_password = "uiNgie3ooK";
const char* mqtt_clientname = device_name;
const char* mqtt_user = device_name;

//topics anpassen - 3/3
const char* mqtt_pub_topic_batterie = "devices/fernbedienung/batterie";
const char* mqtt_pub_topic_geraet_1 = "devices/geraet_1/soll";
const char* mqtt_pub_topic_licht = "devices/licht/soll";
