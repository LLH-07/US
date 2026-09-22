// FE/assets/js/env.example.js
// Copy this file to env.js and adjust values if your local ports change.
window.UCON_ENV = {
  API_BASE: 'http://localhost:8081/api/pep',
  CONTROL_WS_URL: null,
  DEMO_ADMIN_USER_ID: 1,
  SUBSCRIPTION_DEMO_SECONDS: null,
  SUBSCRIPTION_DEMO_MINUTES: null,
  SUBSCRIPTION_DEMO_DAYS: null,
  READER_CACHE_TTL_MS: 90 * 1000,
  READER_MAX_CACHE_CHUNKS: 4,
  READER_DOM_RADIUS: 1,
  PREVIEW_PAGE_LIMIT: 5,
  APP_BUILD_ID: 'ucon-monitor-layout-v2-clean-user-notices-20260626',
  RESET_USER_NOTICES_ON_START: true,
  CONTROL_WS_CONNECT_TIMEOUT_MS: 1500,
  CONTROL_WS_RECONNECT_MS: 2000,
  CONTROL_WS_PING_MS: 25000,
  LIBRARY_PAGE_SIZE: 12,
  NOTIFICATION_DATA_VERSION: 'ucon-monitor-layout-v2-clean-user-notices-20260626'
};
