// Family shared-volumes management page.
function purchasedPaidVolumesForSharing(){
  const result = [];
  purchasedLibrary
    .filter(item => normalizeAccessModel(item.default_access_model) === 'PAID')
    .forEach(item => {
      const book = bookById(item.book_id) || {
        id: Number(item.book_id),
        title: item.title || item.book_title || 'Sách đã mua',
        authors: item.authors || 'Đang cập nhật',
        default_access_model: 'PAID',
        status: 'active'
      };
      const ids = Array.isArray(item.purchased_volume_ids) ? item.purchased_volume_ids : [];
      ids.forEach(volumeId => {
        const volume = volumeById(volumeId) || {id:Number(volumeId), book_id:Number(book.id), volume_no:1, status:'active'};
        if(Number(volume.book_id) !== Number(book.id)) return;
        result.push({book, volume});
      });
    });
  return result.sort((a,b) => String(a.book.title).localeCompare(String(b.book.title)) || Number(a.volume.volume_no) - Number(b.volume.volume_no));
}


function groupEntriesByBook(entries){
  const groups = new Map();
  (entries || []).forEach(entry => {
    const key = Number(entry.book.id || entry.volume.book_id || entry.share?.book_id || 0);
    if(!key) return;
    if(!groups.has(key)) groups.set(key, {book:entry.book, items:[]});
    groups.get(key).items.push(entry);
  });
  return [...groups.values()].sort((a,b)=>String(a.book.title).localeCompare(String(b.book.title)));
}
const SHARED_VOLUME_PAGE_SIZE = 24;
let sharedVolumePageByBook = {};
function setSharedVolumePage(bookId, page){
  sharedVolumePageByBook[Number(bookId)] = Math.max(1, Number(page) || 1);
  familySharedManager(sharingManagerGroupId);
}
function availableShareBooks(availableVolumes){
  return groupEntriesByBook(availableVolumes.map(entry => ({...entry, share:null})));
}

function purchasedCountTextForBook(bookId){
  const libraryItem = purchasedLibrary.find(item => Number(item.book_id) === Number(bookId));
  const purchasedIds = Array.isArray(libraryItem?.purchased_volume_ids) ? libraryItem.purchased_volume_ids : [];
  const purchasedCount = Number(libraryItem?.purchased_volume_count || purchasedIds.length || 0) || 0;
  const totalVolumes = bookVolumesOf(bookId).length || purchasedCount || 1;
  return `Đã mua ${purchasedCount}/${totalVolumes} tập`;
}


function groupSharedVolumeCards(sharedVolumes){
  const groups = groupEntriesByBook(sharedVolumes);
  return groups.map(group => {
    const bookId = Number(group.book.id);
    const items = group.items.sort((a,b) => Number(a.volume.volume_no || 1) - Number(b.volume.volume_no || 1));
    const totalPages = Math.max(1, Math.ceil(items.length / SHARED_VOLUME_PAGE_SIZE));
    const page = Math.min(Math.max(1, sharedVolumePageByBook[bookId] || 1), totalPages);
    sharedVolumePageByBook[bookId] = page;
    const visible = items.slice((page - 1) * SHARED_VOLUME_PAGE_SIZE, page * SHARED_VOLUME_PAGE_SIZE);
    const chips = visible.map(entry => `<span class="shared-volume-chip manager-chip"><a href="#reader/${Number(entry.volume.id || entry.share.volume_id)}" title="Đọc tập đang chia sẻ"><b>Tập ${String(entry.volume.volume_no || 1).padStart(2,'0')}</b><small>${dateVN(entry.share.shared_at)}</small></a><button onclick="removeSharedVolume(${Number(entry.share.id)})" title="Bỏ chia sẻ tập ${String(entry.volume.volume_no || 1).padStart(2,'0')}">×</button></span>`).join('');
    const pager = items.length > SHARED_VOLUME_PAGE_SIZE ? `<div class="shared-volume-pager">
      <span>Hiển thị ${((page-1)*SHARED_VOLUME_PAGE_SIZE)+1}-${Math.min(items.length, page*SHARED_VOLUME_PAGE_SIZE)} / ${items.length} tập</span>
      <div>
        <button class="text-button" ${page<=1?'disabled':''} onclick="setSharedVolumePage(${bookId}, ${page-1})">Trước</button>
        <b>${page}/${totalPages}</b>
        <button class="text-button" ${page>=totalPages?'disabled':''} onclick="setSharedVolumePage(${bookId}, ${page+1})">Sau</button>
      </div>
    </div>` : '';
    return `<article class="shared-book-card shared-manager-book-card">
      <a class="shared-book-mark" href="#book/${bookId}" title="Xem thông tin sách">${initials(group.book.title)}</a>
      <div class="shared-book-body">
        <div class="shared-book-head manager-book-head">
          <div><a class="shared-book-title-link" href="#book/${bookId}"><h3>${escapeHtml(group.book.title)}</h3></a><p>${escapeHtml(group.book.authors || 'Đang cập nhật')}</p></div>
          <span>${items.length} tập</span>
        </div>
        <div class="shared-volume-chip-row paged-chip-row">${chips}</div>
        ${pager}
      </div>
    </article>`;
  }).join('');
}

function familySharedManager(id){
  if(!familyLoadedFromBackend && !familyHydrating) hydrateFamiliesFromBackend(true);
  if(!purchaseLibraryLoadedFromBackend && !purchaseLibraryHydrating) hydratePurchaseLibraryFromBackend(true);
  const group = familyGroups.find(g => Number(g.id) === Number(id)) || groupsOfCurrentUser()[0];
  if(!group){ families(); return; }
  sharingManagerGroupId = Number(group.id);
  const sharedVolumes = groupSharedVolumes(group.id)
    .map(share => {
      const volume = volumeById(share.volume_id) || {id:Number(share.volume_id), book_id:Number(share.book_id), volume_no:Number(share.volume_no || 1)};
      const book = bookById(volume.book_id) || {id:Number(share.book_id), title:share.book_title || 'Sách đang chia sẻ', authors:share.authors || 'Đang cập nhật'};
      return {share, volume, book};
    })
    .sort((a,b) => String(a.book.title).localeCompare(String(b.book.title)) || Number(a.volume.volume_no) - Number(b.volume.volume_no));
  const sharedVolumeIds = new Set(sharedVolumes.map(entry => Number(entry.volume.id)));
  const availableVolumes = purchasedPaidVolumesForSharing().filter(entry => !sharedVolumeIds.has(Number(entry.volume.id)));
  const loadingPurchases = purchaseLibraryHydrating || !purchaseLibraryLoadedFromBackend;
  const loadingShares = familyHydrating || !familyLoadedFromBackend;
  const availableOptions = availableVolumes.map(entry=>`<option value="${entry.volume.id}">${escapeHtml(entry.book.title)} — Tập ${String(entry.volume.volume_no || 1).padStart(2,'0')}</option>`).join('');
  const availableBookGroups = availableShareBooks(availableVolumes);
  const availableBookOptions = availableBookGroups.map(group=>`<option value="${Number(group.book.id)}">${escapeHtml(group.book.title)} — ${purchasedCountTextForBook(Number(group.book.id))}</option>`).join('');
  const addContent = loadingPurchases
    ? `<p class="muted">Đang tải danh sách tập sách trả phí đã mua...</p>`
    : availableVolumes.length
      ? `<div class="share-manager-actions">
          <section class="share-action-card single-volume share-action-text-card">
            <div><p class="share-action-label">Theo từng tập</p><h3>Chia sẻ một tập</h3><p>Chọn 1 tập đã mua.</p></div>
            <select id="share-volume-select" class="input select-input">${availableOptions}</select>
            <button class="profile-action primary" onclick="addSharedVolume()">Thêm tập</button>
          </section>
          <section class="share-action-card whole-volume share-action-text-card">
            <div><p class="share-action-label">Theo sách</p><h3>Chia sẻ toàn bộ tập đã mua của sách này</h3><p>Thêm các tập đã mua của sách.</p></div>
            <select id="share-book-select" class="input select-input">${availableBookOptions}</select>
            <button class="profile-action" onclick="shareAllPurchasedVolumesOfBook()">Chia sẻ các tập đã mua</button>
          </section>
        </div>`
      : `<div class="share-manager-actions all-shared-state">
          <section class="share-action-card single-volume disabled-card share-action-text-card">
            <div><p class="share-action-label">Hoàn tất</p><h3>Đã chia sẻ toàn bộ tập có thể chia sẻ</h3><p>Không còn tập đã mua để thêm.</p></div>
            <button class="profile-action primary" disabled>Không còn tập để thêm</button>
          </section>
          <section class="share-action-card whole-volume disabled-card share-action-text-card">
            <div><p class="share-action-label">Theo sách</p><h3>Chia sẻ toàn bộ tập đã mua của sách này</h3><p>Chưa có sách/tập đã mua mới.</p></div>
            <button class="profile-action" disabled>Đã chia sẻ toàn bộ</button>
          </section>
        </div>`;
  app.innerHTML = `${nav('families')}<section class="family-detail share-manager-page">
    <div class="family-back-row"><a class="outline-btn" href="#family/${group.id}">← Quay lại nhóm</a></div>
    <div class="share-manager-hero">
      <div>
        <p class="eyebrow">Quản lý tập sách chia sẻ</p>
        <h1 class="h1">${escapeHtml(group.name)}</h1>
        <p class="muted">Chia sẻ tập đã mua.</p>
      </div>
    </div>
    <div class="share-manager-layout">
      <div class="admin-card share-manager-add-card"><h2 class="h2">Thêm chia sẻ</h2>${addContent}</div>
      <div class="admin-card shared-manager-card"><div class="shared-card-title"><div><h2 class="h2">Tập đang chia sẻ</h2><p class="muted">Các tập đang được chia sẻ.</p></div></div><div class="shared-book-list">${loadingShares ? '<p class="muted">Đang tải danh sách tập đang chia sẻ...</p>' : (groupSharedVolumeCards(sharedVolumes) || '<p class="muted">Chưa có tập nào đang chia sẻ.</p>')}</div></div>
    </div>
  </section><div class="footer-block"></div>`;
}

async function addSharedVolume(){
  const select = document.getElementById('share-volume-select');
  const volumeId = Number(select?.value || 0);
  if(!volumeId) return;
  const volume = volumeById(volumeId);
  const book = bookById(volume?.book_id);
  if(book && normalizeAccessModel(book.default_access_model) !== 'PAID'){
    showSystemNotice('Không thể chia sẻ sách thuê/miễn phí', 'Nhóm gia đình chỉ được chia sẻ tập sách trả phí mà chủ nhóm đã mua.');
    return;
  }
  if(!hasPurchasedVolume(volumeId)){
    showSystemNotice('Chưa mua tập này', 'Bạn chỉ được chia sẻ các tập sách bạn đã mua.');
    return;
  }
  try{
    const params = new URLSearchParams({ownerUserId: String(currentUserId), volumeId: String(volumeId)});
    const res = await fetch(`${API_BASE}/families/${sharingManagerGroupId}/shared-volumes?${params.toString()}`, {method:'POST'});
    let data = await res.json().catch(() => ({}));
    data = cleanBackendPayload(data);
    if(!res.ok) throw new Error(data.reason || data.message || 'SHARE_VOLUME_FAILED');
    syncFamiliesFromPayload(data);
    familyLoadedFromBackend = true;
    await hydrateFamiliesFromBackend(true);
    const volumeLabel = `Tập ${String(volume?.volume_no || 1).padStart(2,'0')}`;
    // Chủ nhóm không nhận notification cho thao tác chia sẻ của chính mình.
    groupMembers(sharingManagerGroupId)
      .filter(m => Number(m.user_id) !== currentUserId)
      .forEach(m => pushFamilyNotification(Number(m.user_id), `Nhóm gia đình của bạn vừa có tập sách mới được chia sẻ: ${book?.title || 'sách mới'} · ${volumeLabel}.`, Number(sharingManagerGroupId)));
    showSystemNotice('Đã chia sẻ tập', 'Tập sách đã được thêm vào danh sách chia sẻ của nhóm. Thành viên trong nhóm có thể đọc tập được chia sẻ.');
    familySharedManager(sharingManagerGroupId);
  }catch(error){
    console.error('Không chia sẻ được tập:', error);
    showSystemNotice('Không chia sẻ được tập', error.message === 'OWNER_HAS_NOT_PURCHASED_VOLUME'
      ? 'Bạn chỉ được chia sẻ tập sách trả phí mà bạn đã mua.'
      : error.message === 'ONLY_PAID_VOLUME_CAN_BE_PURCHASED'
        ? 'Không được chia sẻ sách thuê hoặc sách miễn phí trong nhóm gia đình.'
        : 'Bạn thử lại sau nha.');
  }
}


async function shareAllPurchasedVolumesOfBook(){
  const select = document.getElementById('share-book-select');
  const bookId = Number(select?.value || 0);
  if(!bookId) return;
  const sharedVolumeIds = new Set(groupSharedVolumes(sharingManagerGroupId).map(x => Number(x.volume_id)));
  const entries = purchasedPaidVolumesForSharing()
    .filter(entry => Number(entry.book.id) === bookId)
    .filter(entry => !sharedVolumeIds.has(Number(entry.volume.id)));
  const book = bookById(bookId) || entries[0]?.book;
  if(!entries.length){
    showSystemNotice('Không còn tập để chia sẻ', 'Các tập đã mua của sách này đều đã được chia sẻ hoặc bạn chưa mua tập nào.');
    return;
  }
  const ok = await showSystemConfirm({
    title: 'Chia sẻ toàn bộ tập đã mua',
    message: `Sẽ chia sẻ ${entries.length} tập đã mua của "${book?.title || 'sách này'}" cho nhóm. Tập chưa mua sẽ không được chia sẻ.`,
    confirmText: 'Chia sẻ các tập đã mua',
    cancelText: 'Hủy'
  });
  if(!ok) return;
  let success = 0;
  try{
    for(const entry of entries){
      const params = new URLSearchParams({ownerUserId: String(currentUserId), volumeId: String(entry.volume.id)});
      const res = await fetch(`${API_BASE}/families/${sharingManagerGroupId}/shared-volumes?${params.toString()}`, {method:'POST'});
      let data = await res.json().catch(() => ({}));
      data = cleanBackendPayload(data);
      if(res.ok){
        success++;
        syncFamiliesFromPayload(data);
      }
    }
    familyLoadedFromBackend = true;
    await hydrateFamiliesFromBackend(true);
    // Chủ nhóm không nhận notification cho thao tác chia sẻ của chính mình.
    groupMembers(sharingManagerGroupId)
      .filter(m => Number(m.user_id) !== currentUserId)
      .forEach(m => pushFamilyNotification(Number(m.user_id), `Nhóm gia đình vừa được chia sẻ thêm ${success} tập của sách: ${book?.title || 'sách mới'}.`, Number(sharingManagerGroupId)));
    showSystemNotice('Đã chia sẻ các tập đã mua', `Đã chia sẻ ${success}/${entries.length} tập đã mua của sách này cho nhóm.`);
    familySharedManager(sharingManagerGroupId);
  }catch(error){
    console.error('Không chia sẻ được các tập đã mua:', error);
    showSystemNotice('Không chia sẻ được các tập đã mua', 'Một số tập chưa được chia sẻ. Bạn thử lại sau nha.');
  }
}


function shareWholePurchasedBook(){ return shareAllPurchasedVolumesOfBook(); }

async function removeSharedVolume(shareId){
  const shareBeforeRemove = groupSharedVolumes(sharingManagerGroupId).find(x => Number(x.id) === Number(shareId));
  const volumeBeforeRemove = volumeById(shareBeforeRemove?.volume_id);
  const bookBeforeRemove = bookById(volumeBeforeRemove?.book_id);
  const removedVolumeLabel = volumeBeforeRemove ? `Tập ${String(volumeBeforeRemove.volume_no || 1).padStart(2,'0')}` : 'tập sách';
  const ok = await showSystemConfirm({
    title: 'Bỏ chia sẻ tập',
    message: 'Bạn có chắc muốn gỡ tập sách này khỏi danh sách chia sẻ của nhóm không?',
    confirmText: 'Bỏ chia sẻ',
    cancelText: 'Không',
    tone: 'danger'
  });
  if(!ok) return;
  try{
    const params = new URLSearchParams({ownerUserId: String(currentUserId)});
    const res = await fetch(`${API_BASE}/families/${sharingManagerGroupId}/shared-volumes/${Number(shareId)}/remove?${params.toString()}`, {method:'POST'});
    let data = await res.json().catch(() => ({}));
    data = cleanBackendPayload(data);
    if(!res.ok) throw new Error(data.reason || data.message || 'REMOVE_SHARED_VOLUME_FAILED');
    syncFamiliesFromPayload(data);
    familyLoadedFromBackend = true;
    await hydrateFamiliesFromBackend(true);
    // Chủ nhóm không nhận notification cho thao tác bỏ chia sẻ của chính mình.
    groupMembers(sharingManagerGroupId)
      .filter(m => Number(m.user_id) !== currentUserId)
      .forEach(m => pushFamilyNotification(Number(m.user_id), `${bookBeforeRemove?.title || 'Một tập sách'} — ${removedVolumeLabel} vừa được gỡ khỏi danh sách chia sẻ của nhóm.`, Number(sharingManagerGroupId)));
    showSystemNotice('Đã gỡ tập chia sẻ', 'Tập sách đã được gỡ khỏi danh sách chia sẻ của nhóm.');
    familySharedManager(sharingManagerGroupId);
  }catch(error){
    console.error('Không gỡ được tập chia sẻ:', error);
    showSystemNotice('Không gỡ được tập chia sẻ', 'Bạn thử lại sau nha.');
  }
}

// Giữ lại tên hàm cũ để không lỗi nếu có onclick cũ trong cache trình duyệt.
function addSharedBook(){ return addSharedVolume(); }
function removeSharedBook(bookId){
  showSystemNotice('Chia sẻ theo từng tập', 'Bản hiện tại quản lý chia sẻ theo từng tập. Bạn gỡ từng tập trong danh sách chia sẻ nha.');
}
