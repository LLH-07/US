// FE/assets/js/env.js
// Local environment for Hoa's current machine.
window.UCON_ENV = {
  API_BASE: 'http://localhost:8081/api/pep',
  OBLIGATION_API_BASE: 'http://localhost:8081/api/ucon/obligations',
  CONTROL_WS_URL: null,
  DEMO_ADMIN_USER_ID: 1,
  SUBSCRIPTION_DEMO_SECONDS: null,
  SUBSCRIPTION_DEMO_MINUTES: null,
  SUBSCRIPTION_DEMO_DAYS: null,
  READER_CACHE_TTL_MS: 90 * 1000,
  READER_MAX_CACHE_CHUNKS: 4,
  READER_DOM_RADIUS: 1,
  PREVIEW_PAGE_LIMIT: 5,
  APP_BUILD_ID: 'ucon-pre-obligation-fulfillment-20260720',
  RESET_USER_NOTICES_ON_START: true,
  CONTROL_WS_CONNECT_TIMEOUT_MS: 1500,
  CONTROL_WS_RECONNECT_MS: 2000,
  CONTROL_WS_PING_MS: 25000,
  LIBRARY_PAGE_SIZE: 12,
  NOTIFICATION_DATA_VERSION: 'ucon-pre-obligation-fulfillment-20260720'
};
