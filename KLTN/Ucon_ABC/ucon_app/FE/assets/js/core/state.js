// Shared state and constants. Keep this file small; page UI lives in FE/assets/js/pages/*.js
const app = document.getElementById('app');

let currentUserId = Number(sessionStorage.getItem('ucon_current_user_id') || '1');
const DEMO_ADMIN_USER_ID = Number(window.UCON_ENV?.DEMO_ADMIN_USER_ID ?? 1);

if ('scrollRestoration' in history) history.scrollRestoration = 'manual';

const API_BASE = window.UCON_ENV?.API_BASE || 'http://localhost:8081/api/pep';
const SUBSCRIPTION_DEMO_SECONDS = window.UCON_ENV?.SUBSCRIPTION_DEMO_SECONDS ?? null;
const SUBSCRIPTION_DEMO_MINUTES = window.UCON_ENV?.SUBSCRIPTION_DEMO_MINUTES ?? null;
const SUBSCRIPTION_DEMO_DAYS = window.UCON_ENV?.SUBSCRIPTION_DEMO_DAYS ?? null;
const READER_CACHE_TTL_MS = window.UCON_ENV?.READER_CACHE_TTL_MS ?? (90 * 1000);
const READER_MAX_CACHE_CHUNKS = window.UCON_ENV?.READER_MAX_CACHE_CHUNKS ?? 4;
const READER_DOM_RADIUS = window.UCON_ENV?.READER_DOM_RADIUS ?? 1;
const PREVIEW_PAGE_LIMIT = window.UCON_ENV?.PREVIEW_PAGE_LIMIT ?? 5;
const APP_BUILD_ID = window.UCON_ENV?.APP_BUILD_ID || 'ucon-preview-all-volumes-member-input-final-20260704';
const NOTIFICATION_DATA_VERSION = window.UCON_ENV?.NOTIFICATION_DATA_VERSION || APP_BUILD_ID;
const RESET_USER_NOTICES_ON_START = window.UCON_ENV?.RESET_USER_NOTICES_ON_START === true;

function clearUserNoticeStorage(){
  Object.keys(localStorage).forEach(key => {
    if(
      key.startsWith('ucon_family_notifications_user_') ||
      key === 'ucon_family_notifications_version' ||
      key === 'ucon_last_system_notice' ||
      key === 'ucon_last_system_modal'
    ){
      localStorage.removeItem(key);
    }
  });
  // Những thứ này chỉ là trạng thái UI tạm, không phải dữ liệu DB.
  sessionStorage.removeItem('ucon_pending_notice');
  sessionStorage.removeItem('ucon_last_notice');
}

function resetTransientUiStateIfNeeded(){
  const buildKey = 'ucon_app_build_id';
  const oldBuild = localStorage.getItem(buildKey);

  // Mỗi bản zip/build mới phải bắt đầu sạch thông báo cũ trong browser.
  if(oldBuild !== APP_BUILD_ID){
    clearUserNoticeStorage();
    localStorage.setItem(buildKey, APP_BUILD_ID);
    localStorage.setItem('ucon_family_notifications_version', NOTIFICATION_DATA_VERSION);
    return;
  }

  // Demo mode chỉ dọn notification cũ khi bật rõ RESET_USER_NOTICES_ON_START=true.
  // Mặc định giữ notification để khi đổi tài khoản/thành viên đăng nhập lại vẫn thấy badge chuông.
  if(RESET_USER_NOTICES_ON_START){
    clearUserNoticeStorage();
    localStorage.setItem(buildKey, APP_BUILD_ID);
    localStorage.setItem('ucon_family_notifications_version', NOTIFICATION_DATA_VERSION);
  }
}
resetTransientUiStateIfNeeded();

const BOOK_BACKEND_SAMPLE_VOLUME_ID = {};
const BACKEND_VOLUME_ID_MAP = {};

let readerState = null;
let readerScrollHandler = null;
let readerObserver = null;
let readHistoryHydrating = false;
let readHistoryLoadedFromBackend = false;
let subscriptionHydrating = false;
let subscriptionLoadedFromBackend = false;
let familyHydrating = false;
let familyLoadedFromBackend = false;
let purchaseLibraryHydrating = false;
let purchaseLibraryLoadedFromBackend = false;
const purchasedLibrary = [];
let notificationPopoverOpen = false;
let familyNotifications = [];
// Sau khi dọn storage lúc khởi động, state trong RAM cũng bắt đầu rỗng.
familyNotifications = [];

let catalogLoadedFromBackend = false;
let catalogHydrating = false;
