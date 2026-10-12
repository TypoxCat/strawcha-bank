import { useState } from 'react'
import { api } from '../lib/api'
import AppHeader from '../components/AppHeader'

export default function ChangePin() {
  const [oldPin, setOldPin] = useState('')
  const [newPin, setNewPin] = useState('')
  const [msg, setMsg] = useState('')
  const [ok, setOk] = useState(false)
  const [busy, setBusy] = useState(false)

  const submit = async (e) => {
    e.preventDefault()
    setBusy(true); setMsg('')
    try {
      await api.changePin(oldPin, newPin)
      setOk(true); setMsg('PIN berhasil diganti.')
      setOldPin(''); setNewPin('')
    } catch (err) {
      setOk(false); setMsg(err.message)
    } finally {
      setBusy(false)
    }
  }

  return (
    <>
      <AppHeader title="Ganti PIN" subtitle="Perbarui PIN keamanan" back />
      <div className="page narrow">
        <form className="card stack" onSubmit={submit}>
          <div className="field">
            <label className="label">PIN Lama</label>
            <input type="password" maxLength={6} value={oldPin} onChange={e=>setOldPin(e.target.value)} autoFocus placeholder="••••••" />
          </div>
          <div className="field">
            <label className="label">PIN Baru (6 digit)</label>
            <input type="password" maxLength={6} value={newPin} onChange={e=>setNewPin(e.target.value)} placeholder="••••••" />
          </div>
          {msg && <div className={ok ? 'tag' : 'tag fail'}>{msg}</div>}
          <button className="block" disabled={busy}>{busy ? 'Menyimpan…' : 'Simpan PIN Baru'}</button>
        </form>
      </div>
    </>
  )
}
