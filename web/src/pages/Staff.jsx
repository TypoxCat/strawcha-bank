import { useState } from 'react'
import { api, getStaffKey, setStaffKey } from '../lib/api'
import AppHeader from '../components/AppHeader'
import ReceiptModal from '../components/ReceiptModal'
import TxIcon, { txMeta } from '../components/TxIcon'

const rupiah = (n) => 'Rp ' + Number(n).toLocaleString('id-ID')

export default function Staff() {
  const [key, setKey] = useState(getStaffKey())
  const [authed, setAuthed] = useState(false)
  const [data, setData] = useState(null)
  const [err, setErr] = useState('')
  const [loading, setLoading] = useState(false)
  const [selected, setSelected] = useState(null)
  const [tab, setTab] = useState('tx')

  const unlock = async () => {
    try { setData(await api.staffLog()); setAuthed(true); setErr('') }
    catch (e) { setErr(e.message) }
  }
  const monthEnd = async () => {
    setLoading(true); setErr('')
    try { await api.staffMonthEnd(); setData(await api.staffLog()) }
    catch (e) { setErr(e.message) }
    finally { setLoading(false) }
  }

  if (!authed) {
    return (
      <div className="appframe">
        <AppHeader title="Staff" subtitle="Khusus bank staff" back />
        <div style={{ padding:'0 0.9rem' }}>
          <div className="card stack">
            <div className="field">
              <label className="label">Staff Passcode</label>
              <input value={key} onChange={e=>setKey(e.target.value)} placeholder="staff123" />
            </div>
            {err && <div className="tag fail">{err}</div>}
            <button className="matcha block" onClick={()=>{ setStaffKey(key); unlock() }}>Buka Panel</button>
            <p className="muted mono" style={{ fontSize:'0.78rem', textAlign:'center' }}>Passcode default: staff123</p>
          </div>
        </div>
      </div>
    )
  }

  return (
    <div className="appframe">
      <AppHeader title="Staff Panel" subtitle={`${data?.accounts?.length || 0} rekening · ${data?.transactions?.length || 0} transaksi`} back
        right={<button className="tab active" onClick={monthEnd} disabled={loading}>{loading ? '…' : 'Month-end'}</button>} />
      <div style={{ padding:'0 0.9rem', display:'grid', gap:'1rem' }}>
        {err && <div className="card" style={{ color:'var(--berry-700)' }}>{err}</div>}

        <section>
          <div className="section-head"><span className="section-title">Rekening</span></div>
          <div className="list">
            {data?.accounts?.map((a, i) => (
              <div key={a.accountNumber} className={'acct-card ' + (i % 2 ? 'berry' : 'matcha')}>
                <div className="between">
                  <strong>{a.ownerName} · {a.typeName}</strong>
                  <span className="mono">{rupiah(a.balance)}</span>
                </div>
                <div className="muted mono" style={{ fontSize:'0.8rem' }}>{a.accountNumber}</div>
              </div>
            ))}
          </div>
        </section>

        <section>
          <div className="section-head"><span className="section-title">Transaksi</span></div>
          <div className="card pad-sm">
            <div className="divider-list">
              {data?.transactions?.length === 0 && <div className="empty">Belum ada transaksi.</div>}
              {data?.transactions?.map(t => (
                <div key={t.id} className="tx-row" style={{ cursor:'pointer' }} onClick={()=>setSelected(t)}>
                  <TxIcon t={t} />
                  <div className="tx-main">
                    <div className="tx-title">{txMeta(t).label} <span className={t.status==='SUCCESS'?'tag':'tag fail'} style={{ marginLeft:4 }}>{t.status}</span></div>
                    <div className="tx-sub">{t.timestamp} · src {t.sourceAccount}</div>
                  </div>
                  <div className="tx-amt out">-{rupiah(t.amount)}</div>
                </div>
              ))}
            </div>
          </div>
        </section>
      </div>
      <ReceiptModal tx={selected} onClose={()=>setSelected(null)} />
    </div>
  )
}
