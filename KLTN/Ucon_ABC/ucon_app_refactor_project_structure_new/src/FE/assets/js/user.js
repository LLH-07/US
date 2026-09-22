// FE entrypoint. The old single-file UI was split into smaller files for easier reading.
// Keep the order below because files share global state and functions.
(function loadUconFrontend(){
  const scripts = [
    'env.js',
    'core/state.js',
    'core/control-channel.js',
    'core/utils.js',
    'services/catalog-service.js',
    'services/obligation-service.js',
    'data/demo-data.js',
    'core/session-state.js',
    'components/common.js',
    'reader/reader-core.js',
    'pages/login.js',
    'pages/register.js',
    'pages/library.js',
    'pages/book-detail.js',
    'pages/reader-page.js',
    'pages/profile.js',
    'pages/families-list.js',
    'pages/family-detail.js',
    'pages/family-members.js',
    'pages/family-shared.js',
    'pages/subscription.js',
    'router.js'
  ];
  scripts.forEach(src => {
    document.write(`<script src="assets/js/${src}"></script>`);
  });
})();
