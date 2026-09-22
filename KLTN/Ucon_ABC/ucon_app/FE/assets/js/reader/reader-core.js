// Reader metadata, cache, lazy chunk loading, and session cleanup.
let readerCopyProtectionEnabled = false;
function isInsideReaderContent(target){
  const reader = document.querySelector('.text-only-reader, .reader-text-content, .reader-page-body');
  return Boolean(reader && target && (reader === target || reader.contains(target)));
}
function readerCopyProtectionHandler(event){
  if(!readerState) return;
  if(['copy','cut'].includes(event.type)){
    event.preventDefault();
    return;
  }
  if(!isInsideReaderContent(event.target)) return;
  event.preventDefault();
}
function readerKeyCopyProtectionHandler(event){
  if(!readerState) return;
  const key = String(event.key || '').toLowerCase();
  if((event.ctrlKey || event.metaKey) && ['c','x','a','s','p'].includes(key)){
    event.preventDefault();
  }
}
function enableReaderCopyProtection(){
  if(readerCopyProtectionEnabled) return;
  readerCopyProtectionEnabled = true;
  document.addEventListener('copy', readerCopyProtectionHandler, true);
  document.addEventListener('cut', readerCopyProtectionHandler, true);
  document.addEventListener('contextmenu', readerCopyProtectionHandler, true);
  document.addEventListener('selectstart', readerCopyProtectionHandler, true);
  document.addEventListener('dragstart', readerCopyProtectionHandler, true);
  document.addEventListener('keydown', readerKeyCopyProtectionHandler, true);
}
function disableReaderCopyProtection(){
  if(!readerCopyProtectionEnabled) return;
  readerCopyProtectionEnabled = false;
  document.removeEventListener('copy', readerCopyProtectionHandler, true);
  document.removeEventListener('cut', readerCopyProtectionHandler, true);
  document.removeEventListener('contextmenu', readerCopyProtectionHandler, true);
  document.removeEventListener('selectstart', readerCopyProtectionHandler, true);
  document.removeEventListener('dragstart', readerCopyProtectionHandler, true);
  document.removeEventListener('keydown', readerKeyCopyProtectionHandler, true);
}

function backendVolumeId(uiVolumeId){
  return Number(uiVolumeId);
}
function uiBookIdFromBackendBookId(backendBookId){
  return Number(backendBookId || 1);
}
function uiVolumeIdFromBackend(bookId, backendVolumeId){
  return Number(backendVolumeId || (Number(bookId) * 100 + 1));
}
function backendSampleVolumeIdForBook(bookId){
  return BOOK_BACKEND_SAMPLE_VOLUME_ID[Number(bookId)] || backendVolumeId(bookVolumesOf(bookId)[0]?.id || 1);
}
async function fetchReaderMetadataForBook(bookId){
  const volumeId = backendSampleVolumeIdForBook(bookId);
  const res = await fetch(`${API_BASE}/read/metadata?volumeId=${volumeId}`);
  let data = await res.json().catch(() => ({}));
  data = cleanBackendPayload(data);
  if(!res.ok) throw new Error(data.reason || data.message || 'LOAD_READER_METADATA_FAILED');
  return data;
}
function readerSessionStorageKey(uiVolumeId){
  return `ucon_active_full_session_${Number(currentUserId || 0)}_${Number(backendVolumeId(uiVolumeId) || 0)}`;
}
function savedReaderSessionId(uiVolumeId){
  const raw = sessionStorage.getItem(readerSessionStorageKey(uiVolumeId));
  const id = Number(raw || 0);
  return Number.isFinite(id) && id > 0 ? id : null;
}
function saveReaderSessionId(uiVolumeId, sessionId){
  const id = Number(sessionId || 0);
  if(id > 0) sessionStorage.setItem(readerSessionStorageKey(uiVolumeId), String(id));
}
function clearSavedReaderSessionId(uiVolumeId){
  sessionStorage.removeItem(readerSessionStorageKey(uiVolumeId));
}
function fullAccessRequirementText(book, volume){
  const model = normalizeAccessModel(volume?.access_model_override || book?.default_access_model);
  if(model === 'PAID') return 'Muốn đọc full tập này, người dùng cần mua tập hoặc được nhóm gia đình chia sẻ.';
  if(model === 'RENTAL') return 'Muốn đọc full tập này, người dùng cần có gói thuê đang hoạt động.';
  if(model === 'FREE') return 'Muốn đọc full sách miễn phí, người dùng cần đăng nhập.';
  return 'Bạn chưa có quyền đọc full tập này.';
}
function volumeActionButtons(v, book, label){
  const canFull = canReadFullVolume(v);
  return `<div class="volume-card-actions compact-volume-actions"><a class="volume-option preview" href="#reader/${v.id}/preview">Preview</a><a class="volume-option full ${canFull ? 'ready' : 'locked'}" href="#reader/${v.id}/full">Đọc full</a></div>`;
}
function renderVolumeButtons(bookId){
  const book = bookById(bookId);
  return bookVolumesOf(bookId)
    .map(v => {
      const volumeNo = String(v.volume_no).padStart(2,'0');
      const label = `Tập ${volumeNo}`;
      const canFull = canReadFullVolume(v);
      const purchased = hasPurchasedVolume(v.id);
      const sharedOnly = hasFamilySharedVolume(v.id) && !purchased;
      const model = normalizeAccessModel(v.access_model_override || book?.default_access_model);
      const title = canFull
        ? (sharedOnly ? 'Được chia sẻ' : (model === 'PAID' && purchased ? 'Đã mua tập' : 'Có quyền đọc'))
        : (model === 'PAID' ? 'Chưa mua tập' : (model === 'RENTAL' ? 'Cần đăng ký gói' : 'Đọc thử'));
      const buyAction = model === 'PAID' && !purchased
        ? `<button type="button" class="volume-tile-buy" onclick="event.stopPropagation(); purchasePaidVolume(${Number(v.id)}, false)">Mua tập</button>`
        : '';
      return `<article class="volume-card volume-tile ${canFull ? (sharedOnly ? 'is-shared' : 'is-owned') : 'is-preview'} access-${model.toLowerCase()}" role="button" tabindex="0" aria-label="${escapeHtml(label)} - ${escapeHtml(title)}" onclick="openSingleVolumePicker(${Number(bookId)}, ${Number(v.id)})" onkeydown="if(event.key==='Enter'||event.key===' '){event.preventDefault(); openSingleVolumePicker(${Number(bookId)}, ${Number(v.id)});}">
        <div class="volume-tile-head">
          <span class="volume-tile-no">${label}</span>
        </div>
        <div class="volume-tile-bottom">
          <strong>${title}</strong>
          ${buyAction}
        </div>
      </article>`;
    })
    .join('');
}
function applyAutoVolumesForBook(bookId, metadata){
  // Không tự sinh lại id tập ở FE nữa.
  // Tập/volume phải lấy từ backend catalog để tránh lỗi bấm sách A nhưng mở nhầm sách B.
  // Metadata chỉ dùng để cập nhật trạng thái đọc, không được ghi đè danh mục sách demo.
  return;
}
function refreshBookDetailDom(bookId, metadata){
  const book = bookById(bookId);
  if(!book) return;
  const detailTitle = document.getElementById('detail-book-title');
  const coverTitle = document.getElementById('detail-cover-title');
  const readBtn = document.getElementById('detail-read-first');
  const volumeList = document.getElementById('detail-volume-list');
  if(detailTitle) detailTitle.textContent = book.title;
  if(coverTitle) coverTitle.textContent = book.title;
  const authorEl = document.getElementById('detail-book-authors');
  if(authorEl) authorEl.textContent = book.authors || 'Đang cập nhật';
  if(readBtn) readBtn.setAttribute('href', `#reader/${bookVolumesOf(bookId)[0]?.id || ''}/full`);
  const previewBtn = document.getElementById('detail-read-preview');
  if(previewBtn) previewBtn.setAttribute('href', `#reader/${bookVolumesOf(bookId)[0]?.id || ''}/preview`);
  if(volumeList) volumeList.innerHTML = renderVolumeButtons(bookId);
}
async function hydrateBookDetailFromBackend(bookId){
  try{
    const metadata = await fetchReaderMetadataForBook(bookId);
    applyAutoVolumesForBook(bookId, metadata);
    refreshBookDetailDom(bookId, metadata);
  }catch(error){
    console.warn('Kh\u00f4ng t\u1ea3i \u0111\u01b0\u1ee3c metadata EPUB:', error);
  }
}
function applyReaderMetadata(data){
  if(!readerState || !data) return;
  const book = bookById(readerState.bookId);
  const volume = volumeById(readerState.volumeId);
  const displayVolume = readerVolumeText(volume, data.volumeNo);
  if(volume) volume.title = displayVolume;

  const bookTitleEl = document.getElementById('reader-book-title');
  const volumeTitleEl = document.getElementById('reader-current-volume-title');
  const authorEl = document.getElementById('reader-author');
  const chapterLabelEl = document.getElementById('reader-chapter-label');
  if(bookTitleEl) bookTitleEl.textContent = book?.title || data.bookTitle || 'Sách';
  if(authorEl) authorEl.textContent = book?.authors || data.authors || 'Đang cập nhật';
  if(volumeTitleEl) volumeTitleEl.textContent = displayVolume;
  if(chapterLabelEl) chapterLabelEl.textContent = displayVolume;
}
function escapeHtml(value){
  return String(value ?? '')
    .replaceAll('&', '&amp;')
    .replaceAll('<', '&lt;')
    .replaceAll('>', '&gt;')
    .replaceAll('"', '&quot;')
    .replaceAll("'", '&#039;');
}
function formatReaderContent(value){
  const safe = escapeHtml(value || '');
  const paragraphs = safe.split(/\n{2,}/).map(x => x.trim()).filter(Boolean);
  if(!paragraphs.length) return '<p>N\u1ed9i dung trang n\u00e0y \u0111ang \u0111\u01b0\u1ee3c c\u1eadp nh\u1eadt.</p>';
  return paragraphs.map(p => `<p>${p.replace(/\n/g, '<br>')}</p>`).join('');
}
function readerPageId(chunkNo){ return `reader-page-${chunkNo}`; }
function readerPlaceholderHtml(chunkNo){
  return `<div class="reader-page reader-placeholder" id="${readerPageId(chunkNo)}" data-chunk="${chunkNo}" style="min-height:360px;padding-bottom:32px;margin-bottom:28px;border-bottom:1px solid rgba(58,72,61,.12)">
    <div class="reader-page-title">Trang ${chunkNo}</div>
    <p style="color:#777f75">N\u1ed9i dung trang n\u00e0y \u0111\u00e3 \u0111\u01b0\u1ee3c d\u1ecdn kh\u1ecfi b\u1ed9 nh\u1edb t\u1ea1m \u0111\u1ec3 h\u1ea1n ch\u1ebf l\u01b0u n\u1ed9i dung s\u00e1ch tr\u00ean tr\u00ecnh duy\u1ec7t.</p>
    <button class="outline-btn" onclick="reloadReaderChunk(${chunkNo})">T\u1ea3i l\u1ea1i trang ${chunkNo}</button>
  </div>`;
}
function readerLoadedHtml(chunkNo, content){
  return `<div class="reader-page reader-loaded" id="${readerPageId(chunkNo)}" data-chunk="${chunkNo}" style="padding-bottom:32px;margin-bottom:28px;border-bottom:1px solid rgba(58,72,61,.12)">
    <div class="reader-page-title">Trang ${chunkNo}</div>
    <div class="reader-page-body">${formatReaderContent(content)}</div>
  </div>`;
}
function getReaderChunkEl(chunkNo){ return document.getElementById(readerPageId(chunkNo)); }
function isElementNearViewport(el, margin = 520){
  const rect = el.getBoundingClientRect();
  return rect.bottom >= -margin && rect.top <= window.innerHeight + margin;
}
function getCachedReaderChunk(chunkNo){
  if(!readerState) return null;
  const item = readerState.cache.get(Number(chunkNo));
  if(!item) return null;
  if(Date.now() - item.loadedAt > READER_CACHE_TTL_MS){
    readerState.cache.delete(Number(chunkNo));
    return null;
  }
  return item.content;
}
function setCachedReaderChunk(chunkNo, content){
  if(!readerState) return;
  const key = Number(chunkNo);
  readerState.cache.set(key, {content, loadedAt: Date.now()});
  const entries = [...readerState.cache.entries()].sort((a,b) => a[1].loadedAt - b[1].loadedAt);
  while(entries.length > READER_MAX_CACHE_CHUNKS){
    const [oldKey] = entries.shift();
    readerState.cache.delete(oldKey);
  }
}
function updateReaderActiveChunk(){
  if(!readerState) return;
  const pages = [...document.querySelectorAll('.reader-page')];
  if(!pages.length) return;
  let best = pages[0];
  let bestDistance = Infinity;
  pages.forEach(page => {
    const rect = page.getBoundingClientRect();
    const distance = Math.abs(rect.top - 140);
    if(rect.bottom > 120 && distance < bestDistance){
      best = page;
      bestDistance = distance;
    }
  });
  readerState.activeChunk = Number(best.dataset.chunk || readerState.activeChunk || 1);
}
function pruneReaderDom(){
  if(!readerState) return;
  const active = Number(readerState.activeChunk || 1);
  document.querySelectorAll('.reader-page.reader-loaded').forEach(page => {
    const chunkNo = Number(page.dataset.chunk);
    if(Math.abs(chunkNo - active) > READER_DOM_RADIUS){
      page.outerHTML = readerPlaceholderHtml(chunkNo);
    }
  });
}
function renderReaderChunk(chunkNo, content){
  const target = document.getElementById('reader-content');
  if(!target) return;
  const html = readerLoadedHtml(chunkNo, content);
  const current = getReaderChunkEl(chunkNo);
  if(current){
    current.outerHTML = html;
    return;
  }
  const later = [...target.querySelectorAll('.reader-page')].find(el => Number(el.dataset.chunk) > Number(chunkNo));
  if(later) later.insertAdjacentHTML('beforebegin', html);
  else target.insertAdjacentHTML('beforeend', html);
}

function hideReaderFriendlyPopup(){
  const popup = document.getElementById('reader-friendly-popup');
  if(popup) popup.remove();
}
function showReaderFriendlyPopup(message, actionsHtml = ''){
  hideReaderFriendlyPopup();
  if(typeof pushPopupHistory === 'function') pushPopupHistory('reader-friendly-popup');
  const safeMessage = escapeHtml(message || 'Hiện tại tập sách này chưa có nội dung để hiển thị. Bạn vui lòng quay lại thông tin sách và thử tập khác nhé.');
  const actions = actionsHtml || `<button class="reader-popup-button" onclick="closeReaderFriendlyPopupAndReturn()">Đã hiểu</button>`;
  document.body.insertAdjacentHTML('beforeend', `<div id="reader-friendly-popup" class="reader-popup-backdrop">
    <div class="reader-popup-card" role="dialog" aria-modal="true" aria-labelledby="reader-popup-title">
      <button class="reader-popup-close" onclick="closeReaderFriendlyPopupAndReturn()" aria-label="Đóng thông báo">×</button>
      <div class="reader-popup-icon">!</div>
      <h2 id="reader-popup-title">Thông báo đọc sách</h2>
      <p>${safeMessage}</p>
      <div class="reader-popup-actions">${actions}</div>
    </div>
  </div>`);
}
function closeReaderFriendlyPopupAndReturn(){
  const fallbackHash = readerState?.returnHash || '#library';
  hideReaderFriendlyPopup();
  endActiveReaderSession().finally(() => {
    if(location.hash === fallbackHash) route();
    else location.hash = fallbackHash;
  });
}
function hasReaderContent(value){
  return String(value || '').replace(/\s+/g, '').length > 0;
}

function showFullAccessRequiredPopup(reason=''){
  const book = bookById(readerState?.bookId);
  const volume = volumeById(readerState?.volumeId);
  const model = normalizeAccessModel(volume?.access_model_override || book?.default_access_model);
  let message = 'Bạn chưa có quyền đọc full tập này.';
  let actions = `<a class="reader-popup-button secondary" href="#reader/${readerState?.volumeId}/preview" onclick="hideReaderFriendlyPopup()">Đọc preview</a><button class="reader-popup-back-link" onclick="closeReaderFriendlyPopupAndReturn()">Về thông tin sách</button>`;
  if(model === 'PAID'){
    message = 'Tập này cần mua hoặc được nhóm gia đình chia sẻ để đọc full.';
    actions = `<button class="reader-popup-button" onclick="purchasePaidVolume(${Number(readerState?.volumeId || 0)}, false); hideReaderFriendlyPopup();">Mua tập</button><a class="reader-popup-button secondary" href="#reader/${readerState?.volumeId}/preview" onclick="hideReaderFriendlyPopup()">Đọc preview</a><button class="reader-popup-back-link" onclick="closeReaderFriendlyPopupAndReturn()">Về thông tin sách</button>`;
  }else if(model === 'RENTAL'){
    message = 'Bạn cần gói thuê đang hoạt động để đọc full tập này.';
    actions = `<a class="reader-popup-button" href="#subscription" onclick="hideReaderFriendlyPopup()">Quản lý gói thuê</a><a class="reader-popup-button secondary" href="#reader/${readerState?.volumeId}/preview" onclick="hideReaderFriendlyPopup()">Đọc preview</a><button class="reader-popup-back-link" onclick="closeReaderFriendlyPopupAndReturn()">Về thông tin sách</button>`;
  }else if(model === 'FREE'){
    message = 'Bạn cần đăng nhập để đọc full sách miễn phí.';
  }
  showReaderFriendlyPopup(message, actions);
}
async function startReadSession(uiVolumeId, mode='full'){
  const realVolumeId = backendVolumeId(uiVolumeId);
  const requestedMode = String(mode || 'full').toLowerCase() === 'preview' ? 'preview' : 'full';
  const headers = {};

  // WebSocket control channel is optional at API level, but we try to bind it
  // before creating a full UCON session so revoke/end events can be pushed to UI.
  if(requestedMode === 'full' && typeof waitForPepControlConnection === 'function'){
    const connectionId = await waitForPepControlConnection();
    if(connectionId) headers['X-Control-Connection-Id'] = connectionId;
  }

  const res = await fetch(`${API_BASE}/read/try?userId=${currentUserId}&volumeId=${realVolumeId}&mode=${requestedMode.toUpperCase()}`, {
    method:'POST',
    headers
  });
  let data = await res.json().catch(() => ({}));
  data = cleanBackendPayload(data);
  if(!res.ok){
    const error = new Error(data.reason || data.message || 'START_READ_FAILED');
    error.data = data;
    throw error;
  }
  return data;
}
async function loadReaderChunk(chunkNo){
  if(!readerState || readerState.loadingChunks.has(Number(chunkNo)) || readerState.ended) return;
  const total = readerState.totalChunks;
  if(total && Number(chunkNo) > total) return;

  const cached = getCachedReaderChunk(chunkNo);
  if(cached){
    renderReaderChunk(chunkNo, cached);
    pruneReaderDom();
    return;
  }

  readerState.loadingChunks.add(Number(chunkNo));
  const loadingEl = document.getElementById('reader-loading');
  if(loadingEl) loadingEl.textContent = Number(chunkNo) === 1 ? '\u0110ang t\u1ea3i trang \u0111\u1ea7u ti\u00ean...' : '\u0110ang t\u1ea3i trang ti\u1ebfp theo...';

  try{
    const requestedMode = readerState.requestedMode === 'preview' ? 'preview' : 'full';
    if(!readerState.accessChecked){
      if(requestedMode === 'preview'){
        readerState.sessionId = null;
        readerState.accessChecked = true;
        readerState.readMode = 'PREVIEW';
        readerState.accessPath = 'STATIC_PREVIEW';
        readerState.totalChunks = 5;
      }else{
        const savedSessionId = savedReaderSessionId(readerState.volumeId);
        if(savedSessionId){
          readerState.sessionId = savedSessionId;
          readerState.accessChecked = true;
          readerState.readMode = 'FULL';
          readerState.accessPath = 'RESTORED_SESSION';
        }else{
          const session = await startReadSession(readerState.volumeId, 'full');
          readerState.sessionId = session.sessionId || null;
          readerState.accessChecked = true;
          readerState.readMode = 'FULL';
          readerState.accessPath = session.accessPath || null;
          readerState.totalChunks = Number(session.totalChunks || 0) || null;
          applyReaderMetadata(session);
          if(readerState.sessionId) saveReaderSessionId(readerState.volumeId, readerState.sessionId);
          if(!readerState.sessionId){
            const error = new Error(session.reason || 'FULL_ACCESS_REQUIRED');
            error.data = session;
            throw error;
          }
        }
      }
    }

    const chunkUrl = requestedMode === 'preview'
      ? `${API_BASE}/read/preview/chunks?userId=${currentUserId}&volumeId=${backendVolumeId(readerState.volumeId)}&chunkNo=${chunkNo}`
      : `${API_BASE}/read/sessions/${readerState.sessionId}/chunks/${chunkNo}`;
    const res = await fetch(chunkUrl);
    let data = await res.json().catch(() => ({}));
    data = cleanBackendPayload(data);
    if(!res.ok){
      const error = new Error(data.reason || data.message || 'LOAD_CHUNK_FAILED');
      error.data = data;
      throw error;
    }

    if(readerState.requestedMode === 'preview') readerState.totalChunks = Math.min(Number(data.previewLimit || 5), Number(data.totalChunks || 5)) || 5;
    else readerState.totalChunks = Number(data.totalChunks || readerState.totalChunks || 0) || null;
    applyReaderMetadata(data);
    const content = data.content || '';
    if(!hasReaderContent(content)){
      readerState.noContent = true;
      if(loadingEl) loadingEl.textContent = '';
      await endActiveReaderSession();
      showReaderFriendlyPopup('Hi\u1ec7n t\u1ea1i t\u1eadp s\u00e1ch n\u00e0y ch\u01b0a c\u00f3 n\u1ed9i dung \u0111\u1ec3 \u0111\u1ecdc. B\u1ea1n c\u00f3 th\u1ec3 quay l\u1ea1i trang th\u00f4ng tin s\u00e1ch v\u00e0 ch\u1ecdn t\u1eadp kh\u00e1c.');
      return;
    }
    setCachedReaderChunk(chunkNo, content);
    renderReaderChunk(chunkNo, content);
    readerState.hasLoadedContent = true;
    readerState.nextChunk = Math.max(readerState.nextChunk, Number(chunkNo) + 1);

    if(loadingEl){
      if(readerState.totalChunks && readerState.nextChunk > readerState.totalChunks){
        loadingEl.textContent = '\u0110\u00e3 t\u1ea3i h\u1ebft n\u1ed9i dung.';
      }else{
        loadingEl.textContent = 'K\u00e9o xu\u1ed1ng \u0111\u1ec3 t\u1ea3i trang ti\u1ebfp theo.';
      }
    }

    updateReaderActiveChunk();
    pruneReaderDom();
  }catch(error){
    console.error('Không tải được nội dung:', error);
    if(loadingEl) loadingEl.textContent = '';
    const reason = error?.message || error?.data?.reason || '';
    if(['SESSION_NOT_ACTIVE','DENY_OR_REVOKED','DEPENDENCY_DELETED'].includes(reason)){
      clearSavedReaderSessionId(readerState?.volumeId);
    }
    if(['FULL_ACCESS_REQUIRED','PAID_FULL_REQUIRED','RENTAL_FULL_REQUIRED','VOLUME_PURCHASE_REQUIRED','POLICY_NOT_SATISFIED','DENY','SESSION_NOT_ACTIVE','DENY_OR_REVOKED','DEPENDENCY_DELETED'].includes(reason) || readerState?.requestedMode === 'full'){
      showFullAccessRequiredPopup(reason);
    }else if(['PREVIEW_LIMIT_REACHED','PREVIEW_ONLY_FIRST_VOLUME','RENTAL_PREVIEW_ONLY','PAID_PREVIEW_ONLY'].includes(reason)){
      showPreviewOnlyPopup(bookById(readerState?.bookId), reason);
    }else{
      showReaderFriendlyPopup('Tụi mình chưa tải được nội dung tập sách này. Bạn vui lòng quay lại trang thông tin sách và thử lại sau nhé.');
    }
  }finally{
    readerState?.loadingChunks.delete(Number(chunkNo));
  }
}
function reloadReaderChunk(chunkNo){
  if(readerState) readerState.cache.delete(Number(chunkNo));
  loadReaderChunk(Number(chunkNo));
}
function handleReaderScroll(){
  if(!readerState) return;
  updateReaderActiveChunk();
  document.querySelectorAll('.reader-placeholder').forEach(placeholder => {
    const chunkNo = Number(placeholder.dataset.chunk);
    if(isElementNearViewport(placeholder)) loadReaderChunk(chunkNo);
  });
  const nearBottom = window.innerHeight + window.scrollY >= document.body.offsetHeight - 520;
  if(nearBottom && (!readerState.totalChunks || readerState.nextChunk <= readerState.totalChunks)){
    loadReaderChunk(readerState.nextChunk);
  }
  pruneReaderDom();
}
function initReaderAutoScroll(){
  readerScrollHandler = handleReaderScroll;
  window.addEventListener('scroll', readerScrollHandler, {passive:true});
}
function endActiveReaderSession(){
  const state = readerState;
  if(!state || !state.sessionId || state.ended) return Promise.resolve();
  clearSavedReaderSessionId(state.volumeId);
  state.ended = true;
  if(state.hasLoadedContent){
    saveReadHistorySession({
      id: state.sessionId,
      sessionId: state.sessionId,
      userId: currentUserId,
      bookId: state.bookId,
      uiVolumeId: state.volumeId,
      bookVolumeId: backendVolumeId(state.volumeId),
      status: 'ended',
      phase: 'post',
      created_at: currentSqlDateTime(),
      ended_at: currentSqlDateTime(),
      post_handled_at: currentSqlDateTime()
    });
    readHistoryLoadedFromBackend = false;
  }
  const url = `${API_BASE}/read/sessions/${state.sessionId}/end`;
  if(navigator.sendBeacon){
    try{
      const ok = navigator.sendBeacon(url, new Blob([], {type:'text/plain'}));
      if(ok) return Promise.resolve();
    }catch(error){
      console.warn('Kh\u00f4ng th\u1ec3 d\u00f9ng sendBeacon \u0111\u1ec3 k\u1ebft th\u00fac phi\u00ean \u0111\u1ecdc:', error);
    }
  }
  return fetch(url, {method:'POST', keepalive:true}).catch(error => {
    console.warn('Kh\u00f4ng th\u1ec3 k\u1ebft th\u00fac phi\u00ean \u0111\u1ecdc:', error);
  });
}
function cleanupReader(){
  disableReaderCopyProtection();
  hideReaderFriendlyPopup();
  if(readerScrollHandler){
    window.removeEventListener('scroll', readerScrollHandler);
    readerScrollHandler = null;
  }
  if(readerObserver){
    readerObserver.disconnect();
    readerObserver = null;
  }
  endActiveReaderSession();
  readerState = null;
}



function handleUsageSessionRevoked(message){
  const usageSessionId = Number(message?.usageSessionId || 0);
  if(!readerState || !usageSessionId || Number(readerState.sessionId) !== usageSessionId) return;

  clearSavedReaderSessionId(readerState.volumeId);
  readerState.ended = true;
  readerState.cache?.clear?.();
  readerState.loadingChunks?.clear?.();

  const loadingEl = document.getElementById('reader-loading');
  if(loadingEl) loadingEl.textContent = 'Phiên đọc đã bị thu hồi.';

  showReaderFriendlyPopup(
    `Phiên đọc đã bị thu hồi bởi UCON${message?.reason ? `: ${message.reason}` : ''}. Bạn vui lòng quay lại thông tin sách hoặc thử lại sau.`,
    `<button class="reader-popup-button" onclick="closeReaderFriendlyPopupAndReturn()">Về thông tin sách</button>`
  );
}

function handleUsageSessionEnded(message){
  const usageSessionId = Number(message?.usageSessionId || 0);
  if(!readerState || !usageSessionId || Number(readerState.sessionId) !== usageSessionId) return;
  clearSavedReaderSessionId(readerState.volumeId);
  readerState.ended = true;
}

window.handleUsageSessionRevoked = handleUsageSessionRevoked;
window.handleUsageSessionEnded = handleUsageSessionEnded;
