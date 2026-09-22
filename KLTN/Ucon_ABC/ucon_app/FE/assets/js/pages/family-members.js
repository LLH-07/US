// Family member management page.
function familyMemberManager(id){
  if(!familyLoadedFromBackend) hydrateFamiliesFromBackend();
  const group = familyGroups.find(g => Number(g.id) === Number(id)) || groupsOfCurrentUser()[0];
  if(!group){ families(); return; }
  const isOwner = Number(group.owner_user_id) === currentUserId;
  const members = groupMembers(group.id)
    .map(m => ({...m, user: users.find(u => Number(u.id) === Number(m.user_id))}))
    .sort((a,b) => Number(b.user_id === group.owner_user_id) - Number(a.user_id === group.owner_user_id) || Number(a.id) - Number(b.id));
  const candidates = availableUsersForFamily(group.id);
  const memberRows = members.length
    ? members.map(m => {
      const ownerRow = Number(group.owner_user_id) === Number(m.user_id);
      const u = m.user || {};
      const removeBtn = isOwner && !ownerRow
        ? `<button class="trash-btn" onclick="removeFamilyMember(${Number(group.id)}, ${Number(m.user_id)})" title="X\u00f3a ${u.name || 'th\u00e0nh vi\u00ean'} kh\u1ecfi nh\u00f3m" aria-label="X\u00f3a ${u.name || 'th\u00e0nh vi\u00ean'} kh\u1ecfi nh\u00f3m" style="width:42px;height:42px;border-radius:50%;font-size:18px;display:inline-flex;align-items:center;justify-content:center">&#128465;</button>`
        : `<span class="muted" style="font-size:13px">Kh\u00f4ng th\u1ec3 x\u00f3a</span>`;
      return `<div class="member-row" style="grid-template-columns:1.35fr .75fr .75fr auto;gap:16px;align-items:center"><span><b>${u.name || 'Ng\u01b0\u1eddi d\u00f9ng'}</b><small style="display:block;color:#777f75;font-size:12px;margin-top:4px">${u.email || 'Ch\u01b0a c\u00f3 email'}</small></span><span>${ownerRow ? 'Ch\u1ee7 nh\u00f3m' : 'Th\u00e0nh vi\u00ean'}</span><span class="muted" style="font-size:13px">Tham gia ${dateVN(m.joined_at) || '-'}</span>${removeBtn}</div>`;
    }).join('')
    : `<p class="muted">Nh\u00f3m n\u00e0y ch\u01b0a c\u00f3 th\u00e0nh vi\u00ean \u0111ang ho\u1ea1t \u0111\u1ed9ng.</p>`;
  const addBox = isOwner
    ? `<div class="admin-card" style="margin-bottom:24px"><h2 class="h2">Th\u00eam th\u00e0nh vi\u00ean</h2><p class="muted" style="margin:0 0 14px">Nhập email hoặc tên tài khoản cần thêm. Hệ thống sẽ khớp với tài khoản đang có.</p><div style="display:flex;gap:10px;align-items:center;flex-wrap:wrap"><input id="family-member-email" class="input" type="text" autocomplete="off" placeholder="Ví dụ: chau.demo@gmail.com hoặc Châu" style="max-width:420px" onkeydown="if(event.key===\'Enter\'){event.preventDefault(); addFamilyMember(${Number(group.id)}); }"><button class="profile-action primary" onclick="addFamilyMember(${Number(group.id)})">Thêm</button></div>${candidates.length ? `<p class="muted" style="font-size:13px;margin-top:12px">Có thể nhập thử: ${candidates.slice(0,4).map(u=>u.email).join(', ')}</p>` : `<p class="muted" style="font-size:13px;margin-top:12px">T\u1ea5t c\u1ea3 t\u00e0i kho\u1ea3n demo \u0111ang ho\u1ea1t \u0111\u1ed9ng \u0111\u00e3 thu\u1ed9c nh\u00f3m n\u00e0y.</p>`}</div>`
    : `<div class="admin-card" style="margin-bottom:24px"><h2 class="h2">Quy\u1ec1n th\u00e0nh vi\u00ean</h2><p class="muted">B\u1ea1n l\u00e0 th\u00e0nh vi\u00ean n\u00ean ch\u1ec9 \u0111\u01b0\u1ee3c xem danh s\u00e1ch th\u00e0nh vi\u00ean, kh\u00f4ng th\u1ec3 th\u00eam ho\u1eb7c x\u00f3a ng\u01b0\u1eddi kh\u00e1c.</p></div>`;
  app.innerHTML = `${nav('families')}<section class="family-detail"><div class="family-back-row"><a class="outline-btn" href="#family/${group.id}">← Quay lại nhóm</a></div><div class="family-title-row"><div><p class="eyebrow">Quản lý thành viên</p><h1 class="h1">${group.name}</h1><p class="muted">Thành viên trong nhóm.</p></div></div>${addBox}<div class="admin-card"><h2 class="h2">Danh sách thành viên (${members.length})</h2>${memberRows}</div></section><div class="footer-block"></div>`;
}
