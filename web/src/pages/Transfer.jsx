import { useEffect, useState } from 'react'
import { api } from '../lib/api'
import AppHeader from '../components/AppHeader'
import ReceiptModal from '../components/ReceiptModal'

const rupiah = (n) => 'Rp ' + Number(n).toLocaleString('id-ID')
const QUICK = [50000, 100000, 200000, 500000]

export default function Transfer() {
  const [me, setMe] = useState(null)
  const [from, setFrom] = useState('')
  const [to, setTo] = useState('')
  const [amount, setAmount] = useState('')
  const [tx, setTx] = useState(null)
  const [err, setErr] = useState('')
  const [loading, setLoading] = useState(false)

  useEffect(() => {
    api.me().then(m => { setMe(m); if (m.accounts[0]) setFrom(m.accounts[0].accountNumber) })
  }, [])

  const fromAcc = me?.accounts.find(a => a.accountNumber === from)

  const submit = async (e) => {
    e.preventDefault(); setErr(''); setLoading(true); setTx(null)
    try { setTx(await api.transfer(from, to, parseFloat(amount))) }
    catch (e) { setErr(e.message) }
    finally { setLoading(false) }
  }

  return (
    <>
      <AppHeader title="Transfer" subtitle="Kirim dana antar rekening" back />
      <div style={{ padding:'0 0.9rem', display:'grid', gap:'1rem' }}>
        <form className="card stack" onSubmit={submit}>
          <div className="field">
            <label className="label">Rekening Sumber</label>
            <select value={from} onChange={e=>setFrom(e.target.value)}>
              {me?.accounts.map(a => <option key={a.accountNumber} value={a.accountNumber}>{a.typeName} · {a.accountNumber}</option>)}
            </select>
            {from && <span className="muted" style={{ fontSize:'0.8rem' }}>Saldo tersedia: {rupiah((me.accounts.find(a=>a.accountNumber===from)||{}).availableFunds || 0)}</span>}
          </div>

          <div className="field">
            <label className="label">Nomor Rekening Tujuan</label>
            <input inputMode="numeric" value={to} onChange={e=>setTo(e.target.value)} placeholder="contoh 1000000003" />
          </div>

          <div className="field">
            <label className="label">Nominal</label>
            <input inputMode="numeric" value={amount} onChange={e=>setAmount(e.target.value)} placeholder="0" />
            <div className="row" style={{ marginTop:'0.4rem' }}>
              {[50000,100000,200000,500000].map(v => (
                <button type="button" key={v} className="tab" onClick={()=>setAmount(String(v))}>{(v/1000)}rb</button>
              ))}
            </div>
          </div>

          {err && <div className="tag fail" style={{ justifyContent:'center' }}>{err}</div>}
          <button className="block" disabled={loading}>{loading ? 'Memproses…' : 'Kirim Transfer'}</button>
          <p className="muted mono" style={{ fontSize:'0.78rem', textAlign:'center' }}>Petunjuk: kirim ke 1000000003 (rekening Nawa).</p>
        </form>
      </div>
      <ReceiptModal tx={tx} onClose={()=>setTx(null)} />
    </>
  )
}
