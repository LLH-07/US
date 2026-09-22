// Current user, profile helpers, history, notification state.
function refreshCurrentUserFromUsers(){
  currentUser = users.find(u => Number(u.id) === Number(currentUserId)) || users[0];
}
function setCurrentUser(userId){
  currentUserId = Number(userId) || 1;
  sessionStorage.setItem('ucon_current_user_id', String(currentUserId));
  refreshCurrentUserFromUsers();
  subscriptionLoadedFromBackend = false;
  subscriptionHydrating = false;
  familyLoadedFromBackend = false;
  familyHydrating = false;
  readHistoryLoadedFromBackend = false;
  readHistoryHydrating = false;
  purchaseLibraryLoadedFromBackend = false;
  purchaseLibraryHydrating = false;
  purchasedLibrary.splice(0, purchasedLibrary.length);
  familyPage = 1;
  historyVisible = 4;
  usageSessions.splice(0, usageSessions.length);
  userRentals.splice(0, userRentals.length);
  if(typeof syncCurrentFamilyNotifications === 'function') syncCurrentFamilyNotifications();
  if(typeof reconnectPepControlChannel === 'function') reconnectPepControlChannel();
}

function handleLogin(event){
  event.preventDefault();
  const email = (document.getElementById('login-email')?.value || currentUser.email || '').trim().toLowerCase();
  const password = (document.getElementById('login-password')?.value || '').trim();

  if(email === 'admin@ucon.vn'){
    window.location.href = 'admin.html#dashboard';
    return;
  }

  const user = users.find(u => String(u.email || '').toLowerCase() === email);
  if(!user){
    showSystemNotice('Không tìm thấy tài khoản', 'Bạn có thể dùng một email có trong data.sql, ví dụ nguyenbinh123@gmail.com.');
    return;
  }
  if(password && password !== 'demo' && password !== '123456'){
    showSystemNotice('Sai mật khẩu', 'Mật khẩu demo là demo hoặc 123456.');
    return;
  }

  setCurrentUser(user.id);
  location.hash = 'library';
}
function logoutUser(){
  if(typeof disconnectPepControlChannel === 'function') disconnectPepControlChannel();
  cleanupReader();
  sessionStorage.removeItem('ucon_current_user_id');
  setCurrentUser(1);
  location.hash = 'login';
}

function initials(name){ return name.split(' ').map(x => x[0]).slice(-2).join('').toUpperCase(); }
function money(v){ return Number(v || 0).toLocaleString('vi-VN') + '\u0111'; }
function dateVN(value){
  if(!value) return '-';
  const [date] = String(value).split(' ');
  const [y,m,d] = date.split('-');
  return y && m && d ? `${d}/${m}/${y}` : value;
}
function badgeClass(type){
  if(type === 'FREE') return 'free';
  if(type === 'RENTAL') return 'rental';
  if(type === 'PAID') return 'paid';
  return 'free';
}
function typeLabel(type){
  if(type === 'FREE') return 'MI\u1ec4N PH\u00cd';
  if(type === 'RENTAL') return 'THU\u00ca';
  return 'TR\u1ea2 PH\u00cd';
}
function bookVolumesOf(bookId){ return bookVolumes.filter(v => Number(v.book_id) === Number(bookId) && v.status === 'active'); }
function volumeById(id){ return bookVolumes.find(v => Number(v.id) === Number(id)); }
function bookById(id){ return books.find(b => Number(b.id) === Number(id)); }
function volumeLabel(volume){
  const b = bookById(volume.book_id);
  return `${b?.title || 'S\u00e1ch'} \u00b7 ${volume.title}`;
}
function effectiveAccessModel(book, volume){ return volume?.access_model_override || book?.default_access_model || 'FREE'; }
function userRental(){ return userRentals.find(r => Number(r.user_id) === currentUserId) || null; }
function rentalStatusLabel(status){
  if(status === 'active') return '\u0110ang ho\u1ea1t \u0111\u1ed9ng';
  if(status === 'expired') return '\u0110\u00e3 h\u1ebft h\u1ea1n';
  if(status === 'cancelled') return '\u0110\u00e3 h\u1ee7y';
  return 'Ch\u01b0a \u0111\u0103ng k\u00fd';
}
function rentalClass(status){ return status === 'active' ? 'active' : ''; }
function subscriptionActions(){
  const rental = userRental();
  const status = rental?.status || 'none';
  if(status === 'active' && rental?.plan_id) return `<button class="profile-action danger-subtle" onclick="cancelRentalPlan()">Hủy gói</button>`;
  return '';
}
function membershipRole(group){ return Number(group.owner_user_id) === currentUserId ? 'Ch\u1ee7 s\u1edf h\u1eefu' : 'Th\u00e0nh vi\u00ean'; }
function userMemberships(){ return familyGroupMembers.filter(m => Number(m.user_id) === currentUserId && m.status === 'active'); }
function groupsOfCurrentUser(){
  const ids = new Set(userMemberships().map(m => Number(m.group_id)));
  return familyGroups.filter(g => (ids.has(Number(g.id)) || Number(g.owner_user_id) === currentUserId) && g.status === 'active');
}
function groupMember(groupId){ return familyGroupMembers.find(m => Number(m.group_id) === Number(groupId) && Number(m.user_id) === currentUserId); }
function groupMembers(groupId){ return familyGroupMembers.filter(m => Number(m.group_id) === Number(groupId) && m.status === 'active'); }
function groupSharedVolumes(groupId){ return familySharedVolumes.filter(x => Number(x.group_id) === Number(groupId) && x.status === 'active'); }
function groupSharedBooks(groupId){
  const map = new Map();
  groupSharedVolumes(groupId).forEach(share => {
    const volume = volumeById(share.volume_id);
    const bookId = Number(volume?.book_id || share.book_id || 0);
    if(!bookId) return;
    const book = bookById(bookId) || {
      id: bookId,
      title: share.book_title || 'Sách đang chia sẻ',
      authors: share.authors || 'Đang cập nhật',
      default_access_model: 'PAID',
      status: 'active',
      contentAvailable: true
    };
    if(normalizeAccessModel(book.default_access_model) !== 'PAID') return;
    const key = String(book.id);
    if(!map.has(key)){
      map.set(key, {book, shares: [], firstSharedAt: share.shared_at || '', firstVolumeId: Number(volume?.id || share.volume_id)});
    }
    const entry = map.get(key);
    entry.shares.push(share);
    if(!entry.firstSharedAt || (share.shared_at && share.shared_at < entry.firstSharedAt)) entry.firstSharedAt = share.shared_at;
    const volumeNo = Number(volume?.volume_no || share.volume_no || 1);
    if(volumeNo === 1 || !entry.firstVolumeId) entry.firstVolumeId = Number(volume?.id || share.volume_id);
  });
  return [...map.values()].sort((a,b) => String(a.book.title).localeCompare(String(b.book.title)));
}

function paidPurchasedBooks(){
  return purchasedLibrary
    .filter(item => normalizeAccessModel(item.default_access_model) === 'PAID')
    .map(item => {
      const fromCatalog = bookById(item.book_id);
      if(fromCatalog) return fromCatalog;
      return {
        id: Number(item.book_id),
        title: item.title || item.book_title || 'Sách đã mua',
        authors: item.authors || 'Đang cập nhật',
        genre: item.genre || 'ebook',
        default_access_model: 'PAID',
        status: 'active',
        contentAvailable: true
      };
    })
    .filter(book => Number(book.id) && normalizeAccessModel(book.default_access_model) === 'PAID' && book.status === 'active');
}
function goToSubscriptionPage(event){
  if(event) event.preventDefault();
  location.hash = '#subscription';
}

function currentSqlDateTime(){
  const d = new Date();
  const pad = n => String(n).padStart(2, '0');
  return `${d.getFullYear()}-${pad(d.getMonth()+1)}-${pad(d.getDate())} ${pad(d.getHours())}:${pad(d.getMinutes())}:${pad(d.getSeconds())}`;
}
function historySortValue(session){
  return new Date((session.ended_at || session.last_eval_at || session.activated_at || session.created_at || '').replace(' ', 'T')).getTime() || 0;
}
function isFinishedReadSession(session){
  const status = String(session?.status || '').toLowerCase();
  const phase = String(session?.phase || '').toLowerCase();
  return status === 'ended' || status === 'completed' || phase === 'post' || Boolean(session?.ended_at || session?.endedAt || session?.post_handled_at || session?.postHandledAt);
}
function readerVolumeText(volume, fallbackNo){
  const no = Number(volume?.volume_no || volume?.volumeNo || fallbackNo || 1);
  return `T\u1eadp ${String(no).padStart(2,'0')}`;
}
function loadStoredReadHistory(){
  try{
    const raw = localStorage.getItem(readHistoryStorageKey());
    const parsed = raw ? JSON.parse(raw) : [];
    return Array.isArray(parsed) ? parsed : [];
  }catch(error){
    console.warn('Kh\u00f4ng \u0111\u1ecdc \u0111\u01b0\u1ee3c l\u1ecbch s\u1eed \u0111\u1ecdc local:', error);
    return [];
  }
}
function storeReadHistory(items){
  try{
    localStorage.setItem(readHistoryStorageKey(), JSON.stringify(items.slice(0, 30)));
  }catch(error){
    console.warn('Kh\u00f4ng l\u01b0u \u0111\u01b0\u1ee3c l\u1ecbch s\u1eed \u0111\u1ecdc local:', error);
  }
}
function normalizeHistorySession(item){
  const backendVolume = Number(item.bookVolumeId || item.book_volume_id || item.resource_id || item.volumeId || 0);
  const backendBookId = Number(item.bookId || item.book_id || 1);
  const bookId = uiBookIdFromBackendBookId(backendBookId);
  const uiVolume = item.uiVolumeId || item.ui_volume_id || uiVolumeIdFromBackend(bookId, backendVolume || 1);
  return {
    id: item.id || item.sessionId || item.session_id || `local-${uiVolume}-${item.created_at || currentSqlDateTime()}`,
    subject_user_id: Number(item.subject_user_id || item.userId || item.user_id || currentUserId),
    resource_type: 'book_volume',
    resource_id: Number(uiVolume),
    backend_volume_id: backendVolume || backendVolumeId(uiVolume),
    book_id: Number(item.book_id || item.bookId || 0) || uiBookIdFromBackendBookId(backendBookId),
    book_title: item.book_title || item.bookTitle || '',
    authors: item.authors || '',
    volume_title: item.volume_title || item.volumeTitle || '',
    action_id: 'read',
    access_path: item.access_path || item.accessPath || 'RENTAL',
    phase: String(item.phase || 'post').toLowerCase(),
    status: String(item.status || 'ended').toLowerCase(),
    created_at: item.created_at || item.createdAt || currentSqlDateTime(),
    activated_at: item.activated_at || item.activatedAt || item.created_at || item.createdAt || '',
    last_eval_at: item.last_eval_at || item.lastEvalAt || '',
    ended_at: item.ended_at || item.endedAt || item.post_handled_at || item.postHandledAt || item.last_eval_at || item.lastEvalAt || item.created_at || item.createdAt || '',
    revoked_at: item.revoked_at || item.revokedAt || '',
    revoked_reason: item.revoked_reason || item.revokedReason || '',
    post_handled_at: item.post_handled_at || item.postHandledAt || ''
  };
}
function mergeReadHistory(items){
  const map = new Map();
  [...loadStoredReadHistory(), ...items.map(normalizeHistorySession)].forEach(item => {
    const key = String(item.id || `${item.resource_id}-${item.created_at}`);
    const existing = map.get(key);
    if(!existing || historySortValue(item) >= historySortValue(existing)) map.set(key, item);
  });
  const merged = [...map.values()]
    .filter(s => Number(s.subject_user_id) === currentUserId && s.resource_type === 'book_volume' && s.action_id === 'read' && isFinishedReadSession(s))
    .sort((a,b) => historySortValue(b) - historySortValue(a))
    .slice(0, 30);
  storeReadHistory(merged);
  return merged;
}
function saveReadHistorySession(session){
  mergeReadHistory([session]);
}
function historyBookId(session){
  const directBookId = Number(session.book_id || session.bookId || 0);
  if(directBookId) return directBookId;
  const volume = volumeById(session.resource_id);
  if(volume?.book_id) return Number(volume.book_id);
  const resourceId = Number(session.resource_id || session.bookVolumeId || session.book_volume_id || 0);
  return resourceId >= 100 ? Math.floor(resourceId / 100) : 0;
}
function readHistory(){
  const sessions = mergeReadHistory(usageSessions)
    .filter(s => Number(s.subject_user_id) === currentUserId && s.resource_type === 'book_volume' && s.action_id === 'read' && isFinishedReadSession(s))
    .sort((a,b) => historySortValue(b) - historySortValue(a));

  const byBook = new Map();
  sessions.forEach(session => {
    const bookId = historyBookId(session);
    if(!bookId) return;
    const key = String(bookId);
    const normalized = {...session, book_id: bookId};
    const existing = byBook.get(key);
    if(!existing || historySortValue(normalized) > historySortValue(existing)) byBook.set(key, normalized);
  });

  return [...byBook.values()]
    .sort((a,b) => historySortValue(b) - historySortValue(a))
    .slice(0, 30);
}

async function hydrateReadHistoryFromBackend(){
  if(readHistoryHydrating || readHistoryLoadedFromBackend) return;
  readHistoryHydrating = true;
  try{
    const res = await fetch(`${API_BASE}/read/history?userId=${currentUserId}`);
    let data = await res.json().catch(() => []);
    data = cleanBackendPayload(data);
    if(res.ok && Array.isArray(data)){
      mergeReadHistory(data);
      readHistoryLoadedFromBackend = true;
      if((location.hash || '#login').replace('#','').split('/')[0] === 'profile') profile();
    }
  }catch(error){
    console.warn('Kh\u00f4ng t\u1ea3i \u0111\u01b0\u1ee3c l\u1ecbch s\u1eed \u0111\u1ecdc t\u1eeb backend:', error);
  }finally{
    readHistoryHydrating = false;
  }
}

function notificationStorageKeyForUser(userId = currentUserId){ return `ucon_family_notifications_user_${Number(userId)}`; }
function loadFamilyNotifications(userId = currentUserId){
  try{
    return JSON.parse(localStorage.getItem(notificationStorageKeyForUser(userId)) || '[]') || [];
  }catch(error){
    return [];
  }
}
function storeFamilyNotifications(items, userId = currentUserId){
  try{ localStorage.setItem(notificationStorageKeyForUser(userId), JSON.stringify(items.slice(0, 20))); }catch(error){ console.warn('Không lưu được thông báo gia đình:', error); }
}
function syncCurrentFamilyNotifications(){
  familyNotifications = loadFamilyNotifications(currentUserId);
  renderNotificationIndicator();
}
function pushFamilyNotification(targetUserId, message, groupId = null){
  const id = `noti-${Date.now()}-${Math.random().toString(16).slice(2)}`;
  const items = loadFamilyNotifications(targetUserId);
  items.unshift({id, message, groupId, createdAt: currentSqlDateTime(), read:false});
  storeFamilyNotifications(items, targetUserId);
  if(Number(targetUserId) === currentUserId){
    familyNotifications = items;
    renderNotificationIndicator();
  }
}
function unreadNotificationCount(){ return familyNotifications.filter(n => !n.read).length; }
function renderNotificationIndicator(){
  const badge = document.getElementById('notification-badge');
  const count = unreadNotificationCount();
  if(badge){
    badge.textContent = count > 9 ? '9+' : String(count);
    badge.style.display = count ? 'inline-flex' : 'none';
  }
}
function toggleNotificationPopover(){
  notificationPopoverOpen = !notificationPopoverOpen;
  renderNotificationPopover();
}
function renderNotificationPopover(){
  const old = document.getElementById('notification-popover');
  if(old) old.remove();
  if(!notificationPopoverOpen) return;
  const button = document.getElementById('notification-button');
  const items = familyNotifications.slice(0, 8);
  const html = `<div id="notification-popover" class="notification-popover">
    <div class="notification-head"><b>Thông báo</b><button onclick="markAllNotificationsRead()">Đánh dấu đã đọc</button></div>
    ${items.length ? items.map(n => `<button class="notification-item ${n.read ? '' : 'unread'}" onclick="openFamilyNotification('${n.id}')"><span>${escapeHtml(n.message)}</span><small>${dateVN(n.createdAt)}</small></button>`).join('') : '<p class="muted" style="margin:12px 0 0">Chưa có thông báo mới.</p>'}
  </div>`;
  document.body.insertAdjacentHTML('beforeend', html);
  const pop = document.getElementById('notification-popover');
  if(button && pop){
    const rect = button.getBoundingClientRect();
    pop.style.top = `${rect.bottom + 10 + window.scrollY}px`;
    pop.style.right = `${Math.max(18, window.innerWidth - rect.right)}px`;
  }
}
function markAllNotificationsRead(){
  familyNotifications = familyNotifications.map(n => ({...n, read:true}));
  storeFamilyNotifications(familyNotifications);
  renderNotificationIndicator();
  renderNotificationPopover();
}
function openFamilyNotification(id){
  const item = familyNotifications.find(n => n.id === id);
  familyNotifications = familyNotifications.map(n => n.id === id ? {...n, read:true} : n);
  storeFamilyNotifications(familyNotifications);
  notificationPopoverOpen = false;
  renderNotificationIndicator();
  renderNotificationPopover();
  familyLoadedFromBackend = false;
  hydrateFamiliesFromBackend(true).finally(() => {
    if(item?.groupId) location.hash = `#family/${item.groupId}`;
    else location.hash = '#families';
  });
}
window.addEventListener('storage', event => {
  if(event.key === notificationStorageKeyForUser(currentUserId)){
    familyNotifications = loadFamilyNotifications();
    familyLoadedFromBackend = false;
    hydrateFamiliesFromBackend(true);
    renderNotificationIndicator();
  }
});

