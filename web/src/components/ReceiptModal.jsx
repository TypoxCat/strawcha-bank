export default function ReceiptModal({ tx, onClose }) {
  if (!tx) return null
  const ok = tx.status === 'SUCCESS'
  return (
    <div className="overlay" onClick={onClose}>
      <div className="card modal" style={{ maxWidth:520, width:'100%' }} onClick={e=>e.stopPropagation()}>
        <div className="between">
          <h3>Struk Transaksi</h3>
          <span className={ok ? 'tag' : 'tag fail'}>{tx.status}</span>
        </div>
        <div className="perf" />
        <div style={{ whiteSpace:'pre-wrap', fontFamily:'IBM Plex Mono', fontSize:'0.9rem' }}>{tx.receipt}</div>
        <div className="row" style={{ justifyContent:'flex-end', marginTop:'1rem' }}>
          <button className="butter" onClick={onClose}>Tutup</button>
        </div>
      </div>
    </div>
  )
}
