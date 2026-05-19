/**
 * MQTT 3.1.1 over WebSocket 客户端
 * 适配微信小程序 wx.connectSocket API
 * 支持 QoS 0 的发布和订阅
 */

// UTF-8 编码（不依赖 TextEncoder，兼容性更好）
function utf8Encode(str) {
  const bytes = [];
  for (let i = 0; i < str.length; i++) {
    const c = str.charCodeAt(i);
    if (c < 0x80) {
      bytes.push(c);
    } else if (c < 0x800) {
      bytes.push(0xC0 | (c >> 6), 0x80 | (c & 0x3F));
    } else {
      bytes.push(0xE0 | (c >> 12), 0x80 | ((c >> 6) & 0x3F), 0x80 | (c & 0x3F));
    }
  }
  return bytes;
}

// UTF-8 解码
function utf8Decode(bytes, start, end) {
  let result = '';
  let i = start;
  while (i < end) {
    const b = bytes[i];
    if (b < 0x80) {
      result += String.fromCharCode(b); i++;
    } else if (b < 0xE0) {
      result += String.fromCharCode(((b & 0x1F) << 6) | (bytes[i + 1] & 0x3F)); i += 2;
    } else {
      result += String.fromCharCode(((b & 0x0F) << 12) | ((bytes[i + 1] & 0x3F) << 6) | (bytes[i + 2] & 0x3F)); i += 3;
    }
  }
  return result;
}

// MQTT 字符串编码（2字节长度前缀 + 内容）
function encodeStr(str) {
  const b = utf8Encode(str);
  return [(b.length >> 8) & 0xFF, b.length & 0xFF, ...b];
}

// MQTT 剩余长度编码（可变长）
function encodeRemLen(len) {
  const result = [];
  do {
    let byte = len % 128;
    len = Math.floor(len / 128);
    if (len > 0) byte |= 0x80;
    result.push(byte);
  } while (len > 0);
  return result;
}

// MQTT 剩余长度解码，返回 { value, nextPos }
function decodeRemLen(bytes, start) {
  let value = 0, multiplier = 1, pos = start;
  let b;
  do {
    b = bytes[pos++];
    value += (b & 0x7F) * multiplier;
    multiplier *= 128;
  } while (b & 0x80);
  return { value, nextPos: pos };
}

class MQTTClient {
  constructor({ host, port, clientId, username, password, keepAlive }) {
    // 8083 用 ws://，8084 及其他用 wss://
    const scheme = (port === 8083) ? 'ws' : 'wss';
    this.url = `${scheme}://${host}:${port}/mqtt`;
    this.clientId = clientId;
    this.username = username || '';
    this.password = password || '';
    this.keepAlive = keepAlive || 60;

    this._socket = null;
    this._pingTimer = null;
    this._packetId = 1;

    // 外部回调
    this.onConnect = null;
    this.onMessage = null;
    this.onDisconnect = null;
  }

  connect() {
    this._socket = wx.connectSocket({
      url: this.url,
      protocols: ['mqtt'],
    });

    this._socket.onOpen(() => {
      this._sendConnect();
    });

    this._socket.onMessage(({ data }) => {
      if (!(data instanceof ArrayBuffer)) return;
      this._handlePacket(new Uint8Array(data));
    });

    this._socket.onClose(() => {
      this._stopPing();
      this.onDisconnect && this.onDisconnect('连接已断开');
    });

    this._socket.onError((err) => {
      this._stopPing();
      this.onDisconnect && this.onDisconnect('WebSocket 错误: ' + JSON.stringify(err));
    });
  }

  subscribe(topic, qos = 0) {
    const pid = this._nextPid();
    const topicBytes = encodeStr(topic);
    const varHeader = [(pid >> 8) & 0xFF, pid & 0xFF];
    const payload = [...topicBytes, qos];
    const body = [...varHeader, ...payload];
    this._send([0x82, ...encodeRemLen(body.length), ...body]);
  }

  publish(topic, message) {
    const topicBytes = encodeStr(topic);
    const msgBytes = utf8Encode(String(message));
    const body = [...topicBytes, ...msgBytes];
    this._send([0x30, ...encodeRemLen(body.length), ...body]);
  }

  disconnect() {
    this._stopPing();
    if (this._socket) {
      try { this._send([0xE0, 0x00]); } catch (e) {}
      this._socket.close({});
      this._socket = null;
    }
  }

  _sendConnect() {
    const clientBytes = encodeStr(this.clientId);
    const userBytes   = encodeStr(this.username);
    const passBytes   = encodeStr(this.password);

    // MQTT 3.1.1 CONNECT 报文
    const varHeader = [
      0x00, 0x04, 0x4D, 0x51, 0x54, 0x54, // Protocol Name "MQTT"
      0x04,                                  // Protocol Level 4
      0b11000010,                            // Connect Flags: CleanSession + Username + Password
      (this.keepAlive >> 8) & 0xFF, this.keepAlive & 0xFF,
    ];
    const payload = [...clientBytes, ...userBytes, ...passBytes];
    const body = [...varHeader, ...payload];
    this._send([0x10, ...encodeRemLen(body.length), ...body]);
  }

  _handlePacket(bytes) {
    const type = (bytes[0] >> 4) & 0x0F;

    if (type === 2) {
      // CONNACK
      const rc = bytes[3];
      if (rc === 0) {
        this._startPing();
        this.onConnect && this.onConnect();
      } else {
        const errMsg = ['', '拒绝: 协议版本不支持', '拒绝: ClientID 不合法', '拒绝: 服务不可用', '拒绝: 用户名或密码错误', '拒绝: 未授权'];
        this.onDisconnect && this.onDisconnect('MQTT 连接失败 — ' + (errMsg[rc] || `rc=${rc}`));
      }
    } else if (type === 3) {
      // PUBLISH
      this._handlePublish(bytes);
    }
    // SUBACK (type=9) 和 PINGRESP (type=13) 忽略
  }

  _handlePublish(bytes) {
    const { value: remLen, nextPos } = decodeRemLen(bytes, 1);
    let pos = nextPos;

    const topicLen = (bytes[pos] << 8) | bytes[pos + 1];
    pos += 2;
    const topic = utf8Decode(bytes, pos, pos + topicLen);
    pos += topicLen;

    const payloadEnd = nextPos + remLen;
    const payload = utf8Decode(bytes, pos, payloadEnd);

    this.onMessage && this.onMessage(topic, payload);
  }

  _startPing() {
    this._stopPing();
    this._pingTimer = setInterval(() => {
      this._send([0xC0, 0x00]);
    }, Math.floor(this.keepAlive * 0.8) * 1000);
  }

  _stopPing() {
    if (this._pingTimer) {
      clearInterval(this._pingTimer);
      this._pingTimer = null;
    }
  }

  _nextPid() {
    const id = this._packetId;
    this._packetId = (this._packetId % 65535) + 1;
    return id;
  }

  _send(bytes) {
    if (!this._socket) return;
    this._socket.send({ data: new Uint8Array(bytes).buffer });
  }
}

module.exports = MQTTClient;
