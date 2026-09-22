// API hydrate/sync services and domain actions.
function readHistoryStorageKey(){ return `ucon_read_history_v9_user_${currentUserId}`; }
function syncCatalogFromPayload(data){
  const safeData = cleanBackendPayload(data || {});
  if(Array.isArray(safeData.books)){
    const readableBooks = safeData.books.filter(b => b.contentAvailable !== false && b.content_available !== false);
    books.splice(0, books.length, ...readableBooks.map(b => ({
      id: Number(b.id),
      title: b.title || 'Sách chưa có tên',
      genre: b.genre || 'ebook',
      authors: b.authors || 'Đang cập nhật',
      short_description: b.short_description || b.shortDescription || 'Nội dung sách được lấy trực tiếp từ file EPUB.',
      default_access_model: b.default_access_model || b.defaultAccessModel || 'PAID',
      status: String(b.status || 'ACTIVE').toLowerCase(),
      contentAvailable: true
    })).filter(b => Number.isFinite(b.id)));
  }
  if(Array.isArray(safeData.volumes)){
    bookVolumes.splice(0, bookVolumes.length, ...safeData.volumes
      .filter(v => v.contentAvailable !== false && v.content_available !== false)
      .map(v => ({
        id: Number(v.id),
        book_id: Number(v.book_id || v.bookId),
        title: `Tập ${String(v.volume_no || v.volumeNo || 1).padStart(2,'0')}`,
        volume_no: Number(v.volume_no || v.volumeNo || 1),
        price: Number(v.price || 0),
        access_model_override: v.access_model_override || v.accessModelOverride || 'PAID',
        read_count: Math.max(0, Number(v.read_count ?? v.readCount ?? 0) || 0),
        status: String(v.status || 'ACTIVE').toLowerCase(),
        contentAvailable: true
      }))
      .filter(v => Number.isFinite(v.id) && Number.isFinite(v.book_id)));
  }
}

function catalogLoadingView(message='Đang tải danh mục sách...'){
  app.innerHTML = `${nav('library')}<section class="section"><p class="eyebrow">Kho lưu trữ</p><h1 class="h1">Thư viện của MH Library Demo</h1><div class="empty-read-history">${message}</div></section><div class="footer-block"></div>`;
}

async function hydrateCatalogFromBackend(force = false){
  if(catalogHydrating || (catalogLoadedFromBackend && !force)) return;
  catalogHydrating = true;
  try{
    const res = await fetch(`${API_BASE}/read/catalog`);
    let data = await res.json().catch(() => ({}));
    data = cleanBackendPayload(data);
    if(!res.ok) throw new Error(data.reason || data.message || 'LOAD_CATALOG_FAILED');
    syncCatalogFromPayload(data);
    catalogLoadedFromBackend = true;
    const hash = location.hash || '#login';
    if(hash.startsWith('#library')) library(libraryPage, false);
    if(hash.startsWith('#book/')) route();
    if(hash.startsWith('#reader/')) route();
  }catch(error){
    console.warn('Không tải được danh mục sách từ backend:', error);
    catalogLoadingView('Không tải được danh mục sách. Bạn tải lại trang hoặc thử lại sau.');
  }finally{
    catalogHydrating = false;
  }
}

function isCurrentAdmin(){ return Number(currentUserId) === Number(DEMO_ADMIN_USER_ID); }

async function syncEbooksAsAdmin(){
  if(!isCurrentAdmin()){
    showSystemNotice('Chỉ admin mới được thêm sách', 'Tài khoản thường chỉ được đọc, mua, thuê và tham gia nhóm gia đình. Muốn thêm sách, hãy đăng nhập tài khoản admin demo.');
    return;
  }
  const button = document.getElementById('admin-sync-books-btn');
  if(button){ button.disabled = true; button.textContent = 'Đang đồng bộ...'; }
  try{
    const res = await fetch(`${API_BASE}/admin/books/sync-ebooks?adminUserId=${currentUserId}`, {method:'POST'});
    let data = await res.json().catch(() => ({}));
    data = cleanBackendPayload(data);
    if(!res.ok) throw new Error(data.reason || data.message || 'SYNC_EBOOKS_FAILED');
    syncCatalogFromPayload(data);
    catalogLoadedFromBackend = true;
    const lastPage = Math.max(1, Math.ceil(books.length / LIBRARY_PAGE_SIZE));
    showSystemNotice('Đã đồng bộ sách', `Danh mục hiện có ${books.length} sách hợp lệ. Sách mới sẽ nằm ở trang cuối của thư viện.`, () => {
      location.hash = `#library/${lastPage}`;
    });
  }catch(error){
    console.error('Không đồng bộ được EPUB:', error);
    showSystemNotice('Không đồng bộ được sách', error.message === 'ADMIN_ONLY_CAN_IMPORT_BOOKS'
      ? 'Chỉ admin demo mới được thêm sách.'
      : 'Bạn thử lại sau nha.');
  }finally{
    if(button){ button.disabled = false; button.textContent = 'Đồng bộ EPUB đã paste'; }
  }
}

function normalizeAccessModel(value){
  return String(value || 'FREE').toUpperCase();
}
function hasActiveSubscription(){
  const rental = userRental();
  return rental?.status === 'active' && rental?.plan_id;
}
function purchasedVolumeIdsForBook(bookId){
  const item = purchasedLibrary.find(entry => Number(entry.book_id || entry.bookId || entry.id) === Number(bookId));
  return Array.isArray(item?.purchased_volume_ids) ? item.purchased_volume_ids.map(Number).filter(Boolean) : [];
}
function hasPurchasedBook(bookId){
  return purchasedVolumeIdsForBook(bookId).length > 0;
}
function hasPurchasedVolume(volumeId){
  const volume = volumeById(volumeId);
  if(!volume) return false;
  return purchasedVolumeIdsForBook(volume.book_id).includes(Number(volumeId));
}
function hasFamilySharedVolume(volumeId){
  if(!volumeId) return false;
  const activeGroupIds = new Set(groupsOfCurrentUser().map(g => Number(g.id)));
  return familySharedVolumes.some(s => s.status === 'active' && Number(s.volume_id) === Number(volumeId) && activeGroupIds.has(Number(s.group_id)));
}
function hasAnyPaidVolumeAccess(bookId){
  return bookVolumesOf(bookId).some(v => hasPurchasedVolume(v.id) || hasFamilySharedVolume(v.id));
}
function canReadFullVolume(volume){
  if(!volume) return false;
  const book = bookById(volume.book_id);
  const model = normalizeAccessModel(volume.access_model_override || book?.default_access_model);
  if(model === 'FREE') return currentUserId > 0;
  if(model === 'RENTAL') return !!hasActiveSubscription();
  if(model === 'PAID') return hasPurchasedVolume(volume.id) || hasFamilySharedVolume(volume.id);
  return false;
}
function syncPurchaseLibraryFromPayload(data){
  const items = Array.isArray(data) ? data : (Array.isArray(data?.library) ? data.library : []);
  purchasedLibrary.splice(0, purchasedLibrary.length, ...items.map(item => ({
    book_id: Number(item.book_id || item.bookId || item.id),
    title: item.book_title || item.bookTitle || item.title || 'Sách đã mua',
    genre: item.genre || 'ebook',
    authors: item.authors || 'Đang cập nhật',
    default_access_model: item.default_access_model || item.defaultAccessModel || 'PAID',
    purchased_volume_count: Number(item.purchased_volume_count || item.purchasedVolumeCount || 0),
    purchased_volume_ids: Array.isArray(item.purchased_volume_ids || item.purchasedVolumeIds)
      ? (item.purchased_volume_ids || item.purchasedVolumeIds).map(Number)
      : String(item.purchased_volume_ids || item.purchasedVolumeIds || '').split(',').map(x => Number(x.trim())).filter(Boolean),
    first_purchased_at: item.first_purchased_at || item.firstPurchasedAt || '',
    last_purchased_at: item.last_purchased_at || item.lastPurchasedAt || ''
  })).filter(item => item.book_id));
}
async function hydratePurchaseLibraryFromBackend(force = false){
  if(purchaseLibraryHydrating || (purchaseLibraryLoadedFromBackend && !force)) return;
  purchaseLibraryHydrating = true;
  let shouldRefreshRoute = false;
  try{
    const res = await fetch(`${API_BASE}/purchases/library?userId=${currentUserId}`);
    let data = await res.json().catch(() => []);
    data = cleanBackendPayload(data);
    if(!res.ok) throw new Error(data.reason || data.message || 'LOAD_PURCHASE_LIBRARY_FAILED');
    syncPurchaseLibraryFromPayload(data);
    purchaseLibraryLoadedFromBackend = true;
    shouldRefreshRoute = true;
  }catch(error){
    console.warn('Không tải được thư viện sách đã mua từ backend:', error);
  }finally{
    purchaseLibraryHydrating = false;
  }

  if(shouldRefreshRoute){
    const parts = (location.hash || '#login').replace('#','').split('/');
    const routeName = parts[0];
    if(routeName === 'profile') profile(false);
    if(routeName === 'book') route();
    if(routeName === 'family' && parts[2] === 'shared') familySharedManager(parts[1]);
  }
}

function refreshVisibleBookDetail(bookId){
  const parts = (location.hash || '').replace('#','').split('/');
  if(parts[0] === 'book' && Number(parts[1]) === Number(bookId)){
    const list = document.getElementById('detail-volume-list');
    if(list) list.innerHTML = renderVolumeButtons(bookId);
    const book = bookById(bookId);
    const firstVolume = bookVolumesOf(bookId)[0] || null;
    const readBtn = document.getElementById('detail-read-first');
    const previewBtn = document.getElementById('detail-read-preview');
    if(readBtn && firstVolume) readBtn.setAttribute('href', `#reader/${firstVolume.id}/full`);
    if(previewBtn && firstVolume) previewBtn.setAttribute('href', `#reader/${firstVolume.id}/preview`);
    const buyBtn = document.getElementById('detail-buy-book-btn');
    if(buyBtn){
      const missing = bookVolumesOf(bookId)
        .filter(v => normalizeAccessModel(v.access_model_override || bookById(bookId)?.default_access_model) === 'PAID')
        .filter(v => !hasPurchasedVolume(v.id));
      buyBtn.style.display = missing.length ? 'inline-flex' : 'none';
    }
  }
}
function returnToBookDetail(bookId){
  const targetHash = `#book/${Number(bookId)}`;
  if(location.hash === targetHash) bookDetail(Number(bookId));
  else location.hash = targetHash;
}
function hasFamilySharedBook(bookId){
  if(!bookId) return false;
  const activeGroupIds = new Set(groupsOfCurrentUser().map(g => Number(g.id)));
  return familySharedVolumes.some(s => {
    if(s.status !== 'active' || !activeGroupIds.has(Number(s.group_id))) return false;
    const volume = volumeById(s.volume_id);
    return Number(volume?.book_id) === Number(bookId);
  });
}
function canReadFullBook(book){
  const model = normalizeAccessModel(book?.default_access_model);
  if(model === 'FREE') return currentUserId > 0;
  if(model === 'RENTAL') return !!hasActiveSubscription();
  if(model === 'PAID') return hasAnyPaidVolumeAccess(book?.id);
  return false;
}
function isPreviewOnlyBook(book){
  return !canReadFullBook(book);
}
function previewActionButtons(book){
  const model = normalizeAccessModel(book?.default_access_model);
  const back = `<button class="reader-popup-button secondary" onclick="closeReaderFriendlyPopupAndReturn()">Trở về thông tin sách</button>`;
  if(model === 'RENTAL') return `${back}<button class="reader-popup-button" onclick="goToSubscriptionFromReader()">Đăng ký gói thuê</button>`;
  if(model === 'PAID') return `${back}<button class="reader-popup-button" onclick="purchaseCurrentReaderVolume()">Mua tập này</button>`;
  return `${back}`;
}
function previewMessageForBook(book, reason=''){
  const model = normalizeAccessModel(book?.default_access_model);
  if(reason === 'VOLUME_PURCHASE_REQUIRED') return 'Bạn cần mua tập này để tiếp tục đọc.';
  if(reason === 'PREVIEW_ONLY_FIRST_VOLUME') return 'Bạn đang ở chế độ đọc thử. Mỗi tập đều có thể xem trước một số trang nhất định.';
  if(reason === 'PREVIEW_LIMIT_REACHED') return 'Bạn đã đọc hết phần đọc thử của tập này.';
  if(model === 'RENTAL') return 'Sách này thuộc nhóm sách thuê. Khi chưa có gói thuê đang hoạt động, mỗi tập chỉ được đọc thử một số trang.';
  if(model === 'PAID') return 'Sách này là sách trả phí. Mỗi tập chưa mua vẫn có thể đọc thử; chỉ tập đã mua hoặc được chia sẻ mới đọc toàn bộ.';
  return 'Bạn chỉ đang đọc thử tối đa 5 trang của tập hiện tại.';
}
function showPreviewOnlyPopup(book, reason=''){
  const targetBook = book || bookById(readerState?.bookId) || books[0];
  showReaderFriendlyPopup(previewMessageForBook(targetBook, reason), previewActionButtons(targetBook));
}
function goToSubscriptionFromReader(){
  hideReaderFriendlyPopup();
  endActiveReaderSession().finally(() => location.hash = '#subscription');
}
async function purchasePaidVolume(volumeId, startAfter = false){
  const targetVolume = volumeById(volumeId);
  const targetBook = bookById(targetVolume?.book_id);
  if(!targetVolume || !targetBook) return;
  try{
    const res = await fetch(`${API_BASE}/purchases/volume?userId=${currentUserId}&volumeId=${Number(volumeId)}`, {method:'POST'});
    let data = await res.json().catch(() => ({}));
    data = cleanBackendPayload(data);
    if(!res.ok) throw new Error(data.reason || data.message || 'PURCHASE_FAILED');
    syncPurchaseLibraryFromPayload(data);
    purchaseLibraryLoadedFromBackend = true;
    await hydrateFamiliesFromBackend(true);
    refreshVisibleBookDetail(targetBook.id);
    hideReaderFriendlyPopup();
    const volumeLabel = `Tập ${String(targetVolume.volume_no || data.volumeNo || 1).padStart(2,'0')}`;
    showSystemModal({
      title: 'Mua tập thành công',
      message: `${volumeLabel} đã được thêm vào Thư viện của tôi. Bạn chỉ có quyền đọc đầy đủ tập đã mua; các tập khác vẫn cần mua riêng nếu muốn đọc tiếp.`,
      confirmText: 'Bắt đầu đọc',
      cancelText: 'Về thông tin sách',
      onConfirm: () => {
        const targetHash = `#reader/${targetVolume.id}/full`;
        if(location.hash === targetHash) reader(targetVolume.id, 'full');
        else location.hash = targetHash;
      },
      onCancel: () => returnToBookDetail(targetBook.id)
    });
  }catch(error){
    console.error('Không mua được tập:', error);
    const message = error.message === 'ONLY_PAID_VOLUME_CAN_BE_PURCHASED'
      ? 'Chỉ tập thuộc sách trả phí mới cần mua. Sách miễn phí và sách thuê không được đưa vào thư viện mua tập.'
      : error.message === 'BOOK_CONTENT_NOT_AVAILABLE'
        ? 'Tập này chưa có nội dung đọc hợp lệ.'
        : 'Bạn thử lại sau nha.';
    showSystemNotice('Không mua được tập', message);
  }
}
async function purchasePaidBook(bookId, startAfter = false){
  const targetBook = bookById(bookId);
  const volumes = bookVolumesOf(bookId);
  if(!targetBook || !volumes.length) return;
  try{
    const res = await fetch(`${API_BASE}/purchases/book?userId=${currentUserId}&bookId=${Number(bookId)}`, {method:'POST'});
    let data = await res.json().catch(() => ({}));
    data = cleanBackendPayload(data);
    if(!res.ok) throw new Error(data.reason || data.message || 'PURCHASE_BOOK_FAILED');
    syncPurchaseLibraryFromPayload(data);
    purchaseLibraryLoadedFromBackend = true;
    await hydrateFamiliesFromBackend(true);
    refreshVisibleBookDetail(targetBook.id);
    hideReaderFriendlyPopup();
    const firstReadable = volumes.find(v => canReadFullVolume(v)) || volumes[0];
    showSystemModal({
      title: 'Mua sách thành công',
      message: `${targetBook.title} đã được thêm vào Thư viện của tôi. Bạn có thể đọc toàn bộ các tập của sách này.`,
      confirmText: 'Bắt đầu đọc',
      cancelText: 'Về thông tin sách',
      onConfirm: () => {
        const targetHash = `#reader/${firstReadable.id}/full`;
        if(location.hash === targetHash) reader(firstReadable.id, 'full');
        else location.hash = targetHash;
      },
      onCancel: () => returnToBookDetail(targetBook.id)
    });
  }catch(error){
    console.error('Không mua được toàn bộ sách:', error);
    const message = error.message === 'ONLY_PAID_BOOK_CAN_BE_PURCHASED'
      ? 'Chỉ sách trả phí mới cần mua.'
      : error.message === 'BOOK_CONTENT_NOT_AVAILABLE'
        ? 'Sách này chưa có nội dung đọc hợp lệ.'
        : 'Bạn thử lại sau nha.';
    showSystemNotice('Không mua được sách', message);
  }
}
function purchaseCurrentReaderVolume(){
  const volumeId = readerState?.volumeId;
  if(!volumeId) return;
  endActiveReaderSession().finally(() => purchasePaidVolume(volumeId, true));
}
function purchaseCurrentReaderBook(){
  purchaseCurrentReaderVolume();
}
function showNeedPurchaseVolumePopup(volumeId){
  const volume = volumeById(volumeId);
  const book = bookById(volume?.book_id) || bookById(readerState?.bookId);
  const volumeLabel = volume ? `Tập ${String(volume.volume_no || 1).padStart(2,'0')}` : 'Tập này';
  const closeAction = readerState ? 'closeReaderFriendlyPopupAndReturn()' : 'hideReaderFriendlyPopup()';
  showReaderFriendlyPopup(`${volumeLabel}: Bạn cần mua tập này để tiếp tục đọc.`,
    `<button class="reader-popup-button secondary" onclick="${closeAction}">Đã hiểu</button><button class="reader-popup-button" onclick="purchasePaidVolume(${Number(volumeId)}, true)">Mua tập này</button>`);
}
function normalizeBackendStatus(value){
  return String(value || '').toLowerCase();
}

function syncSubscriptionFromPayload(data){
  if(Array.isArray(data.plans)){
    rentalPlans.splice(0, rentalPlans.length, ...data.plans.map(p => ({
      id: Number(p.id),
      name: p.name || 'G\u00f3i thu\u00ea',
      duration_days: Number(p.duration_days || p.durationDays || 0),
      price: Number(p.price || 0)
    })));
  }

  const rental = data.currentRental || data.current_rental || null;
  userRentals.splice(0, userRentals.length);
  if(rental){
    userRentals.push({
      id: Number(rental.id),
      user_id: Number(rental.user_id || rental.userId || currentUserId),
      plan_id: Number(rental.plan_id || rental.planId),
      started_at: rental.started_at || rental.startedAt || '',
      expires_at: rental.expires_at || rental.expiresAt || '',
      status: normalizeBackendStatus(rental.effective_status || rental.effectiveStatus || rental.status || 'active')
    });
  }
}

async function hydrateSubscriptionFromBackend(force = false){
  if(subscriptionHydrating || (subscriptionLoadedFromBackend && !force)) return;
  subscriptionHydrating = true;
  try{
    const res = await fetch(`${API_BASE}/subscriptions/status?userId=${currentUserId}`);
    let data = await res.json().catch(() => ({}));
    data = cleanBackendPayload(data);
    if(!res.ok) throw new Error(data.reason || data.message || 'LOAD_SUBSCRIPTION_FAILED');
    syncSubscriptionFromPayload(data);
    subscriptionLoadedFromBackend = true;
    const routeName = (location.hash || '#login').replace('#','').split('/')[0];
    if(routeName === 'subscription') subscription(false);
    if(routeName === 'profile') profile(false);
    if(routeName === 'book') route();
  }catch(error){
    console.warn('Kh\u00f4ng t\u1ea3i \u0111\u01b0\u1ee3c th\u00f4ng tin g\u00f3i thu\u00ea t\u1eeb backend:', error);
  }finally{
    subscriptionHydrating = false;
  }
}

async function registerRentalPlan(planId){
  if(hasActiveSubscription()){
    showSystemNotice('Bạn đang có gói thuê hoạt động', 'Bạn chỉ có thể đăng ký gói mới khi gói hiện tại hết hạn hoặc đã bị hủy.');
    return;
  }
  const params = new URLSearchParams({userId: String(currentUserId), planId: String(planId)});
  if(SUBSCRIPTION_DEMO_SECONDS) params.set('demoSeconds', String(SUBSCRIPTION_DEMO_SECONDS));
  if(SUBSCRIPTION_DEMO_MINUTES) params.set('demoMinutes', String(SUBSCRIPTION_DEMO_MINUTES));
  if(SUBSCRIPTION_DEMO_DAYS) params.set('demoDays', String(SUBSCRIPTION_DEMO_DAYS));

  try{
    const res = await fetch(`${API_BASE}/subscriptions/register?${params.toString()}`, {method:'POST'});
    let data = await res.json().catch(() => ({}));
    data = cleanBackendPayload(data);
    if(!res.ok) throw new Error(data.reason || data.message || 'UPDATE_SUBSCRIPTION_FAILED');
    syncSubscriptionFromPayload(data);
    subscriptionLoadedFromBackend = true;
    showSystemNotice('Đăng ký gói thuê thành công', 'Gói thuê đã được kích hoạt. Bạn có thể đọc toàn bộ sách thuê trong thời hạn gói.');
    subscription(false);
  }catch(error){
    console.error('Không cập nhật được gói thuê:', error);
    showSystemNotice('Không cập nhật được gói thuê', error.message === 'ACTIVE_SUBSCRIPTION_EXISTS' ? 'Bạn đang có gói thuê hoạt động. Khi gói hết hạn hoặc bị hủy thì mới đăng ký được gói mới.' : 'Bạn thử lại sau nha.');
  }
}

async function cancelRentalPlan(){
  const rental = userRental();
  if(!rental || rental.status !== 'active'){
    showSystemNotice('Không có gói thuê hoạt động', 'Hiện tại bạn không có gói thuê đang hoạt động để hủy.');
    return;
  }
  const ok = await showSystemConfirm({
    title: 'Hủy gói thuê',
    message: 'Sau khi hủy, bạn sẽ không còn quyền đọc toàn bộ sách thuê. Bạn có chắc muốn hủy không?',
    confirmText: 'Xác nhận hủy',
    cancelText: 'Không',
    tone: 'danger'
  });
  if(!ok) return;
  try{
    const res = await fetch(`${API_BASE}/subscriptions/cancel?userId=${currentUserId}`, {method:'POST'});
    let data = await res.json().catch(() => ({}));
    data = cleanBackendPayload(data);
    if(!res.ok) throw new Error(data.reason || data.message || 'CANCEL_SUBSCRIPTION_FAILED');
    syncSubscriptionFromPayload(data);
    subscriptionLoadedFromBackend = true;
    showSystemNotice('Đã hủy gói thuê', 'Bạn có thể đăng ký lại khi cần.');
    subscription(false);
  }catch(error){
    console.error('Không hủy được gói thuê:', error);
    showSystemNotice('Không hủy được gói thuê', 'Bạn thử lại sau nha.');
  }
}

function syncFamiliesFromPayload(data){
  if(Array.isArray(data.users)){
    users.splice(0, users.length, ...data.users.map(u => ({
      id: Number(u.id),
      name: u.name || 'Ng\u01b0\u1eddi d\u00f9ng',
      email: u.email || '',
      hashed_password: '********',
      status: normalizeBackendStatus(u.status || 'active')
    })));
    refreshCurrentUserFromUsers();
  }
  if(Array.isArray(data.groups)){
    familyGroups.splice(0, familyGroups.length, ...data.groups.map(g => ({
      id: Number(g.id),
      owner_user_id: Number(g.owner_user_id || g.ownerUserId),
      name: g.name || 'Nh\u00f3m gia \u0111\u00ecnh',
      status: normalizeBackendStatus(g.status || 'active')
    })));
  }
  if(Array.isArray(data.members)){
    familyGroupMembers.splice(0, familyGroupMembers.length, ...data.members.map(m => ({
      id: Number(m.id),
      group_id: Number(m.group_id || m.groupId),
      user_id: Number(m.user_id || m.userId),
      status: normalizeBackendStatus(m.status || 'active'),
      joined_at: m.joined_at || m.joinedAt || '',
      removed_at: m.removed_at || m.removedAt || ''
    })));
  }
  if(Array.isArray(data.sharedVolumes || data.shared_volumes)){
    const shared = data.sharedVolumes || data.shared_volumes;
    familySharedVolumes.splice(0, familySharedVolumes.length, ...shared.map(s => ({
      id: Number(s.id),
      group_id: Number(s.group_id || s.groupId),
      volume_id: Number(s.volume_id || s.volumeId),
      book_id: Number(s.book_id || s.bookId || 0),
      book_title: s.book_title || s.bookTitle || '',
      authors: s.authors || '',
      volume_no: Number(s.volume_no || s.volumeNo || 1),
      status: normalizeBackendStatus(s.status || 'active'),
      shared_at: s.shared_at || s.sharedAt || '',
      revoked_at: s.revoked_at || s.revokedAt || ''
    })));
  }
}

async function hydrateFamiliesFromBackend(force = false){
  if(familyHydrating || (familyLoadedFromBackend && !force)) return;
  familyHydrating = true;
  try{
    const res = await fetch(`${API_BASE}/families?userId=${currentUserId}`);
    let data = await res.json().catch(() => ({}));
    data = cleanBackendPayload(data);
    if(!res.ok) throw new Error(data.reason || data.message || 'LOAD_FAMILIES_FAILED');
    syncFamiliesFromPayload(data);
    familyLoadedFromBackend = true;
    const routeName = (location.hash || '#login').replace('#','').split('/')[0];
    if(routeName === 'families') families(false);
    if(routeName === 'profile') profile(false);
    if(routeName === 'family') route();
  }catch(error){
    console.warn('Kh\u00f4ng t\u1ea3i \u0111\u01b0\u1ee3c nh\u00f3m gia \u0111\u00ecnh t\u1eeb backend:', error);
  }finally{
    familyHydrating = false;
  }
}


function availableUsersForFamily(groupId){
  const activeIds = new Set(groupMembers(groupId).map(m => Number(m.user_id)));
  return users.filter(u => normalizeBackendStatus(u.status) === 'active' && !activeIds.has(Number(u.id)));
}

async function addFamilyMember(groupId){
  const memberInput = document.getElementById('family-member-email');
  const rawValue = memberInput?.value?.trim() || '';
  if(!rawValue){
    showSystemNotice('Thiếu thông tin thành viên', 'Bạn nhập email hoặc tên thành viên cần thêm nha.');
    memberInput?.focus?.();
    return;
  }

  const normalized = rawValue.toLowerCase();
  const candidates = availableUsersForFamily(groupId);
  const exactMatch = candidates.find(u =>
    String(u.email || '').toLowerCase() === normalized ||
    String(u.name || '').toLowerCase() === normalized
  );
  const partialMatches = candidates.filter(u =>
    String(u.name || '').toLowerCase().includes(normalized) ||
    String(u.email || '').toLowerCase().includes(normalized)
  );
  const matchedUser = exactMatch || (partialMatches.length === 1 ? partialMatches[0] : null);

  if(!matchedUser && partialMatches.length > 1){
    showSystemNotice('Tên chưa đủ rõ', `Có ${partialMatches.length} tài khoản khớp với "${rawValue}". Bạn nhập email đầy đủ để tránh thêm nhầm nha.`);
    memberInput?.focus?.();
    return;
  }

  try{
    const params = new URLSearchParams({ownerUserId: String(currentUserId)});
    if(matchedUser) params.set('memberUserId', String(matchedUser.id));
    else params.set('memberEmail', rawValue);

    const res = await fetch(`${API_BASE}/families/${groupId}/members?${params.toString()}`, {method:'POST'});
    let data = await res.json().catch(() => ({}));
    data = cleanBackendPayload(data);
    if(!res.ok) throw new Error(data.reason || data.message || 'ADD_MEMBER_FAILED');
    syncFamiliesFromPayload(data);
    familyLoadedFromBackend = true;

    const addedUser = matchedUser || users.find(u => String(u.email || '').toLowerCase() === normalized);
    const groupName = familyGroups.find(g => Number(g.id) === Number(groupId))?.name || 'nhóm gia đình';
    if(addedUser) pushFamilyNotification(Number(addedUser.id), `Bạn vừa được thêm vào nhóm ${groupName}.`, Number(groupId));
    // Chủ nhóm không nhận notification cho thao tác của chính mình; popup xác nhận là đủ.
    showSystemNotice('Đã thêm thành viên', `${addedUser?.name || rawValue} đã được thêm vào nhóm gia đình.`);
    familyMemberManager(groupId);
  }catch(error){
    console.error('Không thêm được thành viên:', error);
    showSystemNotice('Không thêm được thành viên', error.message === 'USER_NOT_FOUND'
      ? 'Không tìm thấy tài khoản theo email/tên bạn nhập. Bạn thử nhập đúng email hoặc chọn một email gợi ý bên dưới.'
      : 'Bạn kiểm tra email/tên thành viên hoặc quyền chủ nhóm rồi thử lại nha.');
  }
}

async function removeFamilyMember(groupId, memberUserId){
  const target = users.find(u => Number(u.id) === Number(memberUserId));
  const ok = await showSystemConfirm({
    title: 'Xóa thành viên khỏi nhóm',
    message: `Bạn có chắc muốn xóa ${target?.name || 'thành viên này'} khỏi nhóm gia đình không?`,
    confirmText: 'Xóa thành viên',
    cancelText: 'Không',
    tone: 'danger'
  });
  if(!ok) return;
  try{
    const params = new URLSearchParams({ownerUserId: String(currentUserId)});
    const res = await fetch(`${API_BASE}/families/${groupId}/members/${memberUserId}/remove?${params.toString()}`, {method:'POST'});
    let data = await res.json().catch(() => ({}));
    data = cleanBackendPayload(data);
    if(!res.ok) throw new Error(data.reason || data.message || 'REMOVE_MEMBER_FAILED');
    syncFamiliesFromPayload(data);
    familyLoadedFromBackend = true;
    const groupName = familyGroups.find(g => Number(g.id) === Number(groupId))?.name || 'nhóm gia đình';
    pushFamilyNotification(Number(memberUserId), `Bạn vừa bị xóa khỏi nhóm ${groupName}.`, null);
    // Không gửi notification cho chủ nhóm hoặc các thành viên khác về việc xóa thành viên.
    showSystemNotice('Đã xóa thành viên', 'Thành viên đã được xóa khỏi nhóm.');
    const currentRoute = (location.hash || '').split('/');
    if(currentRoute[0] === '#family' && currentRoute[2] === 'members') familyMemberManager(groupId);
    else familyDetail(groupId);
  }catch(error){
    console.error('Không xóa được thành viên:', error);
    showSystemNotice('Không xóa được thành viên', 'Không thể xóa chủ nhóm hoặc bạn không có quyền quản lý nhóm này.');
  }
}




async function leaveFamilyGroup(groupId){
  const group = familyGroups.find(g => Number(g.id) === Number(groupId));
  if(!group) return;
  const ok = await showSystemConfirm({
    title: 'Rời nhóm gia đình',
    message: `Bạn có chắc muốn rời khỏi nhóm "${group.name}" không? Nhóm này sẽ không còn hiển thị trong danh sách của bạn.`,
    confirmText: 'Rời nhóm',
    cancelText: 'Không',
    tone: 'danger'
  });
  if(!ok) return;
  try{
    const params = new URLSearchParams({userId: String(currentUserId)});
    const res = await fetch(`${API_BASE}/families/${groupId}/leave?${params.toString()}`, {method:'POST'});
    let data = await res.json().catch(() => ({}));
    data = cleanBackendPayload(data);
    if(!res.ok) throw new Error(data.reason || data.message || 'LEAVE_FAMILY_FAILED');
    syncFamiliesFromPayload(data);
    familyLoadedFromBackend = true;
    if(Number(group.owner_user_id) && Number(group.owner_user_id) !== currentUserId){
      pushFamilyNotification(Number(group.owner_user_id), `${currentUser.name} vừa rời khỏi nhóm ${group.name}.`, Number(group.id));
    }
    showSystemNotice('Đã rời nhóm gia đình', 'Nhóm này đã được xóa khỏi danh sách nhóm của bạn.', () => location.hash = '#families');
  }catch(error){
    console.error('Không rời được nhóm gia đình:', error);
    showSystemNotice('Không rời được nhóm gia đình', error.message === 'OWNER_CANNOT_LEAVE_USE_DISSOLVE'
      ? 'Bạn là chủ nhóm. Nếu muốn xóa nhóm này, hãy dùng chức năng tan rã nhóm.'
      : 'Bạn thử lại sau nha.');
  }
}

async function dissolveFamilyGroup(groupId){
  const group = familyGroups.find(g => Number(g.id) === Number(groupId));
  if(!group) return;
  const ok = await showSystemConfirm({
    title: 'Tan rã nhóm gia đình',
    message: `Bạn có chắc muốn tan rã nhóm "${group.name}" không? Nhóm sẽ bị xóa khỏi danh sách của tất cả thành viên.`,
    confirmText: 'Tan rã nhóm',
    cancelText: 'Không',
    tone: 'danger'
  });
  if(!ok) return;
  try{
    const oldMembers = groupMembers(groupId).map(m => Number(m.user_id));
    const params = new URLSearchParams({ownerUserId: String(currentUserId)});
    const res = await fetch(`${API_BASE}/families/${groupId}/dissolve?${params.toString()}`, {method:'POST'});
    let data = await res.json().catch(() => ({}));
    data = cleanBackendPayload(data);
    if(!res.ok) throw new Error(data.reason || data.message || 'DISSOLVE_FAMILY_FAILED');
    syncFamiliesFromPayload(data);
    familyLoadedFromBackend = true;
    [...new Set([...oldMembers, Number(group.owner_user_id)])].forEach(uid => pushFamilyNotification(uid, `Nhóm ${group.name} đã tan rã.`, null));
    showSystemNotice('Đã tan rã nhóm gia đình', 'Nhóm này đã được xóa khỏi danh sách của tất cả thành viên.', () => location.hash = '#families');
  }catch(error){
    console.error('Không tan rã được nhóm gia đình:', error);
    showSystemNotice('Không tan rã được nhóm gia đình', 'Chỉ chủ nhóm mới có quyền thực hiện thao tác này.');
  }
}


// USER UI ch\u1ec9 hi\u1ec3n th\u1ecb d\u1eef li\u1ec7u th\u00e2n thi\u1ec7n v\u1edbi ng\u01b0\u1eddi \u0111\u1ecdc.
// C\u00e1c b\u1ea3ng k\u1ef9 thu\u1eadt nh\u01b0 POLICY_REGISTRY, USAGE_SESSION_POLICIES, SESSION_EVENTS,
// UCON_TRIGGER_EVENTS, OBLIGATION_EXECUTIONS ch\u1ec9 n\u1eb1m trong admin.html.
