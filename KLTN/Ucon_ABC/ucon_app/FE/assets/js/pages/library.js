// Library/home page with pagination.
function library(page = libraryPage, allowHydrate = true){
  if(typeof page === 'boolean'){
    allowHydrate = page;
    page = libraryPage;
  }
  if(allowHydrate && !catalogLoadedFromBackend){
    catalogLoadingView('Đang tải danh mục sách...');
    hydrateCatalogFromBackend(true);
    return;
  }
  if(allowHydrate) hydrateCatalogFromBackend();
  const readableBooks = books.filter(b => b.status === 'active' && b.contentAvailable !== false && bookVolumesOf(b.id).some(v => v.contentAvailable !== false));
  const totalBooks = readableBooks.length;
  const totalPages = Math.max(1, Math.ceil(totalBooks / LIBRARY_PAGE_SIZE));
  libraryPage = Math.min(Math.max(Number(page) || 1, 1), totalPages);
  const start = (libraryPage - 1) * LIBRARY_PAGE_SIZE;
  const visible = readableBooks.slice(start, start + LIBRARY_PAGE_SIZE);
  const pageButtons = Array.from({length: totalPages}, (_, index) => {
    const pageNo = index + 1;
    return `<button class="library-page-number ${pageNo === libraryPage ? 'active' : ''}" onclick="goLibraryPage(${pageNo})">${pageNo}</button>`;
  }).join('');
  const pager = totalPages > 1
    ? `<div class="library-pagination"><button class="outline-btn" ${libraryPage <= 1 ? 'disabled' : ''} onclick="goLibraryPage(${libraryPage - 1})">Trang trước</button><span>Trang ${libraryPage}/${totalPages} · ${totalBooks} sách hợp lệ</span><div class="library-page-numbers">${pageButtons}</div><button class="outline-btn" ${libraryPage >= totalPages ? 'disabled' : ''} onclick="goLibraryPage(${libraryPage + 1})">Trang sau</button></div>`
    : `<div class="library-pagination single"><span>Đang hiển thị toàn bộ ${totalBooks} sách hợp lệ</span></div>`;
  app.innerHTML = `${nav('library')}<section class="section"><p class="eyebrow">Kho lưu trữ</p><h1 class="h1">Thư viện của MH Library Demo</h1><p class="muted" style="margin:12px 0 0">Thư viện chỉ hiện các sách EPUB có nội dung đọc được. Mỗi trang hiển thị tối đa ${LIBRARY_PAGE_SIZE} sách.</p><div class="book-grid">${visible.map(b=>bookCard(b)).join('')}</div>${pager}</section><div class="footer-block"></div>`;
}
function goLibraryPage(page){
  const target = Math.max(1, Number(page) || 1);
  location.hash = `#library/${target}`;
  setTimeout(() => window.scrollTo({top:0, behavior:'smooth'}), 0);
}
function loadMoreBooks(){ goLibraryPage(libraryPage + 1); }
