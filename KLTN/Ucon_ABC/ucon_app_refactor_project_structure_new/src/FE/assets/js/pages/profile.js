// Profile page: purchased library, read history, family/subscription summary.
function profile(allowHydrate = true){
  if(allowHydrate){ hydrateCatalogFromBackend(); hydrateSubscriptionFromBackend(true); hydrateFamiliesFromBackend(true); hydratePurchaseLibraryFromBackend(true); }
  const groups = groupsOfCurrentUser();
  const rental = userRental();
  const plan = rentalPlans.find(p => Number(p.id) === Number(rental?.plan_id));
  const historyItems = readHistory();
  const visibleHistory = historyItems.slice(0, historyVisible);
  const hasMoreHistory = historyVisible < historyItems.length;
  const historyHtml = visibleHistory.length
    ? `<div class="mini-grid">${visibleHistory.map(miniBookCard).join('')}</div>${hasMoreHistory ? `<button class="load-more compact" onclick="loadMoreHistory()">Xem thêm lịch sử đọc ↓</button>` : `<div class="load-more done compact">Đã hiển thị lịch sử đọc gần đây</div>`}`
    : `<div class="empty-read-history">Hiện tại không có sách đã đọc.</div>`;
  const purchasedLoading = purchaseLibraryHydrating || !purchaseLibraryLoadedFromBackend;
  const paidLibraryItems = purchasedLibrary.filter(item => normalizeAccessModel(item.default_access_model) === 'PAID');
  const purchasedHtml = paidLibraryItems.length
    ? `<div class="mini-grid">${paidLibraryItems.map(item => {
        const book = bookById(item.book_id) || {id:item.book_id,title:item.title,genre:item.genre,authors:item.authors,default_access_model:item.default_access_model,status:'active'};
        const purchasedIds = Array.isArray(item.purchased_volume_ids) ? item.purchased_volume_ids : [];
        const purchasedCount = Number(item.purchased_volume_count || purchasedIds.length || 0) || 0;
        const totalVolumes = bookVolumesOf(book.id).length || purchasedCount || 1;
        const countText = `Đã mua ${purchasedCount}/${totalVolumes} tập`;
        return bookCard(book, `<div class="history-meta">${countText} · ${dateVN(item.last_purchased_at || item.first_purchased_at)}</div>`);
      }).join('')}</div>`
    : purchasedLoading
      ? `<div class="empty-read-history">Đang tải thư viện sách đã mua...</div>`
      : `<div class="empty-read-history">Hiện tại không có sách đã mua.</div>`;
  const familySummary = groups.length
    ? `<div class="profile-summary-list">${groups.slice(0,4).map(g=>`<a class="family-mini no-avatar profile-summary-item" href="#family/${g.id}"><span><b>${g.name}</b><small>Vai trò: ${membershipRole(g)}</small></span></a>`).join('')}</div>${groups.length>4?`<a class="text-button" href="#families">Xem tất cả nhóm →</a>`:''}`
    : `<div class="profile-empty-note"><b>Chưa có nhóm gia đình</b><br>Hiện bạn chưa thuộc nhóm gia đình nào.</div>`;
  const rentalText = plan
    ? `<div class="line-row"><b>Gói:</b> ${plan.name}</div><div class="line-row"><b>Ngày đăng ký:</b> ${dateVN(rental?.started_at)}</div><div class="line-row"><b>Ngày hết hạn:</b> ${dateVN(rental?.expires_at)}</div>`
    : `<p class="profile-empty-note small">Bạn chưa có gói thuê đang hoạt động. Chọn gói để đọc sách thuê.</p><div class="line-row"><b>Gói:</b> Chưa có</div><div class="line-row"><b>Ngày đăng ký:</b> -</div><div class="line-row"><b>Ngày hết hạn:</b> -</div>`;
  app.innerHTML = `${nav('profile')}
  <section class="profile-page-clean schema-profile-clean">
    <div class="profile-main-info">
      <div class="big-avatar-wrap text-avatar"><span>${initials(currentUser.name)}</span></div>
      <div class="profile-name"><h1>${currentUser.name}</h1><p>Email: ${currentUser.email}</p><p>Trạng thái: ${currentUser.status}</p></div>
    </div>
    <div class="profile-panels profile-panels-polished profile-panels-under">
      <div class="profile-summary-card"><h2 class="panel-title">Nhóm gia đình (${groups.length})</h2>${familySummary}</div>
      <div class="profile-summary-card"><h2 class="panel-title">Tình trạng gói thuê</h2><span class="rental-status ${rentalClass(rental?.status)}">${rentalStatusLabel(rental?.status)}</span>${rentalText}<div class="profile-actions"><button class="profile-action primary" onclick="goToSubscriptionPage(event)">Quản lý gói thuê</button></div></div>
    </div>
  </section>
  <section class="read-section"><h2>Thư viện của tôi</h2><p class="read-subtitle">Sách trả phí đã mua.</p>${purchasedHtml}</section>
  <section class="read-section"><h2>Sách đã đọc</h2><p class="read-subtitle">Lịch sử đọc gần đây.</p>${historyHtml}</section><div class="footer-block"></div>`;
}
function loadMoreHistory(){ historyVisible = Math.min(historyVisible + 4, readHistory().length); profile(); }
