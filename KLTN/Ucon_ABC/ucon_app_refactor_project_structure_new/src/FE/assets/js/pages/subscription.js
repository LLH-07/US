// Subscription package page.
function subscription(allowHydrate = true){
  if(allowHydrate) hydrateSubscriptionFromBackend(true);
  const rental = userRental();
  const plan = rentalPlans.find(p => Number(p.id) === Number(rental?.plan_id));
  const active = hasActiveSubscription();
  app.innerHTML = `${nav('subscription')}<section class="section"><p class="eyebrow">Gói thuê</p><h1 class="h1">Đăng ký và tình trạng gói thuê</h1><p class="rental-note">Đăng ký gói thuê sẽ mở quyền đọc toàn bộ sách thuê trong thời hạn gói. Khi đang có gói hoạt động, bạn cần hủy hoặc chờ hết hạn trước khi đăng ký gói mới.</p><div class="subscription-layout"><div class="admin-card"><h2 class="h2">Gói hiện tại</h2><span class="rental-status ${rentalClass(rental?.status)}">${rentalStatusLabel(rental?.status)}</span>${!plan ? `<p class="muted" style="margin:14px 0 18px;line-height:1.55">Bạn chưa có gói thuê đang hoạt động. Chọn một gói ở khung bên phải để đăng ký.</p>` : ''}<div class="line-row"><b>Gói:</b> ${plan ? plan.name : 'Chưa có'}</div><div class="line-row"><b>Ngày đăng ký:</b> ${dateVN(rental?.started_at)}</div><div class="line-row"><b>Ngày hết hạn:</b> ${dateVN(rental?.expires_at)}</div><div class="profile-actions horizontal">${subscriptionActions()}</div></div>
  <div class="admin-card"><h2 class="h2">Các gói có thể đăng ký</h2>${rentalPlans.map(p=>`<div class="plan-row"><div><b>${p.name}</b><span>${p.duration_days} ngày · ${money(p.price)}</span></div>${active ? `<span class="outline-btn disabled current-plan-pill">Đang có gói</span>` : `<button class="outline-btn" onclick="registerRentalPlan(${Number(p.id)})">Chọn gói</button>`}</div>`).join('')}</div></div></section><div class="footer-block"></div>`;
}
