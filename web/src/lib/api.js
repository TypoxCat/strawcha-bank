// In production the API lives elsewhere (e.g. Render). Set VITE_API_URL at build time.
const BASE = (import.meta.env.VITE_API_URL || '').replace(/\/$/, '');

export function getToken() {
  return localStorage.getItem('mbank_token');
}
export function setToken(t) {
  if (t) localStorage.setItem('mbank_token', t); else localStorage.removeItem('mbank_token');
}
export function getStaffKey() {
  return localStorage.getItem('mbank_staff_key') || 'staff123';
}
export function setStaffKey(k) {
  if (k) localStorage.setItem('mbank_staff_key', k);
}

async function request(path, opts = {}) {
  const headers = { 'Content-Type': 'application/json', ...(opts.headers||{}) };
  const token = getToken();
  if (token) headers['Authorization'] = 'Bearer ' + token;
  const res = await fetch(BASE + path, { ...opts, headers });
  const text = await res.text();
  let data = null;
  try { data = text ? JSON.parse(text) : null; } catch (e) { data = { error: text }; }
  if (!res.ok) {
    const msg = (data && (data.error || data.message)) || ('HTTP ' + res.status);
    throw new Error(msg);
  }
  return data;
}

export const api = {
  register: (name, pin) => request('/api/auth/register', { method:'POST', body: JSON.stringify({name,pin}) }),
  login: (customerId, pin) => request('/api/auth/login', { method:'POST', body: JSON.stringify({customerId,pin}) }),
  logout: () => request('/api/auth/logout', { method:'POST' }),
  changePin: (oldPin, newPin) => request('/api/auth/pin', { method:'POST', body: JSON.stringify({oldPin,newPin}) }),
  me: () => request('/api/me'),
  accounts: () => request('/api/accounts'),
  createAccount: (type, initialDeposit) => request('/api/accounts', { method:'POST', body: JSON.stringify({type, initialDeposit}) }),
  account: (num) => request('/api/accounts/' + encodeURIComponent(num)),
  deposit: (num, amount, note) => request('/api/accounts/' + encodeURIComponent(num) + '/deposit', { method:'POST', body: JSON.stringify({amount, note}) }),
  withdraw: (num, amount, note) => request('/api/accounts/' + encodeURIComponent(num) + '/withdraw', { method:'POST', body: JSON.stringify({amount, note}) }),
  transfer: (from,to,amount) => request('/api/transfer', { method:'POST', body: JSON.stringify({from,to,amount}) }),
  topup: (from,provider,phone,amount) => request('/api/topup', { method:'POST', body: JSON.stringify({from,provider,phone,amount}) }),
  paybill: (from,billType,customerNo,amount) => request('/api/paybill', { method:'POST', body: JSON.stringify({from,billType,customerNo,amount}) }),
  myTransactions: () => request('/api/transactions'),
  transaction: (id) => request('/api/transactions/' + id),
  staffMonthEnd: () => request('/api/staff/month-end', { method:'POST', headers: { 'X-Staff-Key': getStaffKey() } }),
  staffLog: () => request('/api/staff/log', { headers: { 'X-Staff-Key': getStaffKey() } })
};