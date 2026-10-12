import { useState } from 'react'
import { useNavigate, Link } from 'react-router-dom'
import { api, setToken } from '../lib/api'
import { useAuth } from '../context/AuthContext'
import StrawberryLogo from '../components/StrawberryLogo'
import { APP_NAME } from '../lib/config'

export default function Register() {
  const nav = useNavigate()
  const { setUser } = useAuth()
  const [name, setName] = useState('')
  const [pin, setPin] = useState('')
  const [err, setErr] = useState('')
  const [loading, setLoading] = useState(false)

  const submit = async (e) => {
    e.preventDefault(); setErr(''); setLoading(true)
    try {
      const reg = await api.register(name, pin)
      const res = await api.login(reg.id, pin)
      setToken(res.token); setUser(res.customer); nav('/dashboard')
    } catch (e) { setErr(e.message) }
    finally { setLoading(false) }
  }

  return (
    <div className="page narrow" style={{ paddingTop:'2rem' }}>
      <div style={{ display:'grid', justifyItems:'center', marginBottom:'1.2rem', textAlign:'center' }}>
        <StrawberryLogo size={72} />
        <h1 style={{ fontSize:'1.8rem', marginTop:'0.6rem' }}>Buat Akun</h1>
        <p className="muted">Daftar ke {APP_NAME}</p>
      </div>
      <form className="card stack" onSubmit={submit}>
        <div className="field">
          <label className="label">Nama Lengkap</label>
          <input value={name} onChange={e=>setName(e.target.value)} placeholder="Nama kamu" autoFocus />
        </div>
        <div className="field">
          <label className="label">PIN (6 digit)</label>
          <input type="password" value={pin} onChange={e=>setPin(e.target.value)} placeholder="••••••" maxLength={6} />
        </div>
        {err && <div className="tag fail" style={{ justifyContent:'center' }}>{err}</div>}
        <button className="matcha block" disabled={loading}>{loading ? 'Memproses…' : 'Daftar'}</button>
        <p className="muted" style={{ textAlign:'center', fontSize:'0.9rem' }}>
          Sudah punya akun? <Link to="/login" style={{ color:'var(--berry-700)', fontWeight:700 }}>Masuk</Link>
        </p>
      </form>
    </div>
  )
}
