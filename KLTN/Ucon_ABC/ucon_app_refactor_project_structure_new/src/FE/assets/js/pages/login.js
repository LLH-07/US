// Login page.
function login(){
  app.innerHTML = `<section class="login-screen">
    <div class="login-hero"><div class="hero-copy"><div class="hero-line"></div><h2>MH Library Demo</h2><h1>N\u01a1i tri th\u1ee9c \u0111\u01b0\u1ee3c t\u00ecm th\u1ea5y</h1><p>Ch\u00e0o m\u1eebng b\u1ea1n \u0111\u1ebfn v\u1edbi m\u00f4i tr\u01b0\u1eddng nghi\u00ean c\u1ee9u v\u00e0 m\u00f4 ph\u1ecfng UCON</p></div></div>
    <div class="login-form-wrap"><div><div class="login-title"><span class="brand">MH Library Demo</span><h1>Ch\u00e0o m\u1eebng tr\u1edf l\u1ea1i</h1><p>N\u01a1i tri th\u1ee9c v\u00e0 th\u1eddi gian giao thoa</p></div>
    <form class="login-card" onsubmit="handleLogin(event)">
      <div class="field"><label class="label">Email</label><input id="login-email" class="input" type="email" placeholder="${currentUser.email}"></div>
      <div class="field"><div class="field-row"><label class="label">M\u1eadt kh\u1ea9u</label><a class="mini-link" href="#login">Qu\u00ean m\u1eadt kh\u1ea9u?</a></div><div style="position:relative"><input id="login-password" class="input" type="password" placeholder="demo" style="padding-right:76px"><button type="button" onclick="togglePasswordVisibility('login-password', this)" aria-label="Hi\u1ec7n m\u1eadt kh\u1ea9u" style="position:absolute;right:10px;top:50%;transform:translateY(-50%);border:0;background:transparent;color:#3f604a;font-weight:800;cursor:pointer">Hi\u1ec7n</button></div></div>
      <button class="submit">\u0110\u0103ng nh\u1eadp \u2192</button>
      <p style="text-align:center;margin:18px 0 0;color:#424842;font-size:14px">Ch\u01b0a c\u00f3 t\u00e0i kho\u1ea3n? <a class="mini-link" href="#register">\u0110\u0103ng k\u00fd</a></p>
    </form></div></div>
  </section>`;
}
