// Service dữ liệu cho Admin.
// Ưu tiên đọc PostgreSQL thật qua BE Admin API. Nếu BE chưa chạy thì tự rơi về dữ liệu seed FE.
(function(){
  const PRIMARY_KEYS = {
    USERS: 'id',
    BOOKS: 'id',
    BOOK_VOLUMES: 'id',
    RENTAL_PLANS: 'id',
    USER_RENTALS: 'id',
    VOLUME_PURCHASES: 'id',
    FAMILY_GROUPS: 'id',
    FAMILY_GROUP_MEMBERS: 'id',
    FAMILY_SHARED_VOLUMES: 'id',
    SYSTEM_PARAMETERS: 'id',
    POLICY_REGISTRY: 'id',
    PIP_ATTRIBUTE_MAPPINGS: 'attr_key',
    PIP_UPDATE_ACTIONS: 'action_code',
    POLICY_ATTRIBUTE_REQUIREMENTS: 'id',
    USAGE_SESSIONS: 'id',
    USAGE_SESSION_POLICIES: 'id',
    USAGE_SESSION_DEPENDENCIES: 'id',
    UCON_TRIGGER_EVENTS: 'id',
    SESSION_EVENTS: 'id',
    OBLIGATION_EXECUTIONS: 'id'
  };

  let dynamicPrimaryKeys = {...PRIMARY_KEYS};

  function mutateObject(target, source){
    if(!target || !source) return target;
    Object.keys(target).forEach(key => delete target[key]);
    Object.entries(source).forEach(([key, value]) => { target[key] = value; });
    return target;
  }

  function applySnapshotMetadata(data){
    if(data?.schemas){
      window.ADMIN_SCHEMAS = mutateObject(window.ADMIN_SCHEMAS || {}, data.schemas);
    }
    if(data?.primaryKeys){
      window.ADMIN_PRIMARY_KEYS = mutateObject(window.ADMIN_PRIMARY_KEYS || {}, data.primaryKeys);
      dynamicPrimaryKeys = {...PRIMARY_KEYS, ...window.ADMIN_PRIMARY_KEYS};
    }
    if(data?.metadata){
      window.ADMIN_DB_METADATA = mutateObject(window.ADMIN_DB_METADATA || {}, data.metadata);
    }
  }

  function clone(value){
    return JSON.parse(JSON.stringify(value));
  }

  function pepApiBase(){
    return window.UCON_ENV?.API_BASE || 'http://localhost:8081/api/pep';
  }

  function adminApiBase(){
    return pepApiBase().replace(/\/api\/pep\/?$/, '/api/admin/db');
  }

  async function requestJson(url, options={}){
    const res = await fetch(url, options);
    let body = {};
    try { body = await res.json(); } catch (_) { body = {}; }
    if(!res.ok){
      const message = body.reason || body.message || `HTTP_${res.status}`;
      const err = new Error(message);
      err.status = res.status;
      err.body = body;
      throw err;
    }
    return body;
  }

  window.AdminDataService = {
    getSchemas(){
      return window.ADMIN_SCHEMAS || {};
    },
    getTableNames(){
      return Object.keys(window.ADMIN_SCHEMAS || {});
    },
    getDatabase(){
      return clone(window.ADMIN_DEMO_DB || {});
    },
    getPrimaryKey(table){
      return dynamicPrimaryKeys[String(table || '').toUpperCase()] || 'id';
    },
    async loadDatabaseSnapshot(){
      const data = await requestJson(`${adminApiBase()}/snapshot`);
      applySnapshotMetadata(data);
      return data;
    },
    async deleteRealRow(table, row, cascade=true){
      const normalizedTable = String(table || '').toUpperCase();
      const primaryKey = this.getPrimaryKey(normalizedTable);
      const keyValue = row?.[primaryKey];
      if(keyValue === undefined || keyValue === null || keyValue === ''){
        throw new Error(`Không tìm thấy khóa chính ${primaryKey} để xóa bảng ${normalizedTable}.`);
      }
      return requestJson(
        `${adminApiBase()}/tables/${encodeURIComponent(normalizedTable)}/rows/${encodeURIComponent(String(keyValue))}?cascade=${cascade ? 'true' : 'false'}`,
        {method:'DELETE'}
      );
    },
    async createRealRow(table, row){
      const normalizedTable = String(table || '').toUpperCase();
      return requestJson(
        `${adminApiBase()}/tables/${encodeURIComponent(normalizedTable)}/rows`,
        {
          method:'POST',
          headers:{'Content-Type':'application/json'},
          body:JSON.stringify(row || {})
        }
      );
    },
    async updateRealRow(table, row){
      const normalizedTable = String(table || '').toUpperCase();
      const primaryKey = this.getPrimaryKey(normalizedTable);
      const keyValue = row?.[primaryKey];
      if(keyValue === undefined || keyValue === null || keyValue === ''){
        throw new Error(`Không tìm thấy khóa chính ${primaryKey} để cập nhật bảng ${normalizedTable}.`);
      }
      return requestJson(
        `${adminApiBase()}/tables/${encodeURIComponent(normalizedTable)}/rows/${encodeURIComponent(String(keyValue))}`,
        {
          method:'PUT',
          headers:{'Content-Type':'application/json'},
          body:JSON.stringify(row || {})
        }
      );
    },
    async runSql(sql){
      return requestJson(
        `${adminApiBase()}/query`,
        {
          method:'POST',
          headers:{'Content-Type':'application/json'},
          body:JSON.stringify({sql})
        }
      );
    },
    getDashboardStats(db){
      const sessions = db.USAGE_SESSIONS || [];
      const triggers = db.UCON_TRIGGER_EVENTS || [];
      const status = value => String(value || '').toLowerCase();
      return {
        policies: (db.POLICY_REGISTRY || []).length,
        tables: Object.keys(window.ADMIN_SCHEMAS || {}).length,
        activeSessions: sessions.filter(x => status(x.status) === 'active').length,
        endedSessions: sessions.filter(x => status(x.status) === 'ended').length,
        revokedSessions: sessions.filter(x => status(x.status) === 'revoked').length,
        pendingTriggers: triggers.filter(x => String(x.processed_flag).toLowerCase() !== 'true').length
      };
    }
  };
})();
