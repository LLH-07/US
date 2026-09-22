// Client-side support for completing user-facing UCON obligations.
// This file only renders the interaction requested by the backend and sends
// evidence back. It does not decide whether an obligation is required or
// whether access should be permitted.

function obligationApiBase(){
  const configured = window.UCON_ENV?.OBLIGATION_API_BASE;
  if(configured) return String(configured).replace(/\/$/, '');

  return String(API_BASE || '')
    .replace(/\/api\/pep\/?$/, '/api/ucon/obligations')
    .replace(/\/$/, '');
}

function requiredObligationAttempt(accessResponse = {}){
  const attempts = Array.isArray(accessResponse?.attempts)
    ? accessResponse.attempts
    : [];

  return attempts.find(attempt =>
    Array.isArray(attempt?.requiredObligations) &&
    attempt.requiredObligations.length > 0
  ) || null;
}

function normalizeObligationType(obligation){
  return String(obligation?.type || '').trim().toUpperCase();
}

function obligationPopupText(obligation){
  return {
    title: obligation?.title || 'Yêu cầu trước khi truy cập',
    description: obligation?.description || 'Bạn cần hoàn thành yêu cầu này trước khi tiếp tục đọc sách.'
  };
}

function closeObligationPopup(){
  document.getElementById('reader-friendly-popup')?.remove();
}

function requestConsentEvidence(obligation, attempt){
  return new Promise(resolve => {
    closeObligationPopup();

    const text = obligationPopupText(obligation);
    const accessPath = attempt?.accessPath ? String(attempt.accessPath) : '';
    const version = obligation?.version ? String(obligation.version) : '';
    const meta = [accessPath, version ? `v${version}` : '']
      .filter(Boolean)
      .join(' · ');

    document.body.insertAdjacentHTML('beforeend', `
      <div id="reader-friendly-popup" class="reader-popup-backdrop">
        <div class="reader-popup-card" role="dialog" aria-modal="true" aria-labelledby="reader-obligation-title">
          <button id="reader-obligation-close" class="reader-popup-close" aria-label="Đóng yêu cầu">×</button>
          <div class="reader-popup-kicker">${escapeHtml(meta || 'UCON obligation')}</div>
          <h2 id="reader-obligation-title">${escapeHtml(text.title)}</h2>
          <p>${escapeHtml(text.description)}</p>
          <div class="reader-popup-actions">
            <button id="reader-obligation-decline" class="reader-popup-button secondary" type="button">Chưa đồng ý</button>
            <button id="reader-obligation-accept" class="reader-popup-button" type="button">Đồng ý và tiếp tục</button>
          </div>
        </div>
      </div>
    `);

    let settled = false;
    const finish = evidence => {
      if(settled) return;
      settled = true;
      closeObligationPopup();
      resolve(evidence);
    };

    document.getElementById('reader-obligation-accept')?.addEventListener('click', () => {
      finish({accepted: true});
    });

    document.getElementById('reader-obligation-decline')?.addEventListener('click', () => {
      finish(null);
    });

    document.getElementById('reader-obligation-close')?.addEventListener('click', () => {
      finish(null);
    });
  });
}

async function collectObligationEvidence(obligation, attempt){
  switch(normalizeObligationType(obligation)){
    case 'CONSENT':
      return requestConsentEvidence(obligation, attempt);

    default: {
      const type = normalizeObligationType(obligation) || 'UNKNOWN';
      const error = new Error(`Giao diện chưa hỗ trợ obligation type ${type}.`);
      error.data = {
        reason: 'UNSUPPORTED_OBLIGATION_TYPE',
        message: error.message,
        obligationType: type
      };
      throw error;
    }
  }
}

async function submitObligationEvidence(obligation, attempt, evidence){
  const obligationId = String(obligation?.obligationId || '').trim();
  const policyRef = String(obligation?.policyRef || attempt?.policyRef || '').trim();

  if(!obligationId){
    const error = new Error('Backend không trả obligationId.');
    error.data = {reason: 'INVALID_REQUIRED_OBLIGATION', message: error.message};
    throw error;
  }

  if(!policyRef){
    const error = new Error('Backend không trả policyRef của obligation.');
    error.data = {reason: 'INVALID_REQUIRED_OBLIGATION', message: error.message};
    throw error;
  }

  const response = await fetch(
    `${obligationApiBase()}/${encodeURIComponent(obligationId)}/fulfill`,
    {
      method: 'POST',
      headers: {'Content-Type': 'application/json'},
      body: JSON.stringify({
        userId: Number(currentUserId),
        policyRef,
        evidence
      })
    }
  );

  let data = await response.json().catch(() => ({}));
  data = cleanBackendPayload(data);

  if(!response.ok){
    const error = new Error(data.reason || data.message || 'OBLIGATION_FULFILLMENT_FAILED');
    error.data = {
      ...data,
      reason: data.reason || 'OBLIGATION_FULFILLMENT_FAILED'
    };
    throw error;
  }

  return data;
}

async function fulfillRequiredObligations(attempt){
  const obligations = Array.isArray(attempt?.requiredObligations)
    ? attempt.requiredObligations
    : [];

  for(const obligation of obligations){
    const evidence = await collectObligationEvidence(obligation, attempt);

    if(!evidence){
      return {
        completed: false,
        declined: true,
        accessPath: attempt?.accessPath || null
      };
    }

    await submitObligationEvidence(obligation, attempt, evidence);
  }

  return {
    completed: true,
    declined: false,
    accessPath: attempt?.accessPath || null
  };
}
