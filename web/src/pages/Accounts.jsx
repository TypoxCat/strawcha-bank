import { useEffect, useState } from 'react'
import { Link, useNavigate, useParams } from 'react-router-dom'
import { api } from '../lib/api'
import AppHeader from '../components/AppHeader'

const rupiah = (n) => 'Rp ' + Number(n).toLocaleString('id-ID')

const splitParts = (s) => String(s || '').split(' | ').map(x => x.trim()).filter(Boolean)

const accountRule = (summary) => {
  const p = splitParts(summary).find(x => /min balance|overdraft/i.test(x))
  if (!p) return null
  return p.replace(/Min balance:\s*/i, 'Min. saldo ').replace(/Overdraft limit:\s*/i, 'Limit overdraft ')
}

const humanize = (d) => d.replace(/^Account opened with /i, 'Rekening dibuka dengan ')

function MutasiRow({ line }) {
  const parts = splitParts(line)
  const time = parts[0]
  const desc = humanize(parts[1] || '')
  const bal = parts.slice(2).join(' · ').replace(/Balance:\s*/i, 'Saldo ')
  return (
    <div className="mutasi-row">
      <div className="mutasi-top">
        <span className="mutasi-desc">{desc}</span>
        {bal && <span className="mutasi-bal mono">{bal}</span>}
      </div>
      <div className="mutasi-time mono">{time}</div>
    </div>
  )
}

function Modal({ title, onClose, children }) {
  return (
    <div className="overlay" onClick={onClose}>
      <div className="card modal" onClick={e=>e.stopPropagation()}>
        <h3 style={{ marginBottom:'0.7rem' }}>{title}</h3>
        {children}
      </div>
    </div>
  )
}

export default function Accounts() {
  const nav = useNavigate()
  const { num } = useParams()
  const [me, setMe] = useState(null)
  const [acc, setAcc] = useState(null)
  const [err, setErr] = useState('')
  const [modal, setModal] = useState(null)
  const [amount, setAmount] = useState('')
  const [note, setNote] = useState('')
  const [type, setType] = useState('savings')
  const [initial, setInitial] = useState('100000')

  async function reload() {
    const m = await api.me(); setMe(m)
    if (num) {
      try { const a = await api.account(num); setAcc(a); setErr('') }
      catch (e) { setErr(e.message); setAcc(null) }
    } else setAcc(null)
  }
  useEffect(() => { reload() }, [num])

  const close = () => { setModal(null); setAmount(''); setNote('') }
  const doDeposit = async () => { try { await api.deposit(num, parseFloat(amount), note || 'Deposit'); close(); await reload() } catch (e) { alert(e.message) } }
  const doWithdraw = async () => { try { await api.withdraw(num, parseFloat(amount), note || 'Withdrawal'); close(); await reload() } catch (e) { alert(e.message) } }
  const doCreate = async () => { try { const a = await api.createAccount(type, parseFloat(initial)); close(); await reload(); nav('/accounts/' + a.accountNumber) } catch (e) { alert(e.message) } }

  return (
    <>
      <AppHeader title="Rekening" subtitle={me ? `${me.accounts.length} rekening aktif` : ''} back />
      <div style={{ padding:'0 0.9rem', display:'grid', gap:'1rem' }}>
        <div className="row" style={{ justifyContent:'space-between' }}>
          <span className="section-title">Daftar Rekening</span>
          <button className="matcha" onClick={()=>setModal('new')} style={{ padding:'0.45rem 0.8rem', fontSize:'0.85rem' }}>+ Baru</button>
        </div>

        <div className="list">
          {me?.accounts.map((a, i) => (
            <Link key={a.accountNumber} to={'/accounts/' + a.accountNumber} className={'acct-card ' + (i % 2 ? 'berry' : 'matcha')}>
              <div className="between">
                <strong>{a.typeName}</strong>
                <span className="mono">{rupiah(a.balance)}</span>
              </div>
              <div className="muted mono" style={{ fontSize:'0.8rem' }}>{a.accountNumber}</div>
            </Link>
          ))}
        </div>

        {acc ? (
          <div className="stack">
            <div className="balance-card">
              <div className="balance-label">{acc.typeName} · {acc.accountNumber}</div>
              <div className="balance-amount" style={{ fontSize:'clamp(1.5rem,6vw,2rem)' }}>{rupiah(acc.balance)}</div>
              {accountRule(acc.summary) && <div className="balance-meta"><span className="chip">{accountRule(acc.summary)}</span></div>}
              <div className="row" style={{ marginTop:'0.9rem' }}>
                <button className="butter" onClick={()=>setModal('deposit')}>Setor</button>
                <button onClick={()=>setModal('withdraw')}>Tarik</button>
              </div>
            </div>

            <section>
              <div className="section-head"><span className="section-title">Mutasi</span></div>
              <div className="card pad-sm">
                <div className="list" style={{ gap:0 }}>
                  {acc.history.length === 0 && <div className="empty">Belum ada mutasi.</div>}
                  {acc.history.map((h,i) => <MutasiRow key={i} line={h} />)}
                </div>
              </div>
            </section>
          </div>
        ) : err ? (
          <div className="card" style={{ color:'var(--berry-700)' }}>{err}</div>
        ) : (
          <div className="card empty">Pilih rekening untuk lihat detail &amp; mutasi.</div>
        )}
      </div>

      {modal === 'deposit' && (
        <Modal title="Setor Dana" onClose={close}>
          <div className="field" style={{ marginBottom:'0.8rem' }}><label className="label">Nominal</label><input inputMode="numeric" value={amount} onChange={e=>setAmount(e.target.value)} placeholder="0" autoFocus /></div>
          <div className="field" style={{ marginBottom:'1rem' }}><label className="label">Catatan (opsional)</label><input value={note} onChange={e=>setNote(e.target.value)} placeholder="Deposit" /></div>
          <div className="row" style={{ justifyContent:'flex-end' }}><button className="ghost" onClick={close}>Batal</button><button onClick={doDeposit}>Setor</button></div>
        </Modal>
      )}
      {modal === 'withdraw' && (
        <Modal title="Tarik Dana" onClose={close}>
          <div className="field" style={{ marginBottom:'0.8rem' }}><label className="label">Nominal</label><input inputMode="numeric" value={amount} onChange={e=>setAmount(e.target.value)} placeholder="0" autoFocus /></div>
          <div className="field" style={{ marginBottom:'1rem' }}><label className="label">Catatan (opsional)</label><input value={note} onChange={e=>setNote(e.target.value)} placeholder="Withdrawal" /></div>
          <div className="row" style={{ justifyContent:'flex-end' }}><button className="ghost" onClick={close}>Batal</button><button className="matcha" onClick={doWithdraw}>Tarik</button></div>
        </Modal>
      )}
      {modal === 'new' && (
        <Modal title="Buka Rekening Baru" onClose={close}>
          <div className="field" style={{ marginBottom:'0.8rem' }}>
            <label className="label">Tipe Rekening</label>
            <select value={type} onChange={e=>setType(e.target.value)}>
              <option value="savings">Savings · min. saldo + bunga</option>
              <option value="checking">Checking · overdraft + biaya</option>
            </select>
          </div>
          <div className="field" style={{ marginBottom:'1rem' }}><label className="label">Setoran Awal</label><input inputMode="numeric" value={initial} onChange={e=>setInitial(e.target.value)} /></div>
          <div className="row" style={{ justifyContent:'flex-end' }}><button className="ghost" onClick={close}>Batal</button><button className="matcha" onClick={doCreate}>Buka</button></div>
        </Modal>
      )}
    </>
  )
}
