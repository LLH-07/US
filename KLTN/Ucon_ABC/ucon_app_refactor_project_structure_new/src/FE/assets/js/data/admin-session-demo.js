// Dữ liệu mô phỏng riêng cho Admin: nhiều session đang diễn ra và thay đổi giữa các pha UCON.
// File này chỉ bổ sung dữ liệu demo để Admin dễ show; không thay đổi UI/UX và không thay đổi luồng đọc qua CH/PEP.
(function(){
  const db = window.ADMIN_DEMO_DB || (window.ADMIN_DEMO_DB = {});
  const pushUnique = (table, rows) => {
    db[table] = db[table] || [];
    const existing = new Set(db[table].map(row => String(row.id)));
    rows.forEach(row => {
      if (!existing.has(String(row.id))) db[table].push(row);
    });
  };

  pushUnique('USAGE_SESSIONS', [
    {id:'8997',subject_user_id:'1',resource_type:'BOOK_VOLUME',resource_id:'1004',action_id:'read',access_path:'PAID',phase:'post',status:'ended',created_at:'2026-06-25 08:20',activated_at:'2026-06-25 08:20',last_eval_at:'2026-06-25 08:45',ended_at:'2026-06-25 08:45',revoked_at:'',revoked_reason:'',post_handled_at:'2026-06-25 08:45'},
    {id:'8998',subject_user_id:'2',resource_type:'BOOK_VOLUME',resource_id:'2005',action_id:'read',access_path:'RENTAL',phase:'post',status:'ended',created_at:'2026-06-25 09:10',activated_at:'2026-06-25 09:10',last_eval_at:'2026-06-25 09:34',ended_at:'2026-06-25 09:34',revoked_at:'',revoked_reason:'',post_handled_at:'2026-06-25 09:34'},
    {id:'8999',subject_user_id:'3',resource_type:'BOOK_VOLUME',resource_id:'29001',action_id:'read',access_path:'PREVIEW',phase:'post',status:'ended',created_at:'2026-06-25 09:50',activated_at:'2026-06-25 09:50',last_eval_at:'2026-06-25 09:53',ended_at:'2026-06-25 09:53',revoked_at:'',revoked_reason:'preview_finished',post_handled_at:'2026-06-25 09:53'},
    {id:'9001',subject_user_id:'2',resource_type:'BOOK_VOLUME',resource_id:'29001',action_id:'read',access_path:'PREVIEW',phase:'ongoing',status:'active',created_at:'2026-06-26 10:05',activated_at:'2026-06-26 10:05',last_eval_at:'2026-06-26 10:07',ended_at:'',revoked_at:'',revoked_reason:'',post_handled_at:''},
    {id:'9002',subject_user_id:'3',resource_type:'BOOK_VOLUME',resource_id:'2001',action_id:'read',access_path:'RENTAL',phase:'ongoing',status:'revoked',created_at:'2026-06-26 10:10',activated_at:'2026-06-26 10:10',last_eval_at:'2026-06-26 10:16',ended_at:'',revoked_at:'2026-06-26 10:16',revoked_reason:'rental_expired',post_handled_at:''},
    {id:'9003',subject_user_id:'1',resource_type:'BOOK_VOLUME',resource_id:'1001',action_id:'read',access_path:'PAID',phase:'post',status:'ended',created_at:'2026-06-26 10:20',activated_at:'2026-06-26 10:20',last_eval_at:'2026-06-26 10:36',ended_at:'2026-06-26 10:36',revoked_at:'',revoked_reason:'',post_handled_at:'2026-06-26 10:36'}
  ]);

  pushUnique('SESSION_EVENTS', [
    {id:'899701',usage_session_id:'8997',event_type:'policy_evaluated',phase:'pre',decision:'PERMIT',reason_code:'volume_purchased',event_at:'2026-06-25 08:20',details_json:'{"policy":"paid_read_policy"}'},
    {id:'899702',usage_session_id:'8997',event_type:'ongoing_check',phase:'ongoing',decision:'PERMIT',reason_code:'reading_ok',event_at:'2026-06-25 08:31',details_json:'{"current_chunk":4}'},
    {id:'899703',usage_session_id:'8997',event_type:'post_update',phase:'post',decision:'PERMIT',reason_code:'history_recorded',event_at:'2026-06-25 08:45',details_json:'{"obligation":"post_log"}'},
    {id:'899801',usage_session_id:'8998',event_type:'policy_evaluated',phase:'pre',decision:'PERMIT',reason_code:'rental_active',event_at:'2026-06-25 09:10',details_json:'{"policy":"rental_read_policy"}'},
    {id:'899802',usage_session_id:'8998',event_type:'ongoing_check',phase:'ongoing',decision:'PERMIT',reason_code:'rental_still_active',event_at:'2026-06-25 09:22',details_json:'{"current_chunk":3}'},
    {id:'899803',usage_session_id:'8998',event_type:'post_update',phase:'post',decision:'PERMIT',reason_code:'session_ended_by_user',event_at:'2026-06-25 09:34',details_json:'{"obligation":"post_log"}'},
    {id:'899901',usage_session_id:'8999',event_type:'policy_evaluated',phase:'pre',decision:'PERMIT',reason_code:'preview_allowed_first_volume',event_at:'2026-06-25 09:50',details_json:'{"policy":"preview_read_policy","max_preview_pages":5}'},
    {id:'899902',usage_session_id:'8999',event_type:'ongoing_check',phase:'ongoing',decision:'PERMIT',reason_code:'preview_within_limit',event_at:'2026-06-25 09:52',details_json:'{"current_page":5}'},
    {id:'899903',usage_session_id:'8999',event_type:'post_update',phase:'post',decision:'PERMIT',reason_code:'preview_session_closed',event_at:'2026-06-25 09:53',details_json:'{"obligation":"close_preview"}'},
    {id:'9001',usage_session_id:'9001',event_type:'policy_evaluated',phase:'pre',decision:'PERMIT',reason_code:'preview_allowed_first_volume',event_at:'2026-06-26 10:05',details_json:'{"policy":"preview_read_policy","max_preview_pages":5}'},
    {id:'9002',usage_session_id:'9001',event_type:'ongoing_check',phase:'ongoing',decision:'PERMIT',reason_code:'preview_within_limit',event_at:'2026-06-26 10:07',details_json:'{"current_chunk":1,"current_page":3}'},
    {id:'9003',usage_session_id:'9002',event_type:'policy_evaluated',phase:'pre',decision:'PERMIT',reason_code:'rental_active',event_at:'2026-06-26 10:10',details_json:'{"policy":"rental_read_policy"}'},
    {id:'9004',usage_session_id:'9002',event_type:'ongoing_check',phase:'ongoing',decision:'DENY',reason_code:'rental_expired',event_at:'2026-06-26 10:16',details_json:'{"current_chunk":5,"trigger_event_id":2}'},
    {id:'9005',usage_session_id:'9003',event_type:'policy_evaluated',phase:'pre',decision:'PERMIT',reason_code:'volume_purchased',event_at:'2026-06-26 10:20',details_json:'{"policy":"paid_read_policy"}'},
    {id:'9006',usage_session_id:'9003',event_type:'post_update',phase:'post',decision:'PERMIT',reason_code:'history_recorded',event_at:'2026-06-26 10:36',details_json:'{"obligation":"post_log"}'}
  ]);

  pushUnique('USAGE_SESSION_DEPENDENCIES', [
    {id:'899701',usage_session_id:'8997',dependency_key:'subject.volume_purchase',entity_type:'VOLUME_PURCHASES',entity_id:'11',created_at:'2026-06-25 08:20',active_flag:'true'},
    {id:'899801',usage_session_id:'8998',dependency_key:'subject.rental_expires_at',entity_type:'USER_RENTALS',entity_id:'2',created_at:'2026-06-25 09:10',active_flag:'true'},
    {id:'899901',usage_session_id:'8999',dependency_key:'object.preview_limit_pages',entity_type:'BOOK_VOLUMES',entity_id:'29001',created_at:'2026-06-25 09:50',active_flag:'true'},
    {id:'9001',usage_session_id:'9001',dependency_key:'object.preview_limit_pages',entity_type:'BOOK_VOLUMES',entity_id:'29001',created_at:'2026-06-26 10:05',active_flag:'true'},
    {id:'9002',usage_session_id:'9002',dependency_key:'subject.rental_expires_at',entity_type:'USER_RENTALS',entity_id:'3',created_at:'2026-06-26 10:10',active_flag:'false'},
    {id:'9003',usage_session_id:'9003',dependency_key:'subject.volume_purchase',entity_type:'VOLUME_PURCHASES',entity_id:'1',created_at:'2026-06-26 10:20',active_flag:'true'}
  ]);

  pushUnique('USAGE_SESSION_POLICIES', [
    {id:'899701',usage_session_id:'8997',policy_id:'4',bound_at:'2026-06-25 08:20'},
    {id:'899801',usage_session_id:'8998',policy_id:'1',bound_at:'2026-06-25 09:10'},
    {id:'899901',usage_session_id:'8999',policy_id:'3',bound_at:'2026-06-25 09:50'},
    {id:'9001',usage_session_id:'9001',policy_id:'3',bound_at:'2026-06-26 10:05'},
    {id:'9002',usage_session_id:'9002',policy_id:'1',bound_at:'2026-06-26 10:10'},
    {id:'9003',usage_session_id:'9003',policy_id:'4',bound_at:'2026-06-26 10:20'}
  ]);

  pushUnique('POLICY_REGISTRY', [
    {id:'9001',policy_ref:'preview_read_policy_demo.xml',policy_name:'Đọc thử 5 trang tập đầu',resource_type:'book_volume',action_id:'read',access_path:'PREVIEW',phase:'ongoing',enabled:'true',description:'Cho phép đọc thử giới hạn trong 5 trang của tập đầu.'},
    {id:'9002',policy_ref:'paid_read_policy_demo.xml',policy_name:'Đọc sách đã mua',resource_type:'book_volume',action_id:'read',access_path:'PAID',phase:'pre',enabled:'true',description:'Cho phép đọc tập sách trả phí khi user đã mua tập đó.'}
  ]);

  pushUnique('POLICY_ATTRIBUTE_REQUIREMENTS', [
    {id:'9001',policy_id:'9001',attr_key:'object.preview_limit_pages',usage_type:'object',required:'true'},
    {id:'9002',policy_id:'9001',attr_key:'environment.current_page',usage_type:'environment',required:'true'},
    {id:'9003',policy_id:'9002',attr_key:'subject.volume_purchase',usage_type:'subject',required:'true'}
  ]);

  pushUnique('PIP_ATTRIBUTE_MAPPINGS', [
    {attr_key:'object.preview_limit_pages',xacml_attribute_id:'object.preview_limit_pages',xacml_category:'resource',xacml_data_type:'integer',reader_name:'previewLimitReader',dependency_name:'SYSTEM_PARAMETERS.preview_limit_pages',monitor_type:'none',mutable:'false',enabled:'true'},
    {attr_key:'environment.current_page',xacml_attribute_id:'environment.current_page',xacml_category:'environment',xacml_data_type:'integer',reader_name:'currentPageReader',dependency_name:'SESSION_EVENTS.current_page',monitor_type:'timer',mutable:'true',enabled:'true'},
    {attr_key:'subject.volume_purchase',xacml_attribute_id:'subject.volume_purchase',xacml_category:'subject',xacml_data_type:'boolean',reader_name:'volumePurchaseReader',dependency_name:'VOLUME_PURCHASES.volume_id',monitor_type:'event',mutable:'true',enabled:'true'}
  ]);

  pushUnique('UCON_TRIGGER_EVENTS', [
    {id:'9001',source_type:'timer',entity_type:'USER_RENTALS',entity_id:'3',attribute_name:'expires_at',event_type:'rental_expired',occurred_at:'2026-06-26 10:15',payload_json:'{"usage_session_id":"9002"}',processed_flag:'true',processed_at:'2026-06-26 10:16'},
    {id:'9002',source_type:'admin',entity_type:'USERS',entity_id:'3',attribute_name:'status',event_type:'user_locked',occurred_at:'2026-06-26 10:30',payload_json:'{"old":"active","new":"locked"}',processed_flag:'false',processed_at:''},
    {id:'9003',source_type:'admin',entity_type:'FAMILY_SHARED_VOLUMES',entity_id:'1',attribute_name:'status',event_type:'family_share_revoked',occurred_at:'2026-06-26 10:32',payload_json:'{"volume_id":"1001"}',processed_flag:'false',processed_at:''}
  ]);

  pushUnique('OBLIGATION_EXECUTIONS', [
    {id:'899701',usage_session_id:'8997',policy_id:'10',obligation_type:'post_log',target_type:'reading_history',status:'completed',payload_json:'{"read_finished":true}',created_at:'2026-06-25 08:45',completed_at:'2026-06-25 08:45',error_message:''},
    {id:'899801',usage_session_id:'8998',policy_id:'1',obligation_type:'post_log',target_type:'reading_history',status:'completed',payload_json:'{"read_finished":true}',created_at:'2026-06-25 09:34',completed_at:'2026-06-25 09:34',error_message:''},
    {id:'899901',usage_session_id:'8999',policy_id:'3',obligation_type:'post_log',target_type:'reading_history',status:'completed',payload_json:'{"preview_finished":true}',created_at:'2026-06-25 09:53',completed_at:'2026-06-25 09:53',error_message:''},
    {id:'9001',usage_session_id:'9002',policy_id:'1',obligation_type:'revoke_session',target_type:'session',status:'completed',payload_json:'{"reason":"rental_expired"}',created_at:'2026-06-26 10:16',completed_at:'2026-06-26 10:16',error_message:''},
    {id:'9002',usage_session_id:'9003',policy_id:'10',obligation_type:'post_log',target_type:'reading_history',status:'completed',payload_json:'{"read_finished":true}',created_at:'2026-06-26 10:36',completed_at:'2026-06-26 10:36',error_message:''}
  ]);

  pushUnique('SYSTEM_PARAMETERS', [
    {id:'9001',parameter_name:'preview_limit_pages',value:'5',unit:'page'},
    {id:'9002',parameter_name:'ongoing_session_monitoring',value:'enabled',unit:'flag'}
  ]);
})();
