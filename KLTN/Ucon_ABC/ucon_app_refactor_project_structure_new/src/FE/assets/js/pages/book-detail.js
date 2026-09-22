// Book information/detail page.
function volumePickerStatus(book, volume){
  const model = normalizeAccessModel(volume?.access_model_override || book?.default_access_model);
  if(model === 'PAID'){
    if(hasPurchasedVolume(volume.id)) return {label:'Đã mua tập', tone:'ok'};
    if(hasFamilySharedVolume(volume.id)) return {label:'Được chia sẻ', tone:'ok'};
    return {label:'Chưa mua tập', tone:'warn'};
  }
  if(model === 'RENTAL') return hasActiveSubscription() ? {label:'Có gói thuê', tone:'ok'} : {label:'Cần đăng ký gói thuê', tone:'warn'};
  return currentUserId > 0 ? {label:'Có quyền', tone:'ok'} : {label:'Cần đăng nhập', tone:'warn'};
}
function closeBookVolumePicker(){
  document.getElementById('book-volume-picker')?.remove();
}
function openSingleVolumePicker(bookId, volumeId){
  const book = bookById(bookId);
  const volume = volumeById(volumeId);
  if(!book || !volume) return;
  const status = volumePickerStatus(book, volume);
  const model = normalizeAccessModel(volume.access_model_override || book.default_access_model);
  const canFull = canReadFullVolume(volume);
  const label = `Tập ${String(volume.volume_no || 1).padStart(2,'0')}`;
  const buy = model === 'PAID' && !hasPurchasedVolume(volume.id)
    ? `<button type="button" class="volume-picker-btn buy" onclick="purchasePaidVolume(${Number(volume.id)}, false); closeBookVolumePicker();">Mua tập</button>`
    : '';
  const fullHint = canFull
    ? 'Có thể mở phiên đọc full qua UCON.'
    : (model === 'RENTAL' ? 'Cần đăng ký gói thuê để đọc full.' : (model === 'PAID' ? 'Cần mua tập hoặc được chia sẻ để đọc full.' : 'Cần đăng nhập để đọc full.'));
  document.getElementById('book-volume-picker')?.remove();
  if(typeof pushPopupHistory === 'function') pushPopupHistory('book-volume-picker');
  document.body.insertAdjacentHTML('beforeend', `<div id="book-volume-picker" class="volume-picker-backdrop compact-picker" role="dialog" aria-modal="true">
    <div class="volume-picker-card single-volume-picker-card">
      <button class="reader-popup-close" onclick="closeBookVolumePicker()" aria-label="Đóng">×</button>
      <p class="eyebrow">Chọn cách đọc</p>
      <h2>${escapeHtml(label)}</h2>
      <div class="single-volume-status ${status.tone}">${escapeHtml(status.label)}</div>
      <div class="single-volume-actions">
        <a class="volume-picker-btn preview" href="#reader/${Number(volume.id)}/preview" onclick="closeBookVolumePicker()">Đọc preview</a>
        <a class="volume-picker-btn full ${canFull ? 'ready' : 'locked'}" href="#reader/${Number(volume.id)}/full" onclick="closeBookVolumePicker()">Đọc full</a>
        ${buy}
      </div>
    </div>
  </div>`);
}

function openBookVolumePicker(bookId, mode='preview'){
  const book = bookById(bookId);
  if(!book) return;
  const requestedMode = String(mode || 'preview').toLowerCase() === 'full' ? 'full' : 'preview';
  const volumes = bookVolumesOf(bookId);
  const bookModel = normalizeAccessModel(book.default_access_model);
  const title = requestedMode === 'full' ? 'Chọn tập để đọc full' : 'Chọn tập để đọc preview';
  const helpText = requestedMode === 'preview'
    ? 'Chọn tập muốn xem trước. Preview là nội dung đọc thử tĩnh, không tạo phiên UCON.'
    : (bookModel === 'RENTAL'
      ? 'Chọn tập muốn đọc full. Nếu chưa có gói thuê, hệ thống sẽ yêu cầu đăng ký gói thuê.'
      : (bookModel === 'PAID'
        ? 'Chọn tập muốn đọc full. Với tập chưa mua, bạn có thể mua riêng ngay tại đây.'
        : 'Chọn tập muốn đọc full.'));
  const rows = volumes.map(v => {
    const label = `Tập ${String(v.volume_no || 1).padStart(2,'0')}`;
    const status = volumePickerStatus(book, v);
    const model = normalizeAccessModel(v.access_model_override || book.default_access_model);
    const canFull = canReadFullVolume(v);
    const readHref = `#reader/${Number(v.id)}/${requestedMode}`;
    const readLabel = requestedMode === 'full' ? 'Đọc full' : 'Preview';
    const buy = requestedMode === 'full' && model === 'PAID' && !hasPurchasedVolume(v.id)
      ? `<button type="button" class="volume-picker-btn buy" onclick="purchasePaidVolume(${Number(v.id)}, false); closeBookVolumePicker();">Mua tập</button>`
      : '';
    const hint = requestedMode === 'preview'
      ? 'Mở bản đọc thử của tập này'
      : (canFull ? 'Có thể mở phiên đọc full' : (model === 'RENTAL' ? 'Cần đăng ký gói thuê để đọc full' : 'Cần mua hoặc có quyền chia sẻ để đọc full'));
    return `<div class="volume-picker-row ${status.tone} ${canFull ? 'can-full' : 'need-action'}">
      <div class="volume-picker-info"><div class="volume-picker-title-line"><b>${escapeHtml(label)}</b><span class="volume-picker-status ${status.tone}">${escapeHtml(status.label)}</span></div></div>
      <div class="volume-picker-actions">${buy}<a class="volume-picker-btn ${requestedMode === 'full' ? (canFull ? 'full ready' : 'full locked') : 'preview'}" href="${readHref}" onclick="closeBookVolumePicker()">${readLabel}</a></div>
    </div>`;
  }).join('');
  document.getElementById('book-volume-picker')?.remove();
  if(typeof pushPopupHistory === 'function') pushPopupHistory('book-volume-picker');
  document.body.insertAdjacentHTML('beforeend', `<div id="book-volume-picker" class="volume-picker-backdrop" role="dialog" aria-modal="true">
    <div class="volume-picker-card">
      <button class="reader-popup-close" onclick="closeBookVolumePicker()" aria-label="Đóng">×</button>
      <p class="eyebrow">${requestedMode === 'full' ? 'Đọc full' : 'Đọc preview'}</p>
      <h2>${escapeHtml(title)}</h2>
      <div class="volume-picker-list">${rows || '<p class="muted">Sách này chưa có tập để đọc.</p>'}</div>
    </div>
  </div>`);
}
function bookDetail(id){
  // Không chặn nguyên trang bằng dòng “Đang tải...” nếu FE đã có seed/catalog local.
  // Trang chi tiết render ngay, dữ liệu backend hydrate ngầm rồi refresh DOM sau.
  if(!catalogLoadedFromBackend && !catalogHydrating) hydrateCatalogFromBackend(false);
  hydrateSubscriptionFromBackend();
  hydratePurchaseLibraryFromBackend();
  hydrateFamiliesFromBackend();
  const b = books.find(x => String(x.id) === String(id));
  if(!b){
    app.innerHTML = `${nav('library')}<section class="section"><p class="eyebrow">Thông tin sách</p><h1 class="h1">Đang cập nhật danh mục</h1><div class="empty-read-history">Sách này chưa sẵn sàng. Đang tải lại dữ liệu.</div></section><div class="footer-block"></div>`;
    hydrateCatalogFromBackend(true);
    return;
  }
  const vols = bookVolumesOf(b.id);
  const model = normalizeAccessModel(b.default_access_model);
  const paidVolumes = vols.filter(v => normalizeAccessModel(v.access_model_override || b.default_access_model) === 'PAID');
  const missingPaidVolumes = paidVolumes.filter(v => !hasPurchasedVolume(v.id));
  const purchaseBtn = model === 'PAID' && missingPaidVolumes.length
    ? `<button type="button" id="detail-buy-book-btn" class="detail-secondary-action buy-book-btn" onclick="purchasePaidBook(${Number(b.id)}, false)">Mua toàn bộ sách</button>`
    : '';
  const rentalManageBtn = model === 'RENTAL'
    ? `<a class="detail-secondary-action rental-manage-btn" href="#subscription">Quản lý gói thuê</a>`
    : '';
  app.innerHTML = `${nav('library')}<section class="detail book-detail-clean ${badgeClass(b.default_access_model)}">
    <aside class="book-detail-sidebar">
      <div class="detail-cover text-cover editorial-cover ${badgeClass(b.default_access_model)}"><div><small>${b.genre}</small><strong id="detail-cover-title">${b.title}</strong></div></div>
      <div class="detail-action-card compact-side-actions">
        <div class="book-detail-action-group side-picker-actions">
          <button type="button" class="detail-primary-action preview-action" id="detail-read-preview" onclick="openBookVolumePicker(${Number(b.id)}, 'preview')">Đọc preview</button>
          <button type="button" class="detail-primary-action full-action" id="detail-read-first" onclick="openBookVolumePicker(${Number(b.id)}, 'full')">Đọc full</button>
        </div>
        <div class="book-extra-actions">${purchaseBtn}${rentalManageBtn}</div>
      </div>
    </aside>
    <main class="book-detail-main">
      <h1 class="main-title" id="detail-book-title">${b.title}</h1>
      <div class="meta"><span id="detail-book-authors">${b.authors}</span><span class="dot"></span><span class="book-type-pill ${badgeClass(b.default_access_model)}">${typeLabel(b.default_access_model)}</span></div>
      <div class="summary"><h2 class="summary-title">▰ Mô tả nội dung sách</h2><div class="summary-body"><p>${b.short_description || ''}</p></div></div>
      <div class="volumes"><p class="vol-title">Danh sách các tập</p><div class="volume-list" id="detail-volume-list">${renderVolumeButtons(b.id)}</div></div>
    </main>
  </section><div class="footer-block"></div>`;
  hydrateBookDetailFromBackend(Number(b.id));
}
