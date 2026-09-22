// Reader page UI.
function readerVolumeNavControl(targetVolume, book, label, extraClass='', mode='full') {
  const requestedMode = String(mode || 'full').toLowerCase() === 'preview' ? 'preview' : 'full';
  if(!targetVolume){
    const isPreviewEnd = requestedMode === 'preview';
    return `<span class="outline-btn reader-nav-btn disabled ${isPreviewEnd ? 'preview-nav' : ''}"><span>${escapeHtml(label)}</span>${isPreviewEnd ? '<small>Preview</small>' : ''}</span>`;
  }

  const canFull = canReadFullVolume(targetVolume);
  const sharedOnly = hasFamilySharedVolume(targetVolume.id) && !hasPurchasedVolume(targetVolume.id);
  const isPreview = requestedMode === 'preview';
  const title = isPreview
    ? `${label} · đọc preview của tập này`
    : (sharedOnly ? `${label} · tập được nhóm gia đình chia sẻ` : (canFull ? label : `${label} · UCON kiểm tra quyền đọc full`));

  return `<a class="outline-btn reader-nav-btn ${extraClass} ${sharedOnly ? 'shared-nav' : ''} ${isPreview || !canFull ? 'preview-nav' : ''}" href="#reader/${targetVolume.id}/${requestedMode}" title="${escapeHtml(title)}"><span>${escapeHtml(label)}</span>${isPreview ? '<small>Preview</small>' : (sharedOnly ? '<small>Được chia sẻ</small>' : '')}</a>`;
}
function reader(volumeId, mode='full'){
  if(!catalogLoadedFromBackend){
    catalogLoadingView('Đang tải dữ liệu đọc sách...');
    hydrateCatalogFromBackend(true);
    return;
  }
  const volume = volumeById(volumeId);
  if(!volume){
    showSystemNotice('Không tìm thấy tập sách', 'Tập sách này không còn trong danh mục hiện tại. Bạn quay lại thư viện và chọn lại sách nha.', () => location.hash = '#library');
    return;
  }
  const book = bookById(volume.book_id);
  if(!book){
    showSystemNotice('Không tìm thấy sách', 'Danh mục vừa được cập nhật. Bạn quay lại thư viện và chọn lại sách nha.', () => location.hash = '#library');
    return;
  }
  const vols = bookVolumesOf(book.id);
  const index = vols.findIndex(v => Number(v.id) === Number(volume.id));
  const prev = vols[index - 1] || null;
  const next = vols[index + 1] || null;
  const requestedMode = String(mode || 'full').toLowerCase() === 'preview' ? 'preview' : 'full';

  window.scrollTo({top:0, left:0, behavior:'auto'});
  requestAnimationFrame(() => window.scrollTo({top:0, left:0, behavior:'auto'}));

  app.innerHTML = `${nav('library')}<article class="reader text-only-reader">
    <a class="reader-back-arrow reader-back-fixed" href="#book/${book.id}" title="Quay l\u1ea1i th\u00f4ng tin s\u00e1ch" aria-label="Quay l\u1ea1i th\u00f4ng tin s\u00e1ch">\u2190</a>
    <div class="reader-volume-topbar">
      ${readerVolumeNavControl(prev, book, 'Tập trước', '', requestedMode)}
      <div class="reader-current-volume"><span>${requestedMode === 'preview' ? 'Đang đọc preview' : 'Đang đọc full'}</span><strong id="reader-current-volume-title">${readerVolumeText(volume)}</strong></div>
      ${readerVolumeNavControl(next, book, 'Tập tiếp theo', 'primary-soft', requestedMode)}
    </div>
    <header class="reader-top compact">
      <div class="chapter-label" id="reader-chapter-label">${readerVolumeText(volume)}</div>
      <h1 id="reader-book-title">${book.title}</h1>
      <div class="author" id="reader-author">${book.authors}</div>
    </header>

    <section class="reader-text-content" id="reader-content">
      <div class="reader-page" id="reader-page-1" data-chunk="1" style="min-height:360px">
        <div class="reader-page-title">\u0110ang t\u1ea3i n\u1ed9i dung...</div>
        <p>\u0110ang chu\u1ea9n b\u1ecb n\u1ed9i dung \u0111\u1ecdc cho t\u1eadp s\u00e1ch n\u00e0y.</p>
      </div>
    </section>

    <div id="reader-loading" class="reader-loading-note">
      \u0110ang t\u1ea3i trang \u0111\u1ea7u ti\u00ean...
    </div>
  </article><div class="footer-block"></div>`;

  enableReaderCopyProtection();

  readerState = {
    volumeId: Number(volume.id),
    bookId: Number(book.id),
    returnHash: `#book/${book.id}`,
    sessionId: null,
    accessChecked: false,
    readMode: null,
    accessPath: null,
    requestedMode,
    nextChunk: 1,
    totalChunks: null,
    activeChunk: 1,
    cache: new Map(),
    loadingChunks: new Set(),
    ended: false,
    hasLoadedContent: false
  };

  loadReaderChunk(1);
  setTimeout(() => {
    if(readerState && Number(readerState.volumeId) === Number(volume.id)) initReaderAutoScroll();
  }, 300);
}


