import { useEffect, useState } from 'react'
import { api } from '../lib/api'
import AppHeader from '../components/AppHeader'
import ReceiptModal from '../components/ReceiptModal'
import TxIcon, { txMeta } from '../components/TxIcon'

const rupiah = (n) => 'Rp ' + Number(n).toLocaleString('id-ID')

export default function History() {
  const [txs, setTxs] = useState([])
  const [selected, setSelected] = useState(null)
  const [err, setErr] = useState('')
  const [loading, setLoading] = useState(true)
  const [filter, setFilter] = useState('all')

  useEffect(() => {
    async function load() {
      try { setTxs((await api.myTransactions()).slice().reverse()) }
      catch (e) { setErr(e.message) }
      finally { setLoading(false) }
    }
    load()
  }, [])

  const types = ['all', ...Array.from(new Set(txs.map(t => t.typeName)))]
  const shown = filter === 'all' ? txs : txs.filter(t => t.typeName === filter)

  return (
    <>
      <AppHeader title="Riwayat" subtitle={`${txs.length} transaksi`} back />
      <div style={{ padding:'0 0.9rem', display:'grid', gap:'1rem' }}>
        <div className="row" style={{ gap:'0.4rem' }}>
          {types.map(t => (
            <button key={t} className={filter===t ? 'tab active' : 'tab'} onClick={()=>setFilter(t)}>
              {t === 'all' ? 'Semua' : txMeta({ typeName:t }).label}
            </button>
          ))}
        </div>

        {err && <div className="card" style={{ color:'var(--berry-700)' }}>{err}</div>}

        <div className="card pad-sm">
          <div className="divider-list">
            {loading && <div className="skeleton" style={{ height: 60 }} />}
            {!loading && shown.length === 0 && <div className="empty">Belum ada transaksi.</div>}
            {shown.map(t => (
              <div key={t.id} className="tx-row" style={{ cursor:'pointer' }} onClick={()=>setSelected(t)}>
                <TxIcon t={t} />
                <div className="tx-main">
                  <div className="tx-title">{txMeta(t).label} <span className={t.status==='SUCCESS'?'tag':'tag fail'} style={{ marginLeft:4 }}>{t.status}</span></div>
                  <div className="tx-sub">{t.timestamp} · dari {t.sourceAccount}</div>
                </div>
                <div className="tx-amt out">-{rupiah(t.amount)}</div>
              </div>
            ))}
          </div>
        </div>
      </div>
      <ReceiptModal tx={selected} onClose={()=>setSelected(null)} />
    </>
  )
}
