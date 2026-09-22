// Family group detail page.
function familyDetail(id){
  if(!familyLoadedFromBackend) hydrateFamiliesFromBackend();
  const group = familyGroups.find(g => Number(g.id) === Number(id)) || groupsOfCurrentUser()[0];
  if(!group){ families(); return; }
  const members = groupMembers(group.id);
  const sharedBooks = groupSharedBooks(group.id);
  const isOwner = Number(group.owner_user_id) === currentUserId;
  const actions = group.status === 'active'
    ? (isOwner
      ? `<a class="profile-action primary" href="#family/${group.id}/shared">Quản lý sách chia sẻ</a><a class="profile-action" href="#family/${group.id}/members">Quản lý thành viên</a><button class="profile-action" onclick="dissolveFamilyGroup(${Number(group.id)})">Tan rã nhóm</button>`
      : `<button class="profile-action" onclick="leaveFamilyGroup(${Number(group.id)})">Rời nhóm</button>`)
    : `<a class="profile-action primary" href="#families">Quay lại danh sách</a>`;
  const membersHtml = members.length
    ? members.slice(0, 5).map(m => {
        const u = users.find(x => Number(x.id) === Number(m.user_id));
        const ownerRow = Number(group.owner_user_id) === Number(m.user_id);
        return `<div class="member-row"><span><b>${u?.name || 'Người dùng'}</b><small style="display:block;color:#777f75;font-size:12px;margin-top:4px">${u?.email || ''}</small></span><b>${ownerRow ? 'Chủ nhóm' : 'Thành viên'}</b></div>`;
      }).join('')
    : `<p class="muted">Nhóm này chưa có thành viên đang hoạt động.</p>`;
  const sharedHtml = sharedBooks.length
    ? `<div class="shared-book-list preview-shared-book-list">${sharedBooks.map(entry=>{
        const sortedShares = [...entry.shares].sort((a,b)=>{
          const av = volumeById(a.volume_id) || {volume_no:a.volume_no || 1};
          const bv = volumeById(b.volume_id) || {volume_no:b.volume_no || 1};
          return Number(av.volume_no || 1) - Number(bv.volume_no || 1);
        });
        const previewLimit = 14;
        const chips = sortedShares.slice(0, previewLimit).map(share => {
          const volume = volumeById(share.volume_id) || {id:Number(share.volume_id), book_id:Number(entry.book.id), volume_no: share.volume_no || 1};
          const label = `Tập ${String(volume.volume_no || 1).padStart(2,'0')}`;
          return `<a class="shared-volume-chip preview-chip clickable-chip" href="#reader/${Number(volume.id || share.volume_id)}" title="Đọc ${escapeHtml(label)}"><b>${escapeHtml(label)}</b><small>${dateVN(share.shared_at)}</small></a>`;
        }).join('');
        const remaining = Math.max(0, sortedShares.length - previewLimit);
        const moreChip = remaining ? `<a class="shared-volume-chip preview-chip more-chip" href="#family/${group.id}/shared"><b>+${remaining}</b><small>tập khác</small></a>` : '';
        return `<article class="shared-book-card shared-book-preview-card">
          <a class="shared-book-mark" href="#book/${entry.book.id}" title="Xem thông tin sách">${initials(entry.book.title)}</a>
          <div class="shared-book-body">
            <div class="shared-book-head"><div><a class="shared-book-title-link" href="#book/${entry.book.id}"><h3>${escapeHtml(entry.book.title)}</h3></a><p>${sortedShares.length} tập đang chia sẻ · từ ${dateVN(entry.firstSharedAt)}</p></div><span>${sortedShares.length} tập</span></div>
            <div class="shared-volume-chip-row preview-chip-row">${chips}${moreChip}</div>
            <div class="shared-book-actions"><a class="text-button" href="#book/${entry.book.id}">Xem thông tin sách</a></div>
          </div>
        </article>`;
      }).join('')}</div>`
    : `<p class="muted">Nhóm này chưa có sách đang chia sẻ.</p>`;
  app.innerHTML = `${nav('families')}<section class="family-detail">
    <div class="family-detail-hero compact"><div class="family-mark large">${initials(group.name)}</div><div><p class="eyebrow">Nhóm gia đình</p><h1 class="h1">${group.name}</h1><p class="muted" style="margin-top:8px">${isOwner ? 'Bạn là chủ nhóm. Bạn có thể quản lý thành viên và chia sẻ sách trả phí đã mua cho nhóm.' : 'Bạn là thành viên của nhóm gia đình này.'}</p><div class="family-card-meta big"><span>${membershipRole(group)}</span><span>${members.length} thành viên</span><span>${sharedBooks.length} sách đang chia sẻ</span></div><div class="profile-actions horizontal">${actions}</div></div></div>
    <div class="family-columns"><div class="admin-card"><h2 class="h2">Thành viên</h2>${membersHtml}${members.length > 5 ? `<a class="text-button" href="#family/${group.id}/members">Xem toàn bộ thành viên →</a>` : ''}</div>
    <div class="admin-card"><h2 class="h2">Sách đang chia sẻ</h2>${sharedHtml}</div></div>
    <div style="margin-top:24px"><a class="outline-btn" href="#families">← Quay lại danh sách nhóm</a></div>
  </section><div class="footer-block"></div>`;
}
