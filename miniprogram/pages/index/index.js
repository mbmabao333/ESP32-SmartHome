const MQTTClient = require('../../utils/mqttClient');
const { MQTT_CONFIG, TOPICS } = require('../../config');

const MODE_LABELS = { manual: '手动', auto: '自动', security: '安防' };

Page({
  data: {
    // 连接状态
    connected: false,
    deviceOnline: false,
    statusText: '正在连接...',

    // 传感器数据
    temp: '--',
    humi: '--',
    lux: '--',
    mq2: '--',
    door: false,
    pir: false,
    rain: false,

    // 报警状态
    alarm: false,
    smokeAlarm: false,
    smokeWarn: false,
    doorAlarm: false,
    alarmDesc: '',

    // 当前模式
    mode: 'manual',
    modeLabel: '手动',

    // 控制状态（本地显示用）
    fanOn: false,
    buzzerOn: false,
    servoAngle: 0,
    servoDisplay: 0,   // 拖动时实时显示，松手才发布
  },

  _mqtt: null,

  onLoad() {
    this._connect();
  },

  onUnload() {
    this._mqtt && this._mqtt.disconnect();
  },

  // ── MQTT 连接 ──────────────────────────────────────
  _connect() {
    const client = new MQTTClient(MQTT_CONFIG);
    this._mqtt = client;

    client.onConnect = () => {
      this.setData({ connected: true, statusText: 'MQTT 已连接' });
      client.subscribe(TOPICS.STATUS_JSON);
      client.subscribe(TOPICS.STATE);
    };

    client.onMessage = (topic, payload) => {
      if (topic === TOPICS.STATE) {
        this.setData({ deviceOnline: payload === 'online' });
        return;
      }
      if (topic === TOPICS.STATUS_JSON) {
        this._applyStatus(payload);
      }
    };

    client.onDisconnect = (reason) => {
      this.setData({ connected: false, deviceOnline: false, statusText: '已断开: ' + reason });
      // 5 秒后自动重连
      setTimeout(() => {
        if (!this.data.connected) this._connect();
      }, 5000);
    };

    client.connect();
  },

  // ── 解析并更新传感器 JSON ──────────────────────────
  _applyStatus(jsonStr) {
    let s;
    try { s = JSON.parse(jsonStr); } catch (e) { return; }

    const alarmDesc = [
      s.smokeAlarm ? '烟雾报警' : '',
      s.doorAlarm  ? '门磁报警' : '',
    ].filter(Boolean).join(' / ');

    this.setData({
      temp: s.temp !== undefined ? s.temp.toFixed(1) : '--',
      humi: s.humi !== undefined ? s.humi.toFixed(1) : '--',
      lux:  s.lux  !== undefined ? s.lux.toFixed(0)  : '--',
      mq2:  s.mq2  !== undefined ? s.mq2             : '--',

      door:      !!s.door,
      pir:       !!s.pir,
      rain:      !!s.rain,
      alarm:     !!s.alarm,
      smokeAlarm:!!s.smokeAlarm,
      smokeWarn: !!s.smokeWarn,
      doorAlarm: !!s.doorAlarm,
      alarmDesc,

      mode:      s.mode || 'manual',
      modeLabel: MODE_LABELS[s.mode] || s.mode,
    });
  },

  // ── 发布命令的统一入口 ─────────────────────────────
  _publish(topic, msg) {
    if (!this.data.connected) {
      wx.showToast({ title: 'MQTT 未连接', icon: 'none' });
      return;
    }
    this._mqtt.publish(topic, msg);
  },

  // ── 模式切换 ──────────────────────────────────────
  setMode(e) {
    const mode = e.currentTarget.dataset.mode;
    this._publish(TOPICS.CMD_MODE, mode);
    this.setData({ mode, modeLabel: MODE_LABELS[mode] });
  },

  // ── LED 控制 ──────────────────────────────────────
  ledCmd(e) {
    this._publish(TOPICS.CMD_LED, e.currentTarget.dataset.cmd);
  },

  // ── 风扇 ──────────────────────────────────────────
  toggleFan(e) {
    const on = e.detail.value;
    this._publish(TOPICS.CMD_FAN, on ? 'on' : 'off');
    this.setData({ fanOn: on });
  },

  // ── 蜂鸣器 ────────────────────────────────────────
  toggleBuzzer(e) {
    const on = e.detail.value;
    this._publish(TOPICS.CMD_BUZZER, on ? 'on' : 'off');
    this.setData({ buzzerOn: on });
  },

  // ── 舵机：拖动时只更新显示，松手才发布 ──────────────
  onServoChanging(e) {
    this.setData({ servoDisplay: e.detail.value });
  },
  onServoChange(e) {
    const angle = e.detail.value;
    this.setData({ servoAngle: angle, servoDisplay: angle });
    this._publish(TOPICS.CMD_SERVO, String(angle));
  },

  // ── 消警 ──────────────────────────────────────────
  clearAlarm() {
    this._publish(TOPICS.CMD_ALARM_CLR, 'clear');
    wx.showToast({ title: '消警命令已发送', icon: 'success' });
  },
});
