// Shared UI components: nav, book card, group card.
function nav(active='library'){
  if(typeof loadFamilyNotifications === 'function'){
    familyNotifications = loadFamilyNotifications(currentUserId);
  }
  const unreadCount = unreadNotificationCount();
  return `<header class="topbar">
    <div class="nav-left">
      <a class="brand" href="#library">MH Library Demo</a>
      <nav class="nav">
        <a class="${active==='library'?'active':''}" href="#library">Th\u01b0 vi\u1ec7n</a>
        <a class="${active==='families'?'active':''}" href="#families">Nh\u00f3m gia \u0111\u00ecnh</a>
        <a class="${active==='subscription'?'active':''}" href="#subscription">G\u00f3i thu\u00ea</a>
        <a class="${active==='profile'?'active':''}" href="#profile">Trang c\u00e1 nh\u00e2n</a>
      </nav>
    </div>
    <div class="right-tools"><button id="notification-button" class="notification-button ${unreadCount ? 'has-unread' : ''}" onclick="toggleNotificationPopover()" title="Thông báo gia đình">🔔<span id="notification-badge" class="notification-badge" style="display:${unreadCount?'inline-flex':'none'}">${unreadCount > 9 ? '9+' : unreadCount}</span></button><a class="user-chip" href="#profile"><span>${initials(currentUser.name)}</span>${currentUser.name}</a><a class="logout-btn" href="#login" onclick="logoutUser();return false">\u0110\u0103ng xu\u1ea5t</a></div>
  </header>`;
}
function bookCard(book, extra=''){
  const accessClass = badgeClass(book.default_access_model);
  const accessLabel = typeLabel(book.default_access_model);
  return `<a class="book-card book-card-text" href="#book/${book.id}">
    <span class="book-card-type-badge ${accessClass}">${accessLabel}</span>
    <div class="cover text-cover editorial-cover ${accessClass}"><div><small>${book.genre}</small><strong>${book.title}</strong></div></div>
    <h3>${book.title}</h3><p>${book.authors}</p>${extra}
  </a>`;
}
function miniBookCard(session){
  const bookId = historyBookId(session);
  const existingBook = bookById(bookId);
  const fallbackTitle = session.book_title || session.bookTitle || 'S\u00e1ch \u0111\u00e3 \u0111\u1ecdc';
  const fallbackAuthors = session.authors || '\u0110ang c\u1eadp nh\u1eadt';
  const book = existingBook || {
    id: bookId || `history-${session.id}`,
    title: fallbackTitle,
    genre: 'ebook',
    authors: fallbackAuthors,
    default_access_model: 'RENTAL',
    status: 'active'
  };
  return bookCard(book, `<div class="history-meta">\u0110\u00e3 \u0111\u1ecdc \u00b7 ${dateVN(session.ended_at || session.created_at)}</div>`);
}

function groupCard(group){
  const memberCount = groupMembers(group.id).length;
  const sharedCount = groupSharedBooks(group.id).length;
  return `<a class="family-card compact" href="#family/${group.id}">
    <div class="family-mark">${initials(group.name)}</div>
    <div class="family-card-body">
      <div class="family-card-head">
        <h3>${group.name}</h3>
        <span class="family-status ${group.status !== 'active' ? 'inactive' : ''}">${group.status === 'active' ? '\u0110ang ho\u1ea1t \u0111\u1ed9ng' : 'T\u1ea1m ng\u01b0ng'}</span>
      </div>
      <div class="family-card-meta"><span>${membershipRole(group)}</span><span>${memberCount} th\u00e0nh vi\u00ean</span><span>${sharedCount} sách được chia sẻ</span></div>
    </div>
  </a>`;
}


