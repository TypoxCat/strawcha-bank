import { useEffect, useState } from 'react'
import { api } from '../lib/api'
import { Link } from 'react-router-dom'
import AppHeader from '../components/AppHeader'
import ReceiptModal from '../components/ReceiptModal'
import TxIcon, { txMeta } from '../components/TxIcon'
import { IconTransfer, IconTopUp, IconBill, IconWallet, IconEye, IconEyeOff } from '../components/Icons'

const rupiah = (n) => 'Rp ' + Number(n).toLocaleString('id-ID')

function QuickAction({ to, label, Icon, tint }) {
  return (
    <Link to={to} className="qa-item">
      <span className="qa-ico" style={{ background: tint }}><Icon size={22} /></span>
      <span className="qa-label">{label}</span>
    </Link>
  )
}

export default function Dashboard() {
  const [me, setMe] = useState(null)
  const [txs, setTxs] = useState([])
  const [err, setErr] = useState('')
  const [hide, setHide] = useState(false)
  const [selected, setSelected] = useState(null)

  useEffect(() => {
    async function load() {
      try {
        const [m, t] = await Promise.all([api.me(), api.myTransactions()])
        setMe(m); setTxs(t.slice(-5).reverse())
      } catch (e) { setErr(e.message) }
    }
    load()
  }, [])

  const mask = (v) => (hide ? 'Rp ••••••' : rupiah(v))

  return (
    <>
      <AppHeader greet />
      <div style={{ padding: '0 0.9rem', display:'grid', gap:'1rem' }}>
        <section className="balance-card">
          <div className="balance-top">
            <div className="balance-label">Saldo Total</div>
            <button className="eye-btn" onClick={()=>setHide(h=>!h)}>{hide ? <><IconEye size={16} /> Tampilkan</> : <><IconEyeOff size={16} /> Sembunyikan</>}</button>
          </div>
          <div className="balance-amount">{me ? mask(me.totalBalance) : 'Rp ••••••'}</div>
          <div className="balance-meta">
            <span className="chip">{me ? me.accounts.length : 0} rekening</span>
            <span className="chip">{me ? 'ID ' + me.id : '—'}</span>
          </div>
        </section>

        <section className="quick-actions">
          <QuickAction to="/transfer" label="Transfer" Icon={IconTransfer} tint="var(--berry-200)" />
          <QuickAction to="/payments" label="Top Up" Icon={IconTopUp} tint="var(--matcha-200)" />
          <QuickAction to="/payments" label="Bayar" Icon={IconBill} tint="var(--butter)" />
          <QuickAction to="/accounts" label="Rekening" Icon={IconWallet} tint="var(--matcha-100)" />
        </section>

        <section>
          <div className="section-head">
            <span className="section-title">Rekening Saya</span>
            <Link className="section-link" to="/accounts">Lihat semua</Link>
          </div>
          <div className="list">
            {err && <div className="card" style={{ color:'var(--berry-700)' }}>{err}</div>}
            {!me && <div className="skeleton" style={{ height: 72 }} />}
            {me?.accounts.map((a, i) => (
              <Link key={a.accountNumber} to={'/accounts/' + a.accountNumber} className={'acct-card ' + (i % 2 ? 'berry' : 'matcha')}>
                <div className="between">
                  <strong>{a.typeName}</strong>
                  <span className="mono">{mask(a.balance)}</span>
                </div>
                <div className="muted mono" style={{ fontSize:'0.8rem' }}>{a.accountNumber}</div>
              </Link>
            ))}
          </div>
        </section>

        <section>
          <div className="section-head">
            <span className="section-title">Transaksi Terakhir</span>
            <Link className="section-link" to="/history">Riwayat</Link>
          </div>
          <div className="card pad-sm">
            <div className="divider-list">
              {txs.length === 0 && <div className="empty">Belum ada transaksi.</div>}
              {txs.map(t => (
                <div key={t.id} className="tx-row" style={{ cursor:'pointer' }} onClick={()=>setSelected(t)}>
                  <TxIcon t={t} />
                  <div className="tx-main">
                    <div className="tx-title">{txMeta(t).label}</div>
                    <div className="tx-sub">{t.timestamp}</div>
                  </div>
                  <div className="tx-amt out">-{rupiah(t.amount)}</div>
                </div>
              ))}
            </div>
          </div>
        </section>
      </div>

      <ReceiptModal tx={selected} onClose={()=>setSelected(null)} />
    </>
  )
}
