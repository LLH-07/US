const admin = document.getElementById('admin-app');

// Admin chỉ render UI. Schema và data demo được tách sang FE/assets/js/data/*.js.
// Luồng đọc sách của User vẫn đi qua PEP/CH ở backend, không gọi thẳng resource.
const schemas = window.AdminDataService?.getSchemas?.() || window.ADMIN_SCHEMAS || {};
let tableNames = window.AdminDataService?.getTableNames?.() || Object.keys(schemas);
let db = window.AdminDataService?.getDatabase?.() || JSON.parse(JSON.stringify(window.ADMIN_DEMO_DB || {}));

let modalState = null;
let toastMessage = '';
let policyCheck = null;
let adminSidebarCollapsed = localStorage.getItem('mh_admin_sidebar') === 'collapsed';
let adminDbSource = 'seed';
let adminDbLoading = false;
let adminDbError = '';
let adminTablePages = {};
const ADMIN_TABLE_PAGE_SIZE = 24;
let monitorAutoRefresh = localStorage.getItem('mh_admin_monitor_auto') !== 'off';
let monitorTimer = null;
let monitorSessionFilter = localStorage.getItem('mh_admin_monitor_filter') || '';
let adminUsageSessionFilter = localStorage.getItem('mh_admin_usage_session_filter') || '';
let adminInfoPayloads = {};
let adminInfoSeq = 1;


function toggleAdminSidebar(){
  adminSidebarCollapsed = !adminSidebarCollapsed;
  localStorage.setItem('mh_admin_sidebar', adminSidebarCollapsed ? 'collapsed' : 'expanded');
  route();
}

const adminMenuItems = [
  {key:'dashboard', href:'#dashboard', icon:'⌂', label:'Tổng quan'},
  {key:'monitor', href:'#monitor', icon:'◌', label:'Theo dõi UCON'},
  {key:'database', href:'#database/USERS', icon:'▦', label:'CSDL / Bảng dữ liệu'},
  {key:'policies', href:'#policies', icon:'⚖', label:'Chính sách truy cập'},
  {key:'sessions', href:'#sessions', icon:'◎', label:'Phiên sử dụng'},
  {key:'sql', href:'#sql', icon:'SQL', label:'Truy vấn SQL'}
];

function shell(active, inner){
  const nav = adminMenuItems.map(item => `<a class="${active===item.key?'active':''}" href="${item.href}" title="${item.label}"><span class="admin-menu-icon">${item.icon}</span><span class="admin-menu-label">${item.label}</span></a>`).join('');
  return `<div class="admin-shell ${adminSidebarCollapsed ? 'sidebar-collapsed' : ''}">
    <aside class="admin-sidebar">
      <div class="admin-side-top">
        <button class="admin-logo-mark" onclick="toggleAdminSidebar()" title="Thu gọn/mở rộng thanh bên">MH</button>
        <div class="admin-brand-wrap"><div class="admin-brand">MH Library</div><div class="admin-brand-sub">Admin console</div></div>
        <button class="admin-collapse-btn" onclick="toggleAdminSidebar()" title="Thu gọn/mở rộng thanh bên">${adminSidebarCollapsed ? '›' : '‹'}</button>
      </div>
      <nav class="admin-menu">${nav}</nav>
      <div class="admin-side-footer"><a class="admin-logout-link" href="index.html#login" onclick="sessionStorage.removeItem('ucon_current_user_id')">Đăng xuất</a></div>
    </aside>
    <section class="admin-content">${toastMessage?`<div class="success-box">${toastMessage}</div>`:''}${inner}</section>${renderModal()}</div>`;
}
function header(title, sub, actions=''){
  return `<div class="admin-header"><div><p class="eyebrow">ADMIN</p><h1 class="h1">${title}</h1><p class="muted">${sub}</p></div><div class="admin-actions">${actions}</div></div>`;
}
function escapeHtml(value){ return String(value ?? '').replaceAll('&','&amp;').replaceAll('<','&lt;').replaceAll('>','&gt;').replaceAll('"','&quot;'); }
function resetAdminInfoPayloads(){
  adminInfoPayloads = {};
  adminInfoSeq = 1;
}
function registerAdminInfoPayload(title, bodyHtml){
  const id = `info_${adminInfoSeq++}`;
  adminInfoPayloads[id] = {title, bodyHtml};
  return id;
}
function openAdminInfoModal(id){
  modalState = {type:'info', id};
  route();
}
function renderInfoModal(id){
  const payload = adminInfoPayloads[id] || {title:'Chi tiết', bodyHtml:'<p class="muted">Không tìm thấy nội dung chi tiết. Bạn refresh lại trang rồi mở lại nha.</p>'};
  return `<div class="modal-backdrop admin-info-backdrop"><div class="modal admin-info-modal"><div class="modal-head"><div><h2>${escapeHtml(payload.title || 'Chi tiết')}</h2><p class="muted">Dùng để đối chiếu log khi cần.</p></div><button type="button" class="btn-outline" onclick="closeModal()">Đóng</button></div><div class="admin-info-modal-body">${payload.bodyHtml}</div><div class="modal-actions"><button class="btn" onclick="closeModal()">Đã hiểu</button></div></div></div>`;
}
function popupButton(label, title, bodyHtml, className='btn-soft'){
  const id = registerAdminInfoPayload(title, bodyHtml);
  return `<button type="button" class="${className}" onclick="openAdminInfoModal('${id}')">${escapeHtml(label)}</button>`;
}
function setToast(message){ toastMessage = message; setTimeout(()=>{ toastMessage=''; route(); }, 1300); }
function adminSystemDialog({title='Thông báo', message='', confirmText='Đã hiểu', cancelText='', tone='default'} = {}){
  return new Promise(resolve => {
    const old = document.getElementById('admin-system-modal');
    if(old) old.remove();
    document.body.insertAdjacentHTML('beforeend', `<div id="admin-system-modal" class="system-modal-backdrop admin-system-modal" role="dialog" aria-modal="true">
      <div class="system-modal-card ${tone}">
        <div class="system-modal-kicker">Admin console</div>
        <h3>${escapeHtml(title)}</h3>
        <p>${escapeHtml(message)}</p>
        <div class="system-modal-actions">
          ${cancelText ? `<button id="admin-system-cancel" class="system-modal-btn secondary">${escapeHtml(cancelText)}</button>` : ''}
          <button id="admin-system-confirm" class="system-modal-btn ${tone === 'danger' ? 'danger' : ''}">${escapeHtml(confirmText)}</button>
        </div>
      </div>
    </div>`);
    const close = value => { document.getElementById('admin-system-modal')?.remove(); resolve(value); };
    document.getElementById('admin-system-confirm')?.addEventListener('click', () => close(true));
    document.getElementById('admin-system-cancel')?.addEventListener('click', () => close(false));
  });
}
function adminConfirmDialog(options){ return adminSystemDialog(options); }
function nextId(table){
  const rows = db[table] || [];
  if(!schemas[table]?.includes('id')) return '';
  return String(rows.reduce((m,r)=>Math.max(m, Number(r.id)||0),0)+1);
}
function semanticCellClass(value){
  const v = String(value ?? '').trim().toUpperCase();
  if(!v) return '';
  if(['ACTIVE','PERMIT','PERMITTED','TRUE','STARTED','ENDED','POST_UPDATE_DONE','USER_END'].includes(v)) return 'semantic-ok';
  if(['PAID','TRẢ PHÍ'].includes(v)) return 'semantic-paid';
  if(['RENTAL','THUÊ'].includes(v)) return 'semantic-rental';
  if(['FREE','MIỄN PHÍ'].includes(v)) return 'semantic-free';
  if(['REVOKED','DENY','DENIED','FALSE','LOCKED','DEPRECATED','POLICY_NOT_SATISFIED','DEPENDENCY_DELETED'].includes(v)) return 'semantic-danger';
  if(['PENDING','PREVIEW','ONGOING','POST'].includes(v)) return 'semantic-warn';
  return '';
}

function statusChipTone(value){
  const v = String(value ?? '').trim().toUpperCase();
  if(!v) return '';
  if(['ACTIVE','PERMIT','PERMITTED','TRUE','STARTED','ENDED','COMPLETED','POST_UPDATE_DONE','USER_END'].includes(v)) return 'chip-ok';
  if(['PAID','TRẢ PHÍ'].includes(v)) return 'chip-paid';
  if(['RENTAL','THUÊ'].includes(v)) return 'chip-rental';
  if(['FREE','MIỄN PHÍ'].includes(v)) return 'chip-free';
  if(['PRE'].includes(v)) return 'chip-pre';
  if(['ONGOING'].includes(v)) return 'chip-ongoing';
  if(['POST'].includes(v)) return 'chip-post';
  if(['REVOKE','REVOKED','DENY','DENIED','FALSE','LOCKED','DEPRECATED','POLICY_NOT_SATISFIED','DEPENDENCY_DELETED'].includes(v)) return 'chip-danger';
  if(['PENDING','PREVIEW','TIMER','DB_EVENT','HYBRID'].includes(v)) return 'chip-warn';
  return '';
}
function statusCell(v){
  const val = String(v ?? '');
  const key = val.toLowerCase();
  const tone = statusChipTone(val);
  if(tone) return `<span class="status-chip value-chip ${tone}">${escapeHtml(val)}</span>`;
  if(['deny','denied','revoked','false','disabled','expired','cancelled','locked'].includes(key)) return `<span class="status-chip value-chip chip-danger">${escapeHtml(val)}</span>`;
  if(['pending','ongoing','timer','db_event','hybrid'].includes(key)) return `<span class="status-chip value-chip chip-warn">${escapeHtml(val)}</span>`;
  return `<span class="cell-text">${escapeHtml(val)}</span>`;
}
function getTableColumns(table, data=[]){
  const schemaCols = schemas[table] || [];
  const dynamicCols = [...new Set((data || []).flatMap(row => Object.keys(row).filter(k => k !== '__rowIndex')))] ;
  return [...new Set([...schemaCols, ...dynamicCols])];
}
function rowOriginalIndex(row, fallbackIndex){
  return Number.isInteger(row.__rowIndex) ? row.__rowIndex : fallbackIndex;
}
function safeJs(value){
  return String(value ?? '').replace(/\\/g, '\\\\').replace(/'/g, "\\'").replace(/\n/g, ' ');
}
function setAdminTablePage(key, page){
  adminTablePages[key] = Math.max(1, Number(page) || 1);
  route();
}
function latestRows(table, count=6){
  const rows = [...(db[table] || [])];
  return rows.sort((a,b) => String(b.event_at || b.occurred_at || b.created_at || b.activated_at || b.id || '').localeCompare(String(a.event_at || a.occurred_at || a.created_at || a.activated_at || a.id || ''))).slice(0,count);
}
function countBy(rows, field){
  return (rows || []).reduce((acc,row)=>{
    const key = String(row?.[field] ?? 'UNKNOWN').toUpperCase();
    acc[key] = (acc[key] || 0) + 1;
    return acc;
  }, {});
}
function routeTableTab(base, selected, tables){
  return `<div class="admin-tabbar">${tables.map(t=>`<button class="${t===selected?'active':''}" onclick="location.hash='${base}/${t}'"><span>${t}</span><small>${tableCount(t)} dòng</small></button>`).join('')}</div>`;
}
function ensureMonitorTimer(active){
  if(monitorTimer){ clearInterval(monitorTimer); monitorTimer = null; }
  if(active && monitorAutoRefresh){
    monitorTimer = setInterval(() => refreshRealDatabase(true), 4000);
  }
}
function toggleMonitorAutoRefresh(){
  monitorAutoRefresh = !monitorAutoRefresh;
  localStorage.setItem('mh_admin_monitor_auto', monitorAutoRefresh ? 'on' : 'off');
  route();
}
function connectionStrip(){ return ''; }

function captureMonitorScrollState(){
  if(!(location.hash || '').startsWith('#monitor')) return null;
  const selectors = [
    '.session-picker-card',
    '.session-row-list.compact-list',
    '.session-detail-card',
    '.detail-columns section:nth-child(1)',
    '.detail-columns section:nth-child(2)',
    '.detail-columns section:nth-child(3)',
    '.detail-event-list',
    '.timeline-list'
  ];
  const items = selectors.map(selector => {
    const el = document.querySelector(selector);
    return el ? {selector, top:el.scrollTop, left:el.scrollLeft} : null;
  }).filter(Boolean);
  return {windowY: window.scrollY || 0, items};
}
function restoreMonitorScrollState(state){
  if(!state) return;
  requestAnimationFrame(() => {
    window.scrollTo(0, state.windowY || 0);
    (state.items || []).forEach(item => {
      const el = document.querySelector(item.selector);
      if(el){
        el.scrollTop = item.top || 0;
        el.scrollLeft = item.left || 0;
      }
    });
  });
}

function tableHTML(table, data, options={}){
  const rows = Array.isArray(data) ? data : [];
  const cols = getTableColumns(table, rows);
  const showActions = options.showActions !== false;
  const mode = options.mode || 'default';
  const contextKey = options.contextKey || `${mode}_${table}`;
  const enablePaging = options.paginate !== false;
  const pageSize = Number(options.pageSize || ADMIN_TABLE_PAGE_SIZE);
  const totalRows = rows.length;
  const totalPages = enablePaging ? Math.max(1, Math.ceil(totalRows / pageSize)) : 1;
  const page = Math.min(Math.max(1, adminTablePages[contextKey] || 1), totalPages);
  adminTablePages[contextKey] = page;
  const visibleRows = enablePaging ? rows.slice((page - 1) * pageSize, page * pageSize) : rows;
  const from = totalRows ? ((page - 1) * pageSize + 1) : 0;
  const to = enablePaging ? Math.min(totalRows, page * pageSize) : totalRows;
  const head = cols.map(c=>`<th>${escapeHtml(c)}</th>`).join('');
  const body = visibleRows.length ? visibleRows.map((row, index)=>{
    const realIndex = rowOriginalIndex(row, (page - 1) * pageSize + index);
    return `<tr>${cols.map(c=>`<td title="${escapeHtml(row[c])}">${statusCell(row[c])}</td>`).join('')}${showActions?`<td class="action-cell"><div class="row-actions"><button class="pill" onclick="openEdit('${table}',${realIndex})">Sửa</button><button class="danger" onclick="deleteRow('${table}',${realIndex})">Xóa</button></div></td>`:''}</tr>`;
  }).join('') : `<tr><td colspan="${cols.length+(showActions?1:0)}" class="muted">Chưa có dữ liệu</td></tr>`;
  const pager = enablePaging && totalRows > pageSize ? `<div class="table-pager">
    <span>Hiển thị ${from}-${to} / ${totalRows} dòng</span>
    <div>
      <button class="btn-outline" ${page<=1?'disabled':''} onclick="setAdminTablePage('${safeJs(contextKey)}', ${page-1})">Trang trước</button>
      <b>Trang ${page}/${totalPages}</b>
      <button class="btn-outline" ${page>=totalPages?'disabled':''} onclick="setAdminTablePage('${safeJs(contextKey)}', ${page+1})">Trang sau</button>
    </div>
  </div>` : `<div class="table-pager single"><span>${totalRows} dòng</span></div>`;
  return `<div class="table-wrap ${mode==='database'?'database-table-wrap':''}"><table class="table ${mode==='database'?'database-table':''}"><thead><tr>${head}${showActions?'<th>Thao tác</th>':''}</tr></thead><tbody>${body}</tbody></table></div>${pager}`;
}
function fieldInput(c, value){
  const readonly = c === 'id' && value !== '';
  if(c.endsWith('_json') || c === 'description' || c === 'payload_json' || c === 'details_json'){
    return `<label class="modal-field"><span>${escapeHtml(c)}</span><textarea name="${escapeHtml(c)}" ${readonly?'readonly':''}>${escapeHtml(value)}</textarea></label>`;
  }
  return `<label class="modal-field"><span>${escapeHtml(c)}</span><input name="${escapeHtml(c)}" value="${escapeHtml(value)}" ${readonly?'readonly':''}></label>`;
}
function getFilteredRows(table, query){
  const q = String(query || '').trim().toLowerCase();
  return (db[table] || []).map((row, index) => ({...row, __rowIndex:index})).filter(row => {
    if(!q) return true;
    return Object.values(row).some(v => String(v).toLowerCase().includes(q));
  });
}
function tableCount(table){ return (db[table] || []).length; }
function columnCount(table){ return getTableColumns(table, db[table] || []).length; }
function tableSummaryCards(){
  return `<div class="schema-list">${tableNames.map(t=>`<a class="schema-chip" href="#database/${t}">${t} · ${tableCount(t)} dòng · ${columnCount(t)} cột</a>`).join('')}</div>`;
}
function tableSearchPlaceholder(table){
  const hints = {
    BOOKS:'Tìm theo title, authors, genre, access_model, status...',
    BOOK_VOLUMES:'Tìm theo book_id, title, volume_no, price, access_model, status...',
    USAGE_SESSIONS:'Tìm theo session_id, user_id, resource_id, access_path, status...',
    SESSION_EVENTS:'Tìm theo session_id, event_type, phase, decision, reason_code...',
    USAGE_SESSION_DEPENDENCIES:'Tìm theo session_id, dependency_key, entity_type, entity_id...',
    USAGE_SESSION_POLICIES:'Tìm theo session_id, policy_id, bound_at...',
    USERS:'Tìm theo id, name, email, status...',
    USER_RENTALS:'Tìm theo user_id, plan_id, status, expires_at...',
    VOLUME_PURCHASES:'Tìm theo user_id, volume_id, purchased_at...',
    FAMILY_GROUPS:'Tìm theo owner_user_id, name, status...',
    FAMILY_GROUP_MEMBERS:'Tìm theo group_id, user_id, status...',
    FAMILY_SHARED_VOLUMES:'Tìm theo group_id, volume_id, status, shared_at...',
    POLICY_REGISTRY:'Tìm theo policy_ref, policy_name, access_path, phase, enabled...',
    POLICY_ATTRIBUTE_REQUIREMENTS:'Tìm theo policy_id, attr_key, usage_type, required...',
    PIP_ATTRIBUTE_MAPPINGS:'Tìm theo attr_key, reader_name, writer_name, dependency_name...',
    PIP_UPDATE_ACTIONS:'Tìm theo action_code, attr_key, handler_name, operation...',
    OBLIGATION_EXECUTIONS:'Tìm theo session_id, policy_id, obligation_type, status...'
  };
  return hints[String(table || '').toUpperCase()] || `Tìm theo các cột của bảng ${table}...`;
}
function sessionIdOfRow(table, row){
  if(!row) return '';
  if(String(table).toUpperCase() === 'USAGE_SESSIONS') return String(row.id ?? row.session_id ?? '');
  return String(row.usage_session_id ?? row.usageSessionId ?? '');
}
function normalizeSessionIdFilter(value){
  return String(value || '').replace('#','').trim();
}
function filterRowsBySessionId(table, rows, sessionId){
  const id = normalizeSessionIdFilter(sessionId);
  if(!id) return rows || [];
  return (rows || []).filter(row => sessionIdOfRow(table, row) === id);
}
function setUsageSessionFilter(value){
  adminUsageSessionFilter = normalizeSessionIdFilter(value);
  localStorage.setItem('mh_admin_usage_session_filter', adminUsageSessionFilter);
  adminTablePages = {...adminTablePages, sessions_USAGE_SESSIONS:1, sessions_SESSION_EVENTS:1, sessions_USAGE_SESSION_DEPENDENCIES:1, sessions_USAGE_SESSION_POLICIES:1, sessions_OBLIGATION_EXECUTIONS:1};
  route();
}

function databaseSourceNotice(){ return adminDbLoading ? `<div class="notice compact-admin-note">Đang cập nhật dữ liệu...</div>` : ''; }
function openAdd(table){ modalState = {type:'add', table}; route(); }
function openEdit(table,index){ modalState = {type:'edit', table, index}; route(); }
function closeModal(){ modalState = null; route(); }
async function submitRow(event){
  event.preventDefault();
  const data = Object.fromEntries(new FormData(event.target).entries());
  const {type, table, index} = modalState;
  db[table] = db[table] || [];

  if(adminDbSource === 'postgresql'){
    try{
      if(type === 'add'){
        await window.AdminDataService.createRealRow(table, data);
      }else{
        const original = db[table][index] || {};
        const primaryKey = window.AdminDataService?.getPrimaryKey?.(table) || 'id';
        data[primaryKey] = original[primaryKey];
        await window.AdminDataService.updateRealRow(table, data);
      }
      modalState = null;
      await refreshRealDatabase(true);
      setToast(type === 'add' ? `Đã thêm dòng thật vào ${table}.` : `Đã cập nhật thật dòng trong ${table}.`);
      route();
    }catch(error){
      const detail = error?.body?.detail ? `\n\nChi tiết: ${error.body.detail}` : '';
      alert(`Không lưu được vào DB thật: ${error.message || error}${detail}`);
    }
    return;
  }

  if(type === 'add') db[table].push(data); else db[table][index] = data;
  modalState = null;
  setToast(type === 'add' ? `Đã thêm dòng demo vào ${table}.` : `Đã cập nhật dòng demo trong ${table}.`);
  route();
}
async function deleteRow(table,index){
  const row = db[table]?.[index];
  if(!row){
    alert('Không tìm thấy dòng cần xóa.');
    return;
  }

  const primaryKey = window.AdminDataService?.getPrimaryKey?.(table) || 'id';
  const keyValue = row[primaryKey];
  const isRealDb = adminDbSource === 'postgresql' && typeof window.AdminDataService?.deleteRealRow === 'function';
  const message = isRealDb
    ? `XÓA THẬT dữ liệu?\n\nBảng: ${table}\nKhóa chính: ${primaryKey} = ${keyValue}\n\nNếu dòng này có dữ liệu con liên quan, hệ thống sẽ xóa kèm dữ liệu phụ thuộc cần thiết để tránh lỗi khóa ngoại.`
    : `Chưa kết nối được dữ liệu thật nên thao tác này chỉ xóa demo trên trình duyệt.\n\nBảng: ${table}\nKhóa chính: ${primaryKey} = ${keyValue}`;

  if(!await adminConfirmDialog({title:'Xác nhận xóa dữ liệu', message, confirmText:'Xóa', cancelText:'Hủy', tone:'danger'})) return;

  if(isRealDb){
    try{
      const result = await window.AdminDataService.deleteRealRow(table, row, true);
      await refreshRealDatabase(true);
      setToast(`Đã xóa dữ liệu thật: ${table} (${result.deletedRows || 0} dòng liên quan).`);
    }catch(error){
      const detail = error?.body?.detail ? `\n\nChi tiết: ${error.body.detail}` : '';
      alert(`Không xóa được trong DB thật: ${error.message || error}${detail}`);
    }
    return;
  }

  db[table].splice(index,1);
  setToast(`Đã xóa demo khỏi bảng ${table}.`);
  route();
}
function renderModal(){
  if(!modalState) return '';
  if(modalState.type === 'policy') return renderPolicyModal(modalState.index);
  if(modalState.type === 'info') return renderInfoModal(modalState.id);
  const {type, table, index} = modalState;
  const row = type === 'edit' ? db[table][index] : Object.fromEntries((schemas[table]||[]).map(c=>[c, c==='id' && adminDbSource !== 'postgresql'?nextId(table):'']));
  const title = type === 'add' ? `Thêm dòng vào ${table}` : `Sửa dòng #${row.id || ''} - ${table}`;
  return `<div class="modal-backdrop"><div class="modal"><form onsubmit="submitRow(event)"><div class="modal-head"><div><h2>${title}</h2><p class="muted">${adminDbSource === 'postgresql' ? 'Lưu thay đổi thật vào dữ liệu hệ thống.' : 'Nhập dữ liệu demo theo cột trong lược đồ CSDL.'}</p></div><button type="button" class="btn-outline" onclick="closeModal()">Đóng</button></div><div class="modal-grid">${(schemas[table]||Object.keys(row)).map(c=>fieldInput(c,row[c]||'')).join('')}</div><div class="modal-actions"><button type="button" class="btn-outline" onclick="closeModal()">Hủy</button><button class="btn">Lưu</button></div></form></div></div>`;
}

function dashboard(){
  const stats = window.AdminDataService?.getDashboardStats?.(db) || {policies:db.POLICY_REGISTRY.length,tables:tableNames.length,activeSessions:db.USAGE_SESSIONS.filter(x=>x.status==='active').length,endedSessions:0,revokedSessions:0};
  admin.innerHTML = shell('dashboard', `${header('Tổng quan quản trị','Tổng quan hệ thống.')}
    ${databaseSourceNotice()}
    ${connectionStrip()}
    <div class="stat-grid"><div class="stat">Chính sách<b>${stats.policies}</b></div><div class="stat">Bảng dữ liệu<b>${stats.tables}</b></div><div class="stat">Phiên đang chạy<b>${stats.activeSessions}</b></div><div class="stat">Phiên đã kết thúc<b>${stats.endedSessions}</b></div><div class="stat">Phiên bị thu hồi<b>${stats.revokedSessions}</b></div></div>
    <div class="admin-card"><h2 class="h2">Phân tách giao diện</h2><p class="notice">Trang User chỉ hiển thị thông tin đọc sách thân thiện với người dùng. Các thông tin kỹ thuật như policy, session, log, SQL, lỗi kiểm tra attribute sẽ nằm ở Admin, VSCode hoặc terminal.</p></div>
    <div class="admin-card"><h2 class="h2">Bảng dữ liệu</h2><p class="muted">Xem nhanh các bảng chính.</p>${tableSummaryCards()}</div>`);
}
function uconEntityCard(title, description, tables){
  return `<article class="ucon-entity-card"><h3>${escapeHtml(title)}</h3><p>${escapeHtml(description)}</p><div>${tables.map(t=>`<a href="#database/${t}">${t}</a>`).join('')}</div></article>`;
}
function database(selected='USERS'){
  const table = selected === 'all' ? 'all' : (schemas[selected] ? selected : 'USERS');
  const totalRows = tableNames.reduce((sum, t) => sum + tableCount(t), 0);
  const populatedTables = tableNames.filter(t=>tableCount(t)>0).length;
  const actions = `<button class="btn-soft" onclick="refreshRealDatabase(false)">Refresh DB thật</button><a class="btn-soft" href="#database/all">Tổng quan bảng</a><a class="btn-soft" href="#sql">Truy vấn SQL</a>`;
  if(table === 'all'){
    admin.innerHTML = shell('database', `${header('CSDL / Tổng quan bảng','Xem nhanh các bảng dữ liệu.', actions)}
      ${databaseSourceNotice()}
      <div class="db-stat-row"><div class="db-stat">Tổng bảng<b>${tableNames.length}</b></div><div class="db-stat">Tổng dòng<b>${totalRows}</b></div><div class="db-stat">Bảng có dữ liệu<b>${populatedTables}</b></div></div>
      <div class="admin-card db-index-card"><div class="db-table-head"><div><h2 class="h2">Danh sách bảng</h2><p class="muted">Chọn bảng để xem dữ liệu.</p></div></div>${tableSummaryCards()}</div>`);
    return;
  }
  const data = db[table] || [];
  const tableNav = tableNames.map(x=>`<button class="${x===table?'active':''}" onclick="location.hash='database/${x}'"><span>${x}</span><small>${tableCount(x)} dòng · ${columnCount(x)} cột</small></button>`).join('');
  admin.innerHTML = shell('database', `${header('CSDL / Bảng dữ liệu','Xem dữ liệu theo bảng.', actions)}
    ${databaseSourceNotice()}
    <div class="db-stat-row"><div class="db-stat">Tổng bảng<b>${tableNames.length}</b></div><div class="db-stat">Tổng dòng<b>${totalRows}</b></div><div class="db-stat">Đang xem<b>${table}</b></div><div class="db-stat">Số cột<b>${columnCount(table)}</b></div></div>
    <div class="database-workbench">
      <aside class="table-list db-table-list"><div class="table-list-title">Tables</div>${tableNav}</aside>
      <div class="admin-card db-main-card">
        <div class="db-table-head"><div><p class="eyebrow">TABLE</p><h2 class="h2">${table}</h2><p class="muted">${columnCount(table)} cột · ${data.length} dòng. Có thể thêm, sửa, xóa khi kết nối DB.</p></div></div>
        <div class="table-tools db-tools"><input class="input" placeholder="${escapeHtml(tableSearchPlaceholder(table))}" oninput="filterDatabaseTable('${table}', this.value)"><button class="btn-outline" onclick="openAdd('${table}')">+ Thêm dòng</button><button class="btn-outline" onclick="refreshRealDatabase(false)">Refresh DB</button></div>
        <div id="database-table">${tableHTML(table,data,{mode:'database', contextKey:`database_${table}`})}</div>
      </div>
    </div>`);
}
function filterDatabaseTable(table, value){
  const data = getFilteredRows(table, value);
  document.getElementById('database-table').innerHTML = tableHTML(table,data,{mode:'database', contextKey:`database_${table}`});
}

function policies(){
  const attrResult = policyCheck ? (policyCheck.invalid.length ? `<div class="error-box"><b>Lỗi attribute:</b><br>Policy có attribute chưa được khai báo/ánh xạ trong PIP_ATTRIBUTE_MAPPINGS: ${policyCheck.invalid.map(escapeHtml).join(', ')}.</div>` : `<div class="success-box">Policy hợp lệ. Tất cả attribute đều có trong PIP_ATTRIBUTE_MAPPINGS.</div>`) : '';
  const actions = `<button class="btn" onclick="openAdd('POLICY_REGISTRY')">+ Thêm chính sách</button>`;
  admin.innerHTML = shell('policies', `${header('Quản lý chính sách truy cập','Quản lý policy đang dùng.', actions)}
    ${databaseSourceNotice()}
    <div class="admin-card"><h2 class="h2">POLICY_REGISTRY</h2>${tableHTML('POLICY_REGISTRY', db.POLICY_REGISTRY, {contextKey:'policies_registry'})}</div>
    <div class="admin-card"><h2 class="h2">Kiểm tra file XACML policy</h2><p class="muted">Chọn file XML/XACML hoặc dán nội dung XML bên dưới. Nếu policy dùng attribute chưa có trong PIP_ATTRIBUTE_MAPPINGS, lỗi sẽ hiện tại Admin.</p>
      <div class="file-input"><input type="file" id="xacml-file" accept=".xml,.xacml" onchange="readXacmlFile(event)"></div>
      <textarea id="xacml-box" class="sql-box" style="margin-top:16px"><Policy>
  <Rule>
    <AttributeDesignator AttributeId="subject.status" />
    <AttributeDesignator AttributeId="object.access_model" />
  </Rule>
</Policy></textarea><br><br>
      <button class="btn-soft" onclick="checkXacml()">Kiểm tra attribute</button>${attrResult}
      <h3 class="font-serif" style="font-size:24px;color:#4A654F;margin-bottom:8px">Attribute hiện có</h3><div class="schema-list compact-chip-list">${db.PIP_ATTRIBUTE_MAPPINGS.map(a=>`<span class="schema-chip">${a.attr_key}</span>`).join('')}</div>
    </div>`);
}
function readXacmlFile(event){ const file = event.target.files?.[0]; if(!file) return; const reader = new FileReader(); reader.onload = () => { document.getElementById('xacml-box').value = reader.result; }; reader.readAsText(file); }
function checkXacml(){
  const xml = document.getElementById('xacml-box').value;
  const matches = [...xml.matchAll(/AttributeId\s*=\s*["']([^"']+)["']/g)].map(m=>m[1]);
  const known = new Set(db.PIP_ATTRIBUTE_MAPPINGS.flatMap(a=>[a.attr_key,a.xacml_attribute_id]));
  const invalid = [...new Set(matches.filter(x=>!known.has(x)))];
  policyCheck = {matches, invalid};
  route();
}
function sessions(selected='USAGE_SESSIONS'){
  const tables = ['USAGE_SESSIONS','SESSION_EVENTS','USAGE_SESSION_DEPENDENCIES','USAGE_SESSION_POLICIES','OBLIGATION_EXECUTIONS'];
  const table = tables.includes(selected) ? selected : 'USAGE_SESSIONS';
  const allSessions = db.USAGE_SESSIONS || [];
  const filterId = normalizeSessionIdFilter(adminUsageSessionFilter);
  const visibleSessions = filterRowsBySessionId('USAGE_SESSIONS', allSessions, filterId);
  const byStatus = countBy(visibleSessions, 'status');
  const tableRows = filterRowsBySessionId(table, db[table] || [], filterId);
  const empty = filterId && !visibleSessions.length
    ? `<div class="empty-read-history session-filter-empty">Không tìm thấy dữ liệu cho Session ID ${escapeHtml(filterId)}.</div>`
    : '';
  admin.innerHTML = shell('sessions', `${header('Phiên sử dụng','Gộp usage_sessions, session_events, dependency, policy binding và obligation theo cùng một nơi để dễ đối chiếu DB.')}
    ${connectionStrip()}
    <div class="session-filter-bar admin-card">
      <div><h2 class="h2">Lọc theo Session ID</h2><p class="muted">Nhập 1 hoặc #1 để lọc toàn bộ bảng con theo cùng session.</p></div>
      <div class="session-filter-actions"><input class="input" value="${escapeHtml(filterId)}" placeholder="VD: 1 hoặc #1" oninput="setUsageSessionFilter(this.value)"><button class="btn-outline" onclick="setUsageSessionFilter('')">Hiện tất cả</button></div>
    </div>
    <div class="mini-metric-row">
      <div class="mini-metric">Active<b>${byStatus.ACTIVE || 0}</b></div>
      <div class="mini-metric">Ended<b>${byStatus.ENDED || 0}</b></div>
      <div class="mini-metric">Revoked<b>${byStatus.REVOKED || 0}</b></div>
      <div class="mini-metric">Tổng session<b>${visibleSessions.length}</b></div>
    </div>
    ${empty}
    ${routeTableTab('sessions', table, tables)}
    <div class="admin-card observability-card"><div class="db-table-head"><div><h2 class="h2">${table}</h2><p class="muted">${filterId ? `Đang lọc theo Session ID ${escapeHtml(filterId)}.` : 'Dữ liệu theo session.'}</p></div><button class="btn-outline" onclick="refreshRealDatabase(false)">Refresh</button></div>${tableHTML(table, tableRows, {contextKey:`sessions_${table}_${filterId || 'all'}`})}</div>`);
}
function logs(selected='SESSION_EVENTS'){
  const tables = ['SESSION_EVENTS','OBLIGATION_EXECUTIONS'];
  const table = tables.includes(selected) ? selected : 'SESSION_EVENTS';
  const events = db.SESSION_EVENTS || [];
  const denies = events.filter(x => String(x.decision || '').toUpperCase() === 'DENY').length;
  admin.innerHTML = shell('logs', `${header('Nhật ký kiểm soát','Xem log theo từng nhóm để không bị kéo dài lê thê.')}
    ${connectionStrip()}
    <div class="mini-metric-row">
      <div class="mini-metric">Event session<b>${events.length}</b></div>
      <div class="mini-metric">DENY/Revoke<b>${denies}</b></div>
      <div class="mini-metric">Obligation<b>${tableCount('OBLIGATION_EXECUTIONS')}</b></div>
    </div>
    ${routeTableTab('logs', table, tables)}
    <div class="admin-card observability-card"><div class="db-table-head"><div><h2 class="h2">${table}</h2><p class="muted">Dữ liệu mới nhất nằm trong bảng, có phân trang và scroll ngang/dọc.</p></div><button class="btn-outline" onclick="refreshRealDatabase(false)">Refresh</button></div>${tableHTML(table, db[table] || [], {contextKey:`logs_${table}`})}</div>`);
}

function eventSessionId(event){
  return String(event?.usage_session_id || event?.usageSessionId || '');
}
function eventsForSession(sessionId){
  const id = String(sessionId || '');
  return (db.SESSION_EVENTS || []).filter(e => eventSessionId(e) === id);
}
function latestSessionEvent(sessionId){
  return [...eventsForSession(sessionId)].sort((a,b)=>String(b.event_at || '').localeCompare(String(a.event_at || '')))[0] || null;
}
function sourceTag(label){
  return `<span class="source-tag">Nguồn: ${escapeHtml(label)}</span>`;
}
function hasSessionPhase(sessionEvents, phase){
  const p = String(phase || '').toUpperCase();
  return sessionEvents.some(e => String(e.phase || '').toUpperCase() === p);
}
function sessionWasRevoked(session, sessionEvents){
  return String(session?.status || '').toLowerCase() === 'revoked'
    || sessionEvents.some(e => String(e.decision || '').toUpperCase() === 'DENY');
}
function phaseEventCount(phase){
  const p = String(phase || '').toUpperCase();
  return (db.SESSION_EVENTS || []).filter(e => String(e.phase || '').toUpperCase() === p).length;
}
function sessionCountWithPhase(sessions, phase){
  return (sessions || []).filter(s => hasSessionPhase(eventsForSession(s.id || s.session_id), phase)).length;
}
function sessionCountRevoked(sessions){
  return (sessions || []).filter(s => sessionWasRevoked(s, eventsForSession(s.id || s.session_id))).length;
}
function setMonitorCardPage(page){
  adminTablePages.monitor_session_cards = Math.max(1, Number(page) || 1);
  route();
}
function monitorDisplaySessions(){
  const sessions = db.USAGE_SESSIONS || [];
  const runningStatuses = new Set(['active','pending']);
  const runningSessions = sessions.filter(x => runningStatuses.has(String(x.status || '').toLowerCase()));
  const recentClosed = sessions
    .filter(x => !runningStatuses.has(String(x.status || '').toLowerCase()))
    .sort((a,b)=>String(b.ended_at || b.revoked_at || b.created_at || '').localeCompare(String(a.ended_at || a.revoked_at || a.created_at || '')))
    .slice(0, 50);
  return [...runningSessions, ...recentClosed];
}
function monitorSessionDisplayNo(sessionId){
  const index = monitorDisplaySessions().findIndex(s => String(s.id) === String(sessionId));
  return index >= 0 ? index + 1 : '';
}
function renderSessionListPanel(sessions, selectedSessionId){
  const rows = Array.isArray(sessions) ? sessions : [];
  const pageSize = 18;
  const totalRows = rows.length;
  const totalPages = Math.max(1, Math.ceil(totalRows / pageSize));
  const page = Math.min(Math.max(1, adminTablePages.monitor_session_cards || 1), totalPages);
  adminTablePages.monitor_session_cards = page;
  const visible = rows.slice((page - 1) * pageSize, page * pageSize);
  const from = totalRows ? ((page - 1) * pageSize + 1) : 0;
  const to = Math.min(totalRows, page * pageSize);
  const list = visible.map(s => renderSessionRowCard(s, String(s.id) === String(selectedSessionId), monitorSessionDisplayNo(s.id))).join('') || '<p class="muted">Không có session phù hợp.</p>';
  const pager = totalRows > pageSize ? `<div class="table-pager session-card-pager">
    <span>${from}-${to} / ${totalRows} session</span>
    <div>
      <button class="btn-outline" ${page<=1?'disabled':''} onclick="setMonitorCardPage(${page-1})">Trước</button>
      <b>${page}/${totalPages}</b>
      <button class="btn-outline" ${page>=totalPages?'disabled':''} onclick="setMonitorCardPage(${page+1})">Sau</button>
    </div>
  </div>` : `<div class="table-pager single"><span>${totalRows} session</span></div>`;
  return `<div class="session-row-list compact-list">${list}</div>${pager}`;
}
function renderSessionRowCard(s, selected=false, displayNo=''){
  return `<button class="session-row-card ${selected ? 'selected' : ''}" onclick="openMonitorSession(${Number(s.id)})">
    <div><b class="session-row-id">Session ID: ${escapeHtml(s.id || '')}</b><span>${escapeHtml(s.status || '')}</span></div>
    <p>User ${escapeHtml(s.subject_user_id || '')} · ${escapeHtml(s.access_path || '')} · ${escapeHtml(s.resource_type || '')} ${escapeHtml(s.resource_id || '')}</p>
    ${renderSessionFlowMini(s)}
  </button>`;
}

function renderPhaseStep(label, active, detail=''){
  const phaseClass = `step-${String(label || '').toLowerCase().replace(/[^a-z0-9]+/g, '-')}`;
  return `<div class="ucon-step ${phaseClass} ${active ? 'done' : 'empty'}">
    <span>${active ? '✓' : ''}</span>
    <b>${label}</b>
    ${detail ? `<small>${escapeHtml(detail)}</small>` : ''}
  </div>`;
}
function renderSessionFlowCard(session){
  const sessionId = session.id || session.session_id || '';
  const sessionNo = monitorSessionDisplayNo(sessionId) || sessionId;
  const sessionEvents = eventsForSession(sessionId);
  const preDone = hasSessionPhase(sessionEvents, 'PRE');
  const ongoingDone = hasSessionPhase(sessionEvents, 'ONGOING');
  const postDone = hasSessionPhase(sessionEvents, 'POST');
  const revokedDone = sessionWasRevoked(session, sessionEvents);
  const lastEvent = [...sessionEvents].sort((a,b)=>String(b.event_at || '').localeCompare(String(a.event_at || '')))[0];
  return `<article class="ucon-session-card">
    <div class="ucon-session-head">
      <div>
        <p>Session ID: ${escapeHtml(sessionId)}</p>
        <h3>${escapeHtml(session.access_path || '')} · ${escapeHtml(session.resource_type || '')} ${escapeHtml(session.resource_id || '')}</h3>
      </div>
      <span class="ucon-live-badge ${String(session.status||'').toLowerCase()}">${escapeHtml(session.status || '')}</span>
    </div>
    <div class="ucon-flow-line">
      ${renderPhaseStep('PRE', preDone, preDone ? 'Đã kiểm tra trước khi cấp quyền' : '')}
      ${renderPhaseStep('ONGOING', ongoingDone, ongoingDone ? 'Đang/đã kiểm tra khi đọc' : '')}
      ${renderPhaseStep('POST', postDone, postDone ? 'Đã xử lý sau khi kết thúc' : '')}
      ${renderPhaseStep('REVOKE', revokedDone, revokedDone ? 'Đã thu hồi/chặn' : '')}
    </div>
    <div class="ucon-session-foot">
      <span>Event gần nhất: ${escapeHtml(lastEvent?.event_at || 'Chưa có event')}</span>
      <span>${lastEvent ? `${escapeHtml(lastEvent.event_type || '')} · ${escapeHtml(lastEvent.phase || '')} · ${escapeHtml(lastEvent.decision || '')}` : 'Chưa có log'}</span>
    </div>
  </article>`;
}
function renderLatestTimeline(events){
  if(!events.length) return '<p class="muted">Chưa có event được ghi nhận.</p>';
  return events.map(e=>`<div class="timeline-item ${String(e.decision||'').toLowerCase()}">
    <b>${escapeHtml(e.event_type || 'EVENT')}</b>
    <span>${escapeHtml(e.phase || '')} · ${escapeHtml(e.decision || '')} · session ${escapeHtml(e.usage_session_id || '')}</span>
    <small>${escapeHtml(e.event_at || '')}</small>
  </div>`).join('');
}

function setMonitorFilter(value){
  monitorSessionFilter = String(value || '');
  localStorage.setItem('mh_admin_monitor_filter', monitorSessionFilter);
  adminTablePages.monitor_sessions = 1;
  route();
}
function openMonitorSession(sessionId){
  location.hash = `monitor/${sessionId}`;
}
function eventSessionId(event){
  return String(event?.usage_session_id || event?.usageSessionId || '');
}
function eventsForSession(sessionId){
  const id = String(sessionId || '');
  return (db.SESSION_EVENTS || []).filter(e => eventSessionId(e) === id);
}
function latestSessionEvent(sessionId){
  return [...eventsForSession(sessionId)].sort((a,b)=>String(b.event_at || '').localeCompare(String(a.event_at || '')))[0] || null;
}
function sourceTag(label){
  return `<span class="source-tag">Nguồn: ${escapeHtml(label)}</span>`;
}
function hasSessionPhase(sessionEvents, phase){
  const p = String(phase || '').toUpperCase();
  return sessionEvents.some(e => String(e.phase || '').toUpperCase() === p);
}
function sessionWasRevoked(session, sessionEvents){
  return String(session?.status || '').toLowerCase() === 'revoked'
    || sessionEvents.some(e => String(e.decision || '').toUpperCase() === 'DENY');
}
function phaseEventCount(phase){
  const p = String(phase || '').toUpperCase();
  return (db.SESSION_EVENTS || []).filter(e => String(e.phase || '').toUpperCase() === p).length;
}
function parseDetailsJson(value){
  if(!value) return {};
  if(typeof value === 'object') return value;
  try{return JSON.parse(String(value));}catch(_){return {message:String(value)};}
}
function policiesForSession(sessionId){
  const bound = (db.USAGE_SESSION_POLICIES || []).filter(x => String(x.usage_session_id) === String(sessionId));
  return bound.map(b => (db.POLICY_REGISTRY || []).find(p => String(p.id) === String(b.policy_id))).filter(Boolean);
}
function policyRequirements(policyId){
  return (db.POLICY_ATTRIBUTE_REQUIREMENTS || []).filter(r => String(r.policy_id) === String(policyId));
}
function attrMapping(attrKey){
  return (db.PIP_ATTRIBUTE_MAPPINGS || []).find(a => String(a.attr_key) === String(attrKey));
}
function latestEvaluationDetails(sessionEvents){
  const withDetails = [...sessionEvents]
    .sort((a,b)=>String(b.event_at || '').localeCompare(String(a.event_at || '')))
    .map(e => ({event:e, details:parseDetailsJson(e.details_json)}))
    .find(x => Array.isArray(x.details.checkedAttributes));
  return withDetails || {event:null, details:{}};
}
function latestEvaluationDetailsForPhase(sessionEvents, phase){
  const p = String(phase || '').toUpperCase();
  const withDetails = [...(sessionEvents || [])]
    .filter(e => String(e.phase || '').toUpperCase() === p)
    .sort((a,b)=>String(b.event_at || '').localeCompare(String(a.event_at || '')))
    .map(e => ({event:e, details:parseDetailsJson(e.details_json)}))
    .find(x => Array.isArray(x.details.checkedAttributes));
  return withDetails || {event:null, details:{}};
}
function renderPhaseStep(label, active, detail=''){
  const phaseClass = `step-${String(label || '').toLowerCase().replace(/[^a-z0-9]+/g, '-')}`;
  return `<div class="ucon-step ${phaseClass} ${active ? 'done' : 'empty'}">
    <span>${active ? '✓' : ''}</span>
    <b>${label}</b>
    ${detail ? `<small>${escapeHtml(detail)}</small>` : ''}
  </div>`;
}
function renderSessionFlowMini(session){
  const sessionId = session.id || session.session_id || '';
  const sessionEvents = eventsForSession(sessionId);
  const preDone = hasSessionPhase(sessionEvents, 'PRE');
  const ongoingDone = hasSessionPhase(sessionEvents, 'ONGOING');
  const postDone = hasSessionPhase(sessionEvents, 'POST');
  const revokedDone = sessionWasRevoked(session, sessionEvents);
  return `<div class="mini-flow" title="PRE / ONGOING / POST / REVOKE">
    <span class="${preDone?'on':''}">P</span><span class="${ongoingDone?'on':''}">O</span><span class="${postDone?'on':''}">P</span><span class="${revokedDone?'bad':''}">R</span>
  </div>`;
}
function renderSessionRowCards(sessions){
  if(!sessions.length) return '<p class="muted">Không có session phù hợp.</p>';
  return sessions.map(s=>renderSessionRowCard(s, false)).join('');
}

function checkedAttributeByName(attrs, name){
  const key = String(name || '').toLowerCase();
  return (attrs || []).find(a => String(a.name || a.attrKey || '').toLowerCase() === key);
}
function checkedValue(attrs, names, fallback=''){
  const found = (names || []).map(name => checkedAttributeByName(attrs, name)).find(Boolean);
  if(!found) return fallback;
  return found.value ?? found.attrValue ?? found.currentValue ?? fallback;
}
function renderCheckCard(title, rows, tone=''){
  const visibleRows = (rows || []).filter(row => row && row.label);
  return `<div class="check-card ${tone}">
    <h4>${escapeHtml(title)}</h4>
    ${visibleRows.map(row=>`<div><span>${escapeHtml(row.label)}</span><b>${displayCheckValue(row.value)}</b></div>`).join('')}
  </div>`;
}
function renderCheckOverview(session, checkedAttrs, dependencies){
  const policyPhaseList = policiesForSession(session.id || session.session_id).map(p => `${p.phase || ''}: ${p.policy_name || p.policy_ref || ''}`).filter(Boolean).join(' | ') || 'chưa bind policy';
  const depText = (dependencies || []).map(d => `${d.dependency_key || ''} → ${d.entity_type || ''} #${d.entity_id || ''}`).filter(Boolean).join(' | ') || 'không có';
  return `<div class="check-overview-grid">
    ${renderCheckCard('Người dùng / subject', [
      {label:'subject_user_id', value:session.subject_user_id},
      {label:'subject.status', value:checkedValue(checkedAttrs, ['subject.status', 'subjectStatus'])},
      {label:'access_path', value:session.access_path},
      {label:'action_id', value:session.action_id}
    ])}
    ${renderCheckCard('Tài nguyên / resource', [
      {label:'resource_type', value:session.resource_type},
      {label:'resource_id', value:session.resource_id},
      {label:'book.status', value:checkedValue(checkedAttrs, ['book.status', 'bookStatus'])},
      {label:'resource.status', value:checkedValue(checkedAttrs, ['resource.status', 'resourceStatus'])},
      {label:'access_model', value:checkedValue(checkedAttrs, ['resource.effective_access_model', 'object.access_model', 'resource.access_model'])}
    ])}
    ${renderCheckCard('Quyền thuê / mua / chia sẻ', [
      {label:'currentRental.active', value:checkedValue(checkedAttrs, ['currentRental.active', 'rental.active'])},
      {label:'currentRental.expires_at', value:checkedValue(checkedAttrs, ['currentRental.expires_at', 'rental.expires_at'])},
      {label:'volumePurchase.active', value:checkedValue(checkedAttrs, ['volumePurchase.active', 'currentPurchase.active'])},
      {label:'familyShare.active', value:checkedValue(checkedAttrs, ['familyShare.active', 'sharedVolume.active'])}
    ], 'accent')}
    ${renderCheckCard('Policy / dependency', [
      {label:'policy đã bind', value:policyPhaseList},
      {label:'dependency theo dõi', value:depText},
      {label:'latest session_events.event_at', value:latestSessionEvent(session.id || session.session_id)?.event_at || ''},
      {label:'status', value:session.status}
    ], 'muted-card')}
  </div>`;
}

function eventsByPhase(sessionEvents, phase){
  const p = String(phase || '').toUpperCase();
  return (sessionEvents || []).filter(e => String(e.phase || '').toUpperCase() === p);
}
function policiesByPhase(sessionId, phase){
  const p = String(phase || '').toUpperCase();
  return policiesForSession(sessionId).filter(policy => String(policy.phase || '').toUpperCase() === p);
}
function hasCheckValue(value){
  const normalized = String(value ?? '').trim();
  if(!normalized) return false;
  const lower = normalized.toLowerCase();
  if(lower.startsWith('chưa') && (lower.includes('ghi') || lower.includes('lưu'))) return false;
  return true;
}
function displayCheckValue(value){
  if(!hasCheckValue(value)) return '';
  return escapeHtml(String(value ?? '').trim());
}
function sessionEndReason(session, sessionEvents){
  const status = String(session?.status || '').toLowerCase();
  const revokedReason = String(session?.revoked_reason || '').trim();
  const deny = [...(sessionEvents || [])].reverse().find(e => String(e.decision || '').toUpperCase() === 'DENY' || String(e.reason_code || '').trim());
  if(revokedReason) return `Bị thu hồi: ${revokedReason}`;
  if(status === 'revoked') return `Bị thu hồi${deny?.reason_code ? ': ' + deny.reason_code : ''}`;
  if(session?.ended_at || session?.post_handled_at) return 'Người dùng kết thúc phiên đọc';
  if(status === 'active') return 'Phiên vẫn đang hoạt động';
  if(status === 'pending') return 'Đang chờ bắt đầu sử dụng';
  return status ? `Trạng thái: ${status}` : 'Chưa có lý do kết thúc';
}
function phaseAttrKeys(sessionId, phase){
  const keys = [];
  policiesByPhase(sessionId, phase).forEach(policy => {
    policyRequirements(policy.id).forEach(req => {
      const key = String(req.attr_key || '').trim();
      if(key && !keys.includes(key)) keys.push(key);
    });
  });
  return keys;
}
function phaseAttributeChecks(sessionId, phase, checkedAttrs, fallbackRows=[]){
  const keys = phaseAttrKeys(sessionId, phase);
  if(!keys.length) return fallbackRows;
  return keys.map(key => ({label:key, value:checkedValue(checkedAttrs, [key])}));
}
function renderPhaseEventsInline(events){
  const rows = (events || []).sort((a,b)=>String(a.event_at || '').localeCompare(String(b.event_at || '')));
  if(!rows.length) return '<p class="phase-event-empty">Chưa có log.</p>';
  return `<div class="phase-event-list-inline all-phase-events">${rows.map(e => `<div class="phase-event-pill ${String(e.decision || '').toLowerCase()}"><b>${escapeHtml(e.event_type || 'EVENT')}</b><span>${escapeHtml(e.decision || '')}${e.reason_code ? ' · ' + escapeHtml(e.reason_code) : ''}</span></div>`).join('')}</div>`;
}
function renderEventTypeSummary(eventTypes){
  const entries = Object.entries(eventTypes || {}).sort((a,b)=>b[1]-a[1]).slice(0,8);
  if(!entries.length) return '<p class="muted">Chưa có event.</p>';
  return `<div class="event-type-summary-grid">${entries.map(([k,v])=>`<div><span>${escapeHtml(k)}</span><b>${v}</b></div>`).join('')}</div>`;
}
function friendlyAttrLabel(label){
  const key = String(label || '').toLowerCase();
  const dict = {
    'subject.status':'User',
    'subject.id':'User ID',
    'resource.type':'Loại tài nguyên',
    'resource.id':'Mã tài nguyên',
    'object.access_model':'Loại sách',
    'access.path':'Quyền đọc',
    'action.id':'Thao tác',
    'rental.active':'Gói thuê',
    'rental.expires_at':'Hạn gói',
    'purchase.exists':'Đã mua',
    'family_share.exists':'Chia sẻ nhóm',
    'lý do kết thúc':'Lý do',
    'ended_at':'Kết thúc',
    'revoked_at':'Thu hồi',
    'post_handled_at':'Hậu xử lý'
  };
  return dict[key] || String(label || '').replace(/_/g,' ');
}
function compactTime(value){
  if(!value) return '';
  const parts = String(value).trim().split(' ');
  if(parts.length >= 2){
    const [y,m,d] = parts[0].split('-');
    return y && m && d ? `${d}/${m}/${y} ${parts[1].slice(0,5)}` : value;
  }
  return value;
}
function compactPhaseFacts(events, checks, snapshotEvent){
  const facts = [];
  const latest = [...(events || [])].sort((a,b)=>String(b.event_at || '').localeCompare(String(a.event_at || '')))[0] || snapshotEvent || null;
  if(latest?.event_at) facts.push({label:'Ghi nhận lúc', value:compactTime(latest.event_at)});
  if(latest?.decision) facts.push({label:'Kết quả', value:latest.decision});
  if(latest?.reason_code) facts.push({label:'Ghi chú', value:latest.reason_code});
  (checks || []).filter(item => hasCheckValue(item.value)).slice(0, 6).forEach(item => {
    facts.push({label:friendlyAttrLabel(item.label), value:item.value});
  });
  return facts.slice(0, 8);
}
function ongoingStatusText(session, hasOngoingEvent){
  if(!hasOngoingEvent) return 'Chưa chạy';
  const status = String(session?.status || '').toLowerCase();
  if((status === 'active' || status === 'pending') && !session?.ended_at && !session?.revoked_at) return 'Đang theo dõi';
  if(status === 'revoked') return 'Đã thu hồi';
  return 'Đã ghi nhận';
}
function renderPhaseCheckPanel({phase, title, summary, status, events, checks, tone='', source='', statusText='', snapshotEvent=null}) {
  const phaseKey = String(phase || '').toLowerCase().replace(/[^a-z0-9]+/g, '-').replace(/^-|-$/g, '') || 'phase';
  const eventCount = (events || []).length;
  const visibleChecks = (checks || []).filter(item => hasCheckValue(item.value));
  const facts = compactPhaseFacts(events, visibleChecks, snapshotEvent);
  if(phaseKey.includes('post') && !status && !eventCount && !visibleChecks.length) return '';
  return `<article class="phase-card-flat phase-${phaseKey} ${tone} ${status ? 'done' : 'empty'}">
    <div class="phase-card-top">
      <span class="phase-name">${escapeHtml(phase)}</span>
      <span class="phase-status">${escapeHtml(statusText || (status ? 'Đã ghi nhận' : 'Chưa chạy'))}</span>
    </div>
    <div class="phase-card-title">
      <h4>${escapeHtml(title)}</h4>
      <p>${escapeHtml(summary)}</p>
    </div>
    ${facts.length ? `<div class="phase-compact-facts">${facts.map(item => `<div><span>${escapeHtml(item.label)}</span><b>${displayCheckValue(item.value)}</b></div>`).join('')}</div>` : ''}
  </article>`;
}
function renderPhaseCheckBoard(session, sessionEvents, checkedAttrs, dependencies){
  const sessionId = session.id || session.session_id || '';
  const preEvents = eventsByPhase(sessionEvents, 'PRE');
  const ongoingEvents = eventsByPhase(sessionEvents, 'ONGOING');
  const postEvents = eventsByPhase(sessionEvents, 'POST');
  const denyEvents = (sessionEvents || []).filter(e => String(e.decision || '').toUpperCase() === 'DENY');
  const preEval = latestEvaluationDetailsForPhase(sessionEvents, 'PRE');
  const ongoingEval = latestEvaluationDetailsForPhase(sessionEvents, 'ONGOING');
  const postEval = latestEvaluationDetailsForPhase(sessionEvents, 'POST');
  const denyEval = [...denyEvents].sort((a,b)=>String(b.event_at || '').localeCompare(String(a.event_at || ''))).map(e => ({event:e, details:parseDetailsJson(e.details_json)})).find(x => Array.isArray(x.details.checkedAttributes)) || {event:null, details:{}};
  const preAttrs = Array.isArray(preEval.details.checkedAttributes) ? preEval.details.checkedAttributes : [];
  const ongoingAttrs = Array.isArray(ongoingEval.details.checkedAttributes) ? ongoingEval.details.checkedAttributes : [];
  return `<div class="phase-check-board">
    ${renderPhaseCheckPanel({
      phase:'PRE',
      title:'Trước khi cấp quyền đọc',
      summary:'Kiểm tra điều kiện trước khi mở phiên đọc.',
      status:preEvents.length > 0,
      statusText:preEvents.length > 0 ? 'Đã đánh giá' : 'Chưa chạy',
      snapshotEvent:preEval.event,
      events:preEvents,
      checks:phaseAttributeChecks(sessionId, 'PRE', preAttrs)
    })}
    ${renderPhaseCheckPanel({
      phase:'ONGOING',
      title:'Trong lúc đọc',
      summary:'Ghi nhận trạng thái gần nhất trong phiên đọc.',
      status:ongoingEvents.length > 0,
      statusText:ongoingStatusText(session, ongoingEvents.length > 0),
      snapshotEvent:ongoingEval.event,
      events:ongoingEvents,
      tone:'accent',
      checks:phaseAttributeChecks(sessionId, 'ONGOING', ongoingAttrs)
    })}
    ${renderPhaseCheckPanel({
      phase:'POST / REVOKE',
      title:'Sau khi kết thúc hoặc thu hồi',
      summary:'Ghi nhận trạng thái cuối phiên và lý do xử lý.',
      status:postEvents.length > 0 || denyEvents.length > 0 || String(session.status || '').toLowerCase() === 'revoked' || Boolean(session.ended_at || session.revoked_at || session.post_handled_at),
      statusText:(postEvents.length > 0 || denyEvents.length > 0 || String(session.status || '').toLowerCase() === 'revoked' || Boolean(session.ended_at || session.revoked_at || session.post_handled_at)) ? 'Đã ghi nhận' : 'Chưa chạy',
      snapshotEvent:(postEval.event || denyEval.event),
      events:[...postEvents, ...denyEvents],
      tone:'post',
      checks:[
        {label:'lý do kết thúc', value:(postEvents.length > 0 || denyEvents.length > 0 || String(session.status || '').toLowerCase() === 'revoked' || Boolean(session.ended_at || session.revoked_at || session.post_handled_at)) ? sessionEndReason(session, [...postEvents, ...denyEvents]) : ''},
        {label:'ended_at', value:session.ended_at || ''},
        {label:'revoked_at', value:session.revoked_at || ''},
        {label:'post_handled_at', value:session.post_handled_at || ''}
      ]
    })}
  </div>`;
}
function renderSessionDetail(session){
  if(!session) return `<div class="admin-card monitor-card session-detail-card empty"><h2 class="h2">Chọn một session</h2><p class="muted">Bấm vào một session trong danh sách để xem chi tiết policy, quyền kiểm tra, event và thời điểm kết thúc.</p></div>`;
  const sessionId = session.id || session.session_id || '';
  const sessionNo = monitorSessionDisplayNo(sessionId) || sessionId;
  const sessionEvents = eventsForSession(sessionId).sort((a,b)=>String(a.event_at || '').localeCompare(String(b.event_at || '')));
  const preDone = hasSessionPhase(sessionEvents, 'PRE');
  const ongoingDone = hasSessionPhase(sessionEvents, 'ONGOING');
  const postDone = hasSessionPhase(sessionEvents, 'POST');
  const revokedDone = sessionWasRevoked(session, sessionEvents);
  const dependencies = (db.USAGE_SESSION_DEPENDENCIES || []).filter(d => String(d.usage_session_id) === String(sessionId));
  const evalDetails = latestEvaluationDetails(sessionEvents).details;
  const checkedAttrs = Array.isArray(evalDetails.checkedAttributes) ? evalDetails.checkedAttributes : [];
  const endReason = sessionEndReason(session, sessionEvents);
  const lastEvent = latestSessionEvent(sessionId);
  const latestEventText = lastEvent?.event_at || '';
  const phaseBoardHtml = renderPhaseCheckBoard(session, sessionEvents, checkedAttrs, dependencies);
  const checkedHtml = checkedAttrs.map(a=>`<div class="checked-attr"><b>${escapeHtml(a.name || '')}</b><span>${escapeHtml(a.value ?? '')}</span></div>`).join('') || '<p class="muted">Session có log phase, nhưng event chưa lưu snapshot giá trị attribute để hiển thị chi tiết.</p>';
  const depsHtml = dependencies.map(d=>`<span class="dependency-pill">${escapeHtml(d.dependency_key || '')}: ${escapeHtml(d.entity_type || '')} #${escapeHtml(d.entity_id || '')}</span>`).join('') || '<p class="muted">Không có dependency đang monitor.</p>';
  const timeline = sessionEvents.map(e=>`<div class="detail-event ${String(e.decision||'').toLowerCase()}"><b>${escapeHtml(e.event_type || '')}</b><span>${escapeHtml(e.phase || '')} · ${escapeHtml(e.decision || '')} · ${escapeHtml(e.reason_code || '')}</span><small>${escapeHtml(e.event_at || '')}</small></div>`).join('') || '<p class="muted">Chưa có event.</p>';
  return `<div class="admin-card monitor-card session-detail-card simple-session-detail">
    <div class="session-detail-head"><div><p class="session-detail-kicker">Chi tiết session</p><h2><span class="session-id-hero">SESSION ID: ${escapeHtml(sessionId)}</span> ${escapeHtml(session.status || '')} · User ${escapeHtml(session.subject_user_id || '')}</h2><span>${escapeHtml(session.access_path || '')} · ${escapeHtml(session.resource_type || '')} ${escapeHtml(session.resource_id || '')}</span></div><a class="btn-outline" href="#database/USAGE_SESSIONS">Mở bảng session</a></div>
    <div class="ucon-flow-line detail-flow">
      ${renderPhaseStep('PRE', preDone, preDone ? 'Đã kiểm tra trước khi cấp quyền' : '')}
      ${renderPhaseStep('ONGOING', ongoingDone, ongoingDone ? ongoingStatusText(session, true) : '')}
      ${renderPhaseStep('POST', postDone, postDone ? 'Đã ghi nhận kết thúc/hậu xử lý' : '')}
      ${renderPhaseStep('REVOKE', revokedDone, revokedDone ? 'Bị chặn hoặc thu hồi' : '')}
    </div>
    <div class="session-fact-grid compact-facts">
      <div><span>Tạo phiên</span><b>${escapeHtml(session.created_at || '')}</b></div>
      <div><span>Bắt đầu đọc</span><b>${escapeHtml(session.activated_at || '')}</b></div>
      <div><span>Sự kiện gần nhất</span><b>${escapeHtml(latestEventText || 'Chưa có event')}</b><em>Thời điểm log mới nhất</em></div>
      <div><span>Kết thúc</span><b>${escapeHtml(session.ended_at || session.revoked_at || '')}</b></div>
      <div class="wide-fact"><span>Lý do</span><b>${escapeHtml(endReason)}</b></div>
    </div>
    <p class="ucon-compact-note">Các thuộc tính trong từng pha được hiển thị theo policy và snapshot UCON đã ghi nhận tại pha đó, không phải dữ liệu UI tự đánh giá lại.</p>
    ${phaseBoardHtml}
    <div class="session-detail-actions">
      ${popupButton('Xem chi tiết kỹ thuật', 'Chi tiết kỹ thuật của session', `<div class="detail-columns phase-detail-columns popup-detail-columns"><section><h3>Attribute snapshot đã lưu</h3>${checkedHtml}<h3>Dependency đang theo dõi</h3><div class="dependency-list">${depsHtml}</div></section><section><h3>Timeline của session</h3><div class="detail-event-list">${timeline}</div></section></div>`, 'btn-soft')}
    </div>
  </div>`;
}
function renderLatestTimeline(events){
  if(!events.length) return '<p class="muted">Chưa có event được ghi nhận.</p>';
  return events.map(e=>`<div class="timeline-item ${String(e.decision||'').toLowerCase()}">
    <b>${escapeHtml(e.event_type || 'EVENT')}</b>
    <span>${escapeHtml(e.phase || '')} · ${escapeHtml(e.decision || '')} · session ${escapeHtml(e.usage_session_id || '')}</span>
    <small>${escapeHtml(e.event_at || '')}</small>
  </div>`).join('');
}
function monitor(selectedSessionId=''){
  resetAdminInfoPayloads();
  const sessions = db.USAGE_SESSIONS || [];
  const events = db.SESSION_EVENTS || [];
  const q = String(monitorSessionFilter || '').trim().toLowerCase();
  const runningStatuses = new Set(['active','pending']);
  const runningSessions = sessions.filter(x => runningStatuses.has(String(x.status || '').toLowerCase()));
  const recentClosed = sessions
    .filter(x => !runningStatuses.has(String(x.status || '').toLowerCase()))
    .sort((a,b)=>String(b.ended_at || b.revoked_at || b.created_at || '').localeCompare(String(a.ended_at || a.revoked_at || a.created_at || '')))
    .slice(0, 50);
  const allDisplaySessions = monitorDisplaySessions();
  const filteredSessions = allDisplaySessions.filter(s => !q || Object.values(s).some(v => String(v).toLowerCase().includes(q)));
  const selectedSession = sessions.find(s => String(s.id) === String(selectedSessionId)) || filteredSessions[0] || null;
  const selectedId = selectedSession?.id || '';
  const eventTypes = countBy(events, 'event_type');
  const preCount = sessionCountWithPhase(sessions, 'PRE');
  const ongoingCount = sessionCountWithPhase(sessions, 'ONGOING');
  const postCount = sessionCountWithPhase(sessions, 'POST');
  const revokeCount = sessionCountRevoked(sessions);
  const actions = `<button class="btn-soft" onclick="toggleMonitorAutoRefresh()">Tự cập nhật: ${monitorAutoRefresh ? 'Bật' : 'Tắt'}</button><button class="btn-soft" onclick="refreshRealDatabase(false)">Cập nhật ngay</button>`;
  admin.innerHTML = shell('monitor', `${header('Theo dõi UCON thời gian thực','Theo dõi session thật, event type tổng quát và các pha PRE / ONGOING / POST.', actions)}
    <div class="ucon-stage-board compact-stage">
      <div class="ucon-stage ${preCount ? 'on' : ''}"><i>1</i><span>PRE</span><b>${preCount}</b><small>session đã kiểm tra trước khi cấp quyền</small></div>
      <div class="ucon-stage ${ongoingCount ? 'on' : ''}"><i>2</i><span>ONGOING</span><b>${ongoingCount}</b><small>session đã kiểm tra trong lúc đọc</small></div>
      <div class="ucon-stage ${postCount ? 'on' : ''}"><i>3</i><span>POST</span><b>${postCount}</b><small>session đã ghi nhận kết thúc/hậu xử lý</small></div>
      <div class="ucon-stage ${revokeCount ? 'bad' : ''}"><i>!</i><span>REVOKE</span><b>${revokeCount}</b><small>session bị chặn hoặc thu hồi</small></div>
    </div>
    <div class="admin-card monitor-card monitor-general-events"><div><h2 class="h2">Event type tổng quát</h2><p class="muted">Các loại event đã ghi nhận.</p></div>${renderEventTypeSummary(eventTypes)}</div>
    <div class="monitor-session-strip-card admin-card monitor-card">
      <div class="session-strip-head">
        <div><h2 class="h2">Session</h2><p class="muted">${runningSessions.length} đang diễn ra · ${recentClosed.length} đã kết thúc gần đây</p></div>
        <input class="input" value="${escapeHtml(monitorSessionFilter)}" placeholder="Lọc theo session, user, resource, status..." onchange="setMonitorFilter(this.value)">
      </div>
      ${renderSessionListPanel(filteredSessions, selectedId)}
    </div>
    <main class="monitor-main-detail full-width-detail">
      ${renderSessionDetail(selectedSession)}
    </main>`);
}
async function runSql(){
  const sqlText = document.getElementById('sql-input').value;
  const result = document.getElementById('sql-result');
  if(adminDbSource === 'postgresql' && typeof window.AdminDataService?.runSql === 'function'){
    result.innerHTML = `<h2 class="h2">Đang chạy truy vấn...</h2>`;
    try{
      const data = await window.AdminDataService.runSql(sqlText);
      result.innerHTML = `<h2 class="h2">Kết quả SQL thật</h2><p class="muted">${data.rowCount || 0} dòng từ dữ liệu thật. Chỉ hỗ trợ SELECT.</p>${tableHTML('SQL_RESULT', data.rows || [], {showActions:false, contextKey:'sql_result'})}`;
    }catch(error){
      const detail = error?.body?.detail ? `<pre>${escapeHtml(error.body.detail)}</pre>` : '';
      result.innerHTML = `<div class="error-box"><b>Không chạy được SQL:</b> ${escapeHtml(error.message || error)}${detail}</div>`;
    }
    return;
  }

  let table = 'BOOKS';
  const match = sqlText.match(/from\s+([A-Z_]+)/i);
  if(match && schemas[match[1].toUpperCase()]) table = match[1].toUpperCase();
  result.innerHTML = `<h2 class="h2">Kết quả demo: ${table}</h2>${tableHTML(table, db[table]||[], {showActions:false, contextKey:'sql_demo'})}`;
}
function sql(){
  admin.innerHTML = shell('sql', `${header('Truy vấn SQL','Chạy SELECT để kiểm tra dữ liệu.')} ${databaseSourceNotice()}<div class="admin-card"><textarea id="sql-input" class="sql-box">SELECT * FROM book_volumes WHERE book_id = 2;</textarea><br><br><button class="btn" onclick="runSql()">Chạy truy vấn</button></div><div id="sql-result" class="admin-card"><h2 class="h2">Kết quả</h2><p class="muted">Nhập SELECT rồi chạy.</p></div>`);
}
async function refreshRealDatabase(silent=false){
  if(typeof window.AdminDataService?.loadDatabaseSnapshot !== 'function') return;
  if(adminDbLoading) return;
  const scrollState = silent ? captureMonitorScrollState() : null;
  adminDbLoading = true;
  adminDbError = '';
  if(!silent) route();
  try{
    const snapshot = await window.AdminDataService.loadDatabaseSnapshot();
    if(snapshot?.tables){
      db = snapshot.tables;
      tableNames = window.AdminDataService?.getTableNames?.() || Object.keys(schemas);
      adminDbSource = 'postgresql';
      adminDbError = '';
    }
  }catch(error){
    adminDbSource = 'seed';
    adminDbError = error?.message || String(error);
  }finally{
    adminDbLoading = false;
    route();
    if(silent) restoreMonitorScrollState(scrollState);
  }
}

function renderRouteError(error){
  console.error('Admin route render failed:', error);
  const message = escapeHtml(error?.message || String(error));
  const stack = escapeHtml(error?.stack || '');
  admin.innerHTML = shell('dashboard', `${header('Admin gặp lỗi khi mở tab','Mình đã giữ trang không bị trắng để bạn còn đọc lỗi và tiếp tục thao tác.')}
    <div class="error-box"><b>Lỗi render tab:</b> ${message}${stack ? `<pre style="white-space:pre-wrap;margin-top:12px">${stack}</pre>` : ''}</div>`);
}
function route(){
  try{
    const p = (location.hash || '#dashboard').replace('#','').split('/');
    ensureMonitorTimer(p[0] === 'monitor');
    if(p[0]==='dashboard') return dashboard();
    if(p[0]==='monitor') return monitor(p[1] || '');
    if(p[0]==='database') return database(p[1] || 'USERS');
    if(p[0]==='books') { location.hash = '#database/BOOKS'; return; }
    if(p[0]==='policies') return policies();
    if(p[0]==='sessions') return sessions(p[1] || 'USAGE_SESSIONS');
    if(p[0]==='logs') return sessions('SESSION_EVENTS');
    if(p[0]==='sql') return sql();
    return dashboard();
  }catch(error){
    renderRouteError(error);
  }
}
window.addEventListener('hashchange', route);
route();
refreshRealDatabase(true);
