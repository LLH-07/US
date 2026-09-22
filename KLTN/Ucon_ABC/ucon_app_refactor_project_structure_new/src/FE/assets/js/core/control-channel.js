// WebSocket control channel for UCON runtime notifications.
// Backend endpoint: /ws/pep/control?userId=<currentUserId>
let pepControlSocket = null;
let pepControlConnectionId = null;
let pepControlConnectPromise = null;
let pepControlReconnectTimer = null;
let pepControlPingTimer = null;
let pepControlClosedIntentionally = false;
let pepControlPendingResolve = null;

function derivePepControlWsUrl(){
  const explicit = window.UCON_ENV?.CONTROL_WS_URL;
  if(explicit) return explicit;

  try{
    const api = new URL(API_BASE, window.location.href);
    api.protocol = api.protocol === 'https:' ? 'wss:' : 'ws:';
    api.pathname = api.pathname.replace(/\/api\/pep\/?$/, '/ws/pep/control');
    api.search = '';
    api.hash = '';
    return api.toString();
  }catch(error){
    const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
    return `${protocol}//${window.location.host}/ws/pep/control`;
  }
}

function pepControlConnectTimeoutMs(){
  return Number(window.UCON_ENV?.CONTROL_WS_CONNECT_TIMEOUT_MS || 1500);
}
function pepControlReconnectMs(){
  return Number(window.UCON_ENV?.CONTROL_WS_RECONNECT_MS || 2000);
}
function pepControlPingMs(){
  return Number(window.UCON_ENV?.CONTROL_WS_PING_MS || 25000);
}
function getPepControlConnectionId(){
  return pepControlConnectionId;
}
function isPepControlReady(){
  return Boolean(
    pepControlSocket &&
    pepControlSocket.readyState === WebSocket.OPEN &&
    pepControlConnectionId
  );
}
function clearPepControlTimers(){
  if(pepControlReconnectTimer){
    clearTimeout(pepControlReconnectTimer);
    pepControlReconnectTimer = null;
  }
  if(pepControlPingTimer){
    clearInterval(pepControlPingTimer);
    pepControlPingTimer = null;
  }
}
function schedulePepControlPing(){
  if(pepControlPingTimer) clearInterval(pepControlPingTimer);
  pepControlPingTimer = setInterval(() => {
    try{
      if(pepControlSocket && pepControlSocket.readyState === WebSocket.OPEN){
        pepControlSocket.send('PING');
      }
    }catch(error){
      console.warn('Không gửi được PING tới UCON control WebSocket:', error);
    }
  }, pepControlPingMs());
}
function resolvePepControlConnection(connectionId){
  if(pepControlPendingResolve){
    pepControlPendingResolve(connectionId || null);
    pepControlPendingResolve = null;
  }
}
function rejectPepControlConnection(){
  resolvePepControlConnection(null);
  pepControlConnectPromise = null;
}
function schedulePepControlReconnect(){
  if(pepControlClosedIntentionally || pepControlReconnectTimer) return;
  pepControlReconnectTimer = setTimeout(() => {
    pepControlReconnectTimer = null;
    connectPepControlChannel(true);
  }, pepControlReconnectMs());
}
function handlePepControlMessage(raw){
  let message;
  try{
    message = JSON.parse(raw);
  }catch(error){
    console.warn('UCON control WebSocket gửi message không phải JSON:', raw);
    return;
  }

  switch(message.type){
    case 'CONTROL_CONNECTED':
      pepControlConnectionId = message.connectionId || null;
      console.info('UCON control channel connected:', pepControlConnectionId);
      resolvePepControlConnection(pepControlConnectionId);
      window.dispatchEvent(new CustomEvent('ucon-control-connected', {detail: message}));
      break;

    case 'USAGE_BOUND':
      console.info('UCON usage session bound:', message.usageSessionId);
      window.dispatchEvent(new CustomEvent('ucon-usage-bound', {detail: message}));
      break;

    case 'SESSION_REVOKED':
      console.warn('UCON revoked current usage session:', message);
      window.dispatchEvent(new CustomEvent('ucon-session-revoked', {detail: message}));
      if(typeof window.handleUsageSessionRevoked === 'function') window.handleUsageSessionRevoked(message);
      break;

    case 'SESSION_ENDED':
      window.dispatchEvent(new CustomEvent('ucon-session-ended', {detail: message}));
      if(typeof window.handleUsageSessionEnded === 'function') window.handleUsageSessionEnded(message);
      break;

    case 'PONG':
      break;

    case 'ERROR':
      console.warn('UCON control WebSocket error message:', message.message || message);
      break;

    default:
      console.debug('UCON control WebSocket message:', message);
  }
}
function connectPepControlChannel(force = false){
  if(!currentUserId || Number(currentUserId) <= 0) return Promise.resolve(null);
  if(!force && isPepControlReady()) return Promise.resolve(pepControlConnectionId);
  if(!force && pepControlConnectPromise) return pepControlConnectPromise;

  pepControlClosedIntentionally = false;
  clearPepControlTimers();

  if(pepControlSocket){
    try{ pepControlSocket.close(); }catch(error){ /* ignore */ }
  }

  const baseUrl = derivePepControlWsUrl();
  const url = `${baseUrl}${baseUrl.includes('?') ? '&' : '?'}userId=${encodeURIComponent(currentUserId)}`;

  pepControlConnectionId = null;
  pepControlConnectPromise = new Promise(resolve => {
    pepControlPendingResolve = resolve;
    const timeout = setTimeout(() => {
      if(!pepControlConnectionId) resolvePepControlConnection(null);
    }, pepControlConnectTimeoutMs());

    try{
      pepControlSocket = new WebSocket(url);
    }catch(error){
      clearTimeout(timeout);
      console.warn('Không mở được UCON control WebSocket:', error);
      rejectPepControlConnection();
      schedulePepControlReconnect();
      return;
    }

    pepControlSocket.addEventListener('open', () => {
      schedulePepControlPing();
    });

    pepControlSocket.addEventListener('message', event => {
      handlePepControlMessage(event.data);
      if(pepControlConnectionId) clearTimeout(timeout);
    });

    pepControlSocket.addEventListener('close', () => {
      clearTimeout(timeout);
      pepControlSocket = null;
      pepControlConnectionId = null;
      pepControlConnectPromise = null;
      clearPepControlTimers();
      resolvePepControlConnection(null);
      schedulePepControlReconnect();
    });

    pepControlSocket.addEventListener('error', event => {
      clearTimeout(timeout);
      console.warn('UCON control WebSocket gặp lỗi:', event);
      resolvePepControlConnection(null);
    });
  });

  return pepControlConnectPromise;
}
function reconnectPepControlChannel(){
  disconnectPepControlChannel(false);
  return connectPepControlChannel(true);
}
function disconnectPepControlChannel(intentional = true){
  pepControlClosedIntentionally = intentional;
  clearPepControlTimers();
  pepControlConnectionId = null;
  pepControlConnectPromise = null;
  resolvePepControlConnection(null);
  if(pepControlSocket){
    try{ pepControlSocket.close(); }catch(error){ /* ignore */ }
  }
  pepControlSocket = null;
}
async function waitForPepControlConnection(timeoutMs = pepControlConnectTimeoutMs()){
  if(isPepControlReady()) return pepControlConnectionId;

  const connectPromise = connectPepControlChannel();
  const timeoutPromise = new Promise(resolve => setTimeout(() => resolve(null), timeoutMs));
  return Promise.race([connectPromise, timeoutPromise]);
}

window.addEventListener('online', () => connectPepControlChannel(true));
window.addEventListener('beforeunload', () => disconnectPepControlChannel(true));

// Start early. If the socket is not ready by the time the user opens a book,
// startReadSession() will wait briefly and still continue over HTTP if needed.
setTimeout(() => connectPepControlChannel(), 0);
