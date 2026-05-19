// 复制此文件为 config.js，填入你自己的 MQTT 信息
const MQTT_CONFIG = {
  host: 'YOUR_EMQX_HOST',      // EMQX Cloud 连接地址
  port: 8084,                   // WebSocket TLS 端口（固定）
  clientId: 'wx_smarthome_' + Math.random().toString(16).slice(2, 8),
  username: 'YOUR_MQTT_USER',
  password: 'YOUR_MQTT_PASSWORD',
  keepAlive: 60,
};

const TOPICS = {
  STATUS_JSON:  'mb/esp32/smarthome/status/json',
  STATE:        'mb/esp32/smarthome/state',
  CMD_MODE:     'mb/esp32/smarthome/cmd/mode',
  CMD_BUZZER:   'mb/esp32/smarthome/cmd/buzzer',
  CMD_LED:      'mb/esp32/smarthome/cmd/led',
  CMD_SERVO:    'mb/esp32/smarthome/cmd/servo',
  CMD_FAN:      'mb/esp32/smarthome/cmd/fan',
  CMD_ALARM_CLR:'mb/esp32/smarthome/cmd/alarm_clear',
};

module.exports = { MQTT_CONFIG, TOPICS };
