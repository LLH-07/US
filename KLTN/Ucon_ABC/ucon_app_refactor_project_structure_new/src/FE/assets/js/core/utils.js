// General utilities and website-level modal helpers.
function escapeHtml(value){
  return String(value || '')
    .replaceAll('&','&amp;')
    .replaceAll('<','&lt;')
    .replaceAll('>','&gt;')
    .replaceAll('\"','&quot;')
    .replaceAll("'",'&#039;');
}

function mojibakeScore(value){
  return (String(value || '').match(/\u00c3|\u00c2|\u00c4|\u00c5|\u00c6|\u00e1\u00ba|\u00e1\u00bb|\u00e0\u00a1|\ufffd/g) || []).length;
}

function repairMojibakeText(value){
  if(typeof value !== 'string') return value;
  if(!mojibakeScore(value)) return value;
  try{
    const bytes = new Uint8Array(Array.from(value, ch => ch.charCodeAt(0) & 0xff));
    const repaired = new TextDecoder('utf-8', {fatal:false}).decode(bytes);
    return mojibakeScore(repaired) < mojibakeScore(value) ? repaired : value;
  }catch(error){
    return value;
  }
}

function cleanBackendPayload(value){
  if(Array.isArray(value)) return value.map(cleanBackendPayload);
  if(value && typeof value === 'object'){
    const out = {};
    Object.entries(value).forEach(([key, item]) => out[key] = cleanBackendPayload(item));
    return out;
  }
  return repairMojibakeText(value);
}

function pushPopupHistory(key='popup'){
  try{
    history.pushState(Object.assign({}, history.state || {}, {mhPopup:key}), '', location.href);
  }catch(error){}
}
function closeAnyOpenPopupFromHistory(){
  let closed = false;
  const picker = document.getElementById('book-volume-picker');
  if(picker && typeof closeBookVolumePicker === 'function'){
    closeBookVolumePicker(false);
    closed = true;
  }
  const readerPopup = document.getElementById('reader-friendly-popup');
  if(readerPopup){
    readerPopup.remove();
    closed = true;
  }
  const systemModal = document.getElementById('system-modal');
  if(systemModal){
    systemModal.remove();
    closed = true;
  }
  return closed;
}
if(!window.__mhPopupBackBound){
  window.__mhPopupBackBound = true;
  window.addEventListener('popstate', () => closeAnyOpenPopupFromHistory());
}

function togglePasswordVisibility(inputId, button){
  const input = document.getElementById(inputId);
  if(!input) return;
  const hidden = input.type === 'password';
  input.type = hidden ? 'text' : 'password';
  if(button){
    button.textContent = hidden ? '\u1ea8n' : 'Hi\u1ec7n';
    button.setAttribute('aria-label', hidden ? '\u1ea8n m\u1eadt kh\u1ea9u' : 'Hi\u1ec7n m\u1eadt kh\u1ea9u');
  }
}

function showSystemModal({title='Thông báo', message='', confirmText='Đã hiểu', cancelText='', tone='default', onConfirm=null, onCancel=null} = {}){
  const titleKey = String(title || '').toLowerCase();
  const modalExtraClass = titleKey.includes('hủy gói thuê') ? ' subscription-cancel-modal' : (titleKey.includes('bỏ chia sẻ tập') ? ' remove-share-modal' : '');
  const old = document.getElementById('system-modal');
  if(old) old.remove();
  if(typeof pushPopupHistory === 'function') pushPopupHistory('system-modal');
  document.body.insertAdjacentHTML('beforeend', `
    <div id="system-modal" class="system-modal-backdrop" role="dialog" aria-modal="true">
      <div class="system-modal-card ${tone}${modalExtraClass}">
        <div class="system-modal-kicker">MH Library Demo</div>
        <h3>${escapeHtml(title)}</h3>
        <p>${escapeHtml(message)}</p>
        <div class="system-modal-actions">
          ${cancelText ? `<button id="system-modal-cancel" class="system-modal-btn secondary">${escapeHtml(cancelText)}</button>` : ''}
          <button id="system-modal-confirm" class="system-modal-btn ${tone === 'danger' ? 'danger' : ''}">${escapeHtml(confirmText)}</button>
        </div>
      </div>
    </div>
  `);
  document.getElementById('system-modal-confirm')?.addEventListener('click', () => {
    document.getElementById('system-modal')?.remove();
    if(typeof onConfirm === 'function') onConfirm();
  });
  document.getElementById('system-modal-cancel')?.addEventListener('click', () => {
    document.getElementById('system-modal')?.remove();
    if(typeof onCancel === 'function') onCancel();
  });
}
function showSystemNotice(title, message, onConfirm = null){
  showSystemModal({title, message, confirmText:'Đã hiểu', onConfirm});
}
function showSystemConfirm({title='Xác nhận thao tác', message='', confirmText='Xác nhận', cancelText='Không', tone='default'} = {}){
  return new Promise(resolve => {
    showSystemModal({
      title,
      message,
      confirmText,
      cancelText,
      tone,
      onConfirm: () => resolve(true),
      onCancel: () => resolve(false)
    });
  });
}
