// Hash router and global listeners.
function route(){
  familyNotifications = loadFamilyNotifications();
  notificationPopoverOpen = false;
  const p = (location.hash || '#login').replace('#','').split('/');
  const nextReaderVolumeId = p[0] === 'reader' ? Number(p[1]) : null;
  if(p[0] !== 'reader' || (readerState && nextReaderVolumeId !== Number(readerState.volumeId))) cleanupReader();
  if(p[0]==='login') return login();
  if(p[0]==='register') return register();
  if(p[0]==='library') return library(Number(p[1] || 1));
  if(p[0]==='book') return bookDetail(p[1]);
  if(p[0]==='reader') return reader(p[1], p[2] || 'full');
  if(p[0]==='profile') return profile();
  if(p[0]==='families') return families();
  if(p[0]==='family' && p[2]==='members') return familyMemberManager(p[1]);
  if(p[0]==='family' && p[2]==='shared') return familySharedManager(p[1]);
  if(p[0]==='family') return familyDetail(p[1]);
  if(p[0]==='subscription') return subscription();
  return login();
}

document.addEventListener('click', (event) => {
  if(!notificationPopoverOpen) return;
  const pop = document.getElementById('notification-popover');
  const btn = document.getElementById('notification-button');
  if(pop && pop.contains(event.target)) return;
  if(btn && btn.contains(event.target)) return;
  notificationPopoverOpen = false;
  renderNotificationPopover();
});

window.addEventListener('hashchange', route);
// Không kết thúc session khi hard reload để reload #reader/{volumeId}/full vẫn giữ đúng trang đọc.
// Session vẫn kết thúc khi người dùng rời reader qua route nội bộ (cleanupReader).
route();


function startUserRealtimeRefresh(){
  if(window.__mhUserRealtimeTimer) return;
  window.__mhUserRealtimeTimer = setInterval(() => {
    const activeTag = String(document.activeElement?.tagName || '').toLowerCase();
    if(['input','textarea','select'].includes(activeTag)) return;

    const parts = (location.hash || '#login').replace('#','').split('/');
    const routeName = parts[0];
    if(['login','register','reader'].includes(routeName)) return;

    if(['library','book'].includes(routeName)){
      hydrateCatalogFromBackend(true);
      hydratePurchaseLibraryFromBackend(true);
      hydrateSubscriptionFromBackend(true);
      hydrateFamiliesFromBackend(true);
      return;
    }

    if(['families','family','profile'].includes(routeName)){
      // Không auto-refresh các trang đang nhập/chọn trong nhóm vì sẽ làm mất input hoặc đóng dropdown.
      if(routeName === 'family' && ['shared','members'].includes(parts[2])) return;
      hydrateFamiliesFromBackend(true);
      hydratePurchaseLibraryFromBackend(true);
      return;
    }

    if(routeName === 'subscription'){
      hydrateSubscriptionFromBackend(true);
    }
  }, 2500);
}
startUserRealtimeRefresh();
