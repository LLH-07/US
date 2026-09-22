// Family groups list page.
function families(allowHydrate = true){
  if(allowHydrate) hydrateFamiliesFromBackend(true);
  const groups = groupsOfCurrentUser();
  const totalPages = Math.max(1, Math.ceil(groups.length / groupsPerPage));
  familyPage = Math.min(Math.max(familyPage, 1), totalPages);
  const start = (familyPage - 1) * groupsPerPage;
  const pageGroups = groups.slice(start, start + groupsPerPage);
  const groupBody = groups.length
    ? `<div class="family-grid">${pageGroups.map(groupCard).join('')}</div><div class="pagination"><button class="outline-btn" ${familyPage<=1?'disabled':''} onclick="changeFamilyPage(-1)">\u2190 Trang tr\u01b0\u1edbc</button><span>Trang ${familyPage}/${totalPages}</span><button class="outline-btn" ${familyPage>=totalPages?'disabled':''} onclick="changeFamilyPage(1)">Trang sau \u2192</button></div>`
    : `<div class="empty-read-history"><b>Ch\u01b0a c\u00f3 nh\u00f3m gia \u0111\u00ecnh</b><p>Hi\u1ec7n b\u1ea1n ch\u01b0a thu\u1ed9c nh\u00f3m gia \u0111\u00ecnh n\u00e0o.</p></div>`;
  app.innerHTML = `${nav('families')}<section class="family-page"><div class="family-title-row"><div><p class="eyebrow">Nh\u00f3m gia \u0111\u00ecnh</p><h1 class="h1">C\u00e1c nh\u00f3m b\u1ea1n thu\u1ed9c v\u1ec1</h1><p class="muted">Các nhóm gia đình của bạn.</p></div></div>
    ${groupBody}
  </section><div class="footer-block"></div>`;
}
function changeFamilyPage(delta){ familyPage += delta; families(); }
