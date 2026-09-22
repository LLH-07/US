// Admin-only ebook import/sync page.
function adminBooks(){
  if(!catalogLoadedFromBackend){
    catalogLoadingView('Đang tải danh mục trước khi mở trang quản lý sách...');
    hydrateCatalogFromBackend(true);
    return;
  }
  if(!isCurrentAdmin()){
    app.innerHTML = `${nav('library')}<section class="section"><p class="eyebrow">Quản lý sách</p><h1 class="h1">Chỉ admin được thêm sách</h1><div class="empty-read-history">Tài khoản hiện tại không có quyền đồng bộ EPUB mới.</div></section><div class="footer-block"></div>`;
    return;
  }
  const totalPages = Math.max(1, Math.ceil(books.length / LIBRARY_PAGE_SIZE));
  app.innerHTML = `${nav('admin-books')}<section class="section admin-books-page">
    <p class="eyebrow">Admin</p>
    <h1 class="h1">Quản lý sách EPUB</h1>
    <div class="admin-card admin-import-card">
      <h2 class="h2">Thêm sách mới bằng cách paste EPUB</h2>
      <p class="muted">Bước 1: paste file <b>.epub</b> vào thư mục <code>BE/ebooks</code>.</p>
      <p class="muted">Bước 2: đặt tiền tố để phân loại: <code>free-</code>, <code>rental-</code>, hoặc <code>paid-</code>. Nếu không đặt tiền tố, backend mặc định là <b>PAID</b>.</p>
      <p class="muted">Bước 3: bấm nút bên dưới. Backend chỉ import sách có nội dung đọc được; sách lỗi hoặc rỗng sẽ không hiện ở trang chính.</p>
      <div class="profile-actions horizontal" style="margin-top:18px">
        <button id="admin-sync-books-btn" class="profile-action primary" onclick="syncEbooksAsAdmin()">Đồng bộ EPUB đã paste</button>
        <a class="profile-action" href="#library/${totalPages}">Xem trang cuối thư viện</a>
      </div>
    </div>
    <div class="admin-card admin-import-card">
      <h2 class="h2">Trạng thái danh mục hiện tại</h2>
      <div class="line-row"><b>Sách hợp lệ đang hiện:</b> ${books.length}</div>
      <div class="line-row"><b>Số trang thư viện:</b> ${totalPages}</div>
      <p class="muted">Sau khi đồng bộ thành công, hệ thống tự chuyển về trang cuối để thấy sách mới nằm sau cuốn cuối cùng hiện tại.</p>
    </div>
  </section><div class="footer-block"></div>`;
}

