import { useEffect, useState } from 'react'
import { api } from '../lib/api'
import AppHeader from '../components/AppHeader'
import ReceiptModal from '../components/ReceiptModal'

const rupiah = (n) => 'Rp ' + Number(n).toLocaleString('id-ID')

export default function Payments() {
  const [me, setMe] = useState(null)
  const [tab, setTab] = useState('topup')
  const [from, setFrom] = useState('')
  const [provider, setProvider] = useState('GoPay')
  const [phone, setPhone] = useState('')
  const [amount, setAmount] = useState('')
  const [billType, setBillType] = useState('Electricity')
  const [customerNo, setCustomerNo] = useState('')
  const [billAmt, setBillAmt] = useState('')
  const [tx, setTx] = useState(null)
  const [err, setErr] = useState('')
  const [loading, setLoading] = useState(false)

  useEffect(() => {
    api.me().then(m => { setMe(m); if (m.accounts[0]) setFrom(m.accounts[0].accountNumber) })
  }, [])

  const run = async (fn) => {
    setErr(''); setLoading(true); setTx(null)
    try { setTx(await fn()) }
    catch (e) { setErr(e.message) }
    finally { setLoading(false) }
  }

  return (
    <>
      <AppHeader title="Bayar & Top Up" subtitle="E-wallet dan tagihan bulanan" back />
      <div style={{ padding:'0 0.9rem', display:'grid', gap:'1rem' }}>
        <div className="row" style={{ gap:'0.4rem' }}>
          <button className={tab==='topup' ? 'tab active' : 'tab'} onClick={()=>{setTab('topup');setErr('');setTx(null)}}>Top Up E-Wallet</button>
          <button className={tab==='bill' ? 'tab active' : 'tab'} onClick={()=>{setTab('bill');setErr('');setTx(null)}}>Bayar Tagihan</button>
        </div>

        <div className="card stack">
          <div className="field">
            <label className="label">Rekening Sumber</label>
            <select value={from} onChange={e=>setFrom(e.target.value)}>
              {me?.accounts.map(a => <option key={a.accountNumber} value={a.accountNumber}>{a.typeName} · {rupiah(a.balance)}</option>)}
            </select>
          </div>

          {tab === 'topup' ? (
            <form className="stack" onSubmit={e=>{e.preventDefault(); run(()=>api.topup(from, provider, phone, parseFloat(amount)))}}>
              <div className="field">
                <label className="label">Provider</label>
                <select value={provider} onChange={e=>setProvider(e.target.value)}>
                  <option>GoPay</option><option>OVO</option><option>DANA</option><option>ShopeePay</option>
                </select>
              </div>
              <div className="field">
                <label className="label">Nomor HP</label>
                <input inputMode="numeric" value={phone} onChange={e=>setPhone(e.target.value)} placeholder="081234567890" />
              </div>
              <div className="field">
                <label className="label">Nominal</label>
                <input inputMode="numeric" value={amount} onChange={e=>setAmount(e.target.value)} placeholder="0" />
                <div className="row" style={{ marginTop:'0.4rem' }}>
                  {[20000,50000,100000,200000].map(v => (
                    <button type="button" key={v} className="tab" onClick={()=>setAmount(String(v))}>{v/1000}rb</button>
                  ))}
                </div>
              </div>
              {err && <div className="tag fail" style={{ justifyContent:'center' }}>{err}</div>}
              <button className="block" disabled={loading}>{loading ? 'Memproses…' : 'Top Up'}</button>
            </form>
          ) : (
            <form className="stack" onSubmit={e=>{e.preventDefault(); run(()=>api.paybill(from, billType, customerNo, parseFloat(billAmt)))}}>
              <div className="field">
                <label className="label">Jenis Tagihan</label>
                <select value={billType} onChange={e=>setBillType(e.target.value)}>
                  <option value="Electricity">Listrik</option><option value="Water">Air (PDAM)</option><option value="Internet">Internet</option>
                </select>
              </div>
              <div className="field">
                <label className="label">Nomor Pelanggan</label>
                <input inputMode="numeric" value={customerNo} onChange={e=>setCustomerNo(e.target.value)} placeholder="5123456789" />
              </div>
              <div className="field">
                <label className="label">Jumlah Tagihan</label>
                <input inputMode="numeric" value={billAmt} onChange={e=>setBillAmt(e.target.value)} placeholder="0" />
              </div>
              {err && <div className="tag fail" style={{ justifyContent:'center' }}>{err}</div>}
              <button className="matcha block" disabled={loading}>{loading ? 'Memproses…' : 'Bayar Tagihan'}</button>
            </form>
          )}
        </div>
      </div>
      <ReceiptModal tx={tx} onClose={()=>setTx(null)} />
    </>
  )
}
