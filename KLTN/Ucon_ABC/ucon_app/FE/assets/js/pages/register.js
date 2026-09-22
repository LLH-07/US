// Register page.
function register(){
  app.innerHTML = `${nav('')}<section class="section" style="max-width:560px"><p class="eyebrow">T\u00e0i kho\u1ea3n</p><h1 class="h1">\u0110\u0103ng k\u00fd ng\u01b0\u1eddi \u0111\u1ecdc</h1><form class="admin-card" style="margin-top:24px" onsubmit="location.hash='library';return false">
    <div class="field"><label class="label">H\u1ecd t\u00ean</label><input class="input" placeholder="Nguy\u1ec5n Th\u1ecb B\u00ecnh"></div>
    <div class="field"><label class="label">Email</label><input class="input" type="email" placeholder="example@archive.vn"></div>
    <div class="field"><label class="label">M\u1eadt kh\u1ea9u</label><div style="position:relative"><input id="register-password" class="input" type="password" placeholder="demo" style="padding-right:76px"><button type="button" onclick="togglePasswordVisibility('register-password', this)" aria-label="Hi\u1ec7n m\u1eadt kh\u1ea9u" style="position:absolute;right:10px;top:50%;transform:translateY(-50%);border:0;background:transparent;color:#3f604a;font-weight:800;cursor:pointer">Hi\u1ec7n</button></div></div>
    <button class="submit">T\u1ea1o t\u00e0i kho\u1ea3n</button>
  </form></section>`;
}
